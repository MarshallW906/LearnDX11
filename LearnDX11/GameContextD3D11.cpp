#include "DirectXTemplatePCH.h"
#include "GameContextD3D11.h"

void GameContextD3D11::Release()
{
	SafeRelease(m_d3dDepthStencilView);
	SafeRelease(m_d3dRenderTargetView);
	SafeRelease(m_d3dDepthStencilBuffer);
	SafeRelease(m_d3dDepthStencilState);
	SafeRelease(m_d3dRasterizerState);
	SafeRelease(m_d3dSwapChain);
	SafeRelease(m_d3dDeviceContext);
	SafeRelease(m_d3dDevice);
}
