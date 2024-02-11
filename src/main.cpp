#include <iostream>
#include <Engine/Core/EntryPoint.h>

class Chessodex : public codex::Application
{
public:
	using codex::Application::Application;
};

codex::Application* codex::CreateApplication(codex::ApplicationCLIArgs args)
{
	return new Chessodex{ codex::ApplicationProperties 
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