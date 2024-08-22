#include "Include/RendererCommand.h"
#include "Renderer/Platform/OpenGL/Include/OpenGLRendererAPI.h"
#include "Renderer/Platform/Vulkan/Include/VulkanRendererAPI.h"

namespace Moonstone
{

namespace Renderer
{

#ifdef MS_OPENGL
RendererAPI* RendererCommand::s_RendererAPI = new OpenGLRendererAPI;
#endif

#ifdef MS_VULKAN
RendererAPI* RendererCommand::s_RendererAPI = new VulkanRendererAPI;
#endif

} // namespace Renderer

} // namespace Moonstone
