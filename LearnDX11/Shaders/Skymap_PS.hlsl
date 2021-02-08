
struct SKYMAP_PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

Texture2D SkyMap;
//TextureCube SkyMap;
SamplerState ObjSamplerState;


float4 SKYMAP_PS(SKYMAP_PS_INPUT input) : SV_Target
{
    return SkyMap.Sample(ObjSamplerState, input.texCoord);
}
