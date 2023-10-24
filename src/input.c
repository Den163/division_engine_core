#include <division_engine_core/input.h>

#include <stdbool.h>
#include <stdlib.h>

bool division_engine_input_system_alloc(
    DivisionContext* context, const DivisionSettings* settings
)
{
    context->input_context = malloc(sizeof(DivisionInputSystemContext));

    return context->input_context != NULL;
}

void division_engine_input_system_free(DivisionContext* context)
{
    free(context->input_context);
}

void division_engine_input_get_input(DivisionContext* context, DivisionInput* out_input)
{
    *out_input = context->input_context->input;
}