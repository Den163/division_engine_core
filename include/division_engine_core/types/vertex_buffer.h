#pragma once

#include "shader.h"

#include <stddef.h>

typedef enum DivisionRenderTopology
{
    DIVISION_TOPOLOGY_TRIANGLES = 1,
    DIVISION_TOPOLOGY_POINTS = 2,
    DIVISION_TOPOLOGY_LINES = 3
} DivisionRenderTopology;

typedef struct DivisionVertexBufferSize
{
    uint32_t vertex_count;
    uint32_t index_count;
    uint32_t instance_count;
} DivisionVertexBufferSize;

typedef struct DivisionVertexAttributeSettings
{
    DivisionShaderVariableType type;
    int32_t location;
} DivisionVertexAttributeSettings;

typedef struct DivisionVertexBufferSettings
{
    DivisionVertexBufferSize size;
    DivisionVertexAttributeSettings* per_vertex_attributes;
    DivisionVertexAttributeSettings* per_instance_attributes;
    int32_t per_vertex_attribute_count;
    int32_t per_instance_attribute_count;
    DivisionRenderTopology topology;
} DivisionVertexBufferSettings;

typedef struct DivisionVertexBufferConstSettings 
{
    DivisionVertexBufferSize size;
    const DivisionVertexAttributeSettings* per_vertex_attributes;
    const DivisionVertexAttributeSettings* per_instance_attributes;
    int32_t per_vertex_attribute_count;
    int32_t per_instance_attribute_count;
    DivisionRenderTopology topology;
} DivisionVertexBufferConstSettings;

typedef struct DivisionVertexAttribute
{
    int32_t offset;
    int32_t base_size;
    int32_t component_count;
} DivisionVertexAttribute;

typedef struct DivisionVertexBuffer
{
    DivisionVertexBufferSettings settings;

    DivisionVertexAttribute* per_vertex_attributes;
    DivisionVertexAttribute* per_instance_attributes;

    size_t per_vertex_data_size;
    size_t per_instance_data_size;
} DivisionVertexBuffer;