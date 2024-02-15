#include "GameLayer.h"

#include "Scripts/SceneManager.h"

#include <ChessodexApp.h>

namespace chx {
    namespace fs = std::filesystem;
    CODEX_USE_ALL_NAMESPACES()

    codex::ResRef<codex::graphics::Shader> GameLayer::m_BatchShader{};
    codex::mem::Box<codex::Camera>         GameLayer::m_Camera{};
    codex::mem::Box<mgl::FrameBuffer>      GameLayer::m_Framebuffer{};
    codex::mem::Box<codex::Scene>          GameLayer::m_Scene{};
    codex::mem::Box<SceneManager>          GameLayer::m_SceneManager{};
    std::array<codex::Vector2f, 2>         GameLayer::m_ViewportBounds;

    mgl::FrameBuffer& GameLayer::GetPrimaryFrameBuffer() noexcept
    {
        return *m_Framebuffer;
    }

    std::array<Vector2f, 2>& GameLayer::GetViewportBounds() noexcept
    {
        return m_ViewportBounds;
    }

    void GameLayer::OnAttach()
    {
        // Initialization stuff here.
        auto&       io             = ImGui::GetIO();
        static auto ini_file_path  = (Chessodex::GetVarAppDataPath() / "imgui.ini").string();
        static auto font_file_path = (Chessodex::GetAppDataPath() / "Fonts/roboto/Roboto-Regular.ttf").string();

        if (!fs::exists(ini_file_path))
        {
            try
            {
                fs::copy_file(Chessodex::GetAppDataPath() / "imgui.ini", Chessodex::GetVarAppDataPath() / "imgui.ini");
            }
            catch (const std::exception& ex)
            {
                fmt::println("Errors, warnings n stuff...");
            }
        }

        f32 font_size  = 14.0f;
        io.IniFilename = ini_file_path.c_str();
        io.FontDefault = io.Fonts->AddFontFromFileTTF(font_file_path.c_str(), font_size);

        // Renderer init.
        const auto width  = Application::GetWindow().GetWidth();
        const auto height = Application::GetWindow().GetHeight();
        m_BatchShader     = Resources::Load<Shader>(Chessodex::GetAppDataPath() / "GL Shaders/batchRenderer.glsl");
        m_BatchShader->CompileShader({ { "CX_MAX_SLOT_COUNT", "8" } });
        m_Camera = Box<Camera>::New(width, height);

        Renderer::Init(width, height);
        BatchRenderer2D::BindShader(m_BatchShader.get());

        mgl::FrameBufferProperties props;
        props.attachments = { mgl::TextureFormat::RGBA8, mgl::TextureFormat::RedInt32 };
        props.width       = 1920;
        props.height      = 1080;
        m_Framebuffer     = Box<mgl::FrameBuffer>::New(props);

        m_Scene = Box<Scene>::New();

        // Create the scene manager.
        m_SceneManager = Box<SceneManager>::New();
        m_SceneManager->Init();
    }

    void GameLayer::OnDetach()
    {
        BatchRenderer2D::Destroy();
        Renderer::Destroy();
    }

    Scene& GameLayer::GetCurrentScene() noexcept
    {
        return *m_Scene;
    }

    void GameLayer::Update(const f32 deltaTime)
    {
        // Logic update here.
        m_BatchShader->Bind();
        m_BatchShader->SetUniformMat4f("u_View", m_Camera->GetViewMatrix());
        m_BatchShader->SetUniformMat4f("u_Proj", m_Camera->GetProjectionMatrix());

        m_Framebuffer->Bind();
        Renderer::SetClearColour(0.2f, 0.2f, 0.2f, 1.0f);
        Renderer::Clear();

        // Render here.
        BatchRenderer2D::Begin();
        m_Scene->Update(deltaTime);
        BatchRenderer2D::End();

        // Update the scene.
        m_SceneManager->Update(deltaTime);

        m_Framebuffer->Unbind();
    }

    void GameLayer::ImGuiRender()
    {
        // Draw ImGUI widgets here.
        auto& io = ImGui::GetIO();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // Chessboard window
        {
            ImGui::Begin("Chessboard");
            const auto viewport_min_region = ImGui::GetWindowContentRegionMin();
            const auto viewport_max_region = ImGui::GetWindowContentRegionMax();
            const auto viewport_offset     = ImGui::GetWindowPos();
            m_ViewportBounds[0]            = { viewport_min_region.x + viewport_offset.x,
                                               viewport_min_region.y + viewport_offset.y };
            m_ViewportBounds[1]            = { viewport_max_region.x + viewport_offset.x,
                                               viewport_max_region.y + viewport_offset.y };

            static ImVec2 viewport_window_size;
            ImVec2        current_viewport_window_size = ImGui::GetContentRegionAvail();
            if (viewport_window_size.x != current_viewport_window_size.x ||
                viewport_window_size.y != current_viewport_window_size.y)
            {
                viewport_window_size = current_viewport_window_size;
                m_Camera->SetWidth((i32)viewport_window_size.x);
                m_Camera->SetHeight((i32)viewport_window_size.y);
            }
            ImGui::Image((void*)(intptr)m_Framebuffer->GetColourAttachmentIdAt(0), current_viewport_window_size,
                         { 0, 1 }, { 1, 0 });
            ImGui::End();
        }

        // Statistics window
        {
            ImGui::Begin("Statistics");
            ImGui::Text("Statistics and stuff");
            ImGui::End();
        }

        // Render info window.
        {
            ImGui::Begin("Render Info");
            ImGui::Text("Renderer information");
            ImGui::Text("FPS: %u", Application::GetFps());
            ImGui::Text("Delta time: %f", Application::GetDelta());
            ImGui::Text("Batch count: %zu", BatchRenderer2D::GeBatchCount());
            ImGui::Text("Total quad count: %zu", BatchRenderer2D::GetQuadCount());
            ImGui::End();
        }
    }

    void GameLayer::OnEvent(Event& e)
    {
        // Dispatch events here.
        // fmt::println("Event: {}", e);
    }
} // namespace chx
