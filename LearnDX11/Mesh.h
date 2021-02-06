#pragma once

#include "GameContextD3D11.h"
#include "MeshInstance.h"
#include <vector>

class MeshInstance;


struct CBWorldMatrices
{
	XMMATRIX wTransforms[1000];
};

class Mesh
{
public:
	Mesh() = delete;
	Mesh(const Mesh&) = delete;

	Mesh(GameContextD3D11* pGameContext,
		ID3D11Buffer** ppVertexBuffers, UINT numVertexBuffers, UINT* pVertexStrides, UINT* pVertexOffsets,
		ID3D11Buffer* pIndexBuffer, UINT indexCount, DXGI_FORMAT indexFormat, UINT indexOffset);
	~Mesh();

	void BindVertexAndIndexBuffers();

	UINT GenerateInstances(XMMATRIX* pLocalTransforms, UINT numTransforms);
	std::vector<MeshInstance*>& GetAllInstances();
	// TODO: instances should not managed by a Mesh class
	// instead we should iterate on all the scene nodes and detemine which objects should be rendered
	// This is just one simplest way to take the advantage of DrawIndexedInstanced()
	void DrawAllInstances();

private:
	GameContextD3D11* m_pGameContext;

	ID3D11Buffer** m_ppVertexBuffers;
	UINT m_numVertexBuffers;
	UINT* m_pVertexStrides;
	UINT* m_pVertexOffsets;

	ID3D11Buffer* m_pIndexBuffer;
	UINT m_indexCount;
	DXGI_FORMAT m_indexFormat;
	UINT m_indexOffset;

	CBWorldMatrices* m_pAllInstanceTransforms;
	std::vector<MeshInstance*> m_MeshInstances;
};

