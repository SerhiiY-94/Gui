#include "Image.h"

#include <memory>

#include <sys/AssetFile.h>

#include "Renderer.h"

ui::Image::Image(const ren::Texture2DRef &tex, const glm::vec2 uvs[2],
                 const glm::vec2 &pos, const glm::vec2 &size, const BaseElement *parent) :
        BaseElement(pos, size, parent), tex_(tex) {
    uvs_[0] = uvs[0]; uvs_[1] = uvs[1];
}

ui::Image::Image(ren::Context &ctx, const char *tex_name, const glm::vec2 uvs[2],
                 const glm::vec2 &pos, const glm::vec2 &size, const BaseElement *parent) :
        BaseElement(pos, size, parent) {
    uvs_[0] = uvs[0]; uvs_[1] = uvs[1];

    sys::AssetFile in_file(tex_name, sys::AssetFile::IN);
    size_t in_file_size = in_file.size();
    std::unique_ptr<char[]> data(new char[in_file_size]);
    in_file.Read(data.get(), in_file_size);

    ren::Texture2DParams p;
    p.filter = ren::Trilinear;
    p.repeat = ren::Repeat;
    ren::eTexLoadStatus status;
    tex_ = ctx.LoadTexture2D(tex_name, data.get(), (int)in_file_size, p, &status);
    assert(status == ren::TexCreatedFromData);
}

void ui::Image::Draw(Renderer *r) {
    r->DrawImageQuad(tex_, dims_, uvs_);
}