#include "Renderer.h"

#include <Ren/Context.h>
#include <Ren/SW/SW.h>
#include <Sys/Json.h>

namespace UIRendererConstants {
enum {
    A_POS,
    A_UV
};

enum { V_UV };

enum { U_COL };

enum { DIFFUSEMAP_SLOT };

inline void BindTexture(int slot, uint32_t tex) {
    swActiveTexture((SWenum)(SW_TEXTURE0 + slot));
    swBindTexture((SWint)tex);
}
}

extern "C" {
    VSHADER ui_program_vs(VS_IN, VS_OUT) {
        using namespace math;
        using namespace UIRendererConstants;

        vec2 uv = make_vec2(V_FATTR(A_UV));
        V_FVARYING(V_UV)[0] = uv[0];
        V_FVARYING(V_UV)[1] = uv[1];

        vec3 pos = make_vec3(V_FATTR(A_POS));
        V_POS_OUT[0] = pos[0];
        V_POS_OUT[1] = pos[1];
        V_POS_OUT[2] = pos[2];
        V_POS_OUT[3] = 1;
    }

    FSHADER ui_program_fs(FS_IN, FS_OUT) {
        using namespace math;
        using namespace UIRendererConstants;

        float rgba[4];
        TEXTURE(DIFFUSEMAP_SLOT, F_FVARYING_IN(V_UV), rgba);

        vec4 col = make_vec4(rgba) * vec4(make_vec3(F_UNIFORM(U_COL)), 1);
        F_COL_OUT[0] = col[0];
        F_COL_OUT[1] = col[1];
        F_COL_OUT[2] = col[2];
        F_COL_OUT[3] = col[3];
    }
}

Gui::Renderer::Renderer(Ren::Context &ctx, const JsObject &config) : ctx_(ctx) {
    using namespace UIRendererConstants;

    Ren::Attribute attrs[] = { { "pos", A_POS, SW_VEC3, 1 }, { "uvs", A_UV, SW_VEC2, 1 }, {} };
    Ren::Uniform unifs[] = { { "col", U_COL, SW_VEC3, 1 }, {} };
    ui_program_ = ctx.LoadProgramSW(UI_PROGRAM_NAME, (void *)ui_program_vs, (void *)ui_program_fs, 2,
                                    attrs, unifs, nullptr);
}

Gui::Renderer::~Renderer() {

}

void Gui::Renderer::BeginDraw() {
    using namespace UIRendererConstants;

    Ren::Program *p = ui_program_.get();

    swUseProgram(p->prog_id());
    const math::vec3 white = { 1, 1, 1 };
    swSetUniform(U_COL, SW_VEC3, math::value_ptr(white));

    swBindBuffer(SW_ARRAY_BUFFER, 0);
    swBindBuffer(SW_INDEX_BUFFER, 0);

    swDisable(SW_PERSPECTIVE_CORRECTION);
    swDisable(SW_FAST_PERSPECTIVE_CORRECTION);
    swDisable(SW_DEPTH_TEST);
    swEnable(SW_BLEND);

    math::ivec2 scissor_test[2] = { { 0, 0 }, { ctx_.w(), ctx_.h() } };
    this->EmplaceParams(math::vec3(1, 1, 1), 0.0f, BL_ALPHA, scissor_test);
}

void Gui::Renderer::EndDraw() {
    swEnable(SW_FAST_PERSPECTIVE_CORRECTION);
    swEnable(SW_DEPTH_TEST);
    swDisable(SW_BLEND);

    this->PopParams();
}

void Gui::Renderer::DrawImageQuad(const Ren::Texture2DRef &tex, const math::vec2 dims[2], const math::vec2 uvs[2]) {
    using namespace UIRendererConstants;

    const float vertices[] = { dims[0].x, dims[0].y, 0,
                               uvs[0].x, uvs[0].y,

                               dims[0].x, dims[0].y + dims[1].y, 0,
                               uvs[0].x, uvs[1].y,

                               dims[0].x + dims[1].x, dims[0].y + dims[1].y, 0,
                               uvs[1].x, uvs[1].y,

                               dims[0].x + dims[1].x, dims[0].y, 0,
                               uvs[1].x, uvs[0].y
                             };

    const unsigned char indices[] = { 2, 1, 0,
                                      3, 2, 0
                                    };

    BindTexture(DIFFUSEMAP_SLOT, tex->tex_id());

    swVertexAttribPointer(A_POS, sizeof(float) * 3, sizeof(float) * 5, &vertices[0]);
    swVertexAttribPointer(A_UV, sizeof(float) * 2, sizeof(float) * 5, &vertices[3]);
    swDrawElements(SW_TRIANGLES, 6, SW_UNSIGNED_BYTE, indices);
}

void Gui::Renderer::DrawUIElement(const Ren::Texture2DRef &tex, ePrimitiveType prim_type,
                                 const std::vector<float> &pos, const std::vector<float> &uvs,
                                 const std::vector<unsigned char> &indices) {
    using namespace UIRendererConstants;

    if (pos.empty()) return;

    assert(pos.size() / 5 < 0xff);

    const DrawParams &cur_params = params_.back();
    this->ApplyParams(ui_program_, cur_params);

    BindTexture(DIFFUSEMAP_SLOT, tex->tex_id());

    swVertexAttribPointer(A_POS, sizeof(float) * 3, 0, (void *)&pos[0]);
    swVertexAttribPointer(A_UV, sizeof(float) * 2, 0, (void *)&uvs[0]);

    if (prim_type == PRIM_TRIANGLE) {
        swDrawElements(SW_TRIANGLES, (SWuint)indices.size(), SW_UNSIGNED_BYTE, &indices[0]);
    }
}

void Gui::Renderer::ApplyParams(Ren::ProgramRef &p, const DrawParams &params) {
    using namespace UIRendererConstants;

    swSetUniform(U_COL, SW_VEC3, math::value_ptr(params.col()));
}

