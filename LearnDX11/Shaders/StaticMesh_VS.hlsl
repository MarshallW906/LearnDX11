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

struct VS_Output
{
	float4 position : SV_POSITION;
	//float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

VS_Output StaticMesh_VS(AppData IN, uint instanceID : SV_InstanceID)
{
	VS_Output OUT;
	float3 pos = IN.position;

	matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix[instanceID]));
	OUT.position = mul(mvp, float4 (pos, 1.0f));
	OUT.texcoord = IN.texcoord;

	return OUT;
}
