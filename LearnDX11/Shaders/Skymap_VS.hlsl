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
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

struct SKYMAP_VS_INPUT
{
    float3 position: POSITION;
    float2 texcoord: TEXCOORD;
};


SKYMAP_VS_OUTPUT SKYMAP_VS(SKYMAP_VS_INPUT IN)
{
    SKYMAP_VS_OUTPUT output;
    matrix wvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));

    //Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
    output.position = mul(wvp, float4(IN.position, 1.0f)).xyww;

    output.texcoord = IN.texcoord;

    return output;
}
