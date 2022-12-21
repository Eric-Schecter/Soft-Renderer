#pragma once

// refer to https://gpuopen.com/learn/understanding-vulkan-objects/

//#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <GLFW/glfw3native.h>
#include <vulkan/vulkan.hpp>

#include <optional>

#include "Vertex.h"

//#define NDEBUG 1;

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	std::optional<uint32_t> transferFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentModes;
};

class Renderer
{
public:
	Renderer(GLFWwindow* _window);
	~Renderer();

	void render();
	void waitIdle() const;

	// set resize mark for the case of aquiring next image may not work
	bool framebufferResized = false;

private:
	#ifdef NDEBUG
		const bool enableValidationLayers = false;
	#else
		const bool enableValidationLayers = true;
	#endif

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	GLFWwindow* window;

	// 1. setup instasnce
	// first object to create, only one in app, specify all layers and extensions
	// start point to get physicsal device
	vk::UniqueInstance instance;
	void createInstance();

	// 2. setup validation layer
	VkDebugUtilsMessengerEXT callback;
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator);
	void setupDebugCallback();

	// 3. setup physical device
	// basically a graphics card
	// able to enumerate Queue Families, like graphics, compute, transfer
	// able to get the best one by different judgements
	vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
	QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
	bool isDeviceSuitable(const vk::PhysicalDevice& device);
	void pickPhysicalDevice();

	// 4. setup logical device and create queue
	// start point to create other objects
	vk::UniqueDevice device;

	// container of commands to be exectued on device
	// GPU action will be excuted by filling CommandBuffers and submitting to queues
	// multiple queue is supported, like graphics queue and compute queue
	vk::Queue graphicsQueue;
	vk::Queue presentQueue;
	vk::Queue transferQueue;
	void createLogicalDevice();

	// 5. create surface
	vk::SurfaceKHR surface;
	void createSurface();

	// 6. create swap chain
	// need to check extension support
	// need to check capabilities, formats and presentModes
	vk::SwapchainKHR swapChain;
	std::vector<vk::Image> swapChainImages;
	vk::Format swapChainImageFormat;
	vk::Extent2D swapChainExtent;

	bool checkDeviceExtensionSupport(vk::PhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device);

	// three settings to be checked
	// contain format and colorSpace
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats);
	// four options
	// vk::PresentModeKHR::eFifo - first in first out
	// vk::PresentModeKHR::eImmediate - transfer to screen right away, may cause tearing
	// vk::PresentModeKHR::eFifoRelaxed
	// vk::PresentModeKHR::eMailbox - replace the old ones with the new one if the queue is not empty
	// eMailbox should be the best, choose fifo if energy need to be considered
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& presentModes);
	// get width and height from glfw window
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

	void createSwapChain();

	// 7. create image view
	// define how to get data from image
	std::vector<vk::ImageView> swapChainImageViews;
	void createImageViews();

	// 8. create pipeline
	// two types: compute pipeline and graphics pipeline
	// pipeline should be recreated when settings change
	// (1) create shader module
	// vulkan doesn't support high level shading language GLSL/HLSL, support SPIR-V
	// shader module represent a piece of shader code
	std::vector<char> readFile(const std::string& filename);
	vk::UniqueShaderModule createShaderModule(const std::vector<char>& code);

	// (2) create fixed functionn state
	// vertex input state - defines format of vertex data
	// input assembly state - defines primitives and whether to enable primitive redraw
	// viewport
	// scissor
	// dynamic state - change state without recreate pipeline
	// rasterization state - include normal settings like polygonMode/cullMode/frontFace
	// multisample state 
	// depth stencil state
	// color blend state - alpha test
	
	// (3) create pipeline layout
	// pipeline layout - uniforms
	vk::PipelineLayout pipelineLayout;

	// (4) create renderPass
	// attachments to store render result
	vk::RenderPass renderPass;
	void createRenderPass();

	// (5) create pipeline
	// container of all the infos above
	vk::Pipeline graphicsPipeline;
	void createGraphicsPipeline();

	// 9. create framebuffer
	// specify renderpass and a set of image views
	std::vector<vk::Framebuffer> swapChainFramebuffers;
	void createFramebuffers();

	// 10. create command pool and command buffer
	// allocate comnand buffer
	// need specific queue family index
	vk::CommandPool commandPoolGraphics;
	vk::CommandPool commandPoolTransfer;
	void createCommandPool(uint32_t familyIndex, vk::CommandPool& commandPool);
	void createCommandPools();

	// create renderPassInfo for drawing, including framebuffer/clearColor/dynamicStates
	std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> commandBuffers;
	void createCommandBuffers();

	// 11. create Semaphores and fences
	// for async execution
	// semaphore defines the order of commands
	// fence wait for gpu to complete
	// 
	std::vector<vk::Semaphore> imageAvailableSemaphores; // for wait signal
	std::vector<vk::Semaphore> renderFinishedSemaphores; // for render finish singal
	std::vector<vk::Fence> inFlightFences; // cpu wait for gpu complete
	const int MAX_FRAMES_IN_FLIGHT = 2;
	size_t currentFrame = 0;
	void createSyncObjects();

	// 12. recreate swapchain
	// need to be recreated when window size change
	void cleanupSwapChain();
	void recreateSwapChain();

	// 13. setup vertex
	// create buffer -> allocate memory -> bind buffer and memory -> copy data to buffer
	// for transferring data, the best practice is: cpu data -> eTransferSrc -> eTransferDst, similar to bind buffer and glVertexAttribPointer in OpenGL
	// the difference is the way of reading vertex data, in command buffer, not in vao settings
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	// set loadModel here for temp
	void loadModel(const std::string& path);

	vk::Buffer vertexBuffer;
	vk::DeviceMemory vertexBufferMemory;
	vk::Buffer indexBuffer;
	vk::DeviceMemory indexBufferMemory;
	void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);
	void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
	void createVertexBuffer();
	void createIndexBuffer();
};
