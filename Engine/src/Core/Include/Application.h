#ifndef APPLICATION_H
#define APPLICATION_H

#include "Core/Include/Core.h"
#include "Core/Include/Logger.h"
#include "Core/Include/Time.h"
#include "Core/Include/Window.h"
#include "Renderer/Include/Camera.h"
#include "Renderer/Include/Shader.h"
#include "Renderer/Include/Textures.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Moonstone
{

namespace Core
{

class Application
{
public:
    Application();
    virtual ~Application() = default;

    void Run();

    inline static Application &GetApplicationInstance() { return *s_ApplicationInstance; }

    void PushLayer(Layer *layer);
    void PopLayer(Layer *layer);
    void PushOverlay(Layer *overlay);
    void PopOverlay(Layer *overlay);

private:
    void RenderLayers();
    void InitializeImGui();
    void InitializeTestRenderData(
        unsigned *shaderProgram, unsigned *VBO, unsigned *VAO, unsigned *EBO, unsigned *textures);

private:
    bool                    m_Running;
    static Application     *s_ApplicationInstance;
    LayerStack              m_LayerStack;
    std::shared_ptr<Window> m_Window;
    Tools::ImGuiLayer      *m_ImGuiLayer;
};

Application* CreateApplicationInstance();

} // namespace Core

} // namespace Moonstone

#endif // APPLICATION_H
