#pragma once

#include <Codex.h>
#include <CommonDef.h>
#include <filesystem>
#include "Scripts/SceneManager.h"

namespace chx {
    class GameLayer : public codex::Layer
    {
    private:
        static codex::ResRef<codex::graphics::Shader> m_BatchShader;
        static codex::mem::Box<codex::Camera>         m_Camera;
        static codex::mem::Box<mgl::FrameBuffer>      m_Framebuffer;
        static codex::mem::Box<codex::Scene>          m_Scene;
        static codex::mem::Box<SceneManager>          m_SceneManager;
        static std::array<codex::Vector2f, 2>         m_ViewportBounds;

    public:
        static mgl::FrameBuffer&               GetPrimaryFrameBuffer() noexcept;
        static std::array<codex::Vector2f, 2>& GetViewportBounds() noexcept;
        static codex::Scene&                   GetCurrentScene() noexcept;

    public:
        void OnAttach() override;
        void OnDetach() override;
        void Update(const codex::f32 deltaTime) override;
        void ImGuiRender() override;

    public:
        void OnEvent(codex::events::Event& e) override;
    };
} // namespace chx
