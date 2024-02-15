#pragma once

#include <Codex.h>

namespace chx {
    // Forward declerations.
    class GameLayer;

    class SceneManager
    {
        friend class GameLayer;

    private:
        enum Piece : codex::u8
        {
            King,
            Queen,
            Bishop,
            Knight,
            Rook,
            Pawn,
            None
        };
        enum PieceColour : codex::u8
        {
            White = 0,
            Black = 128
        };
        struct MoveInfo
        {
            bool valid = false;
            bool doesEat = false;
        };

    private:
        std::unordered_map<std::string, codex::ResRef<codex::graphics::Texture2D>> m_Resources{};
        std::vector<codex::Entity>                                                 m_Chessboard{};
        std::vector<codex::Entity>                                                 m_WhitePieces{};
        std::vector<codex::Entity>                                                 m_BlackPieces{};
        codex::f32                                                                 m_TileSize = 60.0f;
        codex::u32                                                                 m_BoardSize = 8;
        std::array<Piece, 8 * 8>                                                   m_Board;
        bool                                                                       m_WhitesTurn = true;

    private:
        void          Init();
        void          Update(const codex::f32 deltaTime);
        codex::Entity CreateEntity(const std::string_view tag = "default tag");
        void          CreateBoard();
        codex::Entity CreatePiece(const char piece, const bool isWhite, const codex::Vector3f pos);
        void          CreatePieces(const std::string_view fen);
        MoveInfo      ValidateMove(const codex::Vector2 boardPos, const codex::Vector2 boardTargetPos) const noexcept;
        void          TryMakeMove(codex::Entity piece, const codex::Vector2 pos, const codex::Vector2 targetPos) noexcept;
    
    public:
        friend char PieceTypeToChar(const Piece piece) noexcept;
    };
} // namespace chx
