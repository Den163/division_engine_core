#pragma once

#include "division_engine_core/context.h"
#include "division_engine_core_export.h"
#include "keycode.h"

#include <stdint.h>

#define DIVISION_INPUT_MOUSE_LEFT 0
#define DIVISION_INPUT_MOUSE_RIGHT 1
#define DIVISION_INPUT_MOUSE_MIDDLE 2
#define DIVISION_INPUT_MOUSE_BUTTON_COUNT 3
#define DIVISION_INPUT_KEY_STATE_MASK_ARR_LEN (DIVISION_KEYCODE_COUNT / 32) + 1

#define DIVISION_INPUT_SET_KEYBOARD_KEY_STATE(maskptr, key, is_pressed) \
    int mask_index = key / 32; \
    int mask_bit_offset = key % 32; \
    (maskptr[mask_index]) |= (((uint32_t) is_pressed) << mask_bit_offset)

#define DIVISION_INPUT_SET_MOUSE_KEY_STATE(mask, key, is_pressed) \
    (mask) |= (((uint32_t) is_pressed) << key)

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

typedef struct DivisionInputSystemContext
{
    DivisionInput input;
} DivisionInputSystemContext;

bool division_engine_input_system_alloc(
    DivisionContext* context, const DivisionSettings* settings
);
void division_engine_input_system_free(DivisionContext* context);

#ifdef __cplusplus
extern "C"
{
#endif

    DIVISION_EXPORT void division_engine_input_get_input(
        DivisionContext* context, DivisionInput* out_input
    );

#ifdef __cplusplus
}
#endif