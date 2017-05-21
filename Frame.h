#ifndef FRAME_H
#define FRAME_H

#include <vector>

#include <ren/Texture.h>

#include "BaseElement.h"

namespace ui {
    class Frame : public BaseElement {
        ren::Texture2DRef tex_;
        float frame_offset_, frame_offset_uv_;

        std::vector<float> positions_, uvs_;
        std::vector<unsigned char> indices_;
    public:
        Frame(const ren::Texture2DRef &tex, const glm::vec2 &offsets,
              const glm::vec2 &pos, const glm::vec2 &size, const BaseElement *parent);
        Frame(ren::Context &ctx, const char *tex_name, const glm::vec2 &offsets,
              const glm::vec2 &pos, const glm::vec2 &size, const BaseElement *parent);

        const ren::Texture2DRef &tex() const { return tex_; }

        void Resize(const BaseElement *parent) override;
        
        void Draw(Renderer *r) override;
    };
}

#endif // FRAME_H
