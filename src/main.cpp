#include <Engine/Core/EntryPoint.h>
#include <iostream>

#include "Game/GameLayer.h"

#include "ChessodexApp.h"

namespace chx {
    namespace fs = std::filesystem;

    fs::path Chessodex::m_AppDataPath{};
    fs::path Chessodex::m_VarAppDataPath{};

    void Chessodex::Init()
    {
#ifdef CX_PLATFORM_UNIX
        m_AppDataPath = fs::path(CHX_INSTALL_DIR) / fs::path("share/Chessodex");
#elif defined(CX_PLATFORM_WINDOWS)
        m_AppDataPath = fs::path(CHX_INSTALL_DIR) / fs::path("bin");
#endif

        m_VarAppDataPath =
            fs::path(GetSpecialFolder(SpecialFolder::UserApplicationData)) / fs::path("Chessodex/");

        if (!fs::exists(m_VarAppDataPath))
        {
            try
            {
                fs::create_directory(m_VarAppDataPath);
            }
            catch (const std::exception& ex)
            {
                fmt::println("This should have not happened but it did.");
            }
        }

        // Push our layer.
        PushLayer(new chx::GameLayer);
    }
} // namespace chx

codex::Application* codex::CreateApplication(codex::ApplicationCLIArgs args)
{
    return new chx::Chessodex{ codex::ApplicationProperties{
        .name             = "Chessodex",
        .cwd              = "./",
        .args             = std::move(args),
        .windowProperties = { .title    = "Chessodex",
                              .width    = 800,
                              .height   = 520,
                              .frameCap = 60,
                              .flags    = codex::WindowFlags::Visible,
                              .vsync    = false } } };
}
