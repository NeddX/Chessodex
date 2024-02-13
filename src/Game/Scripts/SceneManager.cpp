#include "SceneManager.h"

#include <ChessodexApp.h>
#include <CommonDef.h>

namespace chx {
    CODEX_USE_ALL_NAMESPACES()

    namespace fs = std::filesystem;

    auto LoadSprite(const fs::path& path)
    {
        return Resources::Load<Texture2D>(Chessodex::GetAppDataPath() / "Sprites/" / path);
    }

    void SceneManager::Init()
    {
        // Load our resources.
        // As of now you cannot render solid colours with Codex
        // so your best bet is to load a solid white image and
        // do whatever is it that you want to do.
        m_Resources["solid_white"] = LoadSprite("solid_white.png");
        m_Resources["pieces"] = LoadSprite("pieces.png");

        CreateBoard();
        CreatePieces("soemthing here");
    }

    void SceneManager::Update(const codex::f32 deltaTime)
    {
    }

    void SceneManager::CreateBoard()
    {
        static unsigned board_size = 8;
        static unsigned tile_size  = 60;
        static unsigned offset     = 30;

        for (auto y = 0; y < board_size; ++y)
        {
            for (auto x = 0; x < board_size; ++x)
            {
                m_Chessboard.push_back(CreateEntity(fmt::format("tile_{}_{}", x, y)));

                auto& c    = m_Chessboard.back().GetComponent<TransformComponent>();
                c.position = { offset + x * tile_size, offset + y * tile_size, 0 };

                bool is_white = (x + y) % 2 == 0;

                auto sprite      = Sprite(m_Resources["solid_white"]);
                auto&       sprite_comp = m_Chessboard.back().AddComponent<SpriteRendererComponent>(sprite);
                sprite_comp.GetSprite().SetSize({ tile_size, tile_size });
                sprite_comp.GetSprite().GetColour() =
                    (is_white) ? Vector4f{ 1.0f, 1.0f, 1.0f, 1.0f } : Vector4f{ 0.0f, 0.0f, 0.0f, 1.0f };
            }
        }
    }

    void SceneManager::CreatePieces(const std::string_view fmt)
    {

    }
} // namespace chx
