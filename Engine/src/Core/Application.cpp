#include "Include/Application.h"
#include "mspch.h"

namespace Moonstone
{
namespace Core
{

/**
 * @brief Static pointer to the current application instance.
 *
 * This is used to ensure only one instance of the Application class exists.
 */
Application* Application::s_ApplicationInstance = nullptr;

/**
 * @brief Constructs an Application instance.
 *
 * Ensures no other instance of Application exists and sets the static
 * instance pointer to this instance.
 */
Application::Application()
{
    if (s_ApplicationInstance)
    {
        MS_ASSERT(!s_ApplicationInstance, "application instance already exists");
    }

    s_ApplicationInstance = this;
}

/**
 * @brief Runs the application loop.
 *
 * Initializes the window and rendering data, then enters the main loop where
 * it clears the screen, submits draw commands, renders layers, and updates
 * the window until the window should close.
 */
void Application::Run()
{
    m_Running = true;
    m_Window  = std::unique_ptr<Window>(Window::CreateWindow());

    unsigned shaderProgram, VBO, VAO, EBO, texture, texture2;
    InitializeTestRenderData(shaderProgram, VBO, VAO, EBO, texture, texture2);

    std::string      vertexShaderPath   = std::string(RESOURCE_DIR) + "/Shaders/vshader.vs";
    std::string      fragmentShaderPath = std::string(RESOURCE_DIR) + "/Shaders/fshader.fs";
    Renderer::Shader shader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

    shader.Use(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(shader.ID, "texture"), 0);
    Renderer::RendererCommand::SetUniformInt(shader.ID, "texture2", 1);

    InitializeImGui();
    while (m_Running)
    {
        Renderer::RendererCommand::ClearColor(m_Window->m_WindowColor);
        Renderer::RendererCommand::Clear();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        shader.Use();
        Renderer::RendererCommand::SubmitDrawCommands(shaderProgram, VAO, texture, texture2);

        RenderLayers();

        Window::UpdateWindow(m_Window->m_Window);

        if (glfwWindowShouldClose(m_Window->m_Window))
            m_Running = false;
    }

    Renderer::RendererCommand::Cleanup(VAO, VBO, shaderProgram);
}

/**
 * @brief Renders all layers in the stack.
 *
 * Calls `OnUpdate` on each layer, then starts and ends the ImGui layer, and
 * calls `OnImGuiRender` on each layer.
 */
void Application::RenderLayers()
{
    for (Layer* layer : m_LayerStack)
    {
        layer->OnUpdate();
    }

    m_ImGuiLayer->Start();
    for (Layer* layer : m_LayerStack)
    {
        layer->OnImGuiRender();
    }
    m_ImGuiLayer->End();
}

/**
 * @brief Initializes ImGui and adds it to the layer stack.
 *
 * Creates an ImGui layer, sets the window for ImGui, and sets up callback
 * functions for UI elements.
 */
void Application::InitializeImGui()
{
    m_ImGuiLayer = new Tools::ImGuiLayer();
    m_ImGuiLayer->SetWindow(m_Window->m_Window);
    PushOverlay(m_ImGuiLayer);

    auto exampleLayer = new ExampleLayer;
    exampleLayer->SetBtnCallback(ExampleLayer::ButtonID::Exit, [this]() { m_Window->TerminateWindow(); });
    exampleLayer->SetBtnCallback(ExampleLayer::ButtonID::ApplyBGColor,
                                 [this, exampleLayer]()
                                 {
                                     auto color                = exampleLayer->GetBGColor();
                                     m_Window->m_WindowColor.r = color.x;
                                     m_Window->m_WindowColor.g = color.y;
                                     m_Window->m_WindowColor.b = color.z;
                                     m_Window->m_WindowColor.a = color.w;
                                 });
    exampleLayer->SetBtnCallback(ExampleLayer::ButtonID::ToggleWireframe,
                                 [this]()
                                 {
                                     if (m_Window->m_PolygonMode == Renderer::RendererAPI::DataType::PolygonLine)
                                     {
                                         Renderer::RendererCommand::SetPolygonMode(
                                             Renderer::RendererAPI::DataType::PolygonFill);

                                         m_Window->m_PolygonMode = Renderer::RendererAPI::DataType::PolygonFill;
                                     }
                                     else
                                     {
                                         Renderer::RendererCommand::SetPolygonMode(
                                             Renderer::RendererAPI::DataType::PolygonLine);

                                         m_Window->m_PolygonMode = Renderer::RendererAPI::DataType::PolygonLine;
                                     }
                                 });

    PushLayer(exampleLayer);
}

/**
 * @brief Creates an instance of Application.
 *
 * Allocates and returns a new Application object.
 *
 * @return A pointer to the newly created Application instance.
 */
Application* CreateApplicationInstance() { return new Application(); }

/**
 * @brief Adds a layer to the layer stack.
 *
 * Pushes the layer onto the layer stack and calls `OnAttach` on it.
 *
 * @param layer A pointer to the Layer to add.
 */
void Application::PushLayer(Layer* layer)
{
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}

/**
 * @brief Removes a layer from the layer stack.
 *
 * Removes the specified layer from the stack.
 *
 * @param layer A pointer to the Layer to remove.
 */
void Application::PopLayer(Layer* layer) { m_LayerStack.PopLayer(layer); }

/**
 * @brief Adds an overlay layer to the layer stack.
 *
 * Pushes the overlay onto the layer stack and calls `OnAttach` on it.
 *
 * @param layer A pointer to the Layer to add as an overlay.
 */
void Application::PushOverlay(Layer* layer)
{
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}

/**
 * @brief Removes an overlay layer from the layer stack.
 *
 * Removes the specified overlay from the stack.
 *
 * @param overlay A pointer to the Layer to remove as an overlay.
 */
void Application::PopOverlay(Layer* overlay) { m_LayerStack.PopOverlay(overlay); }

/**
 * @brief Initializes test rendering data.
 *
 * Sets up shaders, vertex arrays, buffers, and attributes for rendering.
 *
 * @param shaderProgram A reference to an unsigned integer to store the shader program ID.
 * @param VBO A reference to an unsigned integer to store the vertex buffer object ID.
 * @param VAO A reference to an unsigned integer to store the vertex array object ID.
 * @param EBO A reference to an unsigned integer to store the element buffer object ID.
 */
void Application::InitializeTestRenderData(
    unsigned& shaderProgram, unsigned& VBO, unsigned& VAO, unsigned& EBO, unsigned& texture, unsigned& texture2)
{
    float vertices[] = {0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,

                        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

                        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

                        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    unsigned indices[] = {0,
                          1,
                          2,

                          0,
                          2,
                          3};

    float texCoords[] = {0.0f,
                         0.0f,

                         1.0f,
                         0.0f,

                         0.5f,
                         1.0f};

    Renderer::RendererCommand::InitVertexArray(VAO);
    Renderer::RendererCommand::InitVertexBuffer(VBO, vertices, sizeof(vertices));
    Renderer::RendererCommand::InitElementBuffer(EBO, indices, sizeof(indices));
    Renderer::RendererCommand::InitVertexAttributes();

    std::string texturePath = std::string(RESOURCE_DIR) + "/Textures/container.jpg";

    std::string texture2Path = std::string(RESOURCE_DIR) + "/Textures/geeble.png";

    int            width, height, nrChannels;
    unsigned char* texData = Renderer::Textures::LoadTexture(texturePath.c_str(), width, height, nrChannels);

    Renderer::RendererCommand::CreateTexture(texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(texData);

    Renderer::RendererCommand::CreateTexture(texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    unsigned char* texData2 = Renderer::Textures::LoadTexture(texture2Path.c_str(), width, height, nrChannels);
    if (texData2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(texData2);
}

} // namespace Core
} // namespace Moonstone
