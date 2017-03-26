#include "Renderer.h"

#include <sys/Json.h>
#include <ren/RenderState.h>
#include <ren/SW/SW.h>

using namespace glm;

namespace UIRendererConstants {
    enum { A_POS,
           A_UV };

    enum { V_UV };

    enum { DIFFUSEMAP_SLOT };
}

extern "C" {
	VSHADER ui_program_vs(VS_IN, VS_OUT) {
        using namespace UIRendererConstants;

		V_FVARYING(V_UV)[0] = V_FATTR(A_UV)[0];
		V_FVARYING(V_UV)[1] = V_FATTR(A_UV)[1];
		V_FVARYING(V_UV)[2] = V_FATTR(A_UV)[2];

		V_POS_OUT[0] = V_FATTR(A_POS)[0];
		V_POS_OUT[1] = V_FATTR(A_POS)[1];
		V_POS_OUT[2] = V_FATTR(A_POS)[2];
		V_POS_OUT[3] = 1.0f;
	}

	FSHADER ui_program_fs(FS_IN, FS_OUT) {
        using namespace UIRendererConstants;

		SWfloat uv[2];
		uv[0] = F_FVARYING_IN(V_UV)[0];
		uv[1] = F_FVARYING_IN(V_UV)[1];

		SWfloat rgba[4];
		TEXTURE(DIFFUSEMAP_SLOT, uv, rgba);

		F_COL_OUT[0] = rgba[0];
		F_COL_OUT[1] = rgba[1];
		F_COL_OUT[2] = rgba[2];
		F_COL_OUT[3] = rgba[3];

	}
}

ui::Renderer::Renderer(const JsObject &config) {
	ui_program_ = R::CreateProgramSW(UI_PROGRAM_NAME, (void *)ui_program_vs, (void *)ui_program_fs, 2);

    /*const JsString &js_local_dir = (const JsString &) config.at(LOCAL_DIR_KEY);
    const JsString &js_gl_defines = (const JsString &) config.at(GL_DEFINES_KEY);

    { // Load main shader
        std::string vs_file_name = js_local_dir.val + "/" + UI_PROGRAM_NAME + ".vs",
                fs_file_name = js_local_dir.val + "/" + UI_PROGRAM_NAME + ".fs";
        sys::AssetFile vs_file(vs_file_name.c_str(), sys::AssetFile::IN),
                fs_file(fs_file_name.c_str(), sys::AssetFile::IN);
        size_t vs_file_size = vs_file.size(),
                fs_file_size = fs_file.size();
        std::string vs_source,
                fs_source;
        vs_source.resize(vs_file_size);
        fs_source.resize(fs_file_size);

        vs_file.Read((char *)vs_source.data(), vs_file_size);
        fs_file.Read((char *)fs_source.data(), fs_file_size);

        R::eProgramLoadStatus status;
        ui_program_ = R::LoadProgramGLSL(UI_PROGRAM_NAME, vs_source.data(), fs_source.data(), &status);
        assert(status == R::PROG_CREATED_FROM_DATA);
    }

    glGenBuffers(1, &attribs_buf_id_);
    glBindBuffer(GL_ARRAY_BUFFER, attribs_buf_id_);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat) + 8 * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &indices_buf_id_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buf_id_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLubyte), nullptr, GL_DYNAMIC_DRAW);*/
}

ui::Renderer::~Renderer() {
    /*glDeleteBuffers(1, &attribs_buf_id_);
    glDeleteBuffers(1, &indices_buf_id_);*/
}

void ui::Renderer::BeginDraw() {
	R::Program *p = R::GetProgram(ui_program_);

	R::UseProgram(p->prog_id);

	swBindBuffer(SW_ARRAY_BUFFER, 0);
	swBindBuffer(SW_INDEX_BUFFER, 0);

    swEnable(SW_BLEND);

	ivec2 scissor_test[2] = { { 0, 0 }, { R::w, R::h } };
	this->EmplaceParams(vec3(1, 1, 1), 0.0f, BL_ALPHA, scissor_test);

    /*R::Program *p = R::GetProgram(ui_program_);

    R::UseProgram(p->prog_id);

    glEnableVertexAttribArray((GLuint)p->attribute(0));
    glEnableVertexAttribArray((GLuint)p->attribute(1));

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ARRAY_BUFFER, attribs_buf_id_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buf_id_);*/
}

void ui::Renderer::EndDraw() {

	swEnable(SW_DEPTH_TEST);
    swDisable(SW_BLEND);

	this->PopParams();

    /*R::PrintGLError();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
}

void ui::Renderer::DrawImageQuad(const R::Texture2DRef &tex, const vec2 dims[2], const vec2 uvs[2]) {
    using namespace UIRendererConstants;

    float vertices[] = {dims[0].x, dims[0].y, 0,
						uvs[0].x, uvs[0].y,

						dims[0].x, dims[0].y + dims[1].y, 0,
						uvs[0].x, uvs[1].y,

						dims[0].x + dims[1].x, dims[0].y + dims[1].y, 0,
						uvs[1].x, uvs[1].y,

						dims[0].x + dims[1].x, dims[0].y, 0,
						uvs[1].x, uvs[0].y};

    unsigned char indices[] = {2, 1, 0,
                               3, 2, 0};

    R::BindTexture(DIFFUSEMAP_SLOT, tex.tex_id);

	swVertexAttribPointer(A_POS, sizeof(float) * 3, sizeof(float) * 5, &vertices[0]);
	swVertexAttribPointer(A_UV, sizeof(float) * 2, sizeof(float) * 5, &vertices[3]);
	swDrawElements(SW_TRIANGLES, 6, SW_UNSIGNED_BYTE, indices);
}

void ui::Renderer::DrawUIElement(const R::Texture2DRef &tex, ePrimitiveType prim_type,
								 const std::vector<float> &pos, const std::vector<float> &uvs,
								 const std::vector<unsigned char> &indices) {
    using namespace UIRendererConstants;

    if (pos.empty()) return;

    assert(pos.size() / 5 < 0xff);

    R::Program *p = R::GetProgram(ui_program_);

    const DrawParams &cur_params = params_.back();
    this->ApplyParams(p, cur_params);

    R::BindTexture(DIFFUSEMAP_SLOT, tex.tex_id);

    swVertexAttribPointer(A_POS, sizeof(float) * 3, 0, (void *)&pos[0]);
    swVertexAttribPointer(A_UV, sizeof(float) * 2, 0, (void *)&uvs[0]);

    if (prim_type == PRIM_TRIANGLE) {
        swDrawElements(SW_TRIANGLES, (SWuint)indices.size(), SW_UNSIGNED_BYTE, &indices[0]);
    }
}

void ui::Renderer::ApplyParams(R::Program *p, const DrawParams &params) {

}

