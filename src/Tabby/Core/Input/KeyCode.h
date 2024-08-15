#pragma once

namespace Tabby {
using MouseCode = uint32_t;
using GamepadAxis = int32_t;
using GamepadButtons = uint32_t;
using KeyCode = uint32_t;

namespace Mouse {
    enum : MouseCode {
        Button0 = 1, // SDL_BUTTON_LEFT
        Button1 = 3, // SDL_BUTTON_RIGHT
        Button2 = 2, // SDL_BUTTON_MIDDLE
        Button3 = 4, // SDL_BUTTON_X1
        Button4 = 5, // SDL_BUTTON_X2
        Button5 = 5,
        Button6 = 6,
        Button7 = 7,

        ButtonLast = Button7,
        ButtonLeft = Button0,
        ButtonRight = Button1,
        ButtonMiddle = Button2
    };
}

namespace Key {
    // Copied from sdl
    enum : KeyCode {
        A = 4,
        B = 5,
        C = 6,
        D = 7,
        E = 8,
        F = 9,
        G = 10,
        H = 11,
        I = 12,
        J = 13,
        K = 14,
        L = 15,
        M = 16,
        N = 17,
        O = 18,
        P = 19,
        Q = 20,
        R = 21,
        S = 22,
        T = 23,
        U = 24,
        V = 25,
        W = 26,
        X = 27,
        Y = 28,
        Z = 29,

        D1 = 30,
        D2 = 31,
        D3 = 32,
        D4 = 33,
        D5 = 34,
        D6 = 35,
        D7 = 36,
        D8 = 37,
        D9 = 38,
        D0 = 39,

        Return = 40,
        Escape = 41,
        Backspace = 42,
        Tab = 43,
        Space = 44,

        Minus = 45,
        Equals = 46,
        Leftbracket = 47,
        Rightbracket = 48,
        Backslash = 49, /**< Located at the lower left of the return
                         *   key on ISO keyboards and at the right end
                         *   of the QWERTY row on ANSI keyboards.
                         *   Produces REVERSE SOLIDUS (backslash) and
                         *   VERTICAL LINE in a US layout, REVERSE
                         *   SOLIDUS and VERTICAL LINE in a UK Mac
                         *   layout, NUMBER SIGN and TILDE in a UK
                         *   Windows layout, DOLLAR SIGN and POUND SIGN
                         *   in a Swiss German layout, NUMBER SIGN and
                         *   APOSTROPHE in a German layout, GRAVE
                         *   ACCENT and POUND SIGN in a French Mac
                         *   layout, and ASTERISK and MICRO SIGN in a
                         *   French Windows layout.
                         */
        Nonushash = 50, /**< ISO USB keyboards actually use this code
                         *   instead of 49 for the same key, but all
                         *   OSes I've seen treat the two codes
                         *   identically. So, as an implementor, unless
                         *   your keyboard generates both of those
                         *   codes and your OS treats them differently,
                         *   you should generate SDL_SCANCODE_BACKSLASH
                         *   instead of this code. As a user, you
                         *   should not rely on this code because SDL
                         *   will never generate it with most (all?)
                         *   keyboards.
                         */
        Semicolon = 51,
        ApostrophE = 52,
        Grave = 53, /**< Located in the top left corner (on both ANSI
                     *   and ISO keyboards). Produces GRAVE ACCENT and
                     *   TILDE in a US Windows layout and in US and UK
                     *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                     *   and NOT SIGN in a UK Windows layout, SECTION
                     *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                     *   layouts on ISO keyboards, SECTION SIGN and
                     *   DEGREE SIGN in a Swiss German layout (Mac:
                     *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                     *   DEGREE SIGN in a German layout (Mac: only on
                     *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                     *   French Windows layout, COMMERCIAL AT and
                     *   NUMBER SIGN in a French Mac layout on ISO
                     *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                     *   SIGN in a Swiss German, German, or French Mac
                     *   layout on ANSI keyboards.
                     */
        Comma = 54,
        Period = 55,
        Slash = 56,

        Capslock = 57,

        F1 = 58,
        F2 = 59,
        F3 = 60,
        F4 = 61,
        F5 = 62,
        F6 = 63,
        F7 = 64,
        F8 = 65,
        F9 = 66,
        F10 = 67,
        F11 = 68,
        F12 = 69,

        Printscreen = 70,
        Scrolllock = 71,
        Pause = 72,
        Insert = 73, /**< insert on PC, help on some Mac keyboards (but
                          does send code 73, not 117) */
        Home = 74,
        Pageup = 75,
        Delete = 76,
        End = 77,
        Pagedown = 78,
        Right = 79,
        Left = 80,
        Down = 81,
        Up = 82,

        Numlockclear = 83, /**< num lock on PC, clear on Mac keyboards
                            */
        KpDivide = 84,
        KpMultiply = 85,
        KpMinus = 86,
        KpPlus = 87,
        KpEnter = 88,
        Kp1 = 89,
        Kp2 = 90,
        Kp3 = 91,
        Kp4 = 92,
        Kp5 = 93,
        Kp6 = 94,
        Kp7 = 95,
        Kp8 = 96,
        Kp9 = 97,
        Kp0 = 98,
        KpPeriod = 99,

        Nonusbackslash = 100, /**< This is the additional key that ISO
                               *   keyboards have over ANSI ones,
                               *   located between left shift and Y.
                               *   Produces GRAVE ACCENT and TILDE in a
                               *   US or UK Mac layout, REVERSE SOLIDUS
                               *   (backslash) and VERTICAL LINE in a
                               *   US or UK Windows layout, and
                               *   LESS-THAN SIGN and GREATER-THAN SIGN
                               *   in a Swiss German, German, or French
                               *   layout. */
        Application = 101, /**< windows contextual menu, compose */
        Power = 102, /**< The USB document says this is a status flag,
                      *   not a physical key - but some Mac keyboards
                      *   do have a power key. */
        Kp_equals = 103,
        F13 = 104,
        F14 = 105,
        F15 = 106,
        F16 = 107,
        F17 = 108,
        F18 = 109,
        F19 = 110,
        F20 = 111,
        F21 = 112,
        F22 = 113,
        F23 = 114,
        F24 = 115,
        Execute = 116,
        Help = 117, /**< AL Integrated Help Center */
        Menu = 118, /**< Menu (show menu) */
        Select = 119,
        Stop = 120, /**< AC Stop */
        Again = 121, /**< AC Redo/Repeat */
        Undo = 122, /**< AC Undo */
        Cut = 123, /**< AC Cut */
        Copy = 124, /**< AC Copy */
        Paste = 125, /**< AC Paste */
        Find = 126, /**< AC Find */
        Mute = 127,
        Volumeup = 128,
        Volumedown = 129,
        /* not sure whether there's a reason to enable these */
        /*     SDL_SCANCODE_LOCKINGCAPSLOCK = 130,  */
        /*     SDL_SCANCODE_LOCKINGNUMLOCK = 131, */
        /*     SDL_SCANCODE_LOCKINGSCROLLLOCK = 132, */
        KpComma = 133,
        KpEqualsas400 = 134,

        International1 = 135, /**< used on Asian keyboards, see
                                   footnotes in USB doc */
        International2 = 136,
        International3 = 137, /**< Yen */
        International4 = 138,
        International5 = 139,
        International6 = 140,
        International7 = 141,
        International8 = 142,
        International9 = 143,
        Lang1 = 144, /**< Hangul/English toggle */
        Lang2 = 145, /**< Hanja conversion */
        Lang3 = 146, /**< Katakana */
        Lang4 = 147, /**< Hiragana */
        Lang5 = 148, /**< Zenkaku/Hankaku */
        Lang6 = 149, /**< reserved */
        Lang7 = 150, /**< reserved */
        Lang8 = 151, /**< reserved */
        Lang9 = 152, /**< reserved */

        Alterase = 153, /**< Erase-Eaze */
        Sysreq = 154,
        Cancel = 155, /**< ac Cancel */
        Clear = 156,
        Prior = 157,
        Return2 = 158,
        Separator = 159,
        Out = 160,
        Oper = 161,
        Clearagain = 162,
        Crsel = 163,
        Exsel = 164,

        Kp00 = 176,
        Kp000 = 177,
        Thousandsseparator = 178,
        Decimalseparator = 179,
        Currencyunit = 180,
        Currencysubunit = 181,
        KpLeftparen = 182,
        KpRightparen = 183,
        KpLeftbrace = 184,
        KpRightbrace = 185,
        KpTab = 186,
        KpBackspace = 187,
        KpA = 188,
        KpB = 189,
        KpC = 190,
        KpD = 191,
        KpE = 192,
        KpF = 193,
        KpXor = 194,
        KpPower = 195,
        KpPercent = 196,
        KpLess = 197,
        KpGreater = 198,
        KpAmpersand = 199,
        KpDblampersand = 200,
        KpVerticalbar = 201,
        KpDblverticalbar = 202,
        KpColon = 203,
        KpHash = 204,
        KpSpace = 205,
        KpAt = 206,
        KpExclam = 207,
        KpMemstore = 208,
        KpMemrecall = 209,
        KpMemclear = 210,
        KpMemadd = 211,
        KpMemsubtract = 212,
        KpMemmultiply = 213,
        KpMemdivide = 214,
        KpPlusminus = 215,
        KpClear = 216,
        KpClearentry = 217,
        KpBinary = 218,
        KpOctal = 219,
        KpDecimal = 220,
        KpHexadecimal = 221,

        LControl = 224,
        LShift = 225,
        LAlt = 226, /**< alt, option */
        LSuper = 227, /**< windows, command (apple), meta */
        RCtrl = 228,
        RShift = 229,
        RAlt = 230, /**< alt gr, option */
        RSuper = 231, /**< windows, command (apple), meta */

        Mode = 257, /**< I'm not sure if this is really not covered
                     *   by any of the above, but since there's a
                     *   special KMOD_MODE for it I'm adding it here
                     */

        /* @} */ /* Usage page 0x07 */

        /**
         *  \name Usage page 0x0C
         *
         *  These values are mapped from usage page 0x0C (USB consumer page).
         *  See https://usb.org/sites/default/files/hut1_2.pdf
         *
         *  There are way more keys in the spec than we can represent in the
         *  current scancode range, so pick the ones that commonly come up in
         *  real world usage.
         */
        /* @{ */

        Audionext = 258,
        Audioprev = 259,
        Audiostop = 260,
        Audioplay = 261,
        Audiomute = 262,
        Mediaselect = 263,
        WWW = 264, /**< AL Internet Browser */
        Mail = 265,
        Calculator = 266, /**< AL Calculator */
        Computer = 267,
        ACSearch = 268, /**< AC Search */
        ACHome = 269, /**< AC Home */
        ACBack = 270, /**< AC Back */
        ACForward = 271, /**< AC Forward */
        ACStop = 272, /**< AC Stop */
        ACRefresh = 273, /**< AC Refresh */
        ACBookmarks = 274, /**< AC Bookmarks */

        /* @} */ /* Usage page 0x0C */

        /**
         *  \name Walther keys
         *
         *  These are values that Christian Walther added (for mac keyboard?).
         */
        /* @{ */

        Brightnessdown = 275,
        Brightnessup = 276,
        Displayswitch = 277, /**< display mirroring/dual display
                                  switch, video mode switch */
        Kbdillumtoggle = 278,
        Kbdillumdown = 279,
        Kbdillumup = 280,
        Eject = 281,
        Sleep = 282, /**< SC System Sleep */

        App1 = 283,
        App2 = 284,

        /* @} */ /* Walther keys */

        /**
         *  \name Usage page 0x0C (additional media keys)
         *
         *  These values are mapped from usage page 0x0C (USB consumer page).
         */
        /* @{ */

        Audiorewind = 285,
        Audiofastforward = 286,

        /* @} */ /* Usage page 0x0C (additional media keys) */

        /**
         *  \name Mobile keys
         *
         *  These are values that are often used on mobile phones.
         */
        /* @{ */

        Softleft = 287, /**< Usually situated below the display on phones and
                             used as a multi-function feature key for selecting
                             a software defined function shown on the bottom left
                             of the display. */
        Softright = 288, /**< Usually situated below the display on phones and
                              used as a multi-function feature key for selecting
                              a software defined function shown on the bottom right
                              of the display. */
        Call = 289, /**< Used for accepting phone calls. */
        Endcall = 290, /**< Used for rejecting phone calls. */
        // Space = 44,
        // Apostrophe = 52,
        // Comma = 54,
        // Minus = 45,
        // Period = 55,
        // Slash = 56,
        // D0 = 39,
        // D1 = 30,
        // D2 = 31,
        // D3 = 32,
        // D4 = 33,
        // D5 = 34,
        // D6 = 35,
        // D7 = 36,
        // D8 = 37,
        // D9 = 38,
        // Semicolon = 51,
        // Equal = 46,
        // A = 4,
        // B = 5,
        // C = 6,
        // D = 7,
        // E = 8,
        // F = 9,
        // G = 10,
        // H = 11,
        // I = 12,
        // J = 13,
        // K = 14,
        // L = 15,
        // M = 16,
        // N = 17,
        // O = 18,
        // P = 19,
        // Q = 20,
        // R = 21,
        // S = 22,
        // T = 23,
        // U = 24,
        // V = 25,
        // W = 26,
        // X = 27,
        // Y = 28,
        // Z = 29,
        // LeftBracket = 47,
        // Backslash = 49,
        // RightBracket = 48,
        // GraveAccent = 53,
        // Escape = 41,
        // Enter = 40,
        // Tab = 43,
        // Backspace = 42,
        // Insert = 73,
        // Delete = 76,
        // Right = 79,
        // Left = 80,
        // Down = 81,
        // Up = 82,
        // PageUp = 75,
        // PageDown = 78,
        // Home = 74,
        // End = 77,
        // CapsLock = 57,
        // ScrollLock = 71,
        // NumLock = 83,
        // PrintScreen = 70,
        // Pause = 72,
        // F1 = 58,
        // F2 = 59,
        // F3 = 60,
        // F4 = 61,
        // F5 = 62,
        // F6 = 63,
        // F7 = 64,
        // F8 = 65,
        // F9 = 66,
        // F10 = 67,
        // F11 = 68,
        // F12 = 69,
        // F13 = 104,
        // F14 = 105,
        // F15 = 106,
        // F16 = 107,
        // F17 = 108,
        // F18 = 109,
        // F19 = 110,
        // F20 = 111,
        // F21 = 112,
        // F22 = 113,
        // F23 = 114,
        // F24 = 115,
        // KP1 = 89,
        // KP2 = 90,
        // KP3 = 91,
        // KP4 = 92,
        // KP5 = 93,
        // KP6 = 94,
        // KP7 = 95,
        // KP8 = 96,
        // KP9 = 97,
        // KP0 = 98,
        // KPDecimal = 220,
        // KPDivide = 84,
        // KPMultiply = 85,
        // KPSubtract = 86,
        // KPAdd = 87,
        // KPEnter = 88,
        // KPEqual = 103,
        // LeftShift = 225,
        // LeftControl = 224,
        // LeftAlt = 226,
        // LeftSuper = 227,
        // RightShift = 229,
        // RightControl = 228,
        // RightAlt = 230,
        // RightSuper = 231,
        // Menu = 118
    };
}

namespace Gamepad {

    namespace Axis {
        enum : GamepadAxis {
            GAMEPAD_AXIS_LEFT_X = 0,
            GAMEPAD_AXIS_LEFT_Y = 1,
            GAMEPAD_AXIS_RIGHT_X = 2,
            GAMEPAD_AXIS_RIGHT_Y = 3,
            GAMEPAD_AXIS_LEFT_TRIGGER = 4,
            GAMEPAD_AXIS_RIGHT_TRIGGER = 5
        };
    }

    namespace Buttons {
        enum : GamepadButtons {
            GAMEPAD_BUTTON_RIGHT_FACE_DOWN = 0,
            GAMEPAD_BUTTON_RIGHT_FACE_RIGHT = 1,
            GAMEPAD_BUTTON_RIGHT_FACE_LEFT = 2,
            GAMEPAD_BUTTON_RIGHT_FACE_UP = 3,

            GAMEPAD_BUTTON_MIDDLE_LEFT = 4,
            GAMEPAD_BUTTON_MIDDLE = 5,
            GAMEPAD_BUTTON_MIDDLE_RIGHT = 6,

            GAMEPAD_BUTTON_LEFT_THUMB = 7,
            GAMEPAD_BUTTON_RIGHT_THUMB = 8,

            GAMEPAD_BUTTON_LEFT_SHOULDER = 9,
            GAMEPAD_BUTTON_RIGHT_SHOULDER = 10,

            GAMEPAD_BUTTON_LEFT_FACE_UP = 11,
            GAMEPAD_BUTTON_LEFT_FACE_DOWN = 12,
            GAMEPAD_BUTTON_LEFT_FACE_LEFT = 13,
            GAMEPAD_BUTTON_LEFT_FACE_RIGHT = 14,

            GAMEPAD_BUTTON_LEFT_TRIGGER = 100, // Custom
            GAMEPAD_BUTTON_RIGHT_TRIGGER = 101, // Custom

            // These does nothing
            /* SDL_CONTROLLER_BUTTON_MISC1 */
            /* SDL_CONTROLLER_BUTTON_PADDLE1 */
            /* SDL_CONTROLLER_BUTTON_PADDLE2 */
            /* SDL_CONTROLLER_BUTTON_PADDLE3 */
            /* SDL_CONTROLLER_BUTTON_PADDLE4 */
            /* SDL_CONTROLLER_BUTTON_TOUCHPAD */
        };
    }
}

}
