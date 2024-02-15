#include "SceneManager.h"

#include <ChessodexApp.h>
#include <CommonDef.h>

#include "Pawn.h"

#define RgbToNormalized(r, g, b, a) Vector4f { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }

namespace chx {
    CODEX_USE_ALL_NAMESPACES()

    namespace fs = std::filesystem;

    static auto LoadSprite(const fs::path& path)
    {
        TextureProperties props;
        props.filterMode = TextureFilterMode::Linear;
        return Resources::Load<Texture2D>(Chessodex::GetAppDataPath() / "Sprites/" / path, props);
    }
    static char PieceTypeToChar(const SceneManager::Piece piece) noexcept
    {
        char c{};
        const auto piece_type = piece & ~(1 << sizeof(u8) - 1);
        const bool is_white   = piece & (1 << sizeof(u8) - 1);
        switch (piece_type)
        {
            using enum SceneManager::Piece;

            case King: 
                c = 'k';
                break;
            case Queen: 
                c = 'q'; 
                break;
            case Bishop: 
                c = 'b'; 
                break;
            case Knight: 
                c = 'n';
                break;
            case Rook: 
                c = 'r';
                break;
            case Pawn: 
                c = 'p';
                break;
            case None: 
                c = 0;
                break;
        }
        return (!c) ? c : (is_white) ? std::toupper(c) : c;
    }

    void SceneManager::Init()
    {
        // Load our resources.
        // As of now you cannot render solid colours with Codex
        // so your best bet is to load a solid white image and
        // do whatever is it that you want to do.
        m_Resources["solid_white"] = LoadSprite("solid_white.png");
        m_Resources["pieces"]      = LoadSprite("pieces.png");

        // Fill the board with none's.
        std::fill(m_Board.begin(), m_Board.end(), Piece::None);

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
        static Vector3f  piece_prev_pos;
        static Vector2f  pos_in_viewport;
        if (Input::IsMouseDown(Mouse::LeftMouse) && mouse_x >= 0 && mouse_y >= 0 && mouse_x <= (i32)viewport_size.x &&
            mouse_y <= (i32)viewport_size.y)
        {
            Vector2f scale{ framebuffer.GetProperties().width / viewport_size.x,
                               framebuffer.GetProperties().height / viewport_size.y };
            pos_in_viewport = { mouse_x, viewport_size.y - mouse_y };

            auto pos_in_frame = pos_in_viewport * scale;
            pos_in_frame = glm::round(pos_in_frame);

            if (Input::IsMouseDragging())
            {
                if (selected_piece)
                {
                    piece_pos->x = pos_in_viewport.x;
                    piece_pos->y = viewport_size.y - pos_in_viewport.y;
                }
            }
            else
            {
                const auto id = framebuffer.ReadPixel(1, (i32)pos_in_frame.x, (i32)pos_in_frame.y);
                selected_piece = Entity(id, &GameLayer::GetCurrentScene());
                if (selected_piece)
                {
                    const auto& tag = selected_piece.GetComponent<TagComponent>().tag;
                    if (tag.starts_with("tile"))
                        selected_piece = Entity::None();
                    else if ((m_WhitesTurn && !std::isupper(tag[0])) || (!m_WhitesTurn && std::isupper(tag[0])))
                        selected_piece = Entity::None();
                    else
                    {
                        selected_piece.GetComponent<SpriteRendererComponent>().GetSprite().GetZIndex() = 10;
                        piece_pos = &selected_piece.GetComponent<TransformComponent>().position;
                        piece_prev_pos = *piece_pos;
                    }
                }
            }
        }
        else
        {
            if (selected_piece)
            {
                // Snap the coordinates to the chessboard grid.
                Vector2 grid_snap{ (i32)pos_in_viewport.x % (i32)m_TileSize,
                                   (i32)(viewport_size.y - pos_in_viewport.y) % (i32)m_TileSize };
                grid_snap = { pos_in_viewport.x - grid_snap.x, viewport_size.y - pos_in_viewport.y - grid_snap.y };
                grid_snap = { grid_snap.x + m_TileSize / 2, grid_snap.y + m_TileSize / 2 };

                // Try and execute the move.
                TryMakeMove(selected_piece, piece_prev_pos, grid_snap);

                // Reset.
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
        static auto piece_tile_size      = 333.0f;
        static auto sprite           = Sprite(m_Resources["pieces"]);

        Entity entity                                      = Entity::None();
        entity                                             = CreateEntity(fmt::format("{}_{},{}", (is_white) ? (char)std::toupper(piece) : piece, pos.x, pos.y));
        entity.GetComponent<TransformComponent>().position = { m_TileSize / 2 + pos.x * m_TileSize, m_TileSize / 2 + pos.y * m_TileSize, 0.0f };
        
        
        //entity.AddComponent<NativeBehaviourComponent>().New<pawn::Pawn>();

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
                type                      = Piece::Queen;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)Piece::Queen, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                break;
            case 'b':
                type                      = Piece::Bishop;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)Piece::Bishop, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                break;
            case 'n':
                type                      = Piece::Knight;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)Piece::Knight, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                break;
            case 'r':
                type                      = Piece::Rook;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)Piece::Rook, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                break;
            case 'p':
                type                      = Piece::Pawn;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)Piece::Pawn, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                break;
        }
        entity.AddComponent<SpriteRendererComponent>(sprite);

        m_Board[pos.y * m_BoardSize + pos.x] = (is_white) ? type : (Piece)(type | PieceColour::Black);
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

    SceneManager::MoveInfo SceneManager::ValidateMove(const Vector2 pos, const Vector2 target_pos) const noexcept
    {
        return { true, false };
    }

    void SceneManager::TryMakeMove(Entity piece, const Vector2 pos, const Vector2 targetPos) noexcept
    {
        const Vector2 board_pos{ pos.x / m_TileSize, pos.y / m_TileSize };
        const Vector2 target_board_pos{ targetPos.x / m_TileSize, targetPos.y / m_TileSize }; 
        const auto    type        = m_Board[board_pos.y * m_BoardSize + board_pos.x];
        const auto    target_type = m_Board[target_board_pos.y * m_BoardSize + target_board_pos.x];
        const auto    move_info   = ValidateMove(pos, target_board_pos);
        auto& piece_pos = piece.GetComponent<TransformComponent>().position;

        if (move_info.valid)
        {
            if (move_info.doesEat)
            {
                const auto fmt =
                    fmt::format("{}_{},{}", PieceTypeToChar(target_type), pos.x, pos.y);
                const auto target = GameLayer::GetCurrentScene().GetAllEntitesWithTag(fmt);

                fmt::println("Eating entity: {}", fmt);

                // Not using m_WhitePieces and m_BlackPieces so whatever I assume.
                GameLayer::GetCurrentScene().RemoveEntity(target[0]);
            }

            // Update the tag.
            piece.GetComponent<TagComponent>().tag =
                fmt::format("{}_{},{}", PieceTypeToChar(type), target_board_pos.x, target_board_pos.y);

            // Update the new position.
            piece_pos.x = targetPos.x;
            piece_pos.y = targetPos.y;

            // Update the piece inside the board.
            m_Board[target_board_pos.y * m_BoardSize + target_board_pos.x] = type;
            m_Board[board_pos.y * m_BoardSize + board_pos.x] = Piece::None;

            // Flip the turn.
            m_WhitesTurn = !m_WhitesTurn;
        }
        else
            piece_pos.x = pos.x, piece_pos.y = pos.y;
    }
} // namespace chx
