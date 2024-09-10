// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "shader_recompiler/ir/basic_block.h"
#include "shader_recompiler/ir/program.h"
#include "shader_recompiler/runtime_info.h"

namespace Shader::Optimization {

void SsaRewritePass(IR::BlockList& program);
void IdentityRemovalPass(IR::BlockList& program);
void DeadCodeEliminationPass(IR::Program& program);
void ConstantPropagationPass(IR::BlockList& program);
void ResourceTrackingPass(IR::Program& program, const RuntimeInfo& runtime_info);
void CollectShaderInfoPass(IR::Program& program);
void LowerSharedMemToRegisters(IR::Program& program);
void MergeCopyShaderPass(IR::Program& program, RuntimeInfo& runtime_info);

} // namespace Shader::Optimization
