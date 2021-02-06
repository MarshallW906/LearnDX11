#include "DirectXTemplatePCH.h"
#include "MeshInstance.h"

MeshInstance* MeshInstance::GetParentMeshInstance() const
{
	return m_pParentMeshInstance;
}

MeshInstance::MeshInstance(GameContextD3D11* pGameContext, Mesh *pMesh, XMMATRIX localTransform)
	: m_pGameContext(pGameContext)
	, m_pMesh(pMesh)
	, m_isStatic(false)
	, m_pParentMeshInstance(nullptr)
	, m_localTransform(localTransform)
{
}

bool MeshInstance::IsStatic() const
{
	return m_isStatic; 
}

const XMMATRIX& MeshInstance::GetLocalTransform() const
{
	return m_localTransform;
}

const XMMATRIX& MeshInstance::GetParentTransform() const
{
	return m_pParentMeshInstance == nullptr ? XMMatrixIdentity() : (m_pParentMeshInstance->GetWorldTransform());
}

XMMATRIX MeshInstance::GetWorldTransform() const
{
	return m_localTransform * GetParentTransform();
}

void MeshInstance::ApplyTransform(XMMATRIX transform)
{
	m_localTransform = transform * m_localTransform;
}

void MeshInstance::UpdateLocalTransform(XMMATRIX newLocalTransform)
{
	m_localTransform = newLocalTransform;
}

void MeshInstance::SetParentMeshInstance(MeshInstance* pParentMeshInstance)
{
	m_pParentMeshInstance = pParentMeshInstance;
}