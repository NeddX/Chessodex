#pragma once

#include <Codex.h>

namespace chx {
    class SceneManager : public codex::NativeBehaviour
    {
    private:
        std::vector<codex::Entity>                                                 m_Chessboard{};
        std::unordered_map<std::string, codex::ResRef<codex::graphics::Texture2D>> m_Resources{};

    protected:
        void Init() override;
        void Update(const codex::f32 deltaTime) override;
        void DrawChessboard();
    };
} // namespace chx
