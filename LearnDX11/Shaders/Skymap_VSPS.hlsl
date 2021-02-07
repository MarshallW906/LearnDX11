cbuffer cbPerObject
{
    float4x4 WVP;
};

struct SKYMAP_VS_OUTPUT    //output structure for skymap vertex shader
{
    float4 Pos : SV_POSITION;
    float3 texCoord : TEXCOORD;
};


SKYMAP_VS_OUTPUT SKYMAP_VS(float3 inPos : POSITION)
{
    SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT)0;

    //Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
    output.Pos = mul(float4(inPos, 1.0f), WVP).xyww;

    output.texCoord = inPos;

    return output;
}


Texture2D ObjTexture;
SamplerState ObjSamplerState;
TextureCube SkyMap;

float4 SKYMAP_PS(SKYMAP_VS_OUTPUT input) : SV_Target
{
    return SkyMap.Sample(ObjSamplerState, input.texCoord);
}
