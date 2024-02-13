#pragma once 

#include <Codex.h>
#include <filesystem>
#include <CommonDef.h>

namespace chx {
	class GameLayer : public codex::Layer 
	{
	private:
		codex::ResRef<codex::graphics::Shader> m_BatchShader{};
		codex::mem::Box<codex::Camera> m_Camera{};
		codex::mem::Box<mgl::FrameBuffer> m_Framebuffer{};
		codex::mem::Box<codex::Scene> m_Scene{};
		codex::Entity m_SceneManager{};

	public:
		void OnAttach() override;
		void OnDetach() override;
		void Update(const codex::f32 deltaTime) override;
		void ImGuiRender() override;

	public:
		void OnEvent(codex::events::Event& e) override;
	};
} // namespace chx