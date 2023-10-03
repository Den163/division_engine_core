#include "division_engine_core/font.h"
#include "division_engine_core/context.h"
#include "division_engine_core/data_structures/unordered_id_table.h"
#include "freetype/freetype.h"
#include "freetype/fttypes.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool division_engine_font_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
)
{
    ctx->font_context = malloc(sizeof(DivisionFontSystemContext));
    DivisionFontSystemContext* font_context = ctx->font_context;

    font_context->ft_faces = NULL;
    font_context->ft_face_count = 0;

    FT_Error err = FT_Init_FreeType(&font_context->ft_library);

    division_unordered_id_table_alloc(&font_context->face_id_table, 10);

    return err == FT_Err_Ok;
}

void division_engine_font_system_context_free(DivisionContext* ctx)
{
    DivisionFontSystemContext* font_context = ctx->font_context;

    for (int i = 0; i < font_context->ft_face_count; i++)
    {
        FT_Done_Face(font_context->ft_faces[i]);
    }

    FT_Done_FreeType(font_context->ft_library);
    free(font_context);
}

bool division_engine_font_alloc(
    DivisionContext* ctx,
    const char* font_file_path,
    uint32_t font_height,
    uint32_t* out_font_id
)
{
    DivisionFontSystemContext* font_context = ctx->font_context;
    uint32_t font_id;

    if (!DIVISION_UNORDERED_ID_TABLE_DATA_WITH_TYPE_GROW(
            FT_Face,
            &font_context->face_id_table,
            &font_context->ft_faces,
            &font_context->ft_face_count,
            &font_id
        ))
    {
        ctx->lifecycle.error_callback(
            ctx, DIVISION_INTERNAL_ERROR, "Failed to realloc faces"
        );
        return false;
    }

    FT_Face* ft_face = &font_context->ft_faces[font_id];
    FT_Error ft_error = FT_New_Face(font_context->ft_library, font_file_path, 0, ft_face);

    if (ft_error)
    {
        division_unordered_id_table_remove_id(&font_context->face_id_table, font_id);
        ctx->lifecycle.error_callback(ctx, DIVISION_INTERNAL_ERROR, "FT_New_Face failed");
        return false;
    }

    ft_error = FT_Set_Char_Size(*ft_face, 0, font_height * 64, 0, 0);
    if (ft_error)
    {
        division_unordered_id_table_remove_id(&font_context->face_id_table, font_id);
        ctx->lifecycle.error_callback(
            ctx, DIVISION_INTERNAL_ERROR, "Failed to set character size"
        );
        return false;
    }

    *out_font_id = font_id;
    return true;
}

bool division_engine_font_get_glyph(
    DivisionContext* ctx,
    uint32_t font_id,
    int32_t character,
    DivisionFontGlyph* out_glyph
)
{
    FT_Face ft_face = ctx->font_context->ft_faces[font_id];

    FT_UInt glyph_id = FT_Get_Char_Index(ft_face, (FT_ULong)character);
    FT_Error ft_error = FT_Load_Glyph(ft_face, glyph_id, FT_LOAD_NO_BITMAP);
    if (ft_error)
    {
        ctx->lifecycle.error_callback(
            ctx, DIVISION_INTERNAL_ERROR, "FT_Load_Glyph failed"
        );
        return false;
    }

    FT_Glyph_Metrics* glyph_metrics = &ft_face->glyph->metrics;

    *out_glyph = (DivisionFontGlyph){
        .glyph_id = glyph_id,
        .width = glyph_metrics->width / 64,
        .height = glyph_metrics->height / 64,
    };

    return true;
}

bool division_engine_font_rasterize_glyph(
    DivisionContext* ctx,
    uint32_t font_id,
    const DivisionFontGlyph* glyph,
    uint8_t* bitmap
)
{
    FT_Face ft_face = ctx->font_context->ft_faces[font_id];
    FT_Error ft_error = FT_Load_Glyph(ft_face, glyph->glyph_id, FT_LOAD_DEFAULT);
    if (ft_error)
    {
        ctx->lifecycle.error_callback(
            ctx, DIVISION_INTERNAL_ERROR, "FT_Load_Glyph failed (with bitmap)"
        );
        return false;
    }

    FT_GlyphSlot ft_glyph = ft_face->glyph;
    if (ft_glyph->format != FT_GLYPH_FORMAT_BITMAP)
    {
        ft_error = FT_Render_Glyph(ft_glyph, FT_RENDER_MODE_NORMAL);
        if (ft_error)
        {
            ctx->lifecycle.error_callback(
                ctx, DIVISION_INTERNAL_ERROR, "FT_Render_Glyph failed"
            );
            return false;
        }
    }

    memcpy(bitmap, ft_glyph->bitmap.buffer, glyph->width * glyph->height);

    return true;
}

void division_engine_font_free(DivisionContext* ctx, uint32_t font_id)
{
    DivisionFontSystemContext* font_context = ctx->font_context;

    FT_Face face = font_context->ft_faces[font_id];
    if (face)
    {
        FT_Done_Face(face);
    }

    division_unordered_id_table_remove_id(&font_context->face_id_table, font_id);
}