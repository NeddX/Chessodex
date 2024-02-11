#include <Codex.h>
#include <filesystem>

#define CODEX_USE_ALL_NAMESPACES() \
	using namespace codex; \
	using namespace codex::events; \
	using namespace codex::fs; \
	using namespace codex::imgui; \
	using namespace codex::mem; \
	using namespace codex::graphics; \
	using namespace codex::reflect;

namespace chx {
	class GameLayer : public codex::Layer 
	{
	private:
		std::filesystem::path m_ApplicationDataPath{};
		std::filesystem::path m_VariableApplicationDataPath{};
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