// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <boost/container/static_vector.hpp>

#include "video_core/renderer_vulkan/vk_instance.h"
#include "video_core/renderer_vulkan/vk_pipeline_common.h"
#include "video_core/renderer_vulkan/vk_scheduler.h"
#include "video_core/texture_cache/texture_cache.h"

namespace Vulkan {

Pipeline::Pipeline(const Instance& instance_, Scheduler& scheduler_, DescriptorHeap& desc_heap_,
                   vk::PipelineCache pipeline_cache)
    : instance{instance_}, scheduler{scheduler_}, desc_heap{desc_heap_} {}

Pipeline::~Pipeline() = default;

void Pipeline::BindTextures(VideoCore::TextureCache& texture_cache, const Shader::Info& stage,
                            u32& binding, DescriptorWrites& set_writes) const {
    static boost::container::static_vector<vk::DescriptorImageInfo, 32> image_infos;
    image_infos.clear();

    using ImageBindingInfo = std::tuple<VideoCore::Image*, VideoCore::ImageView*, bool>;
    boost::container::static_vector<ImageBindingInfo, 32> image_bindings;

    for (const auto& image_desc : stage.images) {
        const auto tsharp = image_desc.GetSharp(stage);
        if (tsharp.GetDataFmt() != AmdGpu::DataFormat::FormatInvalid) {
            VideoCore::ImageInfo image_info{tsharp, image_desc.is_depth};
            VideoCore::ImageViewInfo view_info{tsharp, image_desc.is_storage};
            auto& image_view = texture_cache.FindTexture(image_info, view_info);
            auto& image = texture_cache.GetImage(image_view.image_id);
            image.flags |= VideoCore::ImageFlagBits::Bound;
            image_bindings.emplace_back(&image, &image_view, image_desc.is_storage);
        } else {
            image_bindings.emplace_back(nullptr, nullptr, image_desc.is_storage);
        }

        if (texture_cache.IsMeta(tsharp.Address())) {
            LOG_WARNING(Render_Vulkan, "Unexpected metadata read by a PS shader (texture)");
        }
    }

    // Second pass to re-bind images that were updated after binding
    for (auto& [image, image_view, is_storage] : image_bindings) {
        if (!image || !image_view) {
            image_infos.emplace_back(VK_NULL_HANDLE, VK_NULL_HANDLE, vk::ImageLayout::eGeneral);
        } else {
            vk::ImageLayout layout = image->last_state.layout;
            if (True(image->flags & VideoCore::ImageFlagBits::NeedsRebind)) {
                image_view = &texture_cache.FindTexture(image->info, image_view->info);
                layout = texture_cache.GetImage(image_view->image_id).last_state.layout;
            }
            image_infos.emplace_back(VK_NULL_HANDLE, *image_view->image_view, layout);
            image->flags &=
                ~(VideoCore::ImageFlagBits::NeedsRebind | VideoCore::ImageFlagBits::Bound);
        }

        set_writes.push_back({
            .dstSet = VK_NULL_HANDLE,
            .dstBinding = binding++,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType =
                is_storage ? vk::DescriptorType::eStorageImage : vk::DescriptorType::eSampledImage,
            .pImageInfo = &image_infos.back(),
        });
    }
}

} // namespace Vulkan
