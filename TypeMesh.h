#pragma once

#include <string>
#include <vector>

#include "BaseElement.h"

namespace ui {
    class TypeMesh : public BaseElement {
        std::string text_;
        std::vector<float> pos_, uvs_;
        std::vector<unsigned char> indices_;

        math::vec2 center_;

        BitmapFont *font_;
    public:
        TypeMesh(const std::string &text, BitmapFont *font, const math::vec2 &pos, const BaseElement *parent);

        const std::string &text() const { return text_; }
        const std::vector<float> &positions() const { return pos_; }
        const std::vector<float> &uvs() const { return uvs_; }

        void Centrate();

        void Move(const math::vec2 &pos, const BaseElement *parent);

        void Resize(const BaseElement *parent) override;
        //void Resize(const math::vec2 &pos, const math::vec2 &size, const BaseElement *parent) override;

        void Draw(Renderer *r) override;
    };
}

