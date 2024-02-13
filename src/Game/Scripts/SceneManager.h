#pragma once

#include <Codex.h>

namespace chx {
    class SceneManager : public codex::NativeBehaviour
    {
    private:
        std::unordered_map<std::string, codex::ResRef<codex::graphics::Texture2D>> m_Resources{};
        std::vector<codex::Entity>                                                 m_Chessboard{};
        std::vector<codex::Entity>                                                 m_WhitePieces{};
        std::vector<codex::Entity>                                                 m_BlackPieces{};

    protected:
        void Init() override;
        void Update(const codex::f32 deltaTime) override;
        void CreateBoard();
        void CreatePieces(const std::string_view fmt);
    };
} // namespace chx
