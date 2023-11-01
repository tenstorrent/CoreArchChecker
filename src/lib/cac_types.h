#pragma once

#include <bitset>
#include <fmt/format.h>
#include <optional>
#include <stdexcept>
#include <vector>

#include "cvm/bitmanip.hpp"

namespace cac {

using size_1_byte_t = unsigned char;
using size_2_bytes_t = unsigned short int;
using size_4_bytes_t = unsigned int;
using size_8_bytes_t = unsigned long long int;

using hart_t = size_2_bytes_t;
using size_n_bit_t = size_2_bytes_t;
using unit_data_t = size_8_bytes_t;

using data_t = std::vector<bool>;
using mask_t = std::vector<bool>;

template <typename T>
using optional_const_ref = std::optional<std::reference_wrapper<const T>>;

using optional_mask_t = optional_const_ref<mask_t>;

// Converts a primitive into a std::vector<bool>.
template<typename T>
inline data_t CreateBitVec(const T& t) {
    return cvm::bitmanip::slice<T, data_t>(t);
}

// Converts a std::vector<primitive> into a std::vector<bool>.
template<typename T>
inline data_t CreateBitVec(const std::vector<T>& vec) {
    data_t ret;
    ret.reserve(vec.size() * sizeof(T) * 8);
    for (size_t i = 0; i < vec.size(); ++i) {
        data_t current = cvm::bitmanip::slice<T, data_t>(vec[i]);
        ret.insert(ret.begin(), current.begin(), current.end());
    }
    return ret;
}

// Returns a copy of `data` bitmasked by `mask`.
// Precondition: data.size() == mask.size()
inline data_t MaskData(const data_t& data, const mask_t& mask) {
    data_t ret;
    size_t sz = std::min(data.size(), mask.size());
    ret.reserve(sz);
    for (size_t i = 0; i < sz; ++i) {
        ret.push_back(data[i] && mask[i]);
    }
    return ret;
}

inline data_t InvMask(const mask_t& mask) {
    data_t ret;
    for (size_t i = 0; i < mask.size(); ++i) {
        ret.push_back(!mask[i]);
    }
    return ret;
}

inline data_t OrData(const data_t& data1, const data_t& data2) {
    data_t ret;
    size_t sz = std::min(data1.size(), data2.size());
    ret.reserve(sz);
    for (size_t i = 0; i < sz; ++i) {
        ret.push_back(data1[i] || data2[i]);
    }
    return ret;
}

// Converts a std::vector<bool> to a std::bitset.
template<size_t BITS>
inline std::bitset<BITS> ToBitset(const data_t& data, size_t start, size_t num_bits) {
    std::bitset<BITS> ret;
    for (size_t i = 0; i < std::min(BITS, num_bits); ++i) {
        ret[i] = data[start + i];
    }
    return ret;
}

template<size_t BITS>
inline std::bitset<BITS> ToBitset(const data_t& data) {
    return ToBitset<BITS>(data, 0, data.size());
}

// Represents the source of a resource change.
enum class src_t {
    dut,
    iss
};

inline std::string ToString(src_t src) {
    switch(src) {
        case src_t::dut:
            return "DUT";
        case src_t::iss:
            return "ISS";
        default:
            throw std::runtime_error("src doesn't have a defined string in ToString");
    }
    return "";
}

// Start and end are sentinel values for iterating over a resource_t.
enum class resource_t {
    start,
    int_reg,
    fp_reg,
    vec_reg,
    csr_reg,
    pc_reg,
    insn_bytes,
    end
};

// Resource identifier -- uses a resource type and an offset/address.
typedef struct resource_id_t {
    resource_t resource;
    size_8_bytes_t offset;

    std::string ToString() const {
        switch(resource) {
            case resource_t::int_reg:
                return fmt::format("X{}", offset);
            case resource_t::fp_reg:
                return fmt::format("F{}", offset);
            case resource_t::vec_reg:
                return fmt::format("V{}", offset);
            case resource_t::csr_reg:
                return fmt::format("C_0x{:x}", offset);
            case resource_t::pc_reg:
                return "PC";
            case resource_t::insn_bytes:
                return "INSN";
            default:
                throw std::runtime_error(fmt::format("resource_t %d is not defined", static_cast<int>(resource)));
        }
        return "";
    }

    bool Resizable() const {
        if (resource == resource_t::vec_reg) {
            return true;
        }
        return false;
    }

    bool operator==(const resource_id_t &other) const {
        return (resource == other.resource && offset == other.offset);
    }

} resource_id_t;

}

template <>
struct std::hash<cac::resource_id_t> {
    std::size_t operator()(const cac::resource_id_t& id) const {
        return ((std::hash<int>()(static_cast<int>(id.resource)) ^ (std::hash<int>()(id.offset) << 1)) >> 1);
    }
};
