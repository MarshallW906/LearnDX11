#pragma once

#include "DirectXTemplatePCH.h"

using namespace DirectX;

// TODO: sout out which ones are unnecessary
struct GameContextD3D11
{
	// D3D settings
	// Direct3D device and swap chain
	ID3D11Device* m_d3dDevice;
	ID3D11DeviceContext* m_d3dDeviceContext;
	IDXGISwapChain* m_d3dSwapChain;


	// Render target view for the back buffer of the swap chain
	ID3D11RenderTargetView* m_d3dRenderTargetView = nullptr;
	// Depth/Stencil view for use as a depth buffer
	ID3D11DepthStencilView* m_d3dDepthStencilView = nullptr;
	// A texture to associate to the depth stencil view
	ID3D11Texture2D* m_d3dDepthStencilBuffer = nullptr;

	// Define the functionality of the depth/stencil stages
	ID3D11DepthStencilState* m_d3dDepthStencilState = nullptr;
	// Define the functionality of the rasterizer stage
	ID3D11RasterizerState* m_d3dRasterizerState = nullptr;
	D3D11_VIEWPORT m_Viewport = { 0 };

	ID3D11Buffer* m_constantBufferAllMeshPositions = nullptr;

	ID3D11Device* const getDevice() const;
	ID3D11DeviceContext* const getDeviceContext() const;
	ID3D11Buffer* const getConstantBufferAllPos() const;

	void Release();
};

