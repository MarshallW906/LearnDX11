#include "DirectXTemplatePCH.h"
#include "GameContextD3D11.h"

void GameContextD3D11::Release()
{
	SafeRelease(m_d3dDepthStencilView);
	SafeRelease(m_d3dRenderTargetView);
	SafeRelease(m_d3dDepthStencilBuffer);
	SafeRelease(m_d3dDepthStencilStateDefault);
	SafeRelease(m_d3dRasterizerStateDefault);
	SafeRelease(m_d3dSwapChain);
	SafeRelease(m_d3dDeviceContext);
	SafeRelease(m_d3dDevice);
}

ID3D11Device* const GameContextD3D11::getDevice() const { return m_d3dDevice; }
ID3D11DeviceContext* const GameContextD3D11::getDeviceContext() const { return m_d3dDeviceContext; }
ID3D11Buffer* const GameContextD3D11::getConstantBufferAllPos() const { return m_constantBufferAllMeshPositions; }