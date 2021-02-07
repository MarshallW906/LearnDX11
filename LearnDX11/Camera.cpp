#include "DirectXTemplatePCH.h"
#include "Camera.h"
#include "FuncUtils.h"

Camera::Camera()
    : m_isFirstPersonView(false)
    , m_firstViewFollowLocalPosOffset(XMVectorSet(0, 0.0, 1.0, 0))
    , m_ThirdViewFollowDistance(20)
    , m_pFollowMeshInstance(nullptr)
    , m_camPitchInAngle(0)
    , m_camRollInAngle(0)
    , m_camYawInAngle(0)
    , m_camRollMax(80)
    , m_camRollMin(-80)
    , m_finalPosFirstView(XMVectorSet(0, 10, -20, 1))
{
}

void Camera::SetEnableFirstPersonView(bool enabled)
{
    m_isFirstPersonView = enabled;
}

void Camera::ToggleCameraMode()
{
    m_isFirstPersonView = !m_isFirstPersonView;
}

bool Camera::SetThirdViewFollowDistance(float distance)
{
    if (distance > 0)
    {
        m_ThirdViewFollowDistance = distance;
        return true;
    }
    return false;
}

void Camera::SetFollowTarget(MeshInstance* pTargetMeshInstance)
{
    m_pFollowMeshInstance = pTargetMeshInstance;
}

void Camera::SetThirdViewRollPitchYawInAngles(float rollAngle, float pitchAngle, float yawAngle)
{
    // TODO: clamp roll
    
    m_camRollInAngle = FuncUtils::SimpleClamp(rollAngle, m_camRollMin, m_camRollMax);
    m_camPitchInAngle = pitchAngle;
    m_camYawInAngle = yawAngle;
}

XMVECTOR Camera::DebugGetCameraPosInFirstView() const
{
    return m_finalPosFirstView;
}

float Camera::GetCamRollMax() const
{
    return m_camRollMax;
}

float Camera::GetCamRollMin() const
{
    return m_camRollMin;
}

XMMATRIX Camera::GetViewMatrix(XMVECTOR worldUp)
{
    if (!m_pFollowMeshInstance)
    {
        return XMMatrixLookAtLH(XMVectorSet(0, 10, -20, 1), XMVectorSet(0, 0, 0, 1), worldUp);
    }

    XMVECTOR eyePosition;
    XMVECTOR viewFocusPoint;
    XMMATRIX followWorldTransform = m_pFollowMeshInstance->GetWorldTransform();
    
    XMVECTOR followWorldPos;
    XMVECTOR followWorldScaling;
    XMVECTOR followWorldRot;
    bool success = XMMatrixDecompose(&followWorldScaling, &followWorldRot, &followWorldPos, followWorldTransform);

    
    m_finalPosFirstView = followWorldPos + XMVector4Transform(m_firstViewFollowLocalPosOffset, followWorldTransform);
    if (m_isFirstPersonView)
    {
        //eyePosition = followWorldPos + m_firstViewFollowLocalPosOffset;
        eyePosition = m_finalPosFirstView;
        viewFocusPoint = eyePosition + XMVector3Normalize(followWorldTransform.r[2]);
    }
    else
    {
        // third view. we need focusPoint first and then calc cam's real pos
        viewFocusPoint = followWorldPos;
        XMVECTOR xmvNegDistance = XMVectorSet(0, 0, -m_ThirdViewFollowDistance, 0);
        XMVECTOR xmvCamRot = XMQuaternionRotationRollPitchYaw(
            XMConvertToRadians(m_camRollInAngle),
            XMConvertToRadians(m_camPitchInAngle),
            XMConvertToRadians(m_camYawInAngle)
        );

        eyePosition = viewFocusPoint + XMQuaternionMultiply(xmvCamRot, xmvNegDistance);
    }
    return XMMatrixLookAtLH(eyePosition, viewFocusPoint, worldUp);
}
