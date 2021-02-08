cbuffer PerApplication //: register(b0)
{
    matrix projectionMatrix;
}

cbuffer PerFrame //: register(b1)
{
    matrix viewMatrix;
}

cbuffer PerInstance
{
    matrix worldMatrix;
}

struct SKYMAP_VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
};


SKYMAP_VS_OUTPUT SKYMAP_VS(float3 inPos : POSITION)
{
    SKYMAP_VS_OUTPUT output;
    matrix wvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));

    //Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
    output.Pos = mul(wvp, float4(inPos, 1.0f)).xyww;

    output.texCoord = inPos.xy;

    return output;
}
