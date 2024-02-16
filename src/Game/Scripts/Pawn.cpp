#include "Pawn.h"

#include <CommonDef.h>

namespace chx::pawn {
    CODEX_USE_ALL_NAMESPACES()

    void Pawn::Init()
    {
    }

    void Pawn::Update(const f32 deltaTime)
    {
    }

    bool Pawn::CanProceed(const Vector2 targetPos, const bool eatable) const
    {
        if (m_IsWhite)
        {
            if (eatable)
            {
                if (std::abs(m_Pos.x - targetPos.x) == 1 && m_Pos.y - targetPos.y == 1)
                    return true;
            }
            else
            {
                if (m_MoveCount == 0)
                {
                    if (targetPos.x == m_Pos.x && m_Pos.y - targetPos.y > 0 && m_Pos.y - targetPos.y <= 2)
                    {
                        for (int i = 1; i <= 2; ++i)
                        {
                            if (GetPawnAt({ m_Pos.x, m_Pos.y - i }) != Piece::None)
                                return false;
                        }
                        return true;
                    }
                }
                else
                {
                    if (targetPos.x == m_Pos.x && m_Pos.y - targetPos.y == 1)
                        return true;
                }
            }
        }
        else
        {
            if (eatable)
            {
                if (std::abs(targetPos.x - m_Pos.x) == 1 && targetPos.y - m_Pos.y == 1)
                    return true;
            }
            else
            {
                if (m_MoveCount == 0)
                {
                    if (targetPos.x == m_Pos.x && targetPos.y - m_Pos.y > 0 && targetPos.y - m_Pos.y <= 2)
                    {
                        for (int i = 1; i <= 2; ++i)
                        {
                            if (GetPawnAt({ m_Pos.x, m_Pos.y + i }) != Piece::None)
                                return false;
                        }
                        return true;
                    }
                }
                else
                {
                    if (targetPos.x == m_Pos.x && targetPos.y - m_Pos.y == 1)
                        return true;
                }
            }
        }
        return false;
    }

    std::vector<Vector2> Pawn::GetMaxRangeFromCurrentPos() const
    {
        return std::vector<Vector2>();
    }

    bool Knight::CanProceed(const Vector2 targetPos, const bool eatable) const
    {
        // Horizontal movement
        if (std::abs(targetPos.x - m_Pos.x) == 2 && std::abs(targetPos.y - m_Pos.y) == 1)
        {
            return true;
        }
        else if (std::abs(targetPos.y - m_Pos.y) == 2 && std::abs(targetPos.x - m_Pos.x) == 1)
        {
            return true;
        }
        return false;
    }

    bool Bishop::CanProceed(const Vector2 targetPos, const bool eatable) const
    {
        if (std::abs(targetPos.x - m_Pos.x) == std::abs(targetPos.y - m_Pos.y))
        {
            if (targetPos.x - m_Pos.x > 0)
            {
                // Positive X
                if (targetPos.y - m_Pos.y > 0)
                {
                    // Positive Y
                    for (int i = 1; i < targetPos.y - m_Pos.y; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x + i, m_Pos.y + i }) != Piece::None)
                            return false;
                    }
                }
                else
                {
                    // Negative Y
                    for (int i = 1; i < m_Pos.y - targetPos.y; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x + i, m_Pos.y - i }) != Piece::None)
                            return false;
                    }
                }
            }
            else
            {
                // Negative X
                if (targetPos.y - m_Pos.y > 0)
                {
                    // Positive Y
                    for (int i = 1; i < targetPos.y - m_Pos.y; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x - i, m_Pos.y + i }) != Piece::None)
                            return false;
                    }
                }
                else
                {
                    // Negative Y
                    for (int i = 1; i < m_Pos.y - targetPos.y; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x - i, m_Pos.y - i }) != Piece::None)
                            return false;
                    }
                }
            }
            return true;
        }
        return false;
    }

    bool Rook::CanProceed(const Vector2 targetPos, const bool eatable) const
    {
        if (targetPos.x == m_Pos.x || targetPos.y == m_Pos.y)
        {
            if (targetPos.x == m_Pos.x)
            {
                if (m_Pos.y - targetPos.y > 0)
                {
                    for (int i = 1; i < m_Pos.y - targetPos.y; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x, m_Pos.y - i }) != Piece::None)
                            return false;
                    }
                }
                else
                {
                    for (int i = 1; i < targetPos.y - m_Pos.y; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x, m_Pos.y + i }) != Piece::None)
                            return false;
                    }
                }
            }
            else if (targetPos.y == m_Pos.y)
            {
                if (m_Pos.x - targetPos.x > 0)
                {
                    for (int i = 1; i < m_Pos.x - targetPos.x; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x - i, m_Pos.y }) != Piece::None)
                            return false;
                    }
                }
                else
                {
                    for (int i = 1; i < targetPos.x - m_Pos.x; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x + i, m_Pos.y }) != Piece::None)
                            return false;
                    }
                }
            }
            return true;
        }
        return false;
    }

    bool King::CanProceed(const Vector2 targetPos, const bool eatable) const
    {
        if (std::abs(targetPos.x - m_Pos.x) <= 1 && std::abs(targetPos.y - m_Pos.y) <= 1)
            return true;
        return false;
    }

    bool Queen::CanProceed(const Vector2 targetPos, const bool eatable) const
    {
        if (targetPos.x == m_Pos.x || targetPos.y == m_Pos.y)
        {
            if (targetPos.x == m_Pos.x)
            {
                if (m_Pos.y - targetPos.y > 0)
                {
                    for (int i = 1; i < m_Pos.y - targetPos.y; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x, m_Pos.y - i }) != Piece::None)
                            return false;
                    }
                }
                else
                {
                    for (int i = 1; i < targetPos.y - m_Pos.y; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x, m_Pos.y + i }) != Piece::None)
                            return false;
                    }
                }
            }
            else if (targetPos.y == m_Pos.y)
            {
                if (m_Pos.x - targetPos.x > 0)
                {
                    for (int i = 1; i < m_Pos.x - targetPos.x; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x - i, m_Pos.y }) != Piece::None)
                            return false;
                    }
                }
                else
                {
                    for (int i = 1; i < targetPos.x - m_Pos.x; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x + i, m_Pos.y }) != Piece::None)
                            return false;
                    }
                }
            }
            return true;
        }
        else if (std::abs(targetPos.x - m_Pos.x) == std::abs(targetPos.y - m_Pos.y))
        {
            if (targetPos.x - m_Pos.x > 0)
            {
                // Positive X
                if (targetPos.y - m_Pos.y > 0)
                {
                    // Positive Y
                    for (int i = 1; i < targetPos.y - m_Pos.y; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x + i, m_Pos.y + i }) != Piece::None)
                            return false;
                    }
                }
                else
                {
                    // Negative Y
                    for (int i = 1; i < m_Pos.y - targetPos.y; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x + i, m_Pos.y - i }) != Piece::None)
                            return false;
                    }
                }
            }
            else
            {
                // Negative X
                if (targetPos.y - m_Pos.y > 0)
                {
                    // Positive Y
                    for (int i = 1; i < targetPos.y - m_Pos.y; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x - i, m_Pos.y + i }) != Piece::None)
                            return false;
                    }
                }
                else
                {
                    // Negative Y
                    for (int i = 1; i < m_Pos.y - targetPos.y; ++i)
                    {
                        if (GetPawnAt({ m_Pos.x - i, m_Pos.y - i }) != Piece::None)
                            return false;
                    }
                }
            }
            return true;
        }
        return false;
    }

} // namespace chx::pawn
