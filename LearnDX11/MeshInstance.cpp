#include "DirectXTemplatePCH.h"
#include "MeshInstance.h"

MeshInstance* MeshInstance::GetParentMeshInstance() const
{
	return m_pParentMeshInstance;
}

MeshInstance::MeshInstance(GameContextD3D11* pGameContext, Mesh* pMesh, XMMATRIX localTransform, bool isStatic)
	: m_pGameContext(pGameContext)
	, m_pMesh(pMesh)
	, m_isStatic(isStatic)
	, m_pParentMeshInstance(nullptr)
	, m_localTransform(localTransform)
	, m_enableDraw(true)
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

const XMFLOAT3 MeshInstance::GetLocalPos() const
{
	return XMFLOAT3(m_localTransform.r[2].m128_f32[0], m_localTransform.r[2].m128_f32[1], m_localTransform.r[2].m128_f32[2]);
}

const XMMATRIX& MeshInstance::GetParentTransform() const
{
	return m_pParentMeshInstance == nullptr ? XMMatrixIdentity() : (m_pParentMeshInstance->GetWorldTransform());
}

XMMATRIX MeshInstance::GetWorldTransform() const
{
	return m_localTransform * GetParentTransform();
}

void MeshInstance::UpdateLocalTransform(XMMATRIX newLocalTransform)
{
	m_localTransform = newLocalTransform;
}

void MeshInstance::SelfRotate(XMMATRIX rotationMatrix)
{
	m_localTransform = rotationMatrix * m_localTransform;
}

void MeshInstance::SetRotationRollPitchYaw(float Pitch, float Yaw, float Roll)
{
	XMVECTOR localScale, localRotationQuat, localTranslation;
	XMMatrixDecompose(&localScale, &localRotationQuat, &localTranslation, m_localTransform);

	XMVECTOR newRotQuat = XMQuaternionRotationRollPitchYaw(Pitch, Yaw, Roll);
	m_localTransform = XMMatrixAffineTransformation(localScale, XMVECTOR(), newRotQuat, localTranslation);
}

void MeshInstance::SetParentMeshInstance(MeshInstance* pParentMeshInstance)
{
	// newLocal = currentWorldTransform * newParentTransform.Inverse
	m_localTransform = GetWorldTransform() * XMMatrixInverse(nullptr, pParentMeshInstance->GetWorldTransform());

	m_pParentMeshInstance = pParentMeshInstance;
}

bool MeshInstance::IsEnableDraw() const
{
	return m_enableDraw;
}

void MeshInstance::SetEnableDraw(bool enableDraw)
{
	m_enableDraw = enableDraw;
}
