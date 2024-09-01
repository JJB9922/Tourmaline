#ifndef APPLICATION_H
#define APPLICATION_H

#include "Core/Include/Core.h"
#include "Core/Include/Logger.h"
#include "Core/Include/Window.h"
#include "Rendering/Include/Camera.h"
#include "Rendering/Include/Model.h"
#include "Rendering/Include/SceneManager.h"
#include "Rendering/Include/Shader.h"
#include "Rendering/Include/Textures.h"
#include "Tools/Include/BaseShapes.h"

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

        inline static std::unique_ptr<Application> &GetApplicationInstance() { return s_ApplicationInstance; }

        void PushLayer(Layer *layer);
        void PopLayer(Layer *layer);
        void PushOverlay(Layer *overlay);
        void PopOverlay(Layer *overlay);

        // FIX inline void ToggleSunlight() { m_SunLight = !m_SunLight; }

    private:
        void UpdateUILayers();

        void InitializeFramebuffer();
        void InitializeCamera();
        void UpdateCamera();
        void UpdateGrid(Rendering::Shader &gridShader);
        void UpdateModels(Rendering::Shader &meshShader, Rendering::Model &model);

        void UpdateCustomBaseShapes();

        void InitializeImGui();
        void InitializeDefaultScene();

        void AddCube();

    private:
        static std::unique_ptr<Application> s_ApplicationInstance;

        std::shared_ptr<Rendering::Scene> m_CurrentScene;
        Rendering::SceneManager           m_SceneManager;

        bool m_Running;

        //Rendering::Camera m_Camera;
        LayerStack m_LayerStack;

        std::shared_ptr<Rendering::Camera>        m_ActiveCamera;
        std::shared_ptr<Window>  m_Window;
        Tools::ImGuiLayer                        *m_ImGuiLayer;
        std::vector<Layer>                        m_Layers;
        bool                     m_DefaultGrid = true;
        std::vector<Rendering::SceneObject>       m_Objects;
        bool                     m_SunLight  = false;
        glm::vec3                m_TimeOfDay = {-1.0f, -0.5f, 0};

        std::vector<unsigned> m_ShaderProgram, m_VBO, m_VAO, m_EBO, m_Texture;
        unsigned              m_FBShaderID, m_FBO, m_FBOTextureMap, m_FBODepthTexture, m_ScreenQuadVAO, m_ScreenQuadVBO;
};

std::unique_ptr<Application> CreateApplicationInstance();

} // namespace Core

} // namespace Moonstone

#endif // APPLICATION_H
