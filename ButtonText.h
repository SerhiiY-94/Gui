#pragma once

#include <string>

#include "ButtonBase.h"
#include "TypeMesh.h"

namespace ui {
    class ButtonText : public ButtonBase {
    protected:
        TypeMesh type_mesh_;
    public:
        ButtonText(const std::string &text, BitmapFont *font, const math::vec2 &pos, const BaseElement *parent);
	
        bool Check(const math::vec2 &p) const override;
        bool Check(const math::ivec2 &p) const override;

		void Move(const math::vec2 &pos, const BaseElement *parent);

        void Draw(Renderer *r) override;
    };
}

