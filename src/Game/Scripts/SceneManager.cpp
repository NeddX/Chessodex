#include "SceneManager.h"

#include <ChessodexApp.h>
#include <CommonDef.h>

#include "Pawn.h"

#define RgbToNormalized(r, g, b, a)                                                                                    \
    Vector4f                                                                                                           \
    {                                                                                                                  \
        r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f                                                                 \
    }

namespace chx {
    CODEX_USE_ALL_NAMESPACES()

    namespace fs = std::filesystem;

    auto LoadSprite(const fs::path& path)
    {
        TextureProperties props;
        props.filterMode = TextureFilterMode::Linear;
        return Resources::Load<Texture2D>(Chessodex::GetAppDataPath() / "Sprites/" / path, props);
    }
    char PieceTypeToChar(const SceneManager::Piece piece) noexcept
    {
        char       c{};
        const auto piece_type = piece & ~(1 << (sizeof(u8) - 1));
        const bool is_white   = piece & (1 << (sizeof(u8) - 1));
        switch (piece_type)
        {
            using enum SceneManager::Piece;

            case King: c = 'k'; break;
            case Queen: c = 'q'; break;
            case Bishop: c = 'b'; break;
            case Knight: c = 'n'; break;
            case Rook: c = 'r'; break;
            case Pawn: c = 'p'; break;
            case None: c = 0; break;
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

        // Create the scene.
        Reset();
    }

    void SceneManager::ImGuiRender()
    {
        static int column_width = 100;

        // Fancy pants.
        {
            ImGui::Begin("Customizer");

            // Colour picker for the white tiles.
            {
                ImGui::Dummy(ImVec2(0.0f, 10.0f));
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, column_width);
                ImGui::Text("White tile colour: ");
                ImGui::NextColumn();

                auto& colour = m_WhiteTiles[0].GetComponent<SpriteRendererComponent>().GetSprite().GetColour();
                ImGuiColorEditFlags flags = 0;
                flags |= ImGuiColorEditFlags_AlphaBar;
                flags |= ImGuiColorEditFlags_DisplayRGB; // Override display mode
                f32 temp_colour[4]{ colour.x, colour.y, colour.z, colour.w };
                ImGui::ColorPicker4("##wtile_colour_picker", temp_colour, flags);
                Vector4f res = glm::make_vec4(temp_colour);
                if (res != colour)
                {
                    for (auto& e : m_WhiteTiles)
                        e.GetComponent<SpriteRendererComponent>().GetSprite().GetColour() = res;
                }

                ImGui::Columns(1);
            }

            // Colour picker for the black tiles.
            {
                ImGui::Dummy(ImVec2(0.0f, 10.0f));
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, column_width);
                ImGui::Text("Black tile colour: ");
                ImGui::NextColumn();

                auto& colour = m_BlackTiles[0].GetComponent<SpriteRendererComponent>().GetSprite().GetColour();
                ImGuiColorEditFlags flags = 0;
                flags |= ImGuiColorEditFlags_AlphaBar;
                flags |= ImGuiColorEditFlags_DisplayRGB; // Override display mode
                f32 temp_colour[4]{ colour.x, colour.y, colour.z, colour.w };
                ImGui::ColorPicker4("##btile_colour_picker", temp_colour, flags);
                Vector4f res = glm::make_vec4(temp_colour);
                if (res != colour)
                {
                    for (auto& e : m_BlackTiles)
                        e.GetComponent<SpriteRendererComponent>().GetSprite().GetColour() = res;
                }

                ImGui::Columns(1);
            }

            ImGui::End();
        }

        // Statistics window.
        {
            ImGui::Begin("Statistics");
            ImGui::Text("%s", (m_WhitesTurn) ? "White's turn." : "Black's turn.");
            ImGui::Text("Pawns eaten by White: %lu", m_EatenBlackPawns);
            ImGui::Text("Pawns eaten by Black: %lu", m_EatenWhitePawns);

            if (ImGui::Button("Reset"))
                Reset();

            ImGui::SameLine();

            if (ImGui::Button("Load custom from FEN"))
                ImGui::OpenPopup("new_fen_popup");

            if (ImGui::BeginPopup("new_fen_popup", ImGuiWindowFlags_MenuBar))
            {
                static std::string fen_string{};
                ImGui::Text("FEN string: ");
                ImGui::SameLine();
                ImGui::InputText("##fen_str", &fen_string);
                ImGui::SameLine();
                if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::Button("Submit"))
                {
                    Reset(fen_string);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            ImGui::End();
        }
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
            pos_in_frame      = glm::round(pos_in_frame);

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
                const auto id  = framebuffer.ReadPixel(1, (i32)pos_in_frame.x, (i32)pos_in_frame.y);
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
                        piece_pos      = &selected_piece.GetComponent<TransformComponent>().position;
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
                selected_piece                                                                 = Entity::None();
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
                const bool is_white = (x + y) % 2 == 0;

                auto tile = CreateEntity(fmt::format("tile_{},{}", x, y));
                if (is_white)
                    m_WhiteTiles.push_back(tile);
                else
                    m_BlackTiles.push_back(tile);

                auto& c    = tile.GetComponent<TransformComponent>();
                c.position = { m_TileSize / 2 + x * m_TileSize, m_TileSize / 2 + y * m_TileSize, 0.0f };

                static auto sprite      = Sprite(m_Resources["solid_white"]);
                auto&       sprite_comp = tile.AddComponent<SpriteRendererComponent>(sprite);
                sprite_comp.GetSprite().SetSize({ m_TileSize, m_TileSize });
                sprite_comp.GetSprite().GetColour() = (is_white) ? white_tile : black_tile;
            }
        }
    }

    void SceneManager::Reset(const std::string_view customFen)
    {
        // Fill the board with none's.
        std::fill(m_Board.begin(), m_Board.end(), Piece::None);

        // I hate this.
        for (const auto& e : GameLayer::GetCurrentScene().GetAllEntities())
            GameLayer::GetCurrentScene().RemoveEntity(e);

        m_EatenWhitePawns = 0;
        m_EatenBlackPawns = 0;
        m_WhitesTurn      = true;

        CreateBoard();
        CreatePieces(customFen);
    }

    codex::Entity SceneManager::CreatePiece(const char piece, const bool is_white, const Vector3f pos)
    {
        static auto piece_tile_size = 333.0f;
        static auto sprite          = Sprite(m_Resources["pieces"]);

        Entity entity = Entity::None();
        entity = CreateEntity(fmt::format("{}_{},{}", (is_white) ? (char)std::toupper(piece) : piece, pos.x, pos.y));
        entity.GetComponent<TransformComponent>().position = { m_TileSize / 2 + pos.x * m_TileSize,
                                                               m_TileSize / 2 + pos.y * m_TileSize, 0.0f };

        auto&       nbc = entity.AddComponent<NativeBehaviourComponent>();
        pawn::Pawn* script{};

        Piece type = Piece::None;

        sprite.GetZIndex() = 1;
        sprite.SetSize({ m_TileSize, m_TileSize });
        switch (piece)
        {
            case 'k':
                type                      = Piece::King;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)type, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                script                    = &nbc.New<pawn::King>();
                break;
            case 'q':
                type                      = Piece::Queen;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)Piece::Queen, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                script                    = &nbc.New<pawn::Queen>();
                break;
            case 'b':
                type                      = Piece::Bishop;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)Piece::Bishop, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                script                    = &nbc.New<pawn::Bishop>();
                break;
            case 'n':
                type                      = Piece::Knight;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)Piece::Knight, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                script                    = &nbc.New<pawn::Knight>();
                break;
            case 'r':
                type                      = Piece::Rook;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)Piece::Rook, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                script                    = &nbc.New<pawn::Rook>();
                break;
            case 'p':
                type                      = Piece::Pawn;
                sprite.GetTextureCoords() = { piece_tile_size * (f32)Piece::Pawn, (is_white) ? 0.0f : piece_tile_size,
                                              piece_tile_size, piece_tile_size };
                script                    = &nbc.New<pawn::Pawn>();
                break;
        }

        if (script)
        {
            script->m_BoardSize = m_BoardSize;
            script->m_Board     = &m_Board;
            script->m_Pos       = { pos.x, pos.y };
            script->m_IsWhite   = is_white;
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
                // pos = (pos.x + 1 >= 8.0f) ? Vector3f{ 0.0f, ++pos.y, 0.0f } : Vector3f{ ++pos.x, pos.y, 0.0f };
            }
        }
    }

    void SceneManager::TryMakeMove(Entity piece, const Vector2 pos, const Vector2 targetPos) noexcept
    {
        const Vector2 board_pos{ pos.x / m_TileSize, pos.y / m_TileSize };
        const Vector2 target_board_pos{ targetPos.x / m_TileSize, targetPos.y / m_TileSize };
        const auto    type        = m_Board[board_pos.y * m_BoardSize + board_pos.x];
        const auto    target_type = m_Board[target_board_pos.y * m_BoardSize + target_board_pos.x];
        auto&         piece_pos   = piece.GetComponent<TransformComponent>().position;
        const auto    bh = (pawn::Pawn*)piece.GetComponent<NativeBehaviourComponent>().behaviours.begin()->second.Get();
        const bool    eatable = m_Board[target_board_pos.y * m_BoardSize + target_board_pos.x] != Piece::None;

        if (bh->CanProceed(target_board_pos, eatable))
        {
            if (eatable)
            {
                const auto fmt =
                    fmt::format("{}_{},{}", PieceTypeToChar(target_type), target_board_pos.x, target_board_pos.y);
                const auto target = GameLayer::GetCurrentScene().GetAllEntitesWithTag(fmt);

                if (m_WhitesTurn)
                    ++m_EatenBlackPawns;
                else
                    ++m_EatenWhitePawns;

                // Not using m_WhitePieces and m_BlackPieces so whatever I assume.
                GameLayer::GetCurrentScene().RemoveEntity(target[0]);
            }

            // Update the tag.
            piece.GetComponent<TagComponent>().tag =
                fmt::format("{}_{},{}", PieceTypeToChar(type), target_board_pos.x, target_board_pos.y);

            // Update the new position (inside the scene).
            piece_pos.x = targetPos.x;
            piece_pos.y = targetPos.y;

            // Update the new position (inside the board).
            bh->m_Pos = target_board_pos;
            ++bh->m_MoveCount;

            // Update the piece inside the board.
            m_Board[target_board_pos.y * m_BoardSize + target_board_pos.x] = type;
            m_Board[board_pos.y * m_BoardSize + board_pos.x]               = Piece::None;

            // Flip the turn.
            m_WhitesTurn = !m_WhitesTurn;
        }
        else
            piece_pos.x = pos.x, piece_pos.y = pos.y;
    }
} // namespace chx
