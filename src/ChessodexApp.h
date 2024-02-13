#pragma once

#include "Game/GameLayer.h"
#include <Codex.h>
#include <CommonDef.h>
#include <filesystem>

namespace chx {
    CODEX_USE_ALL_NAMESPACES()

    class Chessodex : public codex::Application
	{
    private:
		static std::filesystem::path m_AppDataPath;
        static std::filesystem::path m_VarAppDataPath;

    public:
        using codex::Application::Application;

    public:
        static inline std::filesystem::path GetAppDataPath() noexcept { return m_AppDataPath; }
        static inline std::filesystem::path GetVarAppDataPath() noexcept { return m_VarAppDataPath; }

    public:
        void Init() override;
    };
} // namespace chx
