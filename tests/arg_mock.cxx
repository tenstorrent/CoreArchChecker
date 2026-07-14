// SPDX-FileCopyrightText: 2026 Tenstorrent USA, Inc.
// SPDX-License-Identifier: Apache-2.0

#include "cvm/plusargs.hpp"
#include "vpi_user.h"

DEFINE_bool(bridge_log, true, "Enable bridge trace prints");

// Needed for plusargs
extern "C" {
    PLI_INT32 vpi_get_vlog_info (p_vpi_vlog_info) {
        return 0;
    }
}
