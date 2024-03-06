#ifndef RESOURCE_H
#define RESOURCE_H

#include "cac_lib.h"

#include <bitset>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cac {

// Represent a primitive value that both SIM and DUT can modify.
// E.g. integer registers, CSRs, PC.
class Resource {
    public:
        // Returns the value stored by this resource.
        virtual const data_t& GetValue() const = 0;

        // Attempts to modify the value of the Resource. Returns true if this
        // is successful or false if it fails.
        virtual bool SetValue(const data_t&& data, optional_mask_t mask) = 0;

        // Attempts to modifies the size of this Resource and returns true if
        // this is done. Returns false if the resource's size cannot be modified.
        virtual bool SetSize(size_n_bit_t sz) = 0;

        // Returns true if `data` matches this resource's data.
        bool CompareValue(const data_t& data) const;

        // Returns the resource ID.
        resource_id_t GetResourceId() const;
    
        // Returns the resource name (e.g. PC, X2, etc).
        std::string GetName() const;

        // Returns the resource's number of bits.
        size_n_bit_t GetSize() const;

        // Returns a string in the format: "{type}:[Data:{first 64 bits}_{second 64 bits}...]({total bits})"
        // e.g. "ISS:[Data:0000000000000010_0000000000000020](128)"
        std::string ToString(const std::string& type) const;

        virtual ~Resource() = default;

    protected:
        Resource(resource_id_t rid, size_n_bit_t size_bits);

        // Returns true iff:
        // - `data.size()` is in the rid's ALLOWED_SIZE_BITS (see cac_const.h)
        // - `data.size()` is the same as the rid's DEFAULT_SIZE_BITS (see cac_const.h) or the resource type is resizable (see cac_types.h)
        // - If a `mask` is provided, it's size matches `data.size()`
        // - `rid.resource` is valid (doesn't match a sentinel value)
        // - 0 <= `rid.offset` <= max address for that resource type
        static bool ValidateResource(resource_id_t rid, const data_t& data, optional_mask_t mask);

        // The resource type and address.
        resource_id_t id_;
        // Size of data_ in bits
        size_n_bit_t size_;
};

class VariableSizeResource : public Resource {
    public:
        // Creates a new Resource or returns std::nullopt if the arguments are invalid.
        // See `ValidateResource` for the conditions which determine argument validity.
        static std::optional<std::unique_ptr<VariableSizeResource>> Create(resource_id_t rid, const data_t&& data, optional_mask_t mask = std::nullopt);

        // Create a new resource with the default size for the given `rid`. See cac_types.h for these sizes.
        static std::optional<std::unique_ptr<VariableSizeResource>> Create(resource_id_t rid);

        const data_t& GetValue() const override;

        bool SetValue(const data_t&& data, optional_mask_t mask = std::nullopt) override;

        bool SetSize(size_n_bit_t sz) override;

    private:
        VariableSizeResource(resource_id_t rid, const data_t&& data);

        data_t data_;
};

// A collection of Resources that can be accessed by resource_id_t.
class ResourceSnapshot {
    // Note: For each function where a `resource_id_t id` can be provided, there is an
    // assumption that `Exists(id)` is true.
    public:
        // Creates a ResourceSnapshot. If `reserve_resources` is true, a default Resource for each
        // `resource_id_t` (e.g. every resource type and offset) is added to the snapshot.
        // The `type` should be "ISS" or "DUT".
        ResourceSnapshot(bool reserve_resources, const std::string& type);

        // Getters / Setters

        bool Exists(resource_id_t id) const;

        std::string GetName(resource_id_t id) const;

        const data_t& GetValue(resource_id_t id) const;

        bool SetValue(resource_id_t id, const data_t&& data, optional_mask_t mask = std::nullopt);

        // Returns true iff the value stored with the given `id` matches `data`.
        bool CompareValue(resource_id_t id, const data_t& data) const;

        size_n_bit_t GetSize(resource_id_t id) const;

        // Attempts to set the sizes of all vec_reg resources. Returns true iff this suceeds.
        bool SetVlen(unsigned int vlen);

        std::string ToString(resource_id_t id) const;

        // Returns the number of resources that have been modified since the most recent
        // call to ResetChangedResources().
        int GetChangeCount() const;

        // Returns all of the resource IDs that have been modified since the most recent
        // call to ResetChangedResources().
        const std::unordered_set<resource_id_t>& GetChangedResources() const;

        void ResetChangedResources();
    private:
        // If true, the `snapshot_col` will be filled with values for each Resource type at the time of construction.
        bool reserved_;
        // "DUT" or "ISS"
        std::string type_;
        // Contains all of the resource IDs that were modified since the last call to ResetChangedResources()
        std::unordered_set<resource_id_t> changed_resources_;
        // Contains all of the resource values that have been added with set_value().
        std::unordered_map<resource_id_t, std::unique_ptr<Resource>> snapshot_col_;
};

}

#endif
