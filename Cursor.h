#pragma once

#include "Image.h"

namespace ui {
    class Cursor : public BaseElement {
	protected:
        Image		img_;
        bool		clicked_;
        math::vec2	offset_;
    public:
        Cursor(const ren::Texture2DRef &tex, const math::vec2 uvs[2], const math::vec2 &size, const BaseElement *parent);
        Cursor(ren::Context &ctx, const char *tex_name, const math::vec2 uvs[2], const math::vec2 &size, const BaseElement *parent);

        void set_clicked(bool b) { clicked_ = b; }
        void set_offset(const math::vec2 &offset) { offset_ = offset; }

        void SetPos(const math::vec2 &pos, const BaseElement *parent);

        void Draw(Renderer *r) override;
    };
}

