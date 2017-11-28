#pragma once

#include <sys/Signal_.h>

#include "BaseElement.h"

namespace ui {
class ButtonBase : public BaseElement {
protected:
    enum eState { ST_NORMAL, ST_FOCUSED, ST_PRESSED };
    eState state_;
public:
    ButtonBase(const math::vec2 &pos, const math::vec2 &size, const BaseElement *parent);

    void Focus(const math::ivec2 &p) override;
    void Focus(const math::vec2 &p) override;

    void Press(const math::ivec2 &p, bool push) override;
    void Press(const math::vec2 &p, bool push) override;

    sys::Signal<void()> pressed_signal;
};
}

