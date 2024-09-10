// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <algorithm>
#include <boost/container/static_vector.hpp>

#include "common/assert.h"
#include "common/types.h"
#include "shader_recompiler/shader_stages.h"
#include "video_core/amdgpu/liverpool.h"

namespace Shader {

[[nodiscard]] constexpr Stage StageFromIndex(size_t index) noexcept {
    return static_cast<Stage>(index);
}

enum class MrtSwizzle : u8 {
    Identity = 0,
    Alt = 1,
    Reverse = 2,
    ReverseAlt = 3,
};
static constexpr u32 MaxColorBuffers = 8;

enum class VsOutput : u8 {
    None,
    PointSprite,
    EdgeFlag,
    KillFlag,
    GsCutFlag,
    GsMrtIndex,
    GsVpIndex,
    CullDist0,
    CullDist1,
    CullDist2,
    CullDist3,
    CullDist4,
    CullDist5,
    CullDist6,
    CullDist7,
    ClipDist0,
    ClipDist1,
    ClipDist2,
    ClipDist3,
    ClipDist4,
    ClipDist5,
    ClipDist6,
    ClipDist7,
};
using VsOutputMap = std::array<VsOutput, 4>;
using GsOutputMap = std::array<VsOutput, 4>;

struct VertexRuntimeInfo {
    boost::container::static_vector<VsOutputMap, 3> outputs;
    bool emulate_depth_negative_one_to_one{};

    bool operator==(const VertexRuntimeInfo& other) const noexcept {
        return emulate_depth_negative_one_to_one == other.emulate_depth_negative_one_to_one;
    }
};

struct FragmentRuntimeInfo {
    struct PsInput {
        u8 param_index;
        bool is_default;
        bool is_flat;
        u8 default_value;

        auto operator<=>(const PsInput&) const noexcept = default;
    };
    boost::container::static_vector<PsInput, 32> inputs;
    std::array<MrtSwizzle, MaxColorBuffers> mrt_swizzles;

    bool operator==(const FragmentRuntimeInfo& other) const noexcept {
        return std::ranges::equal(mrt_swizzles, other.mrt_swizzles) &&
               std::ranges::equal(inputs, other.inputs);
    }
};

struct ComputeRuntimeInfo {
    u32 shared_memory_size;
    std::array<u32, 3> workgroup_size;
    std::array<bool, 3> tgid_enable;

    bool operator==(const ComputeRuntimeInfo& other) const noexcept {
        return workgroup_size == other.workgroup_size && tgid_enable == other.tgid_enable;
    }
};

// Forward declaration because Program contains references to Info
namespace IR {
class Program;
}

struct GeometryRuntimeInfo {
    AmdGpu::Liverpool::GsPrimType gs_out_prim_type;
    AmdGpu::Liverpool::GsMode gs_mode;
    AmdGpu::Liverpool::GsCutMode gs_cut_mode;
    // TODO check for instancing
    // TODO check ring sizes
    // can we be more exact about components and offsets in each stage if we inspect the registers
    // closer? Or just use heuristic (offsets written by previous stage correspond to offsets read
    // by next stage in ascending order)? What if some attributes unused by next stage, or not
    // written by previous stage
    IR::Program* gs_copy_shader;

    bool operator==(const GeometryRuntimeInfo& other) const noexcept {
        return gs_out_prim_type == other.gs_out_prim_type && gs_mode == other.gs_mode &&
               gs_cut_mode == other.gs_cut_mode && gs_copy_shader == other.gs_copy_shader;
    }
};

struct GSCopyRuntimeInfo {
    u32 dummy;
};

/**
 * Stores information relevant to shader compilation sourced from liverpool registers.
 * It may potentially differ with the same shader module so must be checked.
 * It's also possible to store any other custom information that needs to be part of shader key.
 */
struct RuntimeInfo {
    Stage stage;
    u32 num_user_data;
    u32 num_input_vgprs;
    u32 num_allocated_vgprs;
    SWStage software_stage; // TODO probably move to just Info
    VertexRuntimeInfo vs_info;
    FragmentRuntimeInfo fs_info;
    ComputeRuntimeInfo cs_info;
    GeometryRuntimeInfo gs_info;
    bool geom_enabled{false}; // TODO delete

    RuntimeInfo(Stage stage_) : stage{stage_} {}

    bool operator==(const RuntimeInfo& other) const noexcept {
        switch (stage) {
        case Stage::Fragment:
            return fs_info == other.fs_info;
        case Stage::Vertex:
            return vs_info == other.vs_info;
        case Stage::Compute:
            return cs_info == other.cs_info;
        case Stage::Geometry:
            return gs_info == other.gs_info;
            return false;
        default:
            return true;
        }
    }

    // TODO delete
    bool isGeometryRelated() const {
        return geom_enabled && (stage == Shader::Stage::Geometry ||
                                stage == Shader::Stage::Export || stage == Shader::Stage::Vertex);
    }
};

} // namespace Shader
