#ifndef EDITORUI_H
#define EDITORUI_H

#include "Core/Include/LayerStack.h"
#include "Core/Include/Window.h"
#include "Core/Layers/Include/BaseLayers.h"
#include "Tools/ImGui/Include/ImGuiLayer.h"
#include "imgui.h"

namespace Moonstone
{

namespace Core
{

class EditorUI
{
  public:
    EditorUI() = default;

    ~EditorUI();

    void PushLayer(std::shared_ptr<Layer> layer);
    void PopLayer(std::shared_ptr<Layer> layer);
    void PushOverlay(std::shared_ptr<Layer> overlay);
    void PopOverlay(std::shared_ptr<Layer> overlay);

    inline void SetWindow(std::shared_ptr<Window> window)
    {
        m_Window = window;
    }

    inline void SetFramebufferParams(unsigned &FBOTexMap, unsigned &FBShaderID, unsigned &screenQuadVAO)
    {
        m_FBOTextureMap = FBOTexMap;
        m_FBShaderID = FBShaderID;
        m_ScreenQuadVAO = screenQuadVAO;
    }

    inline void SetActiveScene(std::shared_ptr<Rendering::Scene> scene)
    {
        m_ActiveScene = scene;
    }

    void Init();
    void Shutdown();
    void Render();

    inline LayerStack GetLayerStack()
    {
        return m_LayerStack;
    }
    inline std::vector<Layer> GetLayers()
    {
        return m_Layers;
    }

  private:
    LayerStack m_LayerStack;
    std::vector<Layer> m_Layers;
    std::shared_ptr<Window> m_Window;
    std::shared_ptr<Tools::ImGuiLayer> m_ImGuiLayer;
    std::shared_ptr<Rendering::Scene> m_ActiveScene;

    unsigned m_FBOTextureMap, m_FBShaderID, m_ScreenQuadVAO;
};

} // namespace Core

} // namespace Moonstone

#endif // EDITORUI_H
