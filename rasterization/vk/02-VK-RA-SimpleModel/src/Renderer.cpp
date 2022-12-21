#include "Renderer.h"

#include <iostream>
#include <set>
#include <vector>
#include <limits>
#include <fstream>
#include <algorithm>

Renderer::Renderer(GLFWwindow* _window):window(_window) {
	createInstance();
	setupDebugCallback();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPools();
	createVertexBuffer();
	createIndexBuffer();
	createCommandBuffers();
	createSyncObjects();
}

Renderer::~Renderer() {
	// UniqueInstance destory instance, device and queue

	cleanupSwapChain();

	device->destroyBuffer(vertexBuffer);
	device->freeMemory(vertexBufferMemory);

	device->destroyBuffer(indexBuffer);
	device->freeMemory(indexBufferMemory);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		device->destroySemaphore(renderFinishedSemaphores[i]);
		device->destroySemaphore(imageAvailableSemaphores[i]);
		device->destroyFence(inFlightFences[i]);
	}

	device->destroyCommandPool(commandPoolGraphics);
	device->destroyCommandPool(commandPoolTransfer);

	instance->destroySurfaceKHR(surface);

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(*instance,callback,nullptr);
	}
}

bool Renderer::checkValidationLayerSupport() {
	auto availableLayers = vk::enumerateInstanceLayerProperties();
	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

void Renderer::createInstance() {
	// check validation layer support
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	// setup app info
	vk::ApplicationInfo appInfo = {
		"Renderer",
		VK_MAKE_VERSION(1,0,0),
		"No Engine",
		VK_MAKE_VERSION(1,0,0),
		VK_API_VERSION_1_0
	};

	// get extensions
	auto extensions = getRequiredExtensions();

	// setup createinfo
	std::vector<const char*> emptyVector{};

	vk::InstanceCreateInfo createInfo = {
		vk::InstanceCreateFlags(),
		&appInfo,
		enableValidationLayers ? validationLayers : emptyVector,
		extensions
	};

	// create instance
	try
	{
		instance = vk::createInstanceUnique(createInfo, nullptr);
	}
	catch (const vk::SystemError& error)
	{
		throw std::runtime_error(std::string("failed to create instance! ") + error.what());
	}

	std::cout << "available extensions:\n";
	for (const auto& extension : vk::enumerateInstanceExtensionProperties()) {
		std::cout << "\t" << extension.extensionName << "\n";
	}
}

std::vector<const char*> Renderer::getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << "\n";

	return VK_FALSE;
}

VkResult Renderer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Renderer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, callback, pAllocator);
	}
}

void Renderer::setupDebugCallback() {
	if (!enableValidationLayers) { return; }

	auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
		vk::DebugUtilsMessengerCreateFlagsEXT(),
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
		debugCallback,
		nullptr
	);

	if (CreateDebugUtilsMessengerEXT(*instance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, &callback) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug callback!");
	}
}

QueueFamilyIndices Renderer::findQueueFamilies(vk::PhysicalDevice device) {
	QueueFamilyIndices indices;
		
	auto queueFamilies = device.getQueueFamilyProperties();

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		// get graphics family
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			indices.graphicsFamily = i;
		}

		if (queueFamily.queueCount > 0 && device.getSurfaceSupportKHR(i, surface)) {
			indices.presentFamily = i;
		}

		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eTransfer) {
			indices.transferFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

bool Renderer::isDeviceSuitable(const vk::PhysicalDevice& device) {
	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

void Renderer::pickPhysicalDevice() {
	auto devices = instance->enumeratePhysicalDevices();
	if (devices.size() == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	
	for (const auto& device : devices) {
		// find device that support graphics
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (!physicalDevice) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

void Renderer::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value(),
		indices.transferFamily.value()
	};

	// specify queue info
	float queuePriority = 1.f;

	for (uint32_t queueFamily : uniqueQueueFamilies) {
		queueCreateInfos.push_back({
			vk::DeviceQueueCreateFlags(),
			queueFamily,
			1, // queueCount
			&queuePriority
		});
	}

	// specify device feature 
	vk::PhysicalDeviceFeatures deviceFeatures = {};

	// specify create info
	std::vector<const char*> emptyVector;

	vk::DeviceCreateInfo createInfo = {
		vk::DeviceCreateFlags(),
		queueCreateInfos,
		validationLayers,
		deviceExtensions,
		&deviceFeatures
	};

	try {
		device = physicalDevice.createDeviceUnique(createInfo);
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to create logical device!");
	}

	// create queue
	graphicsQueue = device->getQueue(indices.graphicsFamily.value(), 0);
	presentQueue = device->getQueue(indices.presentFamily.value(), 0);
	transferQueue = device->getQueue(indices.transferFamily.value(), 0);
}

void Renderer::createSurface() {
	VkSurfaceKHR rawSurface;
	if (glfwCreateWindowSurface(*instance, window, nullptr, &rawSurface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
	surface = rawSurface;
}

bool Renderer::checkDeviceExtensionSupport(vk::PhysicalDevice device) {
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
	for (const auto& extension : device.enumerateDeviceExtensionProperties()) {
		requiredExtensions.erase(extension.extensionName);
	}
	return requiredExtensions.empty();
}

SwapChainSupportDetails Renderer::querySwapChainSupport(const vk::PhysicalDevice& device) {
	SwapChainSupportDetails details = {
		device.getSurfaceCapabilitiesKHR(surface),
		device.getSurfaceFormatsKHR(surface),
		device.getSurfacePresentModesKHR(surface)
	};
	return details;
}

vk::SurfaceFormatKHR Renderer::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
	if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
		return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
	}

	// return rgb 8bit and sRGB
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

vk::PresentModeKHR Renderer::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
	vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo; // default

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == vk::PresentModeKHR::eMailbox) { // best
			return availablePresentMode;
		}
		else if (availablePresentMode == vk::PresentModeKHR::eImmediate) { // better
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

vk::Extent2D Renderer::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
		return capabilities.currentExtent;
	}
	else {
		int width;
		int height;
		glfwGetFramebufferSize(window, &width, &height);
		vk::Extent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		actualExtent.width = max(capabilities.minImageExtent.width, min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = max(capabilities.minImageExtent.height, min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void Renderer::createSwapChain() {
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

	vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	// set image count between (min + 1) and max
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	bool isDifferentQueue = indices.graphicsFamily != indices.presentFamily;

	vk::SwapchainCreateInfoKHR createInfo = {
		vk::SwapchainCreateFlagsKHR(),
		surface,
		imageCount,
		surfaceFormat.format,
		surfaceFormat.colorSpace,
		extent,
		1,			// imageArrayLayers
		vk::ImageUsageFlagBits::eColorAttachment,
		isDifferentQueue ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
		queueFamilyIndices,
		swapChainSupport.capabilities.currentTransform, // rotation of image
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		presentMode,
		VK_TRUE,
		vk::SwapchainKHR(nullptr)
	};

	if (!enableValidationLayers) {
		createInfo.pQueueFamilyIndices = nullptr;
	}

	try {
		swapChain = device->createSwapchainKHR(createInfo);
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to create swap chain!");
	}

	swapChainImages = device->getSwapchainImagesKHR(swapChain);
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void Renderer::createImageViews() {
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); ++i) {
		vk::ComponentMapping mapping = {
			vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity
		};
		vk::ImageSubresourceRange subresourceRange = {
			vk::ImageAspectFlagBits::eColor,
			0,		// baseMipLevel
			1,		// levelCount
			0,		// baseArrayLayer
			1		// layerCount
		};
		vk::ImageViewCreateInfo createInfo = {
			vk::ImageViewCreateFlags(),
			swapChainImages[i],
			vk::ImageViewType::e2D, // 1D/2D/3D/cube/array
			swapChainImageFormat,
			mapping,
			subresourceRange
		};

		try {
			swapChainImageViews[i] = device->createImageView(createInfo);
		}
		catch (vk::SystemError err) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}

std::vector<char> Renderer::readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}
	
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

vk::UniqueShaderModule Renderer::createShaderModule(const std::vector<char>& code) {
	try {
		return device->createShaderModuleUnique({
			vk::ShaderModuleCreateFlags(),
			code.size(),
			reinterpret_cast<const uint32_t*>(code.data())
		});
	
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to create shader module!");
	}
}

void Renderer::createRenderPass() {
	vk::AttachmentDescription colorAttachment = {
		vk::AttachmentDescriptionFlags(),
		swapChainImageFormat,
		vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eStore,
		vk::AttachmentLoadOp::eDontCare,
		vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::ePresentSrcKHR
	};

	vk::AttachmentReference colorAttachmentRef = {
		0,
		vk::ImageLayout::eColorAttachmentOptimal
	};

	vk::SubpassDescription subpass = {
		vk::SubpassDescriptionFlags(),
		vk::PipelineBindPoint::eGraphics,
		nullptr,			// input attachments
		colorAttachmentRef,
		nullptr,			// resolve attachments
		nullptr,			// depth stencil attachments
		nullptr				// presesrve attachments
	};

	// create in draw call setion
	vk::SubpassDependency dependency = {
		VK_SUBPASS_EXTERNAL,								 // srcSubpass;
		0,													 // dstSubpass;
		vk::PipelineStageFlagBits::eColorAttachmentOutput,   // srcStageMask;
		vk::PipelineStageFlagBits::eColorAttachmentOutput,	 // dstStageMask;
		{},										             // srcAccessMask;
		vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,   // dstAccessMask;
		vk::DependencyFlags()								 // dependencyFlags;
	};

	vk::RenderPassCreateInfo renderPassInfo = {
		vk::RenderPassCreateFlags(),
		colorAttachment,
		subpass,
		dependency
	};

	try {
		renderPass = device->createRenderPass(renderPassInfo);
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void Renderer::createGraphicsPipeline() {
	// shader module
	auto vertShaderCode = readFile("./shaders/vert.spv");
	auto fragShaderCode = readFile("./shaders/frag.spv");

	auto vertShaderModule = createShaderModule(vertShaderCode);
	auto fragShaderModule = createShaderModule(fragShaderCode);

	vk::PipelineShaderStageCreateInfo shaderStages[] = {
		{
			vk::PipelineShaderStageCreateFlags(),
			vk::ShaderStageFlagBits::eVertex,
			*vertShaderModule,
			"main"
		},
		{
			vk::PipelineShaderStageCreateFlags(),
			vk::ShaderStageFlagBits::eFragment,
			*fragShaderModule,
			"main"
		 }
	};

	// pipeline layout
	vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {
		vk::PipelineLayoutCreateFlags(),
		0,		// setLayoutCount
		0		// pushConstantRangeCount
	};

	try {
		pipelineLayout = device->createPipelineLayout(pipelineLayoutInfo);
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {
		vk::PipelineVertexInputStateCreateFlags(),
		bindingDescription,
		attributeDescriptions
	};

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {
		vk::PipelineInputAssemblyStateCreateFlags(),
		vk::PrimitiveTopology::eTriangleList,
		VK_FALSE		// primitiveRestartEnable
	};

	vk::Viewport viewport = {
		0,											// x
		0,											// y
		static_cast<float>(swapChainExtent.width),	// width
		static_cast<float>(swapChainExtent.height),	// height
		0,											// min depth
		1											// max depth
	};

	vk::Rect2D scissor = {
		{0,0},			 // offset
		swapChainExtent
	};

	vk::PipelineViewportStateCreateInfo viewportState = {
		vk::PipelineViewportStateCreateFlags(),
		viewport,
		scissor
	};

	vk::PipelineRasterizationStateCreateInfo rasterizer = {
		vk::PipelineRasterizationStateCreateFlags(),
		VK_FALSE,		// depthClampEnable
		VK_FALSE,		// rasterizerDiscardEnable
		vk::PolygonMode::eFill,
		vk::CullModeFlagBits::eBack,
		vk::FrontFace::eClockwise,
		VK_FALSE,		// depthBiasEnable
		0.f,			// depthBiasConstantFactor
		0.f,			// depthBiasClamp
		0.f,			// depthBiasSlopeFactor
		1.f				// lineWidth
	};

	vk::PipelineMultisampleStateCreateInfo multisampling = {
		vk::PipelineMultisampleStateCreateFlags(),
		vk::SampleCountFlagBits::e1,
		VK_FALSE
	};

	vk::PipelineColorBlendAttachmentState colorBlendAttachment = {
		VK_FALSE,
		vk::BlendFactor::eZero,    // srcColorBlendFactor;
		vk::BlendFactor::eZero,            // dstColorBlendFactor;
		vk::BlendOp::eAdd,                // colorBlendOp;
		vk::BlendFactor::eZero,            // srcAlphaBlendFactor;
		vk::BlendFactor::eZero,            // dstAlphaBlendFactor;
		vk::BlendOp::eAdd,                 // alphaBlendOp;
		vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
	};
	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	colorBlendAttachment.blendEnable = VK_FALSE;

	vk::PipelineColorBlendStateCreateInfo colorBlending = {
		vk::PipelineColorBlendStateCreateFlags(),
		VK_FALSE,
		vk::LogicOp::eCopy,
		colorBlendAttachment,
		std::array<float,4>{0.f,0.f,0.f,0.f}
	};

	// pipeline 
	vk::GraphicsPipelineCreateInfo pipelineInfo = {
		vk::PipelineCreateFlags(),
		shaderStages,
		&vertexInputInfo,
		&inputAssembly,
		nullptr,			// tessellation
		&viewportState,
		&rasterizer,
		&multisampling,
		nullptr,			// depth stencil
		&colorBlending,
		nullptr,			// dynamic state
		pipelineLayout,
		renderPass,
		0,					// subpass
		nullptr				// base pipeline
	};

	try {
	    graphicsPipeline = device->createGraphicsPipeline(nullptr, pipelineInfo).value;
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}

void Renderer::createFramebuffers() {
	swapChainFramebuffers.resize(swapChainImageViews.size());
	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		vk::ImageView attachments[] = {
			swapChainImageViews[i]
		};
		
		vk::FramebufferCreateInfo framebufferInfo = {
			vk::FramebufferCreateFlags(),
			renderPass,
			attachments,
			swapChainExtent.width,
			swapChainExtent.height,
			1
		};
		
		try {
			swapChainFramebuffers[i] = device->createFramebuffer(framebufferInfo);
		}
		catch (vk::SystemError err) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void Renderer::createCommandPool(uint32_t familyIndex, vk::CommandPool& commandPool) {
	vk::CommandPoolCreateInfo poolInfo = {
		vk::CommandPoolCreateFlags(),
		familyIndex
	};

	try {
		commandPool = device->createCommandPool(poolInfo);
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void Renderer::createCommandPools() {
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);
	createCommandPool(queueFamilyIndices.graphicsFamily.value(), commandPoolGraphics);
	createCommandPool(queueFamilyIndices.transferFamily.value(), commandPoolTransfer);
}

void Renderer::createCommandBuffers() {
	// command count is same with swapchain framebuffers
	commandBuffers.resize(swapChainFramebuffers.size());

	vk::CommandBufferAllocateInfo allocInfo = {
		commandPoolGraphics,
		vk::CommandBufferLevel::ePrimary,
		static_cast<uint32_t>(commandBuffers.size())
	};

	try {
		commandBuffers = device->allocateCommandBuffers(allocInfo);
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	// setup command
	for (size_t i = 0; i < commandBuffers.size(); ++i) {
		vk::CommandBufferBeginInfo beginInfo = {
			vk::CommandBufferUsageFlagBits::eSimultaneousUse
		};
		
		// start command
		try {
			commandBuffers[i].begin(beginInfo);
		}
		catch (vk::SystemError err) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
		
		vk::ClearValue clearColor = { std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 1.0f } };
		vk::Rect2D renderArea{ vk::Offset2D(0,0), swapChainExtent };
		vk::RenderPassBeginInfo renderPassInfo = {
			renderPass,
			swapChainFramebuffers[i],
			renderArea,
			clearColor
		};
		
		// draw call, but not execute
		// set dynamic state here if needed
		commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
		commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

		vk::Buffer vertexBuffers[] = { vertexBuffer };
		vk::DeviceSize offsets[] = { 0 };
		commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);
		commandBuffers[i].bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint16);
		commandBuffers[i].drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

		commandBuffers[i].endRenderPass();
		
		// end commnad
		try {
			commandBuffers[i].end();
		}
		catch (vk::SystemError err) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void Renderer::createSyncObjects() {
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	
	try {
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			imageAvailableSemaphores[i] = device->createSemaphore({});
			renderFinishedSemaphores[i] = device->createSemaphore({});
			inFlightFences[i] = device->createFence({ vk::FenceCreateFlagBits::eSignaled }); // return immediately when first called
		}
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to create synchronization objects for a frame!");
	}
}

void Renderer::render(const std::vector<Mesh>& scene) {
	Mesh mesh = scene[0];

	// submit is async, cpu needs to wait for gpu's result
	// every frame has two semaphore(wait singal and render singal) and one fence
	// here are two fences, the next next render may be stucked if the gpu doesn't complete
	// the present queue will wait for graphics queue by the help of ssemaphores
	// wait for last frame
	device->waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, (std::numeric_limits<uint64_t>::max)());
	
	// get image to render
	// swap chain may not be suitable
	uint32_t imageIndex;
	try {
		vk::ResultValue result = device->acquireNextImageKHR(
			swapChain, 
			(std::numeric_limits<uint64_t>::max)(),
			imageAvailableSemaphores[currentFrame],
			nullptr
		);
		imageIndex = result.value;
	}
	catch (vk::OutOfDateKHRError err) { // swap chain size is different from window size
		recreateSwapChain();
		return;
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// reset fence, after ensure swap chain is suitable
	device->resetFences(1, &inFlightFences[currentFrame]);
	
	// submit graphic queue
	vk::Semaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	vk::Semaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	vk::SubmitInfo submitInfo = {
		waitSemaphores,
		waitStages,
		commandBuffers[imageIndex],
		signalSemaphores
	};
	
	try {
		graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}
	
	// present result
	vk::SwapchainKHR swapChains[] = { swapChain };
	vk::PresentInfoKHR presentInfo = {
		signalSemaphores,
		swapChains,
		imageIndex,
		nullptr
	};
	
	vk::Result resultPresent;
	try {
		resultPresent = presentQueue.presentKHR(presentInfo);
	}
	catch (vk::OutOfDateKHRError err) {
		resultPresent = vk::Result::eErrorOutOfDateKHR;
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	if (resultPresent == vk::Result::eSuboptimalKHR || framebufferResized) {
		std::cout << "swap chain out of date/suboptimal/window resized - recreating" << std::endl;
		framebufferResized = false;
		recreateSwapChain();
		return;
	}
	
	// update frame index
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::cleanupSwapChain() {
	for (auto framebuffer : swapChainFramebuffers) {
		device->destroyFramebuffer(framebuffer);
	}

	device->freeCommandBuffers(commandPoolGraphics, commandBuffers);

	device->destroyPipeline(graphicsPipeline);
	device->destroyPipelineLayout(pipelineLayout);
	device->destroyRenderPass(renderPass);

	for (auto imageView : swapChainImageViews) {
		device->destroyImageView(imageView);
	}

	device->destroySwapchainKHR(swapChain);
}

void Renderer::recreateSwapChain() {
	int width = 0, height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	device->waitIdle();

	cleanupSwapChain();

	// after logical device, command pool and sync objects donot need to recreate
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandBuffers();
}

void Renderer::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size) {
	// get command buffer from command pool
	vk::CommandBufferAllocateInfo allocInfo = {
		commandPoolTransfer,
		vk::CommandBufferLevel::ePrimary,
		1
	};
	
	vk::CommandBuffer commandBuffer = device->allocateCommandBuffers(allocInfo)[0];

	// build command
	vk::CommandBufferBeginInfo beginInfo = {
		vk::CommandBufferUsageFlagBits::eOneTimeSubmit
	};
	
	commandBuffer.begin(beginInfo);
	
	vk::BufferCopy copyRegion = {
		0,
		0,
		size
	};
	commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);
	
	commandBuffer.end();
	
	vk::SubmitInfo submitInfo = {
		{},			// waitSemaphores,
		{},			// waitStages,
		commandBuffer,
		{}			// signalSemaphores
	};

	transferQueue.submit(submitInfo, nullptr);

	// another option is fence, transfer multiple data at a time
	transferQueue.waitIdle();

	device->freeCommandBuffers(commandPoolTransfer, commandBuffer);
}

void Renderer::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory) {
	// create vertex buffer
	vk::BufferCreateInfo bufferInfo = {
		vk::BufferCreateFlags(),
		size,
		usage,
		vk::SharingMode::eExclusive
	};

	try {
		buffer = device->createBuffer(bufferInfo);
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to create vertex buffer!");
	}

	// allocate memory
	vk::MemoryRequirements memRequirements = device->getBufferMemoryRequirements(buffer);

	vk::MemoryAllocateInfo allocInfo = {
		memRequirements.size,
		findMemoryType(memRequirements.memoryTypeBits,properties)
	};

	try {
		bufferMemory = device->allocateMemory(allocInfo);
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	// bind buffer and memory
	device->bindBufferMemory(buffer, bufferMemory, 0);
}

uint32_t Renderer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
	vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type!");
}

void Renderer::createVertexBuffer() {
	// staging buffer is temp buffer
	// vertex data -> staging buffer -> vertex buffer

	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;

	// src buffer, create and bind
	createBuffer(
		bufferSize,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		stagingBuffer,
		stagingBufferMemory
	);
		
	// copy data to buffer, similar to buffer data in OpenGL
	void* data = device->mapMemory(stagingBufferMemory, 0, bufferSize);
	memcpy(data, vertices.data(), bufferSize);
	device->unmapMemory(stagingBufferMemory);

	// dst buffer, create and bind
	createBuffer(
		bufferSize,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal,  // can not used by cpu
		vertexBuffer,
		vertexBufferMemory
	);

	// similar to glEnableVertexAttribArray in OpenGL
	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
	
	// release temo buffer
	device->destroyBuffer(stagingBuffer);
	device->freeMemory(stagingBufferMemory);
}

void Renderer::createIndexBuffer() {
	// staging buffer is temp buffer
	// vertex data -> staging buffer -> vertex buffer

	vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;

	// src buffer, create and bind
	createBuffer(
		bufferSize,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		stagingBuffer,
		stagingBufferMemory
	);

	// copy data to buffer, similar to buffer data in OpenGL
	void* data = device->mapMemory(stagingBufferMemory, 0, bufferSize);
	memcpy(data, indices.data(), bufferSize);
	device->unmapMemory(stagingBufferMemory);

	// dst buffer, create and bind
	createBuffer(
		bufferSize,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal,  // can not used by cpu
		indexBuffer,
		indexBufferMemory
	);

	// similar to glEnableVertexAttribArray in OpenGL
	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	// release temo buffer
	device->destroyBuffer(stagingBuffer);
	device->freeMemory(stagingBufferMemory);
}

void Renderer::waitIdle() const {
	return device->waitIdle();
}