#include "cac_core.h"

#include <fmt/format.h>
#include <iostream>
#include <iomanip>

#include "cvm/plusargs.hpp"

DEFINE_bool(cac_log, false, "Enable CAC trace prints");
DEFINE_bool(dut_wrote_iss_didnt, false, "If there are still pending ISS resources after exhausting DUT resources, add them to the status string");

namespace cac {

// CacCore
CacCore::CacCore(hart_t num_harts)
  : num_harts_(num_harts),
    cfg_vlen_(VEC_128),
    ss_({}),
    resource_str_("") {

    Reset();
};

void CacCore::Reset() {   
    for(hart_t tid = 0; tid < num_harts_; ++tid){
        hart_data_map_.insert_or_assign(tid, HartData{
            .status = true,
            .step_count = 0,
            // Reserve space for sim resources because we want to initialize all of them to 0.
            // Don't reserve space for DUT resources because they will be updated as they are received.
            .dut_resources = ResourceSnapshot(false, ToString(src_t::dut)),
            .iss_resources = ResourceSnapshot(true, ToString(src_t::iss)),
            .resources_to_check = {},
            .changed_resources = {}
        });
    }
}

int CacCore::GetStep(hart_t tid){
    return(hart_data_map_.at(tid).step_count);
};

bool CacCore::GetStatus(hart_t tid){
    return(hart_data_map_.at(tid).status);
};

std::string CacCore::GetStatusStr(hart_t) {
    return ss_.str();
}

std::string CacCore::GetResourceStr(hart_t) {
    return resource_str_;
}

void CacCore::ResetStatus(hart_t tid){
    hart_data_map_.at(tid).status = true;
};

bool CacCore::SetVlen(unsigned int vlen) {
    cfg_vlen_ = vlen;
    for(hart_t tid = 0; tid < num_harts_; ++tid){
        if (!hart_data_map_.at(tid).iss_resources.SetVlen(cfg_vlen_)) {
            return false;
        }
    }
    return true;
}

bool CacCore::UpdateResource(hart_t tid, src_t src, resource_id_t id, const data_t&& data, optional_mask_t mask) {
    bool successful = true;
    auto& hart_data = hart_data_map_.at(tid);
    if (src == src_t::dut) {
        successful = hart_data.dut_resources.SetValue(id, std::move(data), mask);
    } else if (src == src_t::iss) {
        successful = hart_data.iss_resources.SetValue(id, std::move(data), mask);
    }
    if (hart_data.changed_resources.find(id) == hart_data.changed_resources.end()) {
        hart_data.resources_to_check.push(id);
        hart_data.changed_resources.insert(id);
    }
    return successful;
}

bool CacCore::GetResource(hart_t tid, src_t src, resource_id_t id, data_t& data) {
    bool successful = true;
    auto& hart_data = hart_data_map_.at(tid);
    if (src == src_t::dut) {
        data = hart_data.dut_resources.GetValue(id);
    } else if (src == src_t::iss) {
        data = hart_data.iss_resources.GetValue(id);
    }
    return successful;
}

bool CacCore::CheckIssResource(hart_t tid, resource_id_t id, const data_t& data){
    if (hart_data_map_.at(tid).iss_resources.Exists(id)) {
        return(hart_data_map_.at(tid).iss_resources.CheckValue(id, data));
    }
    return false;
}

bool CacCore::CheckDutResource(hart_t tid, resource_id_t id, const data_t& data){
    if (hart_data_map_.at(tid).dut_resources.Exists(id)) {
        return(hart_data_map_.at(tid).dut_resources.CheckValue(id, data));
    }
    return false;
}

// Returns the format width for a given resource.
int CacCore::GetFormatWidth(resource_id_t id, size_n_bit_t size) {
    // Can we make this better (i.e. no hardcoded widths)
    int width = 48;
    if (id.resource == resource_t::vec_reg) {
        switch (size) {
            case VEC_128:
                width = 71;
                break;
            case VEC_256:
                width = 100;
                break;
            case VEC_512:
                width = 173;
                break;
        }
    }
    return width;
}

void CacCore::Step(hart_t tid, bool verbose) {
    ss_.str("");
    auto& hart_data = hart_data_map_.at(tid);
    auto& resources_to_check = hart_data.resources_to_check;
    auto& dut_resources = hart_data.dut_resources;
    auto& iss_resources = hart_data.iss_resources;
    ++hart_data.step_count;
    bool first_print = true;
    while (!resources_to_check.empty()) {
        resource_id_t id = resources_to_check.front();

        bool matches;
        data_t reg_val;
        std::string reg_name;
        int format_width;

        if (dut_resources.GetChangedResources().find(id) != dut_resources.GetChangedResources().end()) {
            reg_name = dut_resources.GetName(id);
            format_width = GetFormatWidth(id, dut_resources.GetSize(id));
            reg_val = dut_resources.GetValue(id);
            matches = CheckIssResource(tid, id, reg_val);
        } else {
            reg_name = iss_resources.GetName(id);
            format_width = GetFormatWidth(id, iss_resources.GetSize(id));
            reg_val = iss_resources.GetValue(id);
            matches = CheckDutResource(tid, id, reg_val);
        }

        // First mismatch
        if (hart_data.status && !matches) {
            hart_data.status = false;
            resource_str_ = id.ToString();
        }
        if (verbose || !hart_data.status) {
            if (first_print) {
                ss_ << fmt::format("Step: {}\n", hart_data.step_count);
                first_print = false;
            }
            if (dut_resources.GetChangedResources().find(id) != dut_resources.GetChangedResources().end()) {
                ss_ << fmt::format("{:>20}{:>{}}\n", reg_name, dut_resources.ToString(id), format_width);
            }
            if (iss_resources.GetChangedResources().find(id) != iss_resources.GetChangedResources().end()) {
                ss_ << fmt::format("{:>20}{:>{}}\n", reg_name, iss_resources.ToString(id), format_width);
            }
        }
        resources_to_check.pop();
    }
    // Clear
    hart_data.changed_resources.clear();
    dut_resources.ResetChangedResources();
    iss_resources.ResetChangedResources();
};

}
