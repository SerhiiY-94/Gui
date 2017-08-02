#pragma once

#include <ren/Texture.h>

#include "BaseElement.h"

namespace ui {
    class Image : public BaseElement {
	protected:
        ren::Texture2DRef tex_;
        math::vec2       uvs_[2];
    public:
        Image(const ren::Texture2DRef &tex, const math::vec2 uvs[2],
              const math::vec2 &pos, const math::vec2 &size, const BaseElement *parent);
        Image(ren::Context &ctx, const char *tex_name, const math::vec2 uvs[2],
              const math::vec2 &pos, const math::vec2 &size, const BaseElement *parent);

		void set_uvs(const math::vec2 uvs[2]) {
			uvs_[0] = uvs[0];
			uvs_[1] = uvs[1];
		}

        void Draw(Renderer *r) override;
    };
}

