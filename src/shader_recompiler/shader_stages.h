// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "common/enum.h"
#include "common/types.h"

namespace Shader {

enum class Stage : u32 {
    Fragment,
    Vertex,
    Geometry,
    Export,
    Hull,
    Local,
    Compute,
};
constexpr u32 MaxStageTypes = 6;

// TODO put in existing header file
// software stages that can be run on different hardware stages depending
// on the other active software stages
enum class SWStage : u32 { VS, TCS, TES, GS, GSCopy, FS, CS };

} // namespace Shader