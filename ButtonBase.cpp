#include "ButtonBase.h"

ui::ButtonBase::ButtonBase(const math::vec2 &pos, const math::vec2 &size, const BaseElement *parent)
    : BaseElement(pos, size, parent), state_(ST_NORMAL) {
}

void ui::ButtonBase::Focus(const math::ivec2 &p) {
    if (state_ != ST_PRESSED) {
        if (Check(p)) {
            state_ = ST_FOCUSED;
        } else {
            state_ = ST_NORMAL;
        }
    }
}

void ui::ButtonBase::Focus(const math::vec2 &p) {
    if (state_ != ST_PRESSED) {
        if (Check(p)) {
            state_ = ST_FOCUSED;
        } else {
            state_ = ST_NORMAL;
        }
    }
}

void ui::ButtonBase::Press(const math::ivec2 &p, bool push) {
    if (state_ != ST_NORMAL) {
        if (Check(p)) {
            if (push) {
                state_ = ST_PRESSED;
            } else {
                pressed_signal.FireN();
                state_ = ST_FOCUSED;
            }
        } else {
            state_ = ST_NORMAL;
        }
    }
}

void ui::ButtonBase::Press(const math::vec2 &p, bool push) {
    if (state_ != ST_NORMAL) {
        if (Check(p)) {
            if (push) {
                state_ = ST_PRESSED;
            } else {
                pressed_signal.FireN();
                state_ = ST_FOCUSED;
            }
        } else {
            state_ = ST_NORMAL;
        }
    }
}