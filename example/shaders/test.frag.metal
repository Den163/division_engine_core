#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct Uniforms
{
    float4 TestColor;
};

struct frag_out
{
    float4 FragColor [[color(0)]];
};

struct frag_in
{
    float4 VertexColor [[user(locn0)]];
    float2 inUV [[user(locn1)]];
};

fragment frag_out frag(frag_in in [[stage_in]], constant Uniforms& _34 [[buffer(1)]], texture2d<float> _tex [[texture(0)]], sampler _texSmplr [[sampler(0)]])
{
    frag_out out = {};
    float4 tex = float4(_tex.sample(_texSmplr, in.inUV));
    out.FragColor = in.VertexColor * tex;
    float4 _42 = out.FragColor;
    float3 _44 = _42.xyz + _34.TestColor.xyz;
    out.FragColor.x = _44.x;
    out.FragColor.y = _44.y;
    out.FragColor.z = _44.z;
    return out;
}

 