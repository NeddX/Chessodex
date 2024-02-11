#include <iostream>
#include <Engine/Core/EntryPoint.h>

#include "Game/GameLayer.h"

namespace chx {
	class Chessodex : public codex::Application
	{
	public:
		using codex::Application::Application;

	public:
		void Init() override { PushLayer(new chx::GameLayer); }
	};
} // namespace chx

codex::Application* codex::CreateApplication(codex::ApplicationCLIArgs args)
{
	return new chx::Chessodex{ codex::ApplicationProperties 
		{
		.name = "Chessodex",
		.cwd = "./",
		.args = std::move(args),
		.windowProperties = {
			.width = 800,
			.height = 600,
			.frameCap = 300,
			.flags = codex::WindowFlags::Visible | codex::WindowFlags::Resizable,
			.vsync = false
		}
		} };
}