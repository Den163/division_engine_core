#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct vert_out
{
    float4 VertexColor [[user(locn0)]];
    float2 outUV [[user(locn1)]];
    float4 gl_Position [[position]];
};

struct vert_in
{
    float3 pos [[attribute(0)]];
    float4 fColor [[attribute(1)]];
    float2 inUV [[attribute(2)]];
    float4 localToWorld_0 [[attribute(3)]];
    float4 localToWorld_1 [[attribute(4)]];
    float4 localToWorld_2 [[attribute(5)]];
    float4 localToWorld_3 [[attribute(6)]];
};

vertex vert_out vert(vert_in in [[stage_in]])
{
    vert_out out = {};
    float4x4 localToWorld = {};
    localToWorld[0] = in.localToWorld_0;
    localToWorld[1] = in.localToWorld_1;
    localToWorld[2] = in.localToWorld_2;
    localToWorld[3] = in.localToWorld_3;
    out.VertexColor = in.fColor;
    out.outUV = in.inUV;
    out.gl_Position = float4(in.pos, 1.0) * localToWorld;
    return out;
}

 