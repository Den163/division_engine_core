#pragma once

#include <stdint.h>

typedef enum DivisionColorMask
{
    DIVISION_COLOR_MASK_NONE = 0,
    DIVISION_COLOR_MASK_R = 1 << 0,
    DIVISION_COLOR_MASK_G = 1 << 1,
    DIVISION_COLOR_MASK_B = 1 << 2,
    DIVISION_COLOR_MASK_A = 1 << 3,
    DIVISION_COLOR_MASK_RGB =
        DIVISION_COLOR_MASK_R | DIVISION_COLOR_MASK_G | DIVISION_COLOR_MASK_B,
    DIVISION_COLOR_MASK_RGBA = DIVISION_COLOR_MASK_RGB | DIVISION_COLOR_MASK_A,
} DivisionColorMask;

typedef enum DivisionRenderPassDescriptorCapabilityMask
{
    DIVISION_RENDER_PASS_DESCRIPTOR_CAPABILITY_NONE = 0,
    DIVISION_RENDER_PASS_DESCRIPTOR_CAPABILITY_ALPHA_BLEND = 1 << 0,
} DivisionRenderPassDescriptorCapabilityMask;

typedef enum DivisionAlphaBlend
{
    DIVISION_ALPHA_BLEND_ZERO = 0,
    DIVISION_ALPHA_BLEND_ONE = 1,
    DIVISION_ALPHA_BLEND_SRC_COLOR = 2,
    DIVISION_ALPHA_BLEND_SRC_ALPHA = 3,
    DIVISION_ALPHA_BLEND_SRC_ALPHA_SATURATE = 4,
    DIVISION_ALPHA_BLEND_DST_COLOR = 5,
    DIVISION_ALPHA_BLEND_DST_ALPHA = 6,
    DIVISION_ALPHA_BLEND_CONSTANT_COLOR = 7,
    DIVISION_ALPHA_BLEND_CONSTANT_ALPHA = 8,
    DIVISION_ALPHA_BLEND_ONE_MINUS_SRC_COLOR = 9,
    DIVISION_ALPHA_BLEND_ONE_MINUS_SRC_ALPHA = 10,
    DIVISION_ALPHA_BLEND_ONE_MINUS_DST_COLOR = 11,
    DIVISION_ALPHA_BLEND_ONE_MINUS_DST_ALPHA = 12,
    DIVISION_ALPHA_BLEND_ONE_MINUS_CONSTANT_COLOR = 13,
    DIVISION_ALPHA_BLEND_ONE_MINUS_CONSTANT_ALPHA = 14,
} DivisionAlphaBlend;

typedef enum DivisionAlphaBlendOperation
{
    DIVISION_ALPHA_BLEND_OP_ADD = 1,
    DIVISION_ALPHA_BLEND_OP_SUBTRACT = 2,
    DIVISION_ALPHA_BLEND_OP_REVERSE_SUBTRACT = 3,
    DIVISION_ALPHA_BLEND_OP_MIN = 4,
    DIVISION_ALPHA_BLEND_OP_MAX = 5,
} DivisionAlphaBlendOperation;

typedef struct DivisionAlphaBlendingOptions
{
    DivisionAlphaBlend src;
    DivisionAlphaBlend dst;
    DivisionAlphaBlendOperation operation;
    float constant_blend_color[4];
} DivisionAlphaBlendingOptions;

typedef struct DivisionRenderPassDescriptor
{
    DivisionAlphaBlendingOptions alpha_blending_options;
    uint32_t shader_program;
    uint32_t vertex_buffer_id;
    DivisionRenderPassDescriptorCapabilityMask capabilities_mask;
    DivisionColorMask color_mask;
} DivisionRenderPassDescriptor;