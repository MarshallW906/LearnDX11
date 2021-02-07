#pragma once
#include "MeshInstance.h"

class Camera
{
public:
	Camera();

	void SetEnableFirstPersonView(bool enabled);
	bool SetThirdViewFollowDistance(float distance);
	void SetFollowTarget(MeshInstance* pTargetMeshInstance);
	void SetThirdViewRollPitchYaw(float rollAngle, float pitchAngle, float yawAngle);

	XMMATRIX GetViewMatrix(XMVECTOR worldUp);

private:

	bool m_isFirstPersonView;
	float m_ThirdViewFollowDistance;
	
	float m_camRollInAngle;
	float m_camPitchInAngle;
	float m_camYawInAngle;

	float m_camRollMax;
	float m_camRollMin;

	// TODO: change to COM-design, like following a Node/Component
	MeshInstance* m_pFollowMeshInstance;
};

