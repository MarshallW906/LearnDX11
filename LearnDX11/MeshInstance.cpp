#include "DirectXTemplatePCH.h"
#include "MeshInstance.h"

MeshInstance* MeshInstance::GetParentMeshInstance() const
{
	return m_pParentMeshInstance;
}

MeshInstance::MeshInstance(GameContextD3D11* pGameContext, Mesh *pMesh, XMMATRIX localTransform)
	: m_pGameContext(pGameContext)
	, m_pMesh(pMesh)
	, m_isRoot(true)
	, m_isStatic(false)
	, m_pParentMeshInstance(nullptr)
	, m_localTransform(localTransform)
	, m_worldToParentTransform(XMMatrixIdentity())
{
}

bool MeshInstance::IsStatic() const
{
	return m_isStatic; 
}

const bool MeshInstance::IsRoot() const
{
	return m_isRoot;
}

const XMMATRIX& MeshInstance::GetLocalTransform() const
{
	return m_localTransform;
}

const XMMATRIX& MeshInstance::GetWorldToParentTransform() const
{
	return m_worldToParentTransform;
}

XMMATRIX MeshInstance::GetCurrentFinalTransform() const
{
	return m_localTransform * m_worldToParentTransform;
}

void MeshInstance::UpdateLocalTransform(XMMATRIX localTransform)
{
	m_localTransform = localTransform;
}

void MeshInstance::SetParentMeshInstance(MeshInstance* pParentMeshInstance)
{
	m_pParentMeshInstance = pParentMeshInstance;
	m_worldToParentTransform = m_pParentMeshInstance->GetLocalTransform() * m_pParentMeshInstance->GetWorldToParentTransform();
}