#include "division_engine_core/context.h"
#include "division_engine_core_export.h"
#include <stdint.h>

typedef enum DivisionInputState
{
    DIVISION_INPUT_STATE_UP = 0,
    DIVISION_INPUT_STATE_DOWN = 1,
} DivisionInputState;

typedef struct DivisionMouseInput
{
    int32_t pos_x;
    int32_t pos_y;

    DivisionInputState left_button;
    DivisionInputState right_button;
    DivisionInputState middle_button;
} DivisionMouseInput;

typedef struct DivisionInput
{
    DivisionMouseInput mouse;
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