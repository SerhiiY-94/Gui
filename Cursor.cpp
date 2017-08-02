#include "Cursor.h"

#include "Renderer.h"

ui::Cursor::Cursor(const ren::Texture2DRef &tex, const math::vec2 uvs[2], const math::vec2 &size, const BaseElement *parent)
		: BaseElement(math::vec2(0, 0), size, parent),
          img_(tex, uvs, math::vec2(-1, -1), math::vec2(2, 2), this), clicked_(false) { }

ui::Cursor::Cursor(ren::Context &ctx, const char *tex_name, const math::vec2 uvs[2], const math::vec2 &size, const BaseElement *parent)
		: BaseElement(math::vec2(0, 0), size, parent),
          img_(ctx, tex_name, uvs, math::vec2(-1, -1), math::vec2(2, 2), this), clicked_(false) { }

void ui::Cursor::SetPos(const math::vec2 &pos, const BaseElement *parent) {
    using namespace math;

    Resize(pos, dims_[1], parent);
    if (clicked_) {
        img_.Resize(vec2(-1, -1) + offset_ * 0.8f, vec2(2, 2) * 0.8f, this);
    } else {
        img_.Resize(vec2(-1, -1) + offset_, vec2(2, 2), this);
    }
}

void ui::Cursor::Draw(Renderer *r) {
	const auto &cur = r->GetParams();

    r->EmplaceParams(clicked_ ? math::vec3(0.8f, 0.8f, 0.8f) : math::vec3(1, 1, 1),
		cur.z_val(), cur.blend_mode(), cur.scissor_test());
    img_.Draw(r);
    r->PopParams();
}
