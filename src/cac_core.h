#ifndef CAC_CORE_H
#define CAC_CORE_H

#include <map>
#include <optional>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "cac_lib.h"
#include "cvm/bitmanip.hpp"
#include "resource.h"

namespace cac {

// Aggregation of data that each hart will contain.
struct HartData {
    // False if there are any mismatches, true otherwise.
    bool status;
    // Number of calls to CacCore::Step() for this hart.
    int step_count;
    // An aggregate of all DUT resources at the current step.
    ResourceSnapshot dut_resources;
    // An aggregate of all ISS resources at the current step.
    ResourceSnapshot iss_resources;
    // Resources updated by the DUT/ISS. These need to be compared
    // to the corresponding ISS/DUT resources.
    std::queue<resource_id_t> resources_to_check;
    std::unordered_set<resource_id_t> changed_resources;
};

class CacCore {
    // Note: All methods assume that any provided hart IDs are valid
    // (i.e., 0 <= tid < num_harts).
    public:
        // Constructs the CoreArchChecker.
        CacCore(hart_t num_harts);

        // Resets each hart's dut_resources and sim_resources.
        void Reset();

        // Sets the value for a given `tid`, `src` (ISS/DUT), and resource `id` (type and offset).
        // E.g. Whisper updates hart 0, vector register 5 to {0x5}.
        // Returns true iff the resource has been successfully updated. Reasons why this may not occur include:
        //   - Updating `data` to a new size that isn't supported by the given resource type
        //   - The size of the mask doesn't match the size of the data
        bool SetResource(hart_t tid, src_t src, resource_id_t id, const data_t&& data, optional_mask_t mask = std::nullopt, bool check_en = true);

        // Gets the value for a given `tid`, `src` (ISS/DUT), and resource `id` (type and offset)
        // Returns true (always)
        bool GetResource(hart_t tid, src_t src, resource_id_t id, data_t& data);

        // Compare all of the SIM and DUT resources for the hart `tid`. If there are any mismatches, set the status
        // to false and modify the status string to reflect the mismatch. status string is only set if there is a
        // mismatch or verbose is set
        void Step(hart_t tid, bool verbose = true);

        // Returns the current step number.
        int GetStep(hart_t tid);

        // Returns the status (true if there weren't any mismatches or ResetStatus() was called).
        bool GetStatus(hart_t tid);

        // Resets the status to true.
        void ResetStatus(hart_t tid);

        // Returns a string containing all of the resources updated by DUT and the corresponding SIM resources
        // updated during the last step.
        std::string GetStatusStr(hart_t tid);

        // Returns a string containing a mismatching resource ID (if there was a mismatch during the last step).
        std::string GetResourceStr(hart_t tid);

        // Attempt to modify the vlen of the vector registers. Returns true iff this succeeds.
        bool SetVlen(unsigned int vlen);

    private:
        // Compares the value stored for hart ID `tid` and resource ID `id` to `data`.
        // Returns true iff they match.
        bool CompareIssResource(hart_t tid, resource_id_t id, const data_t& data);
        bool CompareDutResource(hart_t tid, resource_id_t id, const data_t& data);

        // Returns the format width for a given resource.
        int GetFormatWidth(resource_id_t id, size_n_bit_t size);

        hart_t num_harts_;
        unsigned int cfg_vlen_;
        std::unordered_map<hart_t, HartData> hart_data_map_;
        std::ostringstream ss_;
        std::string resource_str_;
};

}

#endif
