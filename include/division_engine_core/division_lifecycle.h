#pragma once

#include <stdint.h>

struct DivisionContext;

typedef void (*DivisionErrorFunc)(struct DivisionContext* ctx, int, const char*);
typedef void (*DivisionLifecycleFunc)(struct DivisionContext* ctx);

struct DivisionLifecycle
{
    DivisionLifecycleFunc init_callback;
    DivisionLifecycleFunc update_callback;
    DivisionErrorFunc error_callback;
} typedef DivisionLifecycle;