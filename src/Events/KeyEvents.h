//
// Created by kvtoD on 6/30/2025.
//

#ifndef KEYEVENTS_H
#define KEYEVENTS_H

enum Key {
    Key_0,
    Key_1,
    Key_2,
    Key_3,
    Key_4,
    Key_5,
    Key_6,
    Key_7,
    Key_8,
    Key_9,
    Key_A,
    Key_B,
    Key_C,
    Key_D,
    Key_E,
    Key_F,
    Key_G,
    Key_H,
    Key_I,
    Key_J,
    Key_K,
    Key_L,
    Key_M,
    Key_N,
    Key_O,
    Key_P,
    Key_Q,
    Key_R,
    Key_S,
    Key_T,
    Key_U,
    Key_V,
    Key_W,
    Key_X,
    Key_Y,
    Key_Z,
    Key_Up,
    Key_Down,
    Key_Left,
    Key_Right,
    Key_Enter,
    Key_Escape,
    Key_Tab,
    Key_Space,
    Key_BackSpace,
    Key_Delete,
    Key_Insert,
    Key_Home,
    Key_End,
    Key_PageUp,
    Key_PageDown,
    Key_CapsLock,
    Key_NumLock,
    Key_ScrollLock,
    Key_PrintScreen,
    Key_Pause,
    Key_F1,
    Key_F2,
    Key_F3,
    Key_F4,
    Key_F5,
    Key_F6,
    Key_F7,
    Key_F8,
    Key_F9,
    Key_F10,
    Key_F11,
    Key_F12
};

struct OnKey {
    Key key;
};

struct AtKey {
    Key key;
};


#endif //KEYEVENTS_H