// Pixel
struct PixelShaderInput
{
	// float4 color: COLOR;
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

float4 SimplePixelShader(PixelShaderInput IN) : SV_TARGET{
	//return IN.color;
	return ObjTexture.Sample(ObjSamplerState, IN.texcoord);
}
