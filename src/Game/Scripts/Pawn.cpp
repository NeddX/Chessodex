#include "Pawn.h"

#include <CommonDef.h>

namespace chx::pawn {
    CODEX_USE_ALL_NAMESPACES()

    void Pawn::Init()
    {
        fmt::println("I am a pawn");
    }

    void Pawn::Update(const f32 deltaTime)
    {
    }
} // namespace chx::pawn
