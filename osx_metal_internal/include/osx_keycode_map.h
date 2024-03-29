#pragma once

#import <GameController/GameController.h>
#include <division_engine_core/types/keycode.h>

#include <memory.h>

#define DIVISION_OSX_KEYCODE_MAP_LETTER(code) \
    [DIVISION_KEYCODE_##code] = GCKeyCodeKey##code

#define DIVISION_OSX_KEYCODE_MAP_KEYPAD(code) \
    [DIVISION_KEYCODE_NUMPAD_##code] = GCKeyCodeKeypad##code

#define DIVISION_OSX_KEYCODE_MAP_ELEMENT(code, gcname) \
    [DIVISION_KEYCODE_##code] = GCKeyCode##gcname

#define DIVISION_OSX_KEYCODE_MAP_EXACT(code) \
    [DIVISION_KEYCODE_##code] = GCKeyCode##code

static inline GCKeyCode* osx_keycode_map_alloc()
{
    GCKeyCode keycode_origin[] = {
        DIVISION_OSX_KEYCODE_MAP_LETTER(Q),
        DIVISION_OSX_KEYCODE_MAP_LETTER(W),
        DIVISION_OSX_KEYCODE_MAP_LETTER(E),
        DIVISION_OSX_KEYCODE_MAP_LETTER(R),
        DIVISION_OSX_KEYCODE_MAP_LETTER(T),
        DIVISION_OSX_KEYCODE_MAP_LETTER(Y),
        DIVISION_OSX_KEYCODE_MAP_LETTER(U),
        DIVISION_OSX_KEYCODE_MAP_LETTER(I),
        DIVISION_OSX_KEYCODE_MAP_LETTER(O),
        DIVISION_OSX_KEYCODE_MAP_LETTER(P),
        DIVISION_OSX_KEYCODE_MAP_LETTER(A),
        DIVISION_OSX_KEYCODE_MAP_LETTER(S),
        DIVISION_OSX_KEYCODE_MAP_LETTER(D),
        DIVISION_OSX_KEYCODE_MAP_LETTER(F),
        DIVISION_OSX_KEYCODE_MAP_LETTER(G),
        DIVISION_OSX_KEYCODE_MAP_LETTER(H),
        DIVISION_OSX_KEYCODE_MAP_LETTER(J),
        DIVISION_OSX_KEYCODE_MAP_LETTER(K),
        DIVISION_OSX_KEYCODE_MAP_LETTER(L),
        DIVISION_OSX_KEYCODE_MAP_LETTER(Z),
        DIVISION_OSX_KEYCODE_MAP_LETTER(X),
        DIVISION_OSX_KEYCODE_MAP_LETTER(C),
        DIVISION_OSX_KEYCODE_MAP_LETTER(V),
        DIVISION_OSX_KEYCODE_MAP_LETTER(B),
        DIVISION_OSX_KEYCODE_MAP_LETTER(N),
        DIVISION_OSX_KEYCODE_MAP_LETTER(M),

        // Let's remember how to write numbers in english :)
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(0, Zero),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(1, One),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(2, Two),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(3, Three),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(4, Four),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(5, Five),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(6, Six),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(7, Seven),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(8, Eight),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(9, Nine),

        [DIVISION_KEYCODE_TILDE] = GCKeyCodeGraveAccentAndTilde,
        [DIVISION_KEYCODE_MINUS] = GCKeyCodeHyphen,
        [DIVISION_KEYCODE_EQUAL] = GCKeyCodeEqualSign,
        [DIVISION_KEYCODE_LSQUARE_BRACKET] = GCKeyCodeOpenBracket,
        [DIVISION_KEYCODE_RSQUARE_BRACKET] = GCKeyCodeCloseBracket,
        [DIVISION_KEYCODE_SEMICOLON] = GCKeyCodeSemicolon,
        [DIVISION_KEYCODE_QUOTE] = GCKeyCodeQuote,
        [DIVISION_KEYCODE_BACKSLASH] = GCKeyCodeBackslash,
        [DIVISION_KEYCODE_COMMA] = GCKeyCodeComma,
        [DIVISION_KEYCODE_DOT] = GCKeyCodePeriod,
        [DIVISION_KEYCODE_SLASH] = GCKeyCodeSlash,
        [DIVISION_KEYCODE_PARAGRAPH] = GCKeyCodeNonUSBackslash,

        DIVISION_OSX_KEYCODE_MAP_KEYPAD(0),
        DIVISION_OSX_KEYCODE_MAP_KEYPAD(1),
        DIVISION_OSX_KEYCODE_MAP_KEYPAD(2),
        DIVISION_OSX_KEYCODE_MAP_KEYPAD(3),
        DIVISION_OSX_KEYCODE_MAP_KEYPAD(4),
        DIVISION_OSX_KEYCODE_MAP_KEYPAD(5),
        DIVISION_OSX_KEYCODE_MAP_KEYPAD(6),
        DIVISION_OSX_KEYCODE_MAP_KEYPAD(7),
        DIVISION_OSX_KEYCODE_MAP_KEYPAD(8),
        DIVISION_OSX_KEYCODE_MAP_KEYPAD(9),

        [DIVISION_KEYCODE_NUMPAD_SUB] = GCKeyCodeKeypadHyphen,
        [DIVISION_KEYCODE_NUMPAD_ADD] = GCKeyCodeKeypadPlus,
        [DIVISION_KEYCODE_NUMPAD_NUM_LOCK] = GCKeyCodeKeypadNumLock,
        [DIVISION_KEYCODE_NUMPAD_MUL] = GCKeyCodeKeypadAsterisk,
        [DIVISION_KEYCODE_NUMPAD_DIV] = GCKeyCodeKeypadSlash,
        [DIVISION_KEYCODE_NUMPAD_ENTER] = GCKeyCodeKeypadEnter,
        [DIVISION_KEYCODE_NUMPAD_EQUAL] = GCKeyCodeKeypadEqualSign,
        [DIVISION_KEYCODE_NUMPAD_DOT] = GCKeyCodeKeypadPeriod,
        
        DIVISION_OSX_KEYCODE_MAP_EXACT(F1),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F2),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F3),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F4),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F5),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F6),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F7),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F8),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F9),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F10),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F11),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F12),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F13),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F14),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F15),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F16),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F17),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F18),
        DIVISION_OSX_KEYCODE_MAP_EXACT(F19),

        DIVISION_OSX_KEYCODE_MAP_ELEMENT(LOPTION, LeftAlt),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(ROPTION, RightAlt),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(LCMD, LeftGUI),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(RCMD, RightGUI),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(LCONTROL, LeftControl),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(RCONTROL, RightControl),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(TAB, Tab),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(LSHIFT, LeftShift),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(RSHIFT, RightShift),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(CAPS_LOCK, CapsLock),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(LEFT_ARROW, LeftArrow),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(RIGHT_ARROW, RightArrow),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(UP_ARROW, UpArrow),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(DOWN_ARROW, DownArrow),

        DIVISION_OSX_KEYCODE_MAP_ELEMENT(HOME, Home),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(END, End),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(PAGE_DOWN, PageDown),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(PAGE_UP, PageUp),

        DIVISION_OSX_KEYCODE_MAP_ELEMENT(ENTER, ReturnOrEnter),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(SPACE, Spacebar),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(BACKSPACE, DeleteOrBackspace),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(DELETE, DeleteForward),
        DIVISION_OSX_KEYCODE_MAP_ELEMENT(ESC, Escape),
    };

    GCKeyCode* keycode_map = malloc(sizeof(GCKeyCode) * DIVISION_KEYCODE_COUNT);
    memcpy(keycode_map, keycode_origin, sizeof(GCKeyCode) * DIVISION_KEYCODE_COUNT);

    return keycode_map;
}

static inline void osx_keycode_map_free(GCKeyCode* keycode_map)
{
    free(keycode_map);
}
