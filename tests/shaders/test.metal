#include <metal_stdlib>
using namespace metal;

struct v2f {
    float4 position [[position]];
    half4 color;
};

#pragma pack(1)
struct vert {
    float3 position [[attribute(0)]];
    float4 color [[attribute(1)]];
};


v2f vertex vertexMain(uint vertexId [[vertex_id]],
                      const vert vd [[stage_in]]
) {
    v2f o;
    o.position = float4(vd.position, 1.0);
    o.color = half4(vd.color);

    return o;
}

half4 fragment fragmentMain(v2f in [[stage_in]],
                            constant float4& testColor [[buffer(1)]]
)
{
    return in.color * (half4) testColor;
}
