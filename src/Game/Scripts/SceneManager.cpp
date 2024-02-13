#include "SceneManager.h"

#include <ChessodexApp.h>
#include <CommonDef.h>

namespace chx {
    CODEX_USE_ALL_NAMESPACES()

    void SceneManager::Init()
    {
        // As of now you cannot render solid colours with Codex
        // so your best bet is to load a solid white image and
        // do whatever is it that you want to do.

        // Load our resources.
        m_Resources["solid_white"] =
            Resources::Load<Texture2D>(Chessodex::GetAppDataPath() / "Sprites/solid_white.png");

        DrawChessboard();
    }

    void SceneManager::Update(const codex::f32 deltaTime)
    {
    }

    void SceneManager::DrawChessboard()
    {
        static unsigned board_size = 8;
        static unsigned tile_size  = 60;

        for (auto y = 0; y < board_size; ++y)
        {
            for (auto x = 0; x < board_size; ++x)
            {
                m_Chessboard.push_back(CreateEntity(fmt::format("tile_{}_{}", x, y)));

                auto& c    = m_Chessboard.back().GetComponent<TransformComponent>();
                c.position = { x + tile_size, y + tile_size, 0 };

                bool is_white = (x + y) % 2 == 0;

                static auto sprite      = Sprite(m_Resources["solid_white"]);
                auto&       sprite_comp = m_Chessboard.back().AddComponent<SpriteRendererComponent>(sprite);
                sprite_comp.GetSprite().GetColour() =
                    (is_white) ? Vector4f{ 1.0f, 1.0f, 1.0f, 1.0f } : Vector4f{ 0.0f, 0.0f, 0.0f, 1.0f };
            }
        }
    }
} // namespace chx
