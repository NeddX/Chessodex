#pragma once

#include <Codex.h>

namespace chx::pawn {
	class Pawn : public codex::NativeBehaviour
	{
    protected:
        void Init() override;
        void Update(const codex::f32 deltaTime) override;
	};
} // namespace chx