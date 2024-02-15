#pragma once

#include <Codex.h>

#include "SceneManager.h"

namespace chx::pawn {
    class Pawn : public codex::NativeBehaviour
    {
        friend class SceneManager;
    
    public:
        using Piece = SceneManager::Piece;

    protected:
        bool             m_IsWhite   = true;
        codex::usize     m_MoveCount = 0;
        codex::f32       m_BoardSize{};
        codex::Vector2   m_Pos{};
        Board*           m_Board{};
    
    protected:
        inline Piece GetPawnAt(const codex::Vector2 pos) const noexcept
        { 
            return (Piece)m_Board->operator[](pos.y * m_BoardSize + pos.x);
        }
    
    protected:
        void         Init() override;
        void         Update(const codex::f32 deltaTime) override;
        virtual bool CanProceed(const codex::Vector2 targetPos, const bool eatable = false) const;
        virtual std::vector<codex::Vector2> GetMaxRangeFromCurrentPos() const;
    };

    class Knight : public Pawn
    {
    protected:
        bool CanProceed(const codex::Vector2 targetPos, const bool eatable = false) const;
    };

    class Bishop : public Pawn
    {
    protected:
        bool CanProceed(const codex::Vector2 targetPos, const bool eatable = false) const;
    };

    class Rook : public Pawn
    {
    protected:
        bool CanProceed(const codex::Vector2 targetPos, const bool eatable = false) const;
    };

    class King : public Pawn
    {
    protected:
        bool CanProceed(const codex::Vector2 targetPos, const bool eatable = false) const;
    };

    class Queen : public Pawn
    {
    protected:
        bool CanProceed(const codex::Vector2 targetPos, const bool eatable = false) const;
    };
} // namespace chx::pawn