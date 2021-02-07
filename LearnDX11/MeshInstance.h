#pragma once
#include "GameContextD3D11.h"
#include "Mesh.h"

class Mesh;

// TODO: change to Component design, like derived from a Node or Component
class MeshInstance
{
public:
	MeshInstance() = delete;
	MeshInstance(const MeshInstance&) = delete;

	MeshInstance* GetParentMeshInstance() const;
	MeshInstance(GameContextD3D11* pGameContext, Mesh* pMesh, XMMATRIX localTransform, bool isStatic = false);

	bool IsStatic() const;

	const XMMATRIX& GetLocalTransform() const;
	const XMFLOAT3 GetLocalPos() const;
	const XMMATRIX& GetParentTransform() const;
	XMMATRIX GetWorldTransform() const;
	void UpdateLocalTransform(XMMATRIX newLocalTransform);

	// TODO: SelfRotateRelativelyFromQuat(Quat), this function may need re-writing
	void SelfRotate(XMMATRIX rotationMatrix);
	void SetRotationRollPitchYaw(float Pitch, float Yaw, float Roll);

	void SetParentMeshInstance(MeshInstance* pParentMeshInstance);

	void Draw();

private:

	GameContextD3D11* m_pGameContext;

	MeshInstance* m_pParentMeshInstance;

	friend class Mesh;
	Mesh* m_pMesh;

	bool m_isStatic;
	XMMATRIX m_localTransform;
};

