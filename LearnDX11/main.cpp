#include "DirectXTemplatePCH.h"

#include "GameContextD3D11.h"

#include "VSPSShader.h"

#include "WICTextureLoader.h"


using namespace DirectX;

// global Win32App & general settings
const LONG g_WindowWidth = 1280;
const LONG g_WindowHeight = 720;
LPCWSTR g_WindowClassName = L"DirectXWindowClass";
LPCWSTR g_WindowName = L"DirectX Template";
HWND g_WindowHandle = 0;

const BOOL g_EnableVSync = TRUE;


GameContextD3D11* g_pGameContextD3D11;

// D3D settings
ID3D11InputLayout* g_d3dInputLayout = nullptr;

// texture
ID3D11Buffer* g_d3dVertexTexcoordBuffer = nullptr;
ID3D11Buffer* g_d3dIndexTexcoordBuffer = nullptr;
ID3D11Texture2D* g_pTexture2D = nullptr;
ID3D11Resource* g_pTexture2DResource = nullptr;
ID3D11ShaderResourceView* g_pShaderResourceView = nullptr;
ID3D11SamplerState* g_pSamplerState = nullptr;

// Shader data
VSPSShader* g_VSPSShader = nullptr;

// Shader resources
enum ConstantBuffer
{
	CB_Application,
	CB_Frame,
	CB_Object,
	NumConstantBuffers,
};

ID3D11Buffer* g_d3dConstantBuffers[ConstantBuffer::NumConstantBuffers];


XMMATRIX g_WorldMatrix;
XMMATRIX g_ViewMatrix;
XMMATRIX g_ProjectionMatrix;

struct VertexPosColor
{
	XMFLOAT3 Position;
	XMFLOAT3 Color;
};

struct VertexPosTexcoord
{
	XMFLOAT3 Position;
	XMFLOAT2 TexCoord;
};

// the vertices and indices of a cube
VertexPosColor g_Vertices[8] =
{
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
	{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
	{ XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
	{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
	{ XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
	{ XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
	{ XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
};

WORD g_Indicies[36] =
{
	0, 1, 2, 0, 2, 3,
	4, 6, 5, 4, 7, 6,
	4, 5, 1, 4, 1, 0,
	3, 2, 6, 3, 6, 7,
	1, 5, 6, 1, 6, 2,
	4, 0, 3, 4, 3, 7
};

VertexPosTexcoord g_VerticesTexcoord[] =
{
	// Front Face
	{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f)},
	{XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f)},
	{XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f)},
	{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f)},

	// Back Face
	{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},
	{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
	{XMFLOAT3(1.0f,  1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
	{XMFLOAT3(-1.0f,  1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},

	// Top Face
	{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f)},
	{XMFLOAT3(-1.0f, 1.0f,  1.0f), XMFLOAT2(0.0f, 0.0f)},
	{XMFLOAT3(1.0f, 1.0f,  1.0f), XMFLOAT2(1.0f, 0.0f)},
	{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f)},

	// Bottom Face
	{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f)},
	{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f)},
	{XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT2(0.0f, 0.0f)},
	{XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT2(1.0f, 0.0f)},

	// Left Face
	{XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT2(0.0f, 1.0f)},
	{XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT2(0.0f, 0.0f)},
	{XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f)},
	{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f)},

	// Right Face
	{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f)},
	{XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f)},
	{XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT2(1.0f, 0.0f)},
	{XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT2(1.0f, 1.0f)},
};

WORD g_IndicesTexcoord[] = {
	// Front Face
	0,  1,  2,
	0,  2,  3,

	// Back Face
	4,  5,  6,
	4,  6,  7,

	// Top Face
	8,  9, 10,
	8, 10, 11,

	// Bottom Face
	12, 13, 14,
	12, 14, 15,

	// Left Face
	16, 17, 18,
	16, 18, 19,

	// Right Face
	20, 21, 22,
	20, 22, 23
};


// Forward declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


bool LoadContent();
void UnloadContent();

void Update(float deltaTime);
void Render();
void Cleanup();

// -------------------------------------------

void Update(float deltaTime)
{
	static float angle = 0.0f;
	angle += 4.0f * deltaTime;

	// view matrix
	XMVECTOR eyePosition = XMVectorSet(0, 0, -10, 1);
	XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
	XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
	g_ViewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

	// world(model) matrix
	XMVECTOR rotationAxis = XMVectorSet(1, 0, 1, 0);

	//g_WorldMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));

	// all XMMatrix's matrices are row-major
	// which means the first transformation operation lies on the first factor of matrix multiplication
	g_WorldMatrix = XMMatrixScaling(1, (1.2f + cosf(angle)), 1) * XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle * 20.0f)) * XMMatrixTranslation(sinf(angle), 0, 0);

}

// Clear the color and depth buffers
void Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil)
{
	g_pGameContextD3D11->m_d3dDeviceContext->ClearRenderTargetView(g_pGameContextD3D11->m_d3dRenderTargetView, clearColor);
	g_pGameContextD3D11->m_d3dDeviceContext->ClearDepthStencilView(g_pGameContextD3D11->m_d3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
}

void Present(bool vSync)
{
	if (vSync)
	{
		g_pGameContextD3D11->m_d3dSwapChain->Present(1, 0);
	}
	else
	{
		g_pGameContextD3D11->m_d3dSwapChain->Present(0, 0);
	}
}

void Render()
{
	assert(g_pGameContextD3D11->m_d3dDevice);
	assert(g_pGameContextD3D11->m_d3dDeviceContext);

	// Clear the view. Of course
	// we need to clear both the view of render target(s) and stencil buffer.
	Clear(Colors::CornflowerBlue, 1.0f, 0);


	// Refresh constant buffer resources with the new data (which is the data updated from Update())
	g_pGameContextD3D11->m_d3dDeviceContext->UpdateSubresource(g_d3dConstantBuffers[ConstantBuffer::CB_Frame], 0, nullptr, &g_ViewMatrix, 0, 0);
	g_pGameContextD3D11->m_d3dDeviceContext->UpdateSubresource(g_d3dConstantBuffers[ConstantBuffer::CB_Object], 0, nullptr, &g_WorldMatrix, 0, 0);


	/* RS: rasterizer stage
	* D3D11 Pipeline: RS: rasterizer stage
	* interpolating various vertex attributes output from the vertex shader
	* invoke pixel shader program for each screen pixel which is affected by rendered geometry
	*/
	// I haven't found the doc telling the order of RS stage, but here is a warning I got when I put RS stage after a Draw(),
	//    The warning disappeared when I put RS stage before a Draw() so I guess that's where it should be (or at least, roughly)
	/*
	* D3D11 WARNING: ID3D11DeviceContext::DrawIndexed: There are no viewports currently bound.
	* If any rasterization to RenderTarget(s) and/or DepthStencil is performed, results will be undefined.
	* [ EXECUTION WARNING #384: DEVICE_DRAW_VIEWPORT_NOT_SET]
	*/
	g_pGameContextD3D11->m_d3dDeviceContext->RSSetState(g_pGameContextD3D11->m_d3dRasterizerState);
	g_pGameContextD3D11->m_d3dDeviceContext->RSSetViewports(1, &g_pGameContextD3D11->m_Viewport);


	// D3D11 Pipeline: OM: output merger stage
	// "merges the output from the pixel shader onto the color and depth buffers"
	// this is the stage where pixels are blended after the shaders are finished.
	//
	// OM stage does NOT have to follow a "typical" order when setting up the rendering pipeline
	// Because:
	// we may have multiple stencils and targets if we have addtional vertex and pixel shaders
	//    that generate textures like shadow maps, height maps, or other sampling techniques
	// in this case, as the code is, we will need to choose appropriate rendering target(s) set
	//    BEFORE we call a draw function.
	g_pGameContextD3D11->m_d3dDeviceContext->OMSetRenderTargets(1, &g_pGameContextD3D11->m_d3dRenderTargetView, g_pGameContextD3D11->m_d3dDepthStencilView);
	g_pGameContextD3D11->m_d3dDeviceContext->OMSetDepthStencilState(g_pGameContextD3D11->m_d3dDepthStencilState, 1);

	// D3D11 Pipeline: IA: input assembler stage
	//const UINT vertexStride = sizeof(VertexPosColor);
	const UINT vertexStride = sizeof(VertexPosTexcoord);
	const UINT offset = 0;

	/*
	g_pGameContextD3D11->m_d3dDeviceContext->IASetVertexBuffers(0, 1, &g_d3dVertexBuffer, &vertexStride, &offset);
	g_pGameContextD3D11->m_d3dDeviceContext->IASetIndexBuffer(g_d3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	*/
	g_pGameContextD3D11->m_d3dDeviceContext->IASetVertexBuffers(0, 1, &g_d3dVertexTexcoordBuffer, &vertexStride, &offset);
	g_pGameContextD3D11->m_d3dDeviceContext->IASetIndexBuffer(g_d3dIndexTexcoordBuffer, DXGI_FORMAT_R16_UINT, 0);
	////g_pGameContextD3D11->m_d3dDeviceContext->IASetInputLayout(g_d3dInputLayout);
	g_pGameContextD3D11->m_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// D3D11 Pipeline: VS: vertex shader stage
	////g_pGameContextD3D11->m_d3dDeviceContext->VSSetShader(g_d3dVertexShader, nullptr, 0);
	//g_VertexShader->BindInputLayoutAndShader(); // IASetInputLayout, VSSetShader
	g_pGameContextD3D11->m_d3dDeviceContext->VSSetConstantBuffers(0, 3, g_d3dConstantBuffers);
	g_VSPSShader->BindShaderAndLayout();

	/* test my understanding on the rendering pipeline. succeeded
	g_WorldMatrix = XMMatrixTranslation(2, 0, 0);
	g_pGameContextD3D11->m_d3dDeviceContext->UpdateSubresource(g_d3dConstantBuffers[ConstantBuffer::CB_Object], 0, nullptr, &g_WorldMatrix, 0, 0);
	g_pGameContextD3D11->m_d3dDeviceContext->VSSetConstantBuffers(0, 3, g_d3dConstantBuffers);
	g_pGameContextD3D11->m_d3dDeviceContext->DrawIndexed(_countof(g_Indicies), 0, 0);
	*/

	// D3D11 Pipeline: PS: pixel shader stage
	////g_pGameContextD3D11->m_d3dDeviceContext->PSSetShader(g_d3dPixelShader, nullptr, 0);
	//g_PixelShader->BindPixelShader(); // PSSetShader
	g_pGameContextD3D11->m_d3dDeviceContext->PSSetShaderResources(0, 1, &g_pShaderResourceView);
	g_pGameContextD3D11->m_d3dDeviceContext->PSSetSamplers(0, 1, &g_pSamplerState);

	// finally: we draw the cube
	//g_pGameContextD3D11->m_d3dDeviceContext->DrawIndexed(_countof(g_Indicies), 0, 0);
	g_pGameContextD3D11->m_d3dDeviceContext->DrawIndexedInstanced(_countof(g_Indicies), 1, 0, 0, 0);


	Present(g_EnableVSync);
}

bool LoadContent()
{
	assert(g_pGameContextD3D11->m_d3dDevice);

	{// create vertex & index buffer for the textured cube
		D3D11_BUFFER_DESC indexBufferDesc2;
		ZeroMemory(&indexBufferDesc2, sizeof(indexBufferDesc2));
		indexBufferDesc2.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc2.ByteWidth = sizeof(WORD) * _countof(g_IndicesTexcoord);
		indexBufferDesc2.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc2.CPUAccessFlags = 0;
		indexBufferDesc2.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA resourceData2;
		resourceData2.pSysMem = g_IndicesTexcoord;
		HRESULT hr = g_pGameContextD3D11->m_d3dDevice->CreateBuffer(&indexBufferDesc2, &resourceData2, &g_d3dIndexTexcoordBuffer);
		if (FAILED(hr))
		{
			return false;
		}

		D3D11_BUFFER_DESC vertexBufferDesc2;
		ZeroMemory(&vertexBufferDesc2, sizeof(vertexBufferDesc2));
		vertexBufferDesc2.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc2.ByteWidth = sizeof(VertexPosTexcoord) * _countof(g_VerticesTexcoord);
		vertexBufferDesc2.CPUAccessFlags = 0;
		vertexBufferDesc2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc2.MiscFlags = 0;
		resourceData2.pSysMem = g_VerticesTexcoord;
		hr = g_pGameContextD3D11->m_d3dDevice->CreateBuffer(&vertexBufferDesc2, &resourceData2, &g_d3dVertexTexcoordBuffer);
		if (FAILED(hr))
		{
			return false;
		}
	}
	{// load texture from a file
		HRESULT hr = CreateWICTextureFromFile(
			g_pGameContextD3D11->m_d3dDevice,
			g_pGameContextD3D11->m_d3dDeviceContext,
			L"../Textures/test.png",
			&g_pTexture2DResource,
			&g_pShaderResourceView
		);
		if (FAILED(hr))
		{
			return false;
		}

		// also here we can use 
		// g_pTexture2DResource->GetType(D3D11_RESOURCE_DIMENSION*)
		// to get the type during runtime if we do not know what exactly the type is
		hr = g_pTexture2DResource->QueryInterface(IID_ID3D11Texture2D, (void**)&g_pTexture2D);
		if (FAILED(hr))
		{
			return false;
		}

		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = g_pGameContextD3D11->m_d3dDevice->CreateSamplerState(&samplerDesc, &g_pSamplerState);
		if (FAILED(hr))
		{
			return false;
		}

	}

	{// create the constant buffers for the variables defined 
		D3D11_BUFFER_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.ByteWidth = sizeof(XMMATRIX);
		constantBufferDesc.CPUAccessFlags = 0;
		constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		HRESULT hr = g_pGameContextD3D11->m_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dConstantBuffers[ConstantBuffer::CB_Application]);
		if (FAILED(hr))
		{
			return false;
		}

		hr = g_pGameContextD3D11->m_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dConstantBuffers[ConstantBuffer::CB_Frame]);
		if (FAILED(hr))
		{
			return false;
		}

		hr = g_pGameContextD3D11->m_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dConstantBuffers[ConstantBuffer::CB_Object]);
		if (FAILED(hr))
		{
			return false;
		}
	}

	{// set input layout and compile shader
		D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, Position), D3D11_INPUT_PER_VERTEX_DATA, 0},
			//{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,Color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosColor,Color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		/*
		g_VertexShader = new VertexShader(g_pGameContextD3D11);
		g_VertexShader->LoadFromFileAndCompile(L"../Shaders/SimpleVertexShader.hlsl", "SimpleVertexShader", "latest", vertexLayoutDesc, _countof(vertexLayoutDesc));

		g_PixelShader = new PixelShader(g_pGameContextD3D11);
		g_PixelShader->LoadFromFileAndCompile(L"../Shaders/SimplePixelShader.hlsl", "SimplePixelShader", "latest");
		*/
		g_VSPSShader = new VSPSShader(g_pGameContextD3D11);
		g_VSPSShader->LoadAndCompileShaderWithInputLayout(
			L"../Shaders/SimpleVertexShader.hlsl", "SimpleVertexShader",
			L"../Shaders/SimplePixelShader.hlsl", "SimplePixelShader",
			vertexLayoutDesc, _countof(vertexLayoutDesc));

	}


	{// setup the projection matrix
		RECT clientRect;
		GetClientRect(g_WindowHandle, &clientRect);

		// Compute the exact client dimensions
		// This is required for a correct projection matrix
		float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
		float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

		g_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), clientWidth / clientHeight, 0.1f, 100.0f);
		g_pGameContextD3D11->m_d3dDeviceContext->UpdateSubresource(g_d3dConstantBuffers[ConstantBuffer::CB_Application], 0, nullptr, &g_ProjectionMatrix, 0, 0);
	}
	return true;
}

void UnloadContent()
{
	SafeRelease(g_d3dConstantBuffers[ConstantBuffer::CB_Application]);
	SafeRelease(g_d3dConstantBuffers[ConstantBuffer::CB_Frame]);
	SafeRelease(g_d3dConstantBuffers[ConstantBuffer::CB_Object]);
	SafeRelease(g_d3dInputLayout);
	//SafeRelease(g_d3dVertexShader);
	//SafeRelease(g_d3dPixelShader);
	//delete(g_VertexShader);
	//delete(g_PixelShader);
	delete(g_VSPSShader);
}

void Cleanup()
{
	g_pGameContextD3D11->Release();
	delete(g_pGameContextD3D11);
}

// The main application loop
int Run()
{
	MSG msg = { 0 };

	static DWORD previousTime = timeGetTime();
	static const float targetFramerate = 30.0f;
	static const float maxTimeStep = 1.0f / targetFramerate;

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			DWORD currentTime = timeGetTime();
			float deltaTime = (currentTime - previousTime);
			previousTime = currentTime;

			// Cap the delta time to the max time step
			// (useful if your debugging and you don't want the deltaTime value to explode)
			deltaTime = std::min<float>(deltaTime, maxTimeStep);

			Update(deltaTime);
			Render();
		}
	}
	return static_cast<int>(msg.wParam);
}

DXGI_RATIONAL QueryRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vsync)
{
	DXGI_RATIONAL refreshRate = { 0, 1 };
	if (vsync)
	{
		IDXGIFactory* factory;
		IDXGIAdapter* adapter;
		IDXGIOutput* adapterOutput;
		DXGI_MODE_DESC* displayModeList;

		// Create a DirectX graphics interface factory.
		HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Could not create DXGIFactory instance."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to create DXGIFactory.");
		}

		hr = factory->EnumAdapters(0, &adapter);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to enumerate adapters."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to enumerate adapters.");
		}

		hr = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to enumerate adapter outputs."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to enumerate adapter outputs.");
		}

		UINT numDisplayModes;
		hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, nullptr);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to query display mode list."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to query display mode list.");
		}

		displayModeList = new DXGI_MODE_DESC[numDisplayModes];
		assert(displayModeList);

		hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, displayModeList);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to query display mode list."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to query display mode list.");
		}

		// Now store the refresh rate of the monitor that matches the width and height of the requested screen.
		for (UINT i = 0; i < numDisplayModes; ++i)
		{
			if (displayModeList[i].Width == screenWidth && displayModeList[i].Height == screenHeight)
			{
				refreshRate = displayModeList[i].RefreshRate;
			}
		}

		delete[] displayModeList;
		SafeRelease(adapterOutput);
		SafeRelease(adapter);
		SafeRelease(factory);
	}

	return refreshRate;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	HDC hDC;

	switch (message)
	{
	case WM_PAINT:
	{
		hDC = BeginPaint(hwnd, &paintStruct);
		EndPaint(hwnd, &paintStruct);
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}
//Initialize the application window.
int InitApplication(HINSTANCE hInstance, int cmdShow)
{
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = &WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = g_WindowClassName;

	if (!RegisterClassEx(&wndClass))
	{
		return -1;
	}

	RECT windowRect = { 0, 0, g_WindowWidth, g_WindowHeight };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	g_WindowHandle = CreateWindow(g_WindowClassName, g_WindowName,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);

	if (!g_WindowHandle)
	{
		return -1;
	}

	ShowWindow(g_WindowHandle, cmdShow);
	UpdateWindow(g_WindowHandle);

	return 0;
}

int InitDirectX(HINSTANCE hInstance, BOOL vSync)
{
	// A window handle must have been created already
	assert(g_WindowHandle != 0);

	g_pGameContextD3D11 = new GameContextD3D11();

	RECT clientRect;
	GetClientRect(g_WindowHandle, &clientRect);

	// Compute the exact client dimensions
	// This will be used to initialize the render targets for our swap chain
	unsigned int clientWidth = clientRect.right - clientRect.left;
	unsigned int clientHeight = clientRect.bottom - clientRect.top;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = clientWidth;
	swapChainDesc.BufferDesc.Height = clientHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate = QueryRefreshRate(clientWidth, clientHeight, vSync);
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = g_WindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	/* DXGI WARNING: IDXGIFactory::CreateSwapChain: Blt-model swap effects (DXGI_SWAP_EFFECT_DISCARD and DXGI_SWAP_EFFECT_SEQUENTIAL) are legacy swap effects
	 *     that are predominantly superceded by their flip-model counterparts (DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL and DXGI_SWAP_EFFECT_FLIP_DISCARD).
	 *     Please consider updating your application to leverage flip-model swap effects to benefit from modern presentation enhancements.
	 *     More information is available at http://aka.ms/dxgiflipmodel. [ MISCELLANEOUS WARNING #294: ]
	 * I haven't read it carefully but I noticed that there may be something that the newer swapeffects are not able to do
	 *     So I decided to leave it legacy for now since I haven't fully understood the things here
	*/
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = TRUE;

	UINT createDeviceFlags = 0;
#if _DEBUG
	createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// These are the feature levels that we will accept
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	// This will be the feature level 
	// that is used to create our device and swap chain
	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, createDeviceFlags, featureLevels, _countof(featureLevels),
		D3D11_SDK_VERSION, &swapChainDesc, &g_pGameContextD3D11->m_d3dSwapChain, &g_pGameContextD3D11->m_d3dDevice, &featureLevel,
		&g_pGameContextD3D11->m_d3dDeviceContext
	);

	if (hr == E_INVALIDARG)
	{
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr, D3D_DRIVER_TYPE_HARDWARE,
			nullptr, createDeviceFlags, &featureLevels[1], _countof(featureLevels) - 1,
			D3D11_SDK_VERSION, &swapChainDesc, &g_pGameContextD3D11->m_d3dSwapChain, &g_pGameContextD3D11->m_d3dDevice, &featureLevel,
			&g_pGameContextD3D11->m_d3dDeviceContext
		);
	}

	if (FAILED(hr))
	{
		return -1;
	}

	// Next initialize the back buffer of the swap chain and associate it to a render target view
	ID3D11Texture2D* backBuffer;
	hr = g_pGameContextD3D11->m_d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(hr))
	{
		return -1;
	}

	hr = g_pGameContextD3D11->m_d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &g_pGameContextD3D11->m_d3dRenderTargetView);
	if (FAILED(hr))
	{
		return -1;
	}

	SafeRelease(backBuffer);

	// Create the depth buffer for use with the depth/stencil view
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0; // No CPU access required
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.Width = clientWidth;
	depthStencilBufferDesc.Height = clientHeight;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = g_pGameContextD3D11->m_d3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &g_pGameContextD3D11->m_d3dDepthStencilBuffer);
	if (FAILED(hr))
	{
		return -1;
	}

	hr = g_pGameContextD3D11->m_d3dDevice->CreateDepthStencilView(g_pGameContextD3D11->m_d3dDepthStencilBuffer, nullptr, &g_pGameContextD3D11->m_d3dDepthStencilView);
	if (FAILED(hr))
	{
		return -1;
	}

	// Setup depth/stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilStateDesc.DepthEnable = TRUE;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilStateDesc.StencilEnable = FALSE;

	hr = g_pGameContextD3D11->m_d3dDevice->CreateDepthStencilState(&depthStencilStateDesc, &g_pGameContextD3D11->m_d3dDepthStencilState);
	if (FAILED(hr))
	{
		return -1;
	}

	// Setup rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	// create the rasterizer state object
	hr = g_pGameContextD3D11->m_d3dDevice->CreateRasterizerState(&rasterizerDesc, &g_pGameContextD3D11->m_d3dRasterizerState);
	if (FAILED(hr))
	{
		return -1;
	}

	// Initialize the viewport to occupy the entire client area
	g_pGameContextD3D11->m_Viewport.Width = static_cast<float>(clientWidth);
	g_pGameContextD3D11->m_Viewport.Height = static_cast<float>(clientHeight);
	g_pGameContextD3D11->m_Viewport.TopLeftX = 0.0f;
	g_pGameContextD3D11->m_Viewport.TopLeftY = 0.0f;
	g_pGameContextD3D11->m_Viewport.MinDepth = 0.0f;
	g_pGameContextD3D11->m_Viewport.MaxDepth = 1.0f;

	return 0;
}

/* //test __count_of()
template <typename _MyType, size_t SizeOfArray>
char(*my_count_of(const _MyType(&_InArrayName)[SizeOfArray]))[SizeOfArray];

int testArr[] = { 1, 2, 3 };

size_t f = (sizeof(*my_count_of(testArr)+0));


auto ff = my_count_of(testArr); // this line will result in unresolved symbol because it's a real function call
char* fff[3];
char(*ffff)[3] = ff;
*/


int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE prevInstance,
	_In_ LPWSTR cmdLine,
	_In_ int cmdShow)
{
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	// Check for DirectX Math library support.
	if (!XMVerifyCPUSupport())
	{
		MessageBox(nullptr, TEXT("Failed to verify DirectX Math library support."), TEXT("Error"), MB_OK);
		return -1;
	}

	if (InitApplication(hInstance, cmdShow) != 0)
	{
		MessageBox(nullptr, TEXT("Failed to create applicaiton window."), TEXT("Error"), MB_OK);
		return -1;
	}

	if (InitDirectX(hInstance, g_EnableVSync) != 0)
	{
		MessageBox(nullptr, TEXT("Failed to create DirectX device and swap chain"), TEXT("Error"), MB_OK);
		return -1;
	}

	if (!LoadContent())
	{
		MessageBox(nullptr, TEXT("Failed to load content."), TEXT("Error"), MB_OK);
		return -1;
	}

	int returnCode = Run();

	UnloadContent();
	Cleanup();

	return returnCode;
}

