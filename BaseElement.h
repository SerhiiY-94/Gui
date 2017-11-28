#pragma once

#include <bitset>

#include <math/ivec2.hpp>
#include <math/vec2.hpp>

namespace ui {
// forward declare everything
class BitmapFont;
class ButtonBase;
class ButtonImage;
class ButtonText;
class Cursor;
class EditBox;
class Frame;
class Image;
class LinearLayout;
class Renderer;
class TypeMesh;

enum eFlags {
    Visible,
    Resizable
};

class BaseElement {
protected:
    math::vec2           rel_dims_[2];

    math::vec2           dims_[2];
    math::ivec2          dims_px_[2];
    std::bitset<32>     flags_;
public:
    BaseElement(const math::vec2 &pos, const math::vec2 &size, const BaseElement *parent);
    ~BaseElement() {}

    bool visible() const {
        return flags_[Visible];
    }
    bool resizable() const {
        return flags_[Resizable];
    }

    void set_visible(bool v) {
        flags_[Visible] = v;
    }
    void set_resizable(bool v) {
        flags_[Resizable] = v;
    }

    const math::vec2 &rel_pos() const {
        return rel_dims_[0];
    }
    const math::vec2 &rel_size() const {
        return rel_dims_[1];
    }

    const math::vec2 &pos() const {
        return dims_[0];
    }
    const math::vec2 &size() const {
        return dims_[1];
    }

    const math::ivec2 &pos_px() const {
        return dims_px_[0];
    }
    const math::ivec2 &size_px() const {
        return dims_px_[1];
    }

    virtual void Resize(const BaseElement *parent);
    virtual void Resize(const math::vec2 &pos, const math::vec2 &size, const BaseElement *parent);

    virtual bool Check(const math::ivec2 &p) const;
    virtual bool Check(const math::vec2 &p) const;

    virtual void Focus(const math::ivec2 &p) {}
    virtual void Focus(const math::vec2 &p) {}

    virtual void Press(const math::ivec2 &p, bool push) {}
    virtual void Press(const math::vec2 &p, bool push) {}

    virtual void Draw(Renderer *r) {}
};

class RootElement : public BaseElement {
public:
    RootElement(const math::ivec2 &zone_size) : BaseElement( {
        -1, -1
    }, { 2, 2 }, nullptr) {
        set_zone(zone_size);
        Resize({ -1, -1 }, { 2, 2 }, nullptr);
    }

    void set_zone(const math::ivec2 &zone_size) {
        dims_px_[1] = zone_size;
    }

    void Resize(const BaseElement *parent) override {
        Resize(dims_[0], dims_[1], parent);
    }

    void Resize(const math::vec2 &pos, const math::vec2 &size, const BaseElement *parent) override {
        dims_[0] = pos;
        dims_[1] = size;
    }
};
}
