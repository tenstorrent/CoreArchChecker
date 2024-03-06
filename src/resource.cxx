#include "resource.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fmt/format.h>
#include <iostream>
#include <optional>
#include <vector>

namespace cac {

Resource::Resource(resource_id_t rid, size_n_bit_t size_bits)
  : resource_id_(rid),
    size_(size_bits) {}

resource_id_t Resource::GetResourceId() const {
    return resource_id_;
};

std::string Resource::GetName() const {
    return resource_id_.ToString();
}

size_n_bit_t Resource::GetSize() const {
    return size_;
};

bool Resource::CompareValue(const data_t& data) const {
    return GetValue() == data;
}

std::string Resource::ToString(const std::string& type) const {
    auto value = GetValue();
    constexpr size_n_bit_t bits_per_group = UNIT_BIT_NUM;
    std::vector<std::string> data_strings;
    for (size_t i = 0; i < value.size(); i += bits_per_group) {
        size_n_bit_t bits_to_use = std::min(bits_per_group, static_cast<size_n_bit_t>(value.size() - i));
        auto bits_subset = ToBitset<bits_per_group>(value, i, bits_to_use);
        data_strings.push_back(fmt::format("{:0{}x}", bits_subset.to_ullong(), sizeof(unit_data_t)*2));
    }
    return fmt::format("{}:[Data:{}]({})", type, fmt::join(data_strings, "_"), size_);
}

bool Resource::ValidateResource(resource_id_t rid, const data_t& data, optional_mask_t mask) {
    bool valid;
    valid = (data.size() == DefaultSize(rid.resource) || (rid.Resizable() && AllowedSize(rid.resource, data.size())));
    valid &= (!(rid.resource == resource_t::start || rid.resource == resource_t::end));
    valid &= (rid.offset >= 0 && rid.offset < (1 << OFFSET_BITS.at(rid.resource)));
    valid &= (mask == std::nullopt || mask.value().get().size() == data.size());
    return valid;
}

VariableSizeResource::VariableSizeResource(resource_id_t rid, const data_t&& data)
    : Resource(rid, data.size()),
      data_(data) {}

std::optional<std::unique_ptr<VariableSizeResource>> VariableSizeResource::Create(resource_id_t rid, const data_t&& data, optional_mask_t mask) {
    if (!ValidateResource(rid, data, mask)) {
        return std::nullopt;
    }

    if (mask == std::nullopt) {
        return std::unique_ptr<VariableSizeResource>(new VariableSizeResource(rid, std::move(data)));
    } else {
        data_t masked = MaskData(data, mask.value());
        return std::unique_ptr<VariableSizeResource>(new VariableSizeResource(rid, std::move(masked)));
    }
}

std::optional<std::unique_ptr<VariableSizeResource>> VariableSizeResource::Create(resource_id_t rid) {
    data_t data = data_t(DefaultSize(rid.resource), 0);
    assert(ValidateResource(rid, data, std::nullopt));
    return std::unique_ptr<VariableSizeResource>(new VariableSizeResource(rid, std::move(data)));
}

const data_t& VariableSizeResource::GetValue() const {
    return data_;
}

bool VariableSizeResource::SetValue(const data_t&& data, optional_mask_t mask) {
    if (!ValidateResource(GetResourceId(), data, mask)) {
        return false;
    }
    size_ = data.size();
    if (mask == std::nullopt) {
        data_ = data;
    } else {
        data_ = OrData(MaskData(data_, InvMask(mask.value())), MaskData(data, mask.value()));
    }
    return true;
}

bool VariableSizeResource::SetSize(size_n_bit_t sz) {
    if (!GetResourceId().Resizable() || !AllowedSize(resource_id_.resource, sz)) {
        return false;
    }
    data_.resize(sz);
    size_ = sz;
    return true;
}

ResourceSnapshot::ResourceSnapshot(bool reserve_space, const std::string& type)
  : reserved_(reserve_space),
    type_(type),
    changed_resources_({}) {

    if (!reserved_) {
        return;
    }
    for (unsigned i = static_cast<int>(resource_t::start) + 1; i <  static_cast<int>(resource_t::end); ++i) {
        resource_t resource = static_cast<resource_t>(i);
        // We don't want to track all possible CSRs yet since the address space is large.

        if (resource == resource_t::csr_reg) {
            continue;
        }
        size_n_bit_t max_addr = (1 << OFFSET_BITS.at(resource)) - 1;
        for (unsigned addr = 0; addr <= max_addr; ++addr) {
            resource_id_t rid = {
                .resource = resource,
                .offset = addr
            };
            std::optional<std::unique_ptr<Resource>> r = VariableSizeResource::Create(rid);
            assert(r != std::nullopt);
            snapshot_col_.insert_or_assign(rid, std::move(r.value()));
        }
    }
};

bool ResourceSnapshot::Exists(resource_id_t id) const {
    return (snapshot_col_.find(id) != snapshot_col_.end());
}

std::string ResourceSnapshot::GetName(resource_id_t id) const {
    return snapshot_col_.at(id)->GetName();
}

const data_t& ResourceSnapshot::GetValue(resource_id_t id) const {
    if (!Exists(id)) {
        static const data_t zeros(64, false);
        return zeros;
    }
    return snapshot_col_.at(id)->GetValue();
};

bool ResourceSnapshot::SetValue(resource_id_t id, const data_t&& data, optional_mask_t mask) {
    // We expect the value to already exist in the map if:
    // 1. reserved_ was set to true in the constructor or
    // 2. the value was inserted using a previous call to SetValue()
    // The only exception is for csr registers, which we don't preallocate due
    // to the large address space.

    if ((!reserved_ || id.resource == resource_t::csr_reg) && !Exists(id)) {
        std::optional<std::unique_ptr<VariableSizeResource>> r = VariableSizeResource::Create(id, std::move(data), mask);
        if (r == std::nullopt) {
            return false;
        }
        snapshot_col_.insert_or_assign(id, std::move(r.value()));
    } else if (!snapshot_col_.at(id)->SetValue(std::move(data), mask)) {
        return false;
    }
    changed_resources_.insert(id);
    return true;
}

bool ResourceSnapshot::CompareValue(resource_id_t id, const data_t& data) const {
    return snapshot_col_.at(id)->CompareValue(data);
}

size_n_bit_t ResourceSnapshot::GetSize(resource_id_t id) const {
    return snapshot_col_.at(id)->GetSize();
}

bool ResourceSnapshot::SetVlen(unsigned int vlen) {
    bool ret = true;
    std::vector<resource_t> resources = { resource_t::vec_reg };
    for (const auto& resource : resources) {
        size_n_bit_t max_addr = (1 << OFFSET_BITS.at(resource)) - 1;
        for (unsigned addr = 0; addr <= max_addr; ++addr) {
            resource_id_t rid = {
                .resource = resource,
                .offset = addr
            };
            assert(rid.Resizable());
            if (!snapshot_col_.at(rid)->SetSize(vlen)) {
                ret = false;
            }
        }
    }
    return ret;
}

std::string ResourceSnapshot::ToString(resource_id_t id) const {
    return snapshot_col_.at(id)->ToString(type_);
}

int ResourceSnapshot::GetChangeCount() const {
    return changed_resources_.size();
}

const std::unordered_set<resource_id_t>& ResourceSnapshot::GetChangedResources() const {
    return changed_resources_;
}

void ResourceSnapshot::ResetChangedResources() {
    changed_resources_.clear();
}

}
