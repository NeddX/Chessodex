#include "GameLayer.h"

#include "Scripts/SceneManager.h"

namespace chx {
	namespace fs = std::filesystem;
	CODEX_USE_ALL_NAMESPACES()

	void GameLayer::OnAttach()
	{
		// Initialization stuff here.
#ifdef CX_PLATFORM_LINUX
		m_ApplicationDataPath = fs::path(CHX_INSTALL_DIR) / fs::path("share/Chessodex");
#elif defined(CX_PLATFORM_WINDOWS)
		m_ApplicationDataPath = fs::path(CHX_INSTALL_DIR) / fs::path("bin");
#endif

		m_VariableApplicationDataPath = fs::path(GetSpecialFolder(SpecialFolder::UserApplicationData)) / fs::path("Chessodex/");
		
		if (!fs::exists(m_VariableApplicationDataPath))
		{
			try
			{
				fs::create_directory(m_VariableApplicationDataPath);
			}
			catch (const std::exception& ex)
			{
				fmt::println("This should have not happened but it did.");
			}
		}

		//fmt::println("Application data path: {}", m_ApplicationDataPath.string());
		//fmt::println("Variable application data path: {}", m_VariableApplicationDataPath.string());

		// ImGui init.
		auto& io = ImGui::GetIO();
		static auto ini_file_path = (m_VariableApplicationDataPath / "imgui.ini").string();
		static auto font_file_path = (m_ApplicationDataPath / "Fonts/roboto/Roboto-Regular.ttf").string();
		
		if (!fs::exists(ini_file_path))
		{
			try
			{
				fs::copy_file(m_ApplicationDataPath / "imgui.ini", m_VariableApplicationDataPath / "imgui.ini");
			}
			catch (const std::exception& ex)
			{
				fmt::println("Errors, warnings n stuff...");
			}
		}

		f32 font_size = 14.0f;
		io.IniFilename = ini_file_path.c_str();
		io.FontDefault = io.Fonts->AddFontFromFileTTF(font_file_path.c_str(), font_size);

		// Renderer init.
		const auto width = Application::GetWindow().GetWidth();
		const auto height = Application::GetWindow().GetHeight();
		m_BatchShader = Resources::Load<Shader>(m_ApplicationDataPath / "GL Shaders/batchRenderer.glsl");
		m_BatchShader->CompileShader({ { "CX_MAX_SLOT_COUNT", "16" } });
		m_Camera = Box<Camera>::New(width, height);

		Renderer::Init(width, height);
		BatchRenderer2D::BindShader(m_BatchShader.get());

		mgl::FrameBufferProperties props;
		props.attachments = { mgl::TextureFormat::RGBA8, mgl::TextureFormat::RedInt32 };
		props.width = 1920;
		props.height = 1080;
		m_Framebuffer = Box<mgl::FrameBuffer>::New(props);

		m_Scene = Box<Scene>::New();

		// Create the scene manager entity.
		m_SceneManager = m_Scene->CreateEntity("scene_manager");
		m_SceneManager.AddComponent<NativeBehaviourComponent>().Attach();
	}

	void GameLayer::OnDetach()
	{
		BatchRenderer2D::Destroy();
		Renderer::Destroy();
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

		m_Framebuffer->Unbind();
	}

	void GameLayer::ImGuiRender()
	{
		// Draw ImGUI widgets here.
		auto& io = ImGui::GetIO();

		// Enable docking for the main window.
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		
		// Chessboard
		{
			ImGui::Begin("Chessboard");
			static ImVec2 viewport_window_size{};
			ImVec2 current_viewport_window_size = ImGui::GetContentRegionAvail();
			if (viewport_window_size.x != current_viewport_window_size.x ||
				viewport_window_size.y != current_viewport_window_size.y)
			{
				viewport_window_size = current_viewport_window_size;
				m_Camera->SetWidth((i32)viewport_window_size.x);
				m_Camera->SetHeight((i32)viewport_window_size.y);
			}
			ImGui::Image((void*)(intptr)m_Framebuffer->GetColourAttachmentIdAt(0), current_viewport_window_size, { 0, 1 }, { 1, 0 });
			ImGui::End();
		}

		// Statistics
		{
			ImGui::Begin("Statistics");
			ImGui::Text("Statistics and stuff");
			ImGui::End();
		}
	}

	void GameLayer::OnEvent(Event& e)
	{
		// Dispatch events here.
		fmt::println("Event: {}", e);
	}
} // namespace chx