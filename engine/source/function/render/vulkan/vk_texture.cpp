module crumb.render.vulkan;
import :texture;

import crumb.core;
import crumb.resource;
import vk_mem_alloc;
import vulkan;
import std;

namespace Crumb::Render {

static void SubmitOnce(VKContext &ctx, vk::raii::CommandBuffer &cmd,
                       vk::raii::CommandPool &pool, vk::raii::Queue &queue)
{
	cmd.end();
	vk::SubmitInfo info{};
	info.setCommandBufferCount(1).setPCommandBuffers(&*cmd);
	queue.submit(info);
	ctx.GetDevice().waitIdle();
}

bool LoadTexture2D(VKContext &ctx, const std::string &path, VKTexture &out)
{
	auto img = LoadImageFromFile(path);
	if (!img)
	{
		Log::Error("[VK] Failed to load texture: {}", path);
		return false;
	}
	out.width  = static_cast<std::uint32_t>(img.width);
	out.height = static_cast<std::uint32_t>(img.height);
	vk::DeviceSize imageSize = static_cast<vk::DeviceSize>(img.width) * img.height * 4;

	auto &dev = ctx.GetDevice();
	auto& allocator = ctx.GetAllocator();

	// Staging buffer (VMA-Hpp, 持久映射)
	vk::BufferCreateInfo stagingInfo{};
	stagingInfo.setSize(imageSize)
	           .setUsage(vk::BufferUsageFlagBits::eTransferSrc);

	vma::AllocationCreateInfo stagingAlloc{};
	stagingAlloc.setUsage(vma::MemoryUsage::eAutoPreferHost)
	            .setFlags(vma::AllocationCreateFlagBits::eHostAccessSequentialWrite
	                    | vma::AllocationCreateFlagBits::eMapped);

	vma::AllocationInfo stagingMapInfo{};
	vma::raii::Buffer staging(allocator, stagingInfo, stagingAlloc, &stagingMapInfo);
	std::memcpy(stagingMapInfo.pMappedData, img.pixels.data(), static_cast<std::size_t>(imageSize));

	// GPU 图像 (VMA-Hpp)
	vk::ImageCreateInfo imgInfo{};
	imgInfo.setImageType(vk::ImageType::e2D)
	       .setFormat(vk::Format::eR8G8B8A8Srgb)
	       .setExtent({out.width, out.height, 1})
	       .setMipLevels(1).setArrayLayers(1)
	       .setSamples(vk::SampleCountFlagBits::e1)
	       .setTiling(vk::ImageTiling::eOptimal)
	       .setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
	       .setInitialLayout(vk::ImageLayout::eUndefined);

	vma::AllocationCreateInfo imgAlloc{};
	imgAlloc.setUsage(vma::MemoryUsage::eAutoPreferDevice)
	        .setFlags(vma::AllocationCreateFlagBits::eDedicatedMemory);

	vma::raii::Image gpuImage(allocator, imgInfo, imgAlloc);

	// 录制上传命令
	vk::CommandPoolCreateInfo poolInfo{};
	poolInfo.setQueueFamilyIndex(ctx.GetQueueFamilyIndices().graphicsFamily.value());
	vk::raii::CommandPool cmdPool(dev, poolInfo);
	vk::CommandBufferAllocateInfo cmdAlloc{};
	cmdAlloc.setCommandPool(*cmdPool).setLevel(vk::CommandBufferLevel::ePrimary).setCommandBufferCount(1);
	vk::raii::CommandBuffers cmds(dev, cmdAlloc);
	auto &cmd = cmds[0];
	vk::CommandBufferBeginInfo beginInfo{};
	beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	cmd.begin(beginInfo);

	vk::ImageMemoryBarrier2 toTransfer{};
	toTransfer.setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe).setSrcAccessMask({})
	          .setDstStageMask(vk::PipelineStageFlagBits2::eTransfer)
	          .setDstAccessMask(vk::AccessFlagBits2::eTransferWrite)
	          .setOldLayout(vk::ImageLayout::eUndefined)
	          .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
	          .setImage(*gpuImage)
	          .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
	vk::DependencyInfo dep1{};
	dep1.setImageMemoryBarrierCount(1).setPImageMemoryBarriers(&toTransfer);
	cmd.pipelineBarrier2(dep1);

	vk::BufferImageCopy region{};
	region.setImageSubresource({vk::ImageAspectFlagBits::eColor, 0, 0, 1})
	      .setImageExtent({out.width, out.height, 1});
	cmd.copyBufferToImage(*staging, *gpuImage, vk::ImageLayout::eTransferDstOptimal, region);

	vk::ImageMemoryBarrier2 toShader{};
	toShader.setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
	        .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
	        .setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
	        .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
	        .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
	        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
	        .setImage(*gpuImage)
	        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
	vk::DependencyInfo dep2{};
	dep2.setImageMemoryBarrierCount(1).setPImageMemoryBarriers(&toShader);
	cmd.pipelineBarrier2(dep2);

	SubmitOnce(ctx, cmd, cmdPool, ctx.GetGraphicsQueue());

	// ImageView
	vk::ImageViewCreateInfo viewInfo{};
	viewInfo.setViewType(vk::ImageViewType::e2D).setFormat(vk::Format::eR8G8B8A8Srgb)
	        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
	viewInfo.setImage(*gpuImage);
	out.view = vk::raii::ImageView(dev, viewInfo);

	// Sampler
	vk::SamplerCreateInfo samplerInfo{};
							// 最近采样
	samplerInfo.setMagFilter(vk::Filter::eNearest).setMinFilter(vk::Filter::eNearest)
	           .setMipmapMode(vk::SamplerMipmapMode::eNearest)
	           .setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
	           .setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
	           .setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
	           .setAnisotropyEnable(vk::False).setMaxAnisotropy(1.0f)
	           .setMinLod(0.0f).setMaxLod(0.0f);
	out.sampler = vk::raii::Sampler(dev, samplerInfo);

	out.image = std::move(gpuImage);
	return true;
}

bool CreateTextureRGBA8(VKContext &ctx, std::uint32_t width, std::uint32_t height,
                        const void *pixels, VKTexture &out)
{
    out.width = width;
    out.height = height;
    vk::DeviceSize imageSize = static_cast<vk::DeviceSize>(width) * height * 4;

    auto &dev = ctx.GetDevice();
    auto& allocator = ctx.GetAllocator();

    vk::BufferCreateInfo stagingInfo{};
    stagingInfo.setSize(imageSize)
               .setUsage(vk::BufferUsageFlagBits::eTransferSrc);

    vma::AllocationCreateInfo stagingAlloc{};
    stagingAlloc.setUsage(vma::MemoryUsage::eAutoPreferHost)
                .setFlags(vma::AllocationCreateFlagBits::eHostAccessSequentialWrite
                        | vma::AllocationCreateFlagBits::eMapped);

    vma::AllocationInfo stagingMapInfo{};
    vma::raii::Buffer staging(allocator, stagingInfo, stagingAlloc, &stagingMapInfo);
    std::memcpy(stagingMapInfo.pMappedData, pixels, static_cast<std::size_t>(imageSize));

    vk::ImageCreateInfo imgInfo{};
    imgInfo.setImageType(vk::ImageType::e2D)
           .setFormat(vk::Format::eR8G8B8A8Srgb)
           .setExtent({out.width, out.height, 1})
           .setMipLevels(1).setArrayLayers(1)
           .setSamples(vk::SampleCountFlagBits::e1)
           .setTiling(vk::ImageTiling::eOptimal)
           .setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
           .setInitialLayout(vk::ImageLayout::eUndefined);

    vma::AllocationCreateInfo imgAlloc{};
    imgAlloc.setUsage(vma::MemoryUsage::eAutoPreferDevice)
            .setFlags(vma::AllocationCreateFlagBits::eDedicatedMemory);

    vma::raii::Image gpuImage(allocator, imgInfo, imgAlloc);

    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.setQueueFamilyIndex(ctx.GetQueueFamilyIndices().graphicsFamily.value());
    vk::raii::CommandPool cmdPool(dev, poolInfo);
    vk::CommandBufferAllocateInfo cmdAlloc{};
    cmdAlloc.setCommandPool(*cmdPool).setLevel(vk::CommandBufferLevel::ePrimary).setCommandBufferCount(1);
    vk::raii::CommandBuffers cmds(dev, cmdAlloc);
    auto &cmd = cmds[0];
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmd.begin(beginInfo);

    vk::ImageMemoryBarrier2 toTransfer{};
    toTransfer.setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe).setSrcAccessMask({})
              .setDstStageMask(vk::PipelineStageFlagBits2::eTransfer)
              .setDstAccessMask(vk::AccessFlagBits2::eTransferWrite)
              .setOldLayout(vk::ImageLayout::eUndefined)
              .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
              .setImage(*gpuImage)
              .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    vk::DependencyInfo dep1{};
    dep1.setImageMemoryBarrierCount(1).setPImageMemoryBarriers(&toTransfer);
    cmd.pipelineBarrier2(dep1);

    vk::BufferImageCopy region{};
    region.setImageSubresource({vk::ImageAspectFlagBits::eColor, 0, 0, 1})
          .setImageExtent({out.width, out.height, 1});
    cmd.copyBufferToImage(*staging, *gpuImage, vk::ImageLayout::eTransferDstOptimal, region);

    vk::ImageMemoryBarrier2 toShader{};
    toShader.setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
            .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
            .setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
            .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
            .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
            .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setImage(*gpuImage)
            .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    vk::DependencyInfo dep2{};
    dep2.setImageMemoryBarrierCount(1).setPImageMemoryBarriers(&toShader);
    cmd.pipelineBarrier2(dep2);

    SubmitOnce(ctx, cmd, cmdPool, ctx.GetGraphicsQueue());

    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.setViewType(vk::ImageViewType::e2D).setFormat(vk::Format::eR8G8B8A8Srgb)
            .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    viewInfo.setImage(*gpuImage);
    out.view = vk::raii::ImageView(dev, viewInfo);

    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.setMagFilter(vk::Filter::eNearest).setMinFilter(vk::Filter::eNearest)
               .setMipmapMode(vk::SamplerMipmapMode::eNearest)
               .setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
               .setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
               .setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
               .setAnisotropyEnable(vk::False).setMaxAnisotropy(1.0f)
               .setMinLod(0.0f).setMaxLod(0.0f);
    out.sampler = vk::raii::Sampler(dev, samplerInfo);

    out.image = std::move(gpuImage);
    return true;
}

} // namespace Crumb::Render
