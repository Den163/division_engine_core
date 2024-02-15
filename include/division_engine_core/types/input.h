#pragma once

#include <stdint.h>

typedef struct DivisionMouseInput
{
    int32_t pos_x;
    int32_t pos_y;

    uint32_t mouse_button_state_mask;
} DivisionMouseInput;

typedef struct DivisionKeyboardInput
{
    uint32_t key_state_mask[4];
} DivisionKeyboardInput;

typedef struct DivisionInput
{
    DivisionMouseInput mouse;
    DivisionKeyboardInput keyboard;
} DivisionInput;