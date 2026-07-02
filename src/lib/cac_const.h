// SPDX-FileCopyrightText: © 2026 Tenstorrent USA, Inc.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <set>
#include <unordered_map>

#include "cac_types.h"

namespace cac {

const size_n_bit_t UNIT_BIT_NUM = sizeof(unit_data_t) * 8;
const size_n_bit_t VEC_128 = 128;
const size_n_bit_t VEC_256 = 256;
const size_n_bit_t VEC_512 = 512;

const std::unordered_map<resource_t, size_n_bit_t> OFFSET_BITS = {
    { resource_t::int_reg,     5 },
    { resource_t::fp_reg,      5 },
    { resource_t::vec_reg,     5 },
    { resource_t::csr_reg,    12 },
    { resource_t::pc_reg,      0 },
    { resource_t::insn_bytes,  0 },
    { resource_t::priv_mode,   0 },
    { resource_t::mem_attr,    1 },  // Allow offset 0 and 1 for page-crossing support
    { resource_t::flags,       0 }
};

const std::unordered_map<resource_t, size_n_bit_t> DEFAULT_SIZE_BITS = {
    { resource_t::int_reg,    UNIT_BIT_NUM },
    { resource_t::fp_reg,     UNIT_BIT_NUM },
    { resource_t::vec_reg,    VEC_128 },
    { resource_t::csr_reg,    UNIT_BIT_NUM },
    { resource_t::pc_reg,     UNIT_BIT_NUM },
    { resource_t::insn_bytes, UNIT_BIT_NUM },
    { resource_t::priv_mode,  UNIT_BIT_NUM },
    { resource_t::mem_attr,   UNIT_BIT_NUM },
    { resource_t::flags,      UNIT_BIT_NUM },
};

inline size_n_bit_t DefaultSize(resource_t r) {
    return DEFAULT_SIZE_BITS.at(r);
}

const std::unordered_map<resource_t, std::set<size_n_bit_t>> ALLOWED_SIZE_BITS = {
    { resource_t::int_reg,    {UNIT_BIT_NUM} },
    { resource_t::fp_reg,     {UNIT_BIT_NUM} },
    { resource_t::vec_reg,    {VEC_128, VEC_256, VEC_512} },
    { resource_t::csr_reg,    {UNIT_BIT_NUM} },
    { resource_t::pc_reg,     {UNIT_BIT_NUM} },
    { resource_t::insn_bytes, {UNIT_BIT_NUM} },
    { resource_t::priv_mode,  {UNIT_BIT_NUM} },
    { resource_t::mem_attr,   {UNIT_BIT_NUM} },
    { resource_t::flags,      {UNIT_BIT_NUM} }
};

inline bool AllowedSize(resource_t r, size_n_bit_t size) {
    return (ALLOWED_SIZE_BITS.at(r).find(size) != ALLOWED_SIZE_BITS.at(r).end());
}

}
