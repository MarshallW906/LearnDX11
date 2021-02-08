
struct SKYMAP_PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

Texture2D SkyMap;
//TextureCube SkyMap; // TODO: use cubemap
SamplerState ObjSamplerState;


float4 SKYMAP_PS(SKYMAP_PS_INPUT input) : SV_Target
{
    return SkyMap.Sample(ObjSamplerState, input.texcoord);
}
