#include "SceneManager.h"

#include <ChessodexApp.h>
#include <CommonDef.h>

#define RgbToNormalized(r, g, b, a) Vector4f { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }

namespace chx {
    CODEX_USE_ALL_NAMESPACES()

    namespace fs = std::filesystem;

    auto LoadSprite(const fs::path& path)
    {
        TextureProperties props;
        props.filterMode = TextureFilterMode::Linear;
        return Resources::Load<Texture2D>(Chessodex::GetAppDataPath() / "Sprites/" / path, props);
    }

    void SceneManager::Init()
    {
        // Load our resources.
        // As of now you cannot render solid colours with Codex
        // so your best bet is to load a solid white image and
        // do whatever is it that you want to do.
        m_Resources["solid_white"] = LoadSprite("solid_white.png");
        m_Resources["pieces"]      = LoadSprite("pieces.png");

        CreateBoard();
        //CreatePieces("7k/3N2qp/b5r1/2p1Q1N1/Pp4PK/7P/1P3p2/6r1 w -- 7 4");
        CreatePieces("rnbkqbnr/pppppppp/////PPPPPPPP/RNBKQBNR");
    }

    void SceneManager::Update(const codex::f32 deltaTime)
    {
        auto [mx, my] = ImGui::GetMousePos();

        auto& viewport_bounds = GameLayer::GetViewportBounds();
        auto& framebuffer     = GameLayer::GetPrimaryFrameBuffer();

        mx -= viewport_bounds[0].x;
        my -= viewport_bounds[0].y;
        Vector2f viewport_size = viewport_bounds[1] - viewport_bounds[0];
        i32      mouse_x       = (i32)mx;
        i32      mouse_y       = (i32)my;

        
        static Entity    selected_piece = Entity::None();
        static Vector3f* piece_pos      = nullptr; 
        if (Input::IsMouseDown(Mouse::LeftMouse) && mouse_x >= 0 && mouse_y >= 0 && mouse_x <= (i32)viewport_size.x &&
            mouse_y <= (i32)viewport_size.y)
        {

            Vector2f scale{ framebuffer.GetProperties().width / viewport_size.x,
                               framebuffer.GetProperties().height / viewport_size.y };
            Vector2f pos_in_viewport{ mouse_x, viewport_size.y - mouse_y };

            auto pos_in_frame = pos_in_viewport * scale;
            pos_in_frame = glm::round(pos_in_frame);

            if (Input::IsMouseDragging())
            {
                if (selected_piece)
                {
                    Vector2 grid_snap{ (i32)pos_in_viewport.x % (i32)m_TileSize,
                                       (i32)(viewport_size.y - pos_in_viewport.y) % (i32)m_TileSize };
                    grid_snap = { pos_in_viewport.x - grid_snap.x, viewport_size.y - pos_in_viewport.y - grid_snap.y };

                    piece_pos->x = grid_snap.x + m_TileSize / 2;
                    piece_pos->y = grid_snap.y + m_TileSize / 2;
                    fmt::print("\rpos: {}    ", *piece_pos);
                }
            }
            else
            {
                const auto id = framebuffer.ReadPixel(1, (i32)pos_in_frame.x, (i32)pos_in_frame.y);
                selected_piece = Entity(id, &GameLayer::GetCurrentScene());
                if (selected_piece)
                {
                    if (selected_piece.GetComponent<TagComponent>().tag.starts_with("tile"))
                        selected_piece = Entity::None();
                    else
                    {
                        selected_piece.GetComponent<SpriteRendererComponent>().GetSprite().GetZIndex() = 10;
                        piece_pos = &selected_piece.GetComponent<TransformComponent>().position;
                    }
                }
            }
        }
        else
        {
            if (selected_piece)
            {
                selected_piece.GetComponent<SpriteRendererComponent>().GetSprite().GetZIndex() = 1;
                selected_piece = Entity::None();
            }
        }
    }

    codex::Entity SceneManager::CreateEntity(const std::string_view tag)
    {
        return GameLayer::GetCurrentScene().CreateEntity(tag);
    }

    void SceneManager::CreateBoard()
    {
        static auto board_size = 8;
        static auto white_tile = RgbToNormalized(200, 222, 255, 255);
        static auto black_tile = RgbToNormalized(105, 149, 186, 255);

        for (auto y = 0; y < board_size; ++y)
        {
            for (auto x = 0; x < board_size; ++x)
            {
                m_Chessboard.push_back(CreateEntity(fmt::format("tile_{}_{}", x, y)));

                auto& c    = m_Chessboard.back().GetComponent<TransformComponent>();
                c.position = { m_TileSize / 2 + x * m_TileSize, m_TileSize / 2 + y * m_TileSize, 0.0f };

                bool is_white = (x + y) % 2 == 0;

                static auto sprite      = Sprite(m_Resources["solid_white"]);
                auto&       sprite_comp = m_Chessboard.back().AddComponent<SpriteRendererComponent>(sprite);
                sprite_comp.GetSprite().SetSize({ m_TileSize, m_TileSize });
                sprite_comp.GetSprite().GetColour() = (is_white) ? white_tile : black_tile;
            }
        }
    }

    codex::Entity SceneManager::CreatePiece(const char piece, const bool is_white, const Vector3f pos)
    {
        static auto piece_tile_size = 333.0f;
        static auto piece_counter   = 0;
        static auto sprite          = Sprite(m_Resources["pieces"]);

        Entity entity                                      = Entity::None();
        entity                                             = CreateEntity(fmt::format("{}_{}", piece, piece_counter++));
        entity.GetComponent<TransformComponent>().position = { m_TileSize / 2 + pos.x * m_TileSize, m_TileSize / 2 + pos.y * m_TileSize, 0.0f };

        Piece type = Piece::None;

        sprite.GetZIndex() = 1;
        sprite.SetSize({ m_TileSize, m_TileSize }); 
        switch (piece)
        {
            case 'k':
                type                      = Piece::King;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)type, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                break;
            case 'q':
                type                      = Piece::King;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)Piece::Queen, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                break;
            case 'b':
                type                      = Piece::King;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)Piece::Bishop, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                break;
            case 'n':
                type                      = Piece::King;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)Piece::Knight, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                break;
            case 'r':
                type                      = Piece::King;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)Piece::Rook, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                break;
            case 'p':
                type                      = Piece::King;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)Piece::Pawn, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                break;
        }
        entity.AddComponent<SpriteRendererComponent>(sprite);

        m_Board[pos.y * m_BoardSize + pos.x] = type;
        return entity;
    }

    void SceneManager::CreatePieces(const std::string_view fen)
    {
        Vector3f pos{};
        for (const char c : fen)
        {
            if (std::isdigit(c))
                pos.x += c - '0';
            else if (c == '/')
                pos.x = 0.0f, ++pos.y;
            else if (std::isalpha(c))
            {
                CreatePiece(std::tolower(c), std::isupper(c), pos);
                ++pos.x;
                //pos = (pos.x + 1 >= 8.0f) ? Vector3f{ 0.0f, ++pos.y, 0.0f } : Vector3f{ ++pos.x, pos.y, 0.0f };
            }
        }
    }
} // namespace chx
