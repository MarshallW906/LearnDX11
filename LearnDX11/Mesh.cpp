#include "DirectXTemplatePCH.h"
#include "Mesh.h"

Mesh::Mesh(GameContextD3D11* pGameContext,
	ID3D11Buffer** ppVertexBuffers, UINT numVertexBuffers, UINT* pVertexStrides, UINT* pVertexOffsets,
	ID3D11Buffer* pIndexBuffer, UINT indexCount, DXGI_FORMAT indexFormat, UINT indexOffset)
	: m_pGameContext(pGameContext)
	, m_ppVertexBuffers(ppVertexBuffers)
	, m_numVertexBuffers(numVertexBuffers)
	, m_pVertexStrides(pVertexStrides)
	, m_pVertexOffsets(pVertexOffsets)
	, m_pIndexBuffer(pIndexBuffer)
	, m_indexCount(indexCount)
	, m_indexFormat(indexFormat)
	, m_indexOffset(indexOffset)
	, m_pAllInstanceTransforms(nullptr)
{
	m_pAllInstanceTransforms = new CBWorldMatrices();
}

Mesh::~Mesh()
{
	delete(m_pAllInstanceTransforms);
}

UINT Mesh::GenerateInstances(XMMATRIX* pLocalTransforms, UINT numTransforms, bool isStatic)
{
	//MeshInstance *t = new MeshInstance(m_pGameContext);
	UINT startIndex = m_MeshInstances.size(); // previous size
	for (int i = 0; i < numTransforms; i++)
	{
		MeshInstance* t = new MeshInstance(m_pGameContext, this, pLocalTransforms[i], isStatic);
		m_MeshInstances.push_back(t);
	}
	return startIndex;
}

std::vector<MeshInstance*>& Mesh::GetAllInstances()
{
	return m_MeshInstances;
}

void Mesh::DrawAllInstances()
{
	UINT staticInstanceCount = 0;
	//std::vector<XMMATRIX> staticTransforms;
	for (auto it = m_MeshInstances.begin(); it != m_MeshInstances.end(); it++)
	{
		// TODO: draw static/non-static meshes separatedly
		//if ((*it)->IsStatic())
		if ((*it)->IsEnableDraw())
		{
			m_pAllInstanceTransforms->wTransforms[staticInstanceCount] = (*it)->GetWorldTransform();
			//staticTransforms.push_back((*it)->GetCurrentFinalTransform());
			++staticInstanceCount;
		}
		//else
		//{
		//	(*it)->Draw();
		//}
	}

	// draw all static instances
	BindVertexAndIndexBuffers();
	//m_pGameContext->m_d3dDeviceContext->UpdateSubresource(m_pGameContext->m_constantBufferAllMeshPositions, 0, nullptr, &staticTransforms[0], 0, 0);
	m_pGameContext->m_d3dDeviceContext->UpdateSubresource(m_pGameContext->m_constantBufferAllMeshPositions, 0, nullptr, &m_pAllInstanceTransforms->wTransforms, 0, 0);
	m_pGameContext->m_d3dDeviceContext->DrawIndexedInstanced(m_indexCount, staticInstanceCount, 0, 0, 0);
}

void Mesh::BindVertexAndIndexBuffers()
{
	assert(m_pGameContext->m_d3dDevice);
	assert(m_pGameContext->m_d3dDeviceContext);

	m_pGameContext->m_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // now we just hardcode it to trianglelist

	m_pGameContext->m_d3dDeviceContext->IASetVertexBuffers(0, m_numVertexBuffers, m_ppVertexBuffers, m_pVertexStrides, m_pVertexOffsets);
	m_pGameContext->m_d3dDeviceContext->IASetIndexBuffer(m_pIndexBuffer, m_indexFormat, m_indexOffset);
}
