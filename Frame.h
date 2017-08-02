#pragma once

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
        Frame(const ren::Texture2DRef &tex, const math::vec2 &offsets,
              const math::vec2 &pos, const math::vec2 &size, const BaseElement *parent);
        Frame(ren::Context &ctx, const char *tex_name, const math::vec2 &offsets,
              const math::vec2 &pos, const math::vec2 &size, const BaseElement *parent);

        ren::Texture2DRef &tex() { return tex_; }

        void Resize(const BaseElement *parent) override;
        
        void Draw(Renderer *r) override;
    };
}
