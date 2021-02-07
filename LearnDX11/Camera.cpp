#include "DirectXTemplatePCH.h"
#include "Camera.h"
#include "FuncUtils.h"

Camera::Camera()
    : m_isFirstPersonView(false)
    , m_ThirdViewFollowDistance(20)
    , m_pFollowMeshInstance(nullptr)
    , m_camPitchInAngle(0)
    , m_camRollInAngle(0)
    , m_camYawInAngle(0)
    , m_camRollMax(0)
    , m_camRollMin(-80)
{
}

void Camera::SetEnableFirstPersonView(bool enabled)
{
    m_isFirstPersonView = enabled;
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

void Camera::SetThirdViewRollPitchYaw(float rollAngle, float pitchAngle, float yawAngle)
{
    // TODO: clamp roll
    
    m_camRollInAngle = FuncUtils::SimpleClamp(rollAngle, m_camRollMin, m_camRollMax);
    m_camPitchInAngle = pitchAngle;
    m_camYawInAngle = yawAngle;
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

    if (m_isFirstPersonView)
    {
        eyePosition = followWorldPos;
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
