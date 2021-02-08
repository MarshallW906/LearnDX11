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
	, m_drawnIndividually(false)
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

XMMATRIX MeshInstance::GetWorldTransform()
{
	return m_localTransform * GetParentTransform();
}

void MeshInstance::UpdateLocalTransform(XMMATRIX newLocalTransform)
{
	m_localTransform = newLocalTransform;
}

void MeshInstance::SetRotationRollPitchYaw(float Pitch, float Yaw, float Roll)
{
	XMVECTOR localScale, localRotationQuat, localTranslation;
	XMMatrixDecompose(&localScale, &localRotationQuat, &localTranslation, m_localTransform);

	XMVECTOR newRotQuat = XMQuaternionRotationRollPitchYaw(Pitch, Yaw, Roll);
	m_localTransform = XMMatrixAffineTransformation(localScale, XMVECTOR(), newRotQuat, localTranslation);
}

void MeshInstance::SelfRotateByRollPitchYawInDegrees(float pitchDiff, float yawDiff, float rollDiff)
{
	XMVECTOR localScale, localRotationQuat, localTranslation;
	XMMatrixDecompose(&localScale, &localRotationQuat, &localTranslation, m_localTransform);

	XMVECTOR xmvRotQuat = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(pitchDiff), XMConvertToRadians(yawDiff), XMConvertToRadians(rollDiff));
	XMVECTOR newRotQuat = XMQuaternionMultiply(xmvRotQuat, localRotationQuat);
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

bool MeshInstance::IsDrawnIndividually() const
{
	return m_drawnIndividually;
}

void MeshInstance::SetEnableDraw(bool enableDraw)
{
	m_enableDraw = enableDraw;
}

void MeshInstance::SetDrawnIndividually(bool drawnIndividually)
{
	m_drawnIndividually = drawnIndividually;
}

void MeshInstance::DrawSelf()
{
	// draw all static instances
	m_pMesh->BindVertexAndIndexBuffers();
	//m_pGameContext->m_d3dDeviceContext->UpdateSubresource(m_pGameContext->m_constantBufferAllMeshPositions, 0, nullptr, &staticTransforms[0], 0, 0);
	XMMATRIX worldTransform = GetWorldTransform();
	m_pGameContext->m_d3dDeviceContext->UpdateSubresource(m_pGameContext->m_CBSingleWorldMatrix, 0, nullptr, &worldTransform, 0, 0);
	//m_pGameContext->m_d3dDeviceContext->DrawIndexedInstanced(m_pMesh->m_indexCount, 1, 0, 0, 0);
	m_pGameContext->getDeviceContext()->DrawIndexed(m_pMesh->m_indexCount, 0, 0);
}
