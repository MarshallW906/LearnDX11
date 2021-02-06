#define MAX_NUM_MESH_INSTANCE 1000

// Vertex
cbuffer PerApplication //: register(b0)
{
	matrix projectionMatrix;
}

cbuffer PerFrame //: register(b1)
{
	matrix viewMatrix;
}
/*
cbuffer PerObject : register(b2)
{
	matrix worldMatrix;
}
*/
cbuffer PerInstance
{
	matrix worldMatrix[MAX_NUM_MESH_INSTANCE];
}

struct AppData
{
	float3 position: POSITION;
	//float3 color : COLOR;
	float2 texcoord: TEXCOORD;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	//float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

VertexShaderOutput SimpleVertexShader(AppData IN, uint instanceID : SV_InstanceID)
{
	VertexShaderOutput OUT;
	matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix[instanceID]));
	float3 pos = IN.position;
	//posOffset.x += instanceID * 4.0f; // test instanceID
	OUT.position = mul(mvp, float4 (pos, 1.0f));
	//OUT.position = mul(mvp, float4 (IN.position, 1.0f));
	OUT.texcoord = IN.texcoord;

	return OUT;
}
