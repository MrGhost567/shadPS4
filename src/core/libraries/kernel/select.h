// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "common/assert.h"
#include "common/types.h"

using fd_mask_elt = u64;

static constexpr std::size_t ORBIS_FD_SETSIZE = 1024;
static constexpr std::size_t ORBIS_NFDBITS(sizeof(fd_mask_elt) * 8); /* bits per mask */
static constexpr std::size_t num_mask_elts = (ORBIS_FD_SETSIZE + ORBIS_NFDBITS - 1) / ORBIS_NFDBITS;

struct OrbisFdSet {
    fd_mask_elt bit_mask[num_mask_elts];

    static void fd_set(OrbisFdSet* set, int fd) {
        ASSERT(set);
        set->bit_mask[fd / ORBIS_NFDBITS] |= (1 << (fd % ORBIS_NFDBITS));
    }

    static void fd_clr(OrbisFdSet* set, int fd) {
        ASSERT(set);
        set->bit_mask[fd / ORBIS_NFDBITS] &= ~(1 << (fd % ORBIS_NFDBITS));
    }

    static bool fd_is_set(const OrbisFdSet* set, int fd) {
        ASSERT(set);
        return set->bit_mask[fd / ORBIS_NFDBITS] & (1 << (fd % ORBIS_NFDBITS));
    }

    static bool any(const OrbisFdSet* set) {
        ASSERT(set);
        for (int i = 0; i < num_mask_elts; i++) {
            if (set->bit_mask[i] != 0) {
                return true;
            }
        }
        return false;
    }
};