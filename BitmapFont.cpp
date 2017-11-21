#include "BitmapFont.h"

#include <cassert>
#include <cstring>

#include <sys/AssetFile.h>

#include "BaseElement.h"
#include "Renderer.h"

namespace BitmapFontConstants {
    const int BFG_RS_NONE   = 0x0;
    const int BFG_RS_ALPHA  = 0x1;
    const int BFG_RS_RGB    = 0x2;
    const int BFG_RS_RGBA   = 0x4;

    const int BFG_MAXSTRING = 0xff;

    const int WIDTH_DATA_OFFSET = 20;
    const int MAP_DATA_OFFSET = 276;

    size_t _strnlen(const char *str, size_t maxlen) {
        size_t i;
        for (i = 0; i < maxlen && str[i]; i++);
        return i;
    }
}

std::vector<float> ui::BitmapFont::std_positions, ui::BitmapFont::std_uvs;
std::vector<unsigned char> ui::BitmapFont::std_indices;

ui::BitmapFont::BitmapFont(const char *name, ren::Context *ctx) {
    cur_x_ = cur_y_ = 0;
    r_ = g_ = b_ = 1.0f;
    invert_y_ = false;
    scale_ = 1.0f;

    if (name && ctx) {
        this->Load(name, *ctx);
    }
}

void ui::BitmapFont::set_sharp(bool b) {
#if !defined(USE_SW_RENDER)
    tex_->ChangeFilter(b ? ren::NoFilter : ren::Bilinear, ren::ClampToEdge);
#endif
}

bool ui::BitmapFont::Load(const char *fname, ren::Context &ctx) {
    using namespace BitmapFontConstants;

    std::vector<char> dat, img;
    unsigned long file_size;
    char bpp;
    int img_x, img_y;

    try {
        sys::AssetFile in(fname, sys::AssetFile::IN);

        file_size = in.size();
        dat.resize(file_size);

        in.Read(&dat[0], file_size);
    } catch (...) {
        return false;
    }

    // Check ID is 'BFF2'
    if ((unsigned char) dat[0] != 0xBF || (unsigned char) dat[1] != 0xF2) {
        return false;
    }

    // Grab the rest of the header
    memcpy(&img_x, &dat[2], sizeof(int));
    memcpy(&img_y, &dat[6], sizeof(int));
    memcpy(&cell_x_, &dat[10], sizeof(int));
    memcpy(&cell_y_, &dat[14], sizeof(int));
    bpp = dat[18];
    base_ = dat[19];

    // Check filesize
    if (file_size != ((MAP_DATA_OFFSET) + ((img_x * img_y) * (bpp / 8)))) {
        return false;
    }

    // Calculate font params
    row_pitch_ = img_x / cell_x_;
    col_factor_ = (float) cell_x_ / (float) img_x;
    row_factor_ = (float) cell_y_ / (float) img_y;
    y_offset_ = cell_y_;

    // Determine blending options based on BPP
    switch (bpp) {
        case 8: // Greyscale
            render_style_ = BFG_RS_ALPHA;
            break;
        case 24: // RGB
            render_style_ = BFG_RS_RGB;
            break;
        case 32: // RGBA
            render_style_ = BFG_RS_RGBA;
            break;
        default: // Unsupported BPP
            return false;
    }

    // Allocate space for image
    img.resize((size_t) (img_x * img_y) * (bpp / 8));

    // Grab char widths
    memcpy(width_, &dat[WIDTH_DATA_OFFSET], 256);

    // Grab image data
    memcpy(&img[0], &dat[MAP_DATA_OFFSET], (size_t) (img_x * img_y) * (bpp / 8));

    ren::Texture2DParams p;
    p.w = img_x;
    p.h = img_y;
    p.filter = ren::NoFilter;
    p.repeat = ren::ClampToEdge;
    // Tex creation params are dependent on BPP
    switch (render_style_) {
        case BFG_RS_ALPHA:
#if !defined(USE_SW_RENDER)
            p.format = ren::RawLUM8;
#else
            assert(false);
#endif
            break;
        case BFG_RS_RGB:
            p.format = ren::RawRGB888;
            break;
        case BFG_RS_RGBA:
            p.format = ren::RawRGBA8888;
            break;
        default:
            return false;
    }

    tex_ = ctx.LoadTexture2D(fname, &img[0], 0, p, nullptr);

    return true;
}

void ui::BitmapFont::SetCursor(int x, int y) {
    cur_x_ = x;
    cur_y_ = y;
}

void ui::BitmapFont::ReverseYAxis(bool state) {
    if (state) {
        y_offset_ = -cell_y_;
    } else {
        y_offset_ = cell_y_;
    }
    invert_y_ = state;
}

float ui::BitmapFont::GetTriangles(const char *text, std::vector<float> &positions, std::vector<float> &uvs,
                                   std::vector<unsigned char> &indices, const math::vec2 &pos, const BaseElement *parent) {
    using namespace BitmapFontConstants;
    using namespace math;

    int len;
    int row, col;
    float u, v, u1, v1;

    len = (int) _strnlen(text, BFG_MAXSTRING);

	if (!len) {
		positions.clear();
		return 0.0f;
	}

    char w1251str[BFG_MAXSTRING];
    strcpy(w1251str, text);
    /*convert_utf8_to_windows1251(text, w1251str, BFG_MAXSTRING);
    len = (int) _strnlen(w1251str, BFG_MAXSTRING);*/

    positions.resize((size_t) len * 12);
    uvs.resize((size_t) len * 8);
    indices.resize((size_t) len * 6);

    vec2 p = parent->pos() + 0.5f * (pos + vec2(1, 1)) * parent->size();
    vec2 m = parent->size() / (vec2)parent->size_px();

    cur_x_ = 0, cur_y_ = 0;

    for (int i = 0; i < len; i++) {
        int char_code = w1251str[i];
        if (char_code < 0) {
            char_code += 256;
        }

        row = (char_code - base_) / row_pitch_;
        col = (char_code - base_) - row * row_pitch_;

        u = col * col_factor_;
        v = row * row_factor_;
        u1 = u + col_factor_;
        v1 = v + row_factor_;

        uvs[i * 8 + 0] = u;
        uvs[i * 8 + 1] = v1;
        uvs[i * 8 + 2] = u1;
        uvs[i * 8 + 3] = v1;
        uvs[i * 8 + 4] = u1;
        uvs[i * 8 + 5] = v;
        uvs[i * 8 + 6] = u;
        uvs[i * 8 + 7] = v;

        positions[i * 12 + 0] = p.x + cur_x_ * m.x;
        positions[i * 12 + 1] = p.y + cur_y_ * m.y;
        positions[i * 12 + 3] = p.x + (cur_x_ + cell_x_ * scale_) * m.x;
        positions[i * 12 + 4] = p.y + cur_y_ * m.y;
        positions[i * 12 + 6] = p.x + (cur_x_ + cell_x_ * scale_) * m.x;
        positions[i * 12 + 7] = p.y + (cur_y_ + y_offset_ * scale_) * m.y;
        positions[i * 12 + 9] = p.x + cur_x_ * m.x;
        positions[i * 12 + 10] = p.y + (cur_y_ + y_offset_ * scale_) * m.y;

        indices[i * 6 + 0] = (unsigned char) (4 * i + 1);
        indices[i * 6 + 1] = (unsigned char) (4 * i + 2);
        indices[i * 6 + 2] = (unsigned char) (4 * i + 0);
		indices[i * 6 + 3] = (unsigned char)(4 * i + 3);
		indices[i * 6 + 4] = (unsigned char)(4 * i + 0);
		indices[i * 6 + 5] = (unsigned char)(4 * i + 2);

        cur_x_ += int(width_[char_code] * scale_);
    }
    return cur_x_ * m.x;

}

float ui::BitmapFont::GetWidth(const char *text, const BaseElement *parent) {
    return GetTriangles(text, std_positions, std_uvs, std_indices, { 0, 0 }, parent);
}

float ui::BitmapFont::height(const BaseElement *parent) const {
    return y_offset_ * scale_ * parent->size().y / parent->size_px().y;
}

int ui::BitmapFont::blend_mode() const {
    return (render_style_ == BitmapFontConstants::BFG_RS_ALPHA) ? ui::BL_COLOR : ui::BL_ALPHA;
}

void ui::BitmapFont::DrawText(Renderer *r, const char *text, const math::vec2 &pos, const BaseElement *parent) {
    GetTriangles(text, std_positions, std_uvs, std_indices, pos, parent);
    if (std_positions.empty()) {
        return;
    }

	const auto &cur = r->GetParams();

    r->EmplaceParams(cur.col(), cur.z_val(), (eBlendMode)blend_mode(), cur.scissor_test());
    r->DrawUIElement(tex_, ui::PRIM_TRIANGLE, std_positions, std_uvs, std_indices);
    r->PopParams();
}
