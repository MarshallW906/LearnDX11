#pragma once
#include "MeshInstance.h"

class Camera
{
public:
	Camera();

	void SetEnableFirstPersonView(bool enabled);
	void ToggleCameraMode();
	bool SetThirdViewFollowDistance(float distance);
	void SetFollowTarget(MeshInstance* pTargetMeshInstance);
	void SetViewRollPitchYawInAngles(float rollAngle, float pitchAngle, float yawAngle);

	XMVECTOR DebugGetCameraPosInFirstView() const;
	float GetCamRollMax() const;
	float GetCamRollMin() const;
	bool IsFirstViewMode() const;

	void ApplyMousePos(float xPosNormalized, float yPosNormalized);
	void ApplyMouseWheelDelta(int wheelDelta, float deltaTime);

	XMVECTOR GetCurrentPosition() const;

	XMMATRIX GetViewMatrix(XMVECTOR worldUp);

private:

	bool m_isFirstPersonView;
	XMVECTOR m_currentPosition;

	float m_camRollInAngle;
	float m_camPitchInAngle;
	float m_camYawInAngle;
	float m_camRollMax;
	float m_camRollMin;

	// 1st-person view
	XMVECTOR m_firstViewFollowLocalPosOffset;
	XMVECTOR m_finalPosFirstView;

	// 3rd view
	float m_ThirdViewFollowDistance;
	float m_zoomSpeed;
	float m_minFollowDistance;
	float m_maxFollowDistance;

	// TODO: change to COM-design, like following a Node/Component
	MeshInstance* m_pFollowMeshInstance;
};

