#pragma once

#include <ft2build.h>
#include <stdint.h>
#include <sys/types.h>
#include <unicode/uchar.h>
#include FT_FREETYPE_H

#include "data_structures/unordered_id_table.h"
#include "division_engine_core/context.h"
#include "division_engine_core/settings.h"
#include "division_engine_core_export.h"

typedef struct DivisionFontGlyph
{
    uint32_t width;
    uint32_t height;
    uint32_t advance_x;
    uint32_t left;
    uint32_t top;

} DivisionFontGlyph;

typedef struct DivisionFontSystemContext
{
    DivisionUnorderedIdTable face_id_table;

    FT_Library ft_library;

    FT_Face* ft_faces;
    size_t ft_face_count;

} DivisionFontSystemContext;

bool division_engine_font_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
);

void division_engine_font_system_context_free(DivisionContext* ctx);

#ifdef __cplusplus
extern "C"
{
#endif

    DIVISION_EXPORT bool division_engine_font_alloc(
        DivisionContext* ctx,
        const char* font_file_path,
        uint32_t font_height,
        uint32_t* out_font_id
    );

    DIVISION_EXPORT void division_engine_font_free(
        DivisionContext* ctx, uint32_t font_id
    );

    DIVISION_EXPORT bool division_engine_font_get_glyph(
        DivisionContext* ctx, 
        uint32_t font_id, 
        int32_t character, 
        DivisionFontGlyph* out_glyph
    );

    DIVISION_EXPORT bool division_engine_font_rasterize_glyph(
        DivisionContext* ctx,
        uint32_t font_id,
        int32_t character,
        uint8_t* bitmap
    );

#ifdef __cplusplus
    extern "C"
}
#endif
