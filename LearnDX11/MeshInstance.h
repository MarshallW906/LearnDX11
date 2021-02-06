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
	MeshInstance(GameContextD3D11* pGameContext, Mesh* pMesh, XMMATRIX localTransform);

	bool IsStatic() const;
	const bool IsRoot() const;

	const XMMATRIX& GetLocalTransform() const;
	const XMMATRIX& GetWorldToParentTransform() const;
	XMMATRIX GetCurrentFinalTransform() const;

	void UpdateLocalTransform(XMMATRIX localTransform);

	void SetParentMeshInstance(MeshInstance* pParentMeshInstance);

	void Draw();

private:

	GameContextD3D11* m_pGameContext;

	// If (m_isRoot) then it's parented by a Node
	// otherwise it's parented by another MeshInstance
	// TODO: for now we keep it simple, but we need to change them to COM-objects
	bool m_isRoot;
	MeshInstance* m_pParentMeshInstance;

	friend class Mesh;
	Mesh* m_pMesh;

	bool m_isStatic;
	XMMATRIX m_localTransform;
	XMMATRIX m_worldToParentTransform;
};

