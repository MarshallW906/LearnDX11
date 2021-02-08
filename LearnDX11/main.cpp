#include "DirectXTemplatePCH.h"

#include "GameContextD3D11.h"

#include "VSPSShader.h"
#include "Mesh.h"
#include "MeshInstance.h"
#include "Camera.h"

#include "WICTextureLoader.h"

#include <queue>
#include <iostream>
#include <mutex>

const UINT MAX_NUM_MESH_INSTANCE = 1000;


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

// vertex/index buffer
ID3D11Buffer* g_CubeVertexTexcoordBuffer = nullptr;
ID3D11Buffer* g_CubeIndexBuffer = nullptr;

ID3D11Buffer* g_CylinderVertexTexcoordBuffer = nullptr;
ID3D11Buffer* g_CylinderIndexBuffer = nullptr;

// textures
ID3D11Resource* g_pCubeTexture2DResource = nullptr;
ID3D11ShaderResourceView* g_pCubeShaderResourceView = nullptr;

ID3D11Resource* g_pCylinderTexture2DResource = nullptr;
ID3D11ShaderResourceView* g_pCylinderShaderResourceView = nullptr;

ID3D11Resource* g_pSkyboxTextureResource = nullptr;
ID3D11ShaderResourceView* g_pSkyboxShaderResourceView = nullptr;

ID3D11SamplerState* g_pSamplerState = nullptr;

Camera* g_pCamera;

// Shader data
VSPSShader* g_pShaderSimpleStaticMesh = nullptr;

Mesh* g_pCubeMesh;
Mesh* g_pCylinderMesh;

UINT g_carBodyIndex = 0;
UINT g_carWheelsStartIndex = 0; // 0: front-left, 1: front-right, 2: back-left, 3: back-right
UINT g_skyboxIndex = 0;

UINT g_debugCubeIndexCameraPosFirstView = 0;

// Shader resources
enum CBStaticMesh
{
	CB_Application,
	CB_Frame,
	NumCBStaticMesh,
};

ID3D11Buffer* g_CBStaticMesh[CBStaticMesh::NumCBStaticMesh];

XMMATRIX g_ViewMatrix;
XMMATRIX g_ProjectionMatrix;

struct Vertex
{
	XMFLOAT3 Position;
};

struct VertexPosTexcoord
{
	XMFLOAT3 Position;
	XMFLOAT2 TexCoord;
};

UINT VertexPosStride = sizeof(Vertex);
UINT VertexPosTexcoordStride = sizeof(VertexPosTexcoord);
UINT offset = 0;

VertexPosTexcoord g_CubeVerticesTexcoord[] =
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

WORD g_CubeIndicesTexcoord[] = {
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

void GenerateCylinderVerticesAndTexcoords(float radius, float height, unsigned int numPolygonSides);
std::vector<VertexPosTexcoord> g_CylinderVerticesTexcoord;
std::vector<WORD> g_CylinderIndices;

// skybox
UINT GenerateSphereVertexAndIndexBuffer(int LatLines, int LongLines);
VSPSShader* g_pSkyboxShader = nullptr;

// input events
enum class KeyboardMouseEvents
{
	EVENT_KEY_A_HELD,
	EVENT_KEY_D_HELD,
	EVENT_KEY_W_HELD,
	EVENT_KEY_S_HELD,
	// switch camera mode
	EVENT_KEY_Q_PRESSED,
	// mouse
	EVENT_MOUSE_MOVEMENT,
	EVENT_MOUSE_WHEEL,
};
std::queue<KeyboardMouseEvents> g_eventQueue;
XMFLOAT2 g_moveByInput;
bool g_shouldSwitchCameraMode;
int g_realtimeMouseXPos = 0;
int g_realtimeMouseYPos = 0;
int g_realtimeWheelDelta = 0;
std::mutex g_mutexThreadContext;

// Forward declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void UpdateMousePosRealtime(int xPos, int yPos);
void OnMousePosChangedPerFrame(float deltaTime);
void UpdateMouseWheelRealtime(int wheelDelta);
void OnMouseWheelChangedPerFrame(float deltaTime);

bool LoadAndGenerateBuffers();
bool ConstructWorld();

void Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil);
void Present(bool vSync);

void DetectKeyboardAndMouseEvents();
void ProcessEvents(float deltaTime);

void Update(float deltaTime);
void Render();

void UnloadContent();
void Cleanup();

// -------------------------------------------

const float carRotateSpeed = 50.0f;
const float carWheelRotateSpeed = 100.0f;
const float carMoveSpeed = 5.0f;

void Update(float deltaTime)
{
	static float angle = 0.0f;
	angle += 4.0f * deltaTime;

	float angleRadians = XMConvertToRadians(angle);

	DetectKeyboardAndMouseEvents();
	ProcessEvents(deltaTime);

	if (g_shouldSwitchCameraMode)
	{
		g_pCamera->ToggleCameraMode();
	}

	// view matrix
	XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
	g_ViewMatrix = g_pCamera->GetViewMatrix(upDirection);

	// world(model) matrix
	std::vector<MeshInstance*>& cubeInstances = g_pCubeMesh->GetAllInstances();
	std::vector<MeshInstance*>& cylinderInstances = g_pCylinderMesh->GetAllInstances();

	// keep skybox centered around camera
	static XMMATRIX skyboxScale = XMMatrixIdentity();// XMMatrixScaling(1, 1, 1);
	XMVECTOR curCamPos = g_pCamera->GetCurrentPosition();
	cubeInstances[g_skyboxIndex]->UpdateLocalTransform(
		skyboxScale * XMMatrixTranslation(
			XMVectorGetX(curCamPos), XMVectorGetY(curCamPos), XMVectorGetZ(curCamPos)
		)
	);

	cubeInstances[g_debugCubeIndexCameraPosFirstView]->UpdateLocalTransform(XMMatrixTranslationFromVector(g_pCamera->DebugGetCameraPosInFirstView()));

	// Car movement
	{
		MeshInstance* carBodyInstance = cubeInstances[g_carBodyIndex];

		MeshInstance* pFrontLeftWheel = cylinderInstances[g_carWheelsStartIndex];
		MeshInstance* pFrontRightWheel = cylinderInstances[g_carWheelsStartIndex + 1];
		MeshInstance* pBackLeftWheel = cylinderInstances[g_carWheelsStartIndex + 2];
		MeshInstance* pBackRightWheel = cylinderInstances[g_carWheelsStartIndex + 3];


		bool carShouldMove = fabsf(g_moveByInput.y) > 0;
		bool carShouldTurn = fabsf(g_moveByInput.x) > 0;

		if (carShouldTurn)
		{
			// rotate front wheels around its Y-axis
			pFrontLeftWheel->SetRotationRollPitchYaw(0, XMConvertToRadians(30 * g_moveByInput.x), XMConvertToRadians(90));
			pFrontRightWheel->SetRotationRollPitchYaw(0, XMConvertToRadians(30 * g_moveByInput.x), XMConvertToRadians(90));

			if (carShouldMove)
			{
				bool shouldFlipRotation = g_moveByInput.y < 0;
				float flipRotMultiplier = shouldFlipRotation ? -1 : 1;
				carBodyInstance->SelfRotateByRollPitchYawInDegrees(0, carRotateSpeed * deltaTime * g_moveByInput.x * flipRotMultiplier, 0);
			}
		}
		else
		{
			pFrontLeftWheel->SetRotationRollPitchYaw(0, 0, XMConvertToRadians(90));
			pFrontRightWheel->SetRotationRollPitchYaw(0, 0, XMConvertToRadians(90));
		}

		if (carShouldMove)
		{
			// TODO: rotate front & back wheels when moving by X-axis
			bool shouldFlipRotation = g_moveByInput.y < 0;
			float flipRotMultiplier = shouldFlipRotation ? -1 : 1;

			// [!!!!!!!!!!!!!!!!FATAL!!!!!!!!!!!!!!!!!]
			// uncomment this will show the current rotation calculation's problem
			// which is, that localScale is totally ignored, but we need the localScale to rotate as well
			// also, because the euler angles were pre-set above, it will actually not rotate around 360 degrees
			/* 
			pFrontLeftWheel->UpdateLocalTransform(
				XMMatrixRotationY(carWheelRotateSpeed * flipRotMultiplier) * pFrontLeftWheel->GetLocalTransform()
			);
			pFrontRightWheel->UpdateLocalTransform(
				XMMatrixRotationY(carWheelRotateSpeed * flipRotMultiplier) * pFrontRightWheel->GetLocalTransform()
			);*/
			pBackLeftWheel->UpdateLocalTransform(
				XMMatrixRotationY(carWheelRotateSpeed * flipRotMultiplier) * pBackLeftWheel->GetLocalTransform()
			);
			pBackRightWheel->UpdateLocalTransform(
				XMMatrixRotationY(carWheelRotateSpeed * flipRotMultiplier) * pBackRightWheel->GetLocalTransform()
			);
			
			// TODO: Add util functions like moveForward()
			XMVECTOR vForward = XMVector3Normalize(carBodyInstance->GetLocalTransform().r[2]);
			XMMATRIX translation = XMMatrixTranslationFromVector(vForward * carMoveSpeed * deltaTime * g_moveByInput.y);
			carBodyInstance->UpdateLocalTransform(
				carBodyInstance->GetLocalTransform() * translation
			);
		}
	}

	// post-update
}

void Render()
{
	assert(g_pGameContextD3D11->m_d3dDevice);
	assert(g_pGameContextD3D11->m_d3dDeviceContext);

	// Clear the view. Of course
	// we need to clear both the view of render target(s) and stencil buffer.
	Clear(Colors::CornflowerBlue, 1.0f, 0);

	// Refresh constant buffer resources with the new data (which is the data updated from Update())
	g_pGameContextD3D11->m_d3dDeviceContext->UpdateSubresource(g_CBStaticMesh[CBStaticMesh::CB_Frame], 0, nullptr, &g_ViewMatrix, 0, 0);

	// common settings
	g_pGameContextD3D11->m_d3dDeviceContext->OMSetRenderTargets(1, &g_pGameContextD3D11->m_d3dRenderTargetView, g_pGameContextD3D11->m_d3dDepthStencilView);
	g_pGameContextD3D11->m_d3dDeviceContext->RSSetViewports(1, &g_pGameContextD3D11->m_Viewport);
	g_pGameContextD3D11->m_d3dDeviceContext->VSSetConstantBuffers(0, CBStaticMesh::NumCBStaticMesh, g_CBStaticMesh);
	g_pGameContextD3D11->getDeviceContext()->PSSetSamplers(0, 1, &g_pSamplerState);

	// draw skybox first
	g_pGameContextD3D11->getDeviceContext()->RSSetState(g_pGameContextD3D11->m_d3dRasterizerStateCullNone);
	g_pGameContextD3D11->getDeviceContext()->OMSetDepthStencilState(g_pGameContextD3D11->m_d3dDepthStencilStateLessEqual, 1);

	g_pGameContextD3D11->m_d3dDeviceContext->VSSetConstantBuffers(CBStaticMesh::NumCBStaticMesh, 1, &g_pGameContextD3D11->m_CBSingleWorldMatrix);

	g_pSkyboxShader->BindShaderAndLayout();
	g_pGameContextD3D11->getDeviceContext()->PSSetShaderResources(0, 1, &g_pSkyboxShaderResourceView);
	std::vector<MeshInstance*>& refCubeInstances = g_pCubeMesh->GetAllInstances();
	refCubeInstances[g_skyboxIndex]->DrawSelf();

	// =====================================

	// then all other objects
	g_pGameContextD3D11->m_d3dDeviceContext->RSSetState(g_pGameContextD3D11->m_d3dRasterizerStateDefault);
	g_pGameContextD3D11->m_d3dDeviceContext->OMSetDepthStencilState(g_pGameContextD3D11->m_d3dDepthStencilStateDefault, 1);

	g_pGameContextD3D11->m_d3dDeviceContext->VSSetConstantBuffers(CBStaticMesh::NumCBStaticMesh, 1, &g_pGameContextD3D11->m_constantBufferAllMeshPositions);

	g_pShaderSimpleStaticMesh->BindShaderAndLayout();

	g_pGameContextD3D11->m_d3dDeviceContext->PSSetShaderResources(0, 1, &g_pCubeShaderResourceView);
	g_pCubeMesh->DrawAllInstances();

	g_pGameContextD3D11->m_d3dDeviceContext->PSSetShaderResources(0, 1, &g_pCylinderShaderResourceView);
	g_pCylinderMesh->DrawAllInstances();
	
	//

	Present(g_EnableVSync);
}

bool LoadAndGenerateBuffers()
{
	assert(g_pGameContextD3D11->m_d3dDevice);

	// create vertex & index buffer for the textured cube
	{
		D3D11_BUFFER_DESC indexBufferDesc2;
		ZeroMemory(&indexBufferDesc2, sizeof(indexBufferDesc2));
		indexBufferDesc2.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc2.ByteWidth = sizeof(WORD) * _countof(g_CubeIndicesTexcoord);
		indexBufferDesc2.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc2.CPUAccessFlags = 0;
		indexBufferDesc2.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA resourceData2;
		resourceData2.pSysMem = g_CubeIndicesTexcoord;
		HRESULT hr = g_pGameContextD3D11->m_d3dDevice->CreateBuffer(&indexBufferDesc2, &resourceData2, &g_CubeIndexBuffer);
		if (FAILED(hr))
		{
			return false;
		}

		D3D11_BUFFER_DESC vertexBufferDesc2;
		ZeroMemory(&vertexBufferDesc2, sizeof(vertexBufferDesc2));
		vertexBufferDesc2.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc2.ByteWidth = sizeof(VertexPosTexcoord) * _countof(g_CubeVerticesTexcoord);
		vertexBufferDesc2.CPUAccessFlags = 0;
		vertexBufferDesc2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc2.MiscFlags = 0;
		resourceData2.pSysMem = g_CubeVerticesTexcoord;
		hr = g_pGameContextD3D11->m_d3dDevice->CreateBuffer(&vertexBufferDesc2, &resourceData2, &g_CubeVertexTexcoordBuffer);
		if (FAILED(hr))
		{
			return false;
		}
	}

	// create vertex & index buffer for cylinder
	{
		GenerateCylinderVerticesAndTexcoords(1, 4, 16U);

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VertexPosTexcoord) * g_CylinderVerticesTexcoord.size();
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA resourceData;
		resourceData.pSysMem = &g_CylinderVerticesTexcoord[0];
		HRESULT hr = g_pGameContextD3D11->getDevice()->CreateBuffer(&vertexBufferDesc, &resourceData, &g_CylinderVertexTexcoordBuffer);
		if (FAILED(hr))
		{
			return false;
		}

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(VertexPosTexcoord) * g_CylinderIndices.size();
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.MiscFlags = 0;
		resourceData.pSysMem = &g_CylinderIndices[0];
		hr = g_pGameContextD3D11->getDevice()->CreateBuffer(&indexBufferDesc, &resourceData, &g_CylinderIndexBuffer);
		if (FAILED(hr))
		{
			return false;
		}
	}

	// load image texture from a file
	{
		HRESULT hr = CreateWICTextureFromFile(
			g_pGameContextD3D11->m_d3dDevice,
			g_pGameContextD3D11->m_d3dDeviceContext,
			L"../Textures/brickwall.jpg",
			&g_pCubeTexture2DResource,
			&g_pCubeShaderResourceView
		);
		if (FAILED(hr))
		{
			return false;
		}

		hr = CreateWICTextureFromFile(
			g_pGameContextD3D11->m_d3dDevice,
			g_pGameContextD3D11->m_d3dDeviceContext,
			L"../Textures/crate2_h2.png",
			&g_pCylinderTexture2DResource,
			&g_pCylinderShaderResourceView
		);
		if (FAILED(hr))
		{
			return false;
		}

		hr = CreateWICTextureFromFile(
			g_pGameContextD3D11->m_d3dDevice,
			g_pGameContextD3D11->m_d3dDeviceContext,
			L"../Textures/skybox/top.jpg",
			&g_pSkyboxTextureResource,
			&g_pSkyboxShaderResourceView
		);


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

	// create the constant buffers for the variables defined 
	{
		D3D11_BUFFER_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.ByteWidth = sizeof(XMMATRIX);
		constantBufferDesc.CPUAccessFlags = 0;
		constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		HRESULT hr = g_pGameContextD3D11->m_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_CBStaticMesh[CBStaticMesh::CB_Application]);
		if (FAILED(hr))
		{
			return false;
		}

		hr = g_pGameContextD3D11->m_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_CBStaticMesh[CBStaticMesh::CB_Frame]);
		if (FAILED(hr))
		{
			return false;
		}

		hr = g_pGameContextD3D11->m_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_pGameContextD3D11->m_CBSingleWorldMatrix);
		if (FAILED(hr))
		{
			return false;
		}

		//----------------------------------
		constantBufferDesc.ByteWidth = sizeof(XMMATRIX) * MAX_NUM_MESH_INSTANCE;
		hr = g_pGameContextD3D11->m_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_pGameContextD3D11->m_constantBufferAllMeshPositions);
		if (FAILED(hr))
		{
			return false;
		}
	}

	// generate input layout and compile shader
	{
		// for static meshes
		D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTexcoord, Position), D3D11_INPUT_PER_VERTEX_DATA, 0},
			//{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,Color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosTexcoord, TexCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		g_pShaderSimpleStaticMesh = new VSPSShader(g_pGameContextD3D11);
		g_pShaderSimpleStaticMesh->LoadAndCompileShaderWithInputLayout(
			L"../Shaders/StaticMesh_VS.hlsl", "StaticMesh_VS", "latest",
			L"../Shaders/StaticMesh_PS.hlsl", "StaticMesh_PS", "latest",
			vertexLayoutDesc, _countof(vertexLayoutDesc));
	}

	// skybox shader
	{

		g_pSkyboxShader = new VSPSShader(g_pGameContextD3D11);

		D3D11_INPUT_ELEMENT_DESC skyboxLayoutDesc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTexcoord, Position), D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosTexcoord, TexCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		g_pSkyboxShader->LoadAndCompileShaderWithInputLayout(
			L"../Shaders/Skymap_VS.hlsl", "SKYMAP_VS", "vs_4_0",
			L"../Shaders/Skymap_PS.hlsl", "SKYMAP_PS", "ps_4_0",
			skyboxLayoutDesc, _countof(skyboxLayoutDesc)
		);
	}


	{// setup the projection matrix
		RECT clientRect;
		GetClientRect(g_WindowHandle, &clientRect);

		// Compute the exact client dimensions
		// This is required for a correct projection matrix
		float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
		float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

		g_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), clientWidth / clientHeight, 0.1f, 100.0f);
		g_pGameContextD3D11->m_d3dDeviceContext->UpdateSubresource(g_CBStaticMesh[CBStaticMesh::CB_Application], 0, nullptr, &g_ProjectionMatrix, 0, 0);
	}

	ConstructWorld();
	return true;
}

bool ConstructWorld()
{
	// create Cube & Cylinder Mesh
	g_pCubeMesh = new Mesh(g_pGameContextD3D11, &g_CubeVertexTexcoordBuffer, 1, &VertexPosTexcoordStride, &offset,
		g_CubeIndexBuffer, _countof(g_CubeIndicesTexcoord), DXGI_FORMAT_R16_UINT, offset);

	g_pCylinderMesh = new Mesh(g_pGameContextD3D11,
		&g_CylinderVertexTexcoordBuffer, 1, &VertexPosTexcoordStride, &offset,
		g_CylinderIndexBuffer, g_CylinderIndices.size(), DXGI_FORMAT_R16_UINT, offset);

	std::vector<MeshInstance*>& refCubeInstances = g_pCubeMesh->GetAllInstances();
	std::vector<MeshInstance*>& refCylinderInstances = g_pCylinderMesh->GetAllInstances();

	XMMATRIX skyboxTransform = XMMatrixScaling(500, 500, 500);
	g_skyboxIndex = g_pCubeMesh->GenerateInstances(&skyboxTransform, 1, false);
	refCubeInstances[g_skyboxIndex]->SetDrawnIndividually(true);

	// generate some ornaments
	XMMATRIX cubeOrnaments[] = {
		XMMatrixScaling(50, 0.1, 50) * XMMatrixTranslation(0, -2, 0), // cubeplane
		XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(-14.5, 0, 11.5),
		XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(14.5, 0, 11.5),
		XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(-14.5, 0, -11.5),
		XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(14.5, 0, -11.5),
	};
	g_pCubeMesh->GenerateInstances(cubeOrnaments, _countof(cubeOrnaments), true);

	// 2: construct a car by one cube body & 4 cylinder wheels
	XMMATRIX CarBody = XMMatrixScaling(2, 0.5, 3);
	g_carBodyIndex = g_pCubeMesh->GenerateInstances(&CarBody, 1, false);

	constexpr float radians90Degree = XMConvertToRadians(90);
	XMMATRIX CylinderCarWheels[] = {
		XMMatrixScaling(.6, 0.2, .6) * XMMatrixRotationRollPitchYaw(0, 0, radians90Degree) * XMMatrixTranslation(-1.8, -1.2, 2.2), // front left
		XMMatrixScaling(.6, 0.2, .6) * XMMatrixRotationRollPitchYaw(0, 0, radians90Degree) * XMMatrixTranslation(1.8, -1.2, 2.2), // front right
		XMMatrixScaling(.6, 0.2, .6) * XMMatrixRotationRollPitchYaw(0, 0, radians90Degree) * XMMatrixTranslation(-1.8, -1.2, -2.2), // back left
		XMMatrixScaling(.6, 0.2, .6) * XMMatrixRotationRollPitchYaw(0, 0, radians90Degree) * XMMatrixTranslation(1.8, -1.2, -2.2), // back right
	};
	g_carWheelsStartIndex = g_pCylinderMesh->GenerateInstances(CylinderCarWheels, _countof(CylinderCarWheels), false);
	for (int i = g_carWheelsStartIndex; i < 4; i++)
	{
		refCylinderInstances[g_carWheelsStartIndex + i]->SetParentMeshInstance(refCubeInstances[g_carBodyIndex]);
	}
	XMMATRIX cylinderCarCap = XMMatrixScaling(.5, 0.8, .5) * XMMatrixRotationRollPitchYaw(radians90Degree, 0, 0) * XMMatrixTranslation(0, 1, 1);
	UINT carCapIndex = g_pCylinderMesh->GenerateInstances(&cylinderCarCap, 1, false);
	refCylinderInstances[carCapIndex]->SetParentMeshInstance(refCubeInstances[g_carBodyIndex]);

	// create Camera
	g_pCamera = new Camera();
	g_pCamera->SetEnableFirstPersonView(true);
	g_pCamera->SetFollowTarget(refCubeInstances[g_carBodyIndex]);
	g_pCamera->SetThirdViewFollowDistance(20); // 20 is default value
	XMMATRIX debugCube = XMMatrixScaling(0.3, 0.3, 0.3);
	g_debugCubeIndexCameraPosFirstView = g_pCubeMesh->GenerateInstances(&debugCube, 1, false);
	refCubeInstances[g_debugCubeIndexCameraPosFirstView]->SetEnableDraw(false);


	// [OK] NEXT STEP 1: generate some (TODO: static) cube/cylinders as ornaments, put them in the scene
	// [OK] NEXT STEP 2: construct a "car" (4 cylinders under 1 cube), expose references of the "car"'s root, and its two front wheels
	// [OK] NEXT STEP 3: add keyboard detection, WASD move the car
	// [Partially OK (1/2)](optional) NEXT STEP 3.1: when the car is moving, apply rotation on wheels accordingly 
	//		     [!!!!!!!!!!!!!!!!FATAL!!!!!!!!!!!!!!!!!] TODO: figure out how exactly rotation works in XMMATRIX
	//			 The current method to calc localRotation is not a general solution
	//		     ([Not OK] all wheels: forward/backward; [OK] front wheels: rotate left/right)
	// [OK] NEXT STEP 4: skybox. [TODO]: Use cubemap instead of a simple textured cube diabling back-face culling
	// [OK] NEXT STEP 5.1: camera mode: 1st person view
	// [OK] NEXT STEP 5.2: camera mode: 3rd person view
	// [OK] NEXT STEP 6: camera mode: switch between 1st & 3rd
	// [OK] NEXT STEP 7: camera: mouse movement under 3rd person view: RotateAround
	// [OK]			  Next: mouse-control rotate around
	// [Known issue]: When camera rotates, ONLY the car followed get sheared
	//				may be because of w-component is not correct
	// [OK] NEXT STEP 8: camera: mouse wheel under 3rd person view: Zoom In/Out
	// NEXT STEP 9: shader: light [like a simple phong equation]
	// NEXT STEP 10: Shadow (I dont know how to do that yet)
	return true;
}

void UnloadContent()
{
	SafeRelease(g_CBStaticMesh[CBStaticMesh::CB_Application]);
	SafeRelease(g_CBStaticMesh[CBStaticMesh::CB_Frame]);
	SafeRelease(g_CubeVertexTexcoordBuffer);
	SafeRelease(g_CubeIndexBuffer);
	SafeRelease(g_CylinderVertexTexcoordBuffer);
	SafeRelease(g_CylinderIndexBuffer);

	SafeRelease(g_pCubeTexture2DResource);
	SafeRelease(g_pCylinderTexture2DResource);
	SafeRelease(g_pCubeShaderResourceView);
	SafeRelease(g_pCylinderShaderResourceView);
	SafeRelease(g_pSkyboxTextureResource);
	SafeRelease(g_pSkyboxShaderResourceView);

	delete(g_pShaderSimpleStaticMesh);
	delete(g_pSkyboxShader);

	delete(g_pCubeMesh);
	delete(g_pCylinderMesh);
	delete(g_pCamera);
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
			// system thread
			// we don't acquire the thread context here because only WM_MOUSEWHEEL needs the lock
			// See UpdateMouseWheelRealtime()
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_mutexThreadContext.lock();
			// game thread
			DWORD currentTime = timeGetTime();
			float deltaTime = (currentTime - previousTime);
			previousTime = currentTime;

			// Cap the delta time to the max time step
			// (useful if your debugging and you don't want the deltaTime value to explode)
			deltaTime = std::min<float>(deltaTime, maxTimeStep);

			Update(deltaTime);
			g_mutexThreadContext.unlock();

			// TODO: send Render() to another thread, then switch context between Update() and Render()
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
	case WM_MOUSEMOVE:
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		/*
		static char buf[50];
		sprintf_s(buf, "on mouse move: %d, %d\n", xPos, yPos);
		OutputDebugStringA(buf);
		*/
		// we dont need thread ownership here, see the comments of the function below
		UpdateMousePosRealtime(xPos, yPos);
	}
	break;
	case WM_MOUSEWHEEL:
	{
		// here we don't want it to wait and block the execution of system-side functions
		// so we only use try_lock()
		// if it's currently locked, just skip
		if (g_mutexThreadContext.try_lock()) // tryAcquireThreadOwnerShip()
		{
			//int xPos = GET_X_LPARAM(lParam);
			//int yPos = GET_Y_LPARAM(lParam);
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			//int mButtonDown = (GET_KEYSTATE_WPARAM(wParam) & 0x0010); // Mouse Middle Button

			//static char buf[50];
			//sprintf_s(buf, "on mouse wheel: %d, %d, %d, %d\n", xPos, yPos, zDelta, mButtonDown);
			//OutputDebugStringA(buf);
			UpdateMouseWheelRealtime(zDelta);
			g_mutexThreadContext.unlock();
		}
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

	hr = g_pGameContextD3D11->m_d3dDevice->CreateDepthStencilState(&depthStencilStateDesc, &g_pGameContextD3D11->m_d3dDepthStencilStateDefault);
	if (FAILED(hr))
	{
		return -1;
	}

	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = g_pGameContextD3D11->m_d3dDevice->CreateDepthStencilState(&depthStencilStateDesc, &g_pGameContextD3D11->m_d3dDepthStencilStateLessEqual);
	if (FAILED(hr))
	{
		return -1;
	}

	// Setup rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	//rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	// create the rasterizer state object
	hr = g_pGameContextD3D11->m_d3dDevice->CreateRasterizerState(&rasterizerDesc, &g_pGameContextD3D11->m_d3dRasterizerStateDefault);
	if (FAILED(hr))
	{
		return -1;
	}

	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	hr = g_pGameContextD3D11->m_d3dDevice->CreateRasterizerState(&rasterizerDesc, &g_pGameContextD3D11->m_d3dRasterizerStateCullNone);
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

	if (!LoadAndGenerateBuffers())
	{
		MessageBox(nullptr, TEXT("Failed to load content."), TEXT("Error"), MB_OK);
		return -1;
	}

	int returnCode = Run();

	UnloadContent();
	Cleanup();

	return returnCode;
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


void GenerateCylinderVerticesAndTexcoords(float radius, float height, unsigned int numPolygonSides = 16)
{
	//std::vector<XMFLOAT3> vertices;
	//std::vector<WORD> indices;

	// let's set the origin to the mid-height center
	// bottom
	g_CylinderVerticesTexcoord.push_back({ XMFLOAT3(0, -height / 2, 0), XMFLOAT2(0, 0) }); // [0]
	for (int i = 0; i < numPolygonSides; i++) // [1...numPolygonSides]
	{
		float radians = XMConvertToRadians(i * 360.0f / numPolygonSides);
		float sinValue = sinf(radians);
		float cosValue = cosf(radians);
		g_CylinderVerticesTexcoord.push_back(
			{ XMFLOAT3(sinValue * radius, -height / 2, cosValue * radius), XMFLOAT2((sinValue + 1.0) / 2, (cosValue + 1.0) / 2) }
		);
	}
	// top
	g_CylinderVerticesTexcoord.push_back({ XMFLOAT3(0, height / 2, 0), XMFLOAT2(0, 0) }); // [numPolygonSides+1]
	for (int i = 0; i < numPolygonSides; i++) // [numPolygonSides+1 +1 ... numPolygonSides+1+ numPolygonSides]
	{
		float radians = XMConvertToRadians(i * 360.0f / numPolygonSides);
		float sinValue = sinf(radians);
		float cosValue = cosf(radians);
		g_CylinderVerticesTexcoord.push_back(
			{ XMFLOAT3(sinValue * radius, height / 2, cosValue * radius), XMFLOAT2((sinValue + 1.0) / 2, (cosValue + 1.0) / 2) }
		);
	}

	// generate indices
	// note: front face is CLOCKWISE according to dx11 default
	// bottom face
	UINT topFaceIndexOffset = numPolygonSides + 1;
	for (int i = 1; i <= numPolygonSides; i++)
	{
		g_CylinderIndices.push_back(0);
		g_CylinderIndices.push_back(i == numPolygonSides ? 1 : i + 1);
		g_CylinderIndices.push_back(i);
	}
	// top face
	for (int i = 1; i <= numPolygonSides; i++)
	{
		g_CylinderIndices.push_back(topFaceIndexOffset + 0);
		g_CylinderIndices.push_back(topFaceIndexOffset + i);
		g_CylinderIndices.push_back(i == numPolygonSides ? (topFaceIndexOffset + 1) : (topFaceIndexOffset + i + 1));
	}
	// side faces
	for (int i = 1; i <= numPolygonSides; i++)
	{
		// bottom, top, top-next
		g_CylinderIndices.push_back(i);
		g_CylinderIndices.push_back(i == numPolygonSides ? (topFaceIndexOffset + 1) : (topFaceIndexOffset + i + 1));
		g_CylinderIndices.push_back(i + topFaceIndexOffset);

		// bottom, top-next, bottom-next
		g_CylinderIndices.push_back(i);
		g_CylinderIndices.push_back(i == numPolygonSides ? 1 : i + 1);
		g_CylinderIndices.push_back(i == numPolygonSides ? (topFaceIndexOffset + 1) : (topFaceIndexOffset + i + 1));
	}
}

void DetectKeyboardAndMouseEvents()
{
	if (GetFocus() == g_WindowHandle)
	{
		// check mouse states every frame
		g_eventQueue.push(KeyboardMouseEvents::EVENT_MOUSE_MOVEMENT);
		g_eventQueue.push(KeyboardMouseEvents::EVENT_MOUSE_WHEEL);

		if (GetAsyncKeyState('A') & 0x8000)
		{
			g_eventQueue.push(KeyboardMouseEvents::EVENT_KEY_A_HELD);
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			g_eventQueue.push(KeyboardMouseEvents::EVENT_KEY_D_HELD);
		}
		if (GetAsyncKeyState('W') & 0x8000)
		{
			g_eventQueue.push(KeyboardMouseEvents::EVENT_KEY_W_HELD);
		}
		if (GetAsyncKeyState('S') & 0x8000)
		{
			g_eventQueue.push(KeyboardMouseEvents::EVENT_KEY_S_HELD);
		}
		// TODO: https://www.autoitscript.com/autoit3/docs/libfunctions/_WinAPI_GetAsyncKeyState.htm
		// This 'pressed' is not fully reliable when there is another application
		// that could have already received the 'pressed' event before
		// I guess ultimately this will be changed to like "Held for 0.1s"
		if (GetAsyncKeyState('Q') & 0x01)
		{
			g_eventQueue.push(KeyboardMouseEvents::EVENT_KEY_Q_PRESSED);
		}
	}
}

void ProcessEvents(float deltaTime)
{
	g_moveByInput = XMFLOAT2();
	g_shouldSwitchCameraMode = false;

	while (!g_eventQueue.empty())
	{
		auto inputEvent = g_eventQueue.front();
		g_eventQueue.pop();

		if (inputEvent == KeyboardMouseEvents::EVENT_KEY_A_HELD)
		{
			g_moveByInput.x += -1;
		}
		else if (inputEvent == KeyboardMouseEvents::EVENT_KEY_D_HELD)
		{
			g_moveByInput.x += 1;
		}
		else if (inputEvent == KeyboardMouseEvents::EVENT_KEY_W_HELD)
		{
			g_moveByInput.y += 1;
		}
		else if (inputEvent == KeyboardMouseEvents::EVENT_KEY_S_HELD)
		{
			g_moveByInput.y += -1;
		}
		else if (inputEvent == KeyboardMouseEvents::EVENT_KEY_Q_PRESSED)
		{
			// TODO: swtich camera mode
			g_shouldSwitchCameraMode = true;
		}
		else if (inputEvent == KeyboardMouseEvents::EVENT_MOUSE_MOVEMENT)
		{
			OnMousePosChangedPerFrame(deltaTime);
		}
		else if (inputEvent == KeyboardMouseEvents::EVENT_MOUSE_WHEEL)
		{
			OnMouseWheelChangedPerFrame(deltaTime);
		}
	}
}

// the g_curMouseX(/Y)Pos are only written by UpdateMousePosRealtime (from WndProc)
// and only read by OnMousePosChangedPerFrame()
// and the value change are done only by directly assigning values (like no "+=" calculations which still reads the previous value)
// so there's no race conditions at all
void UpdateMousePosRealtime(int xPos, int yPos)
{
	g_realtimeMouseXPos = xPos;
	g_realtimeMouseYPos = yPos;
}

// However, we cannot do the same thing on WheelDelta because WndProc doesn't receive WM_MOUSEWHEEL when the wheel is not rotating,
// which means that it cannot automatically change that value back to zero
// so we need to accumulate all mouse wheel activities between each frame
// then send it to Game Thread, where we clear it to zero after we no longer need it
// the try_lock() is called outside, from WndProc
void UpdateMouseWheelRealtime(int wheelDelta)
{
	g_realtimeWheelDelta += wheelDelta;
}

void OnMousePosChangedPerFrame(float deltaTime)
{
	// NEXT STEP HERE
	float xPosNormalized = (float)g_realtimeMouseXPos / g_WindowWidth;
	float yPosNormalized = (float)g_realtimeMouseYPos / g_WindowHeight;

	g_pCamera->ApplyMousePos(xPosNormalized, yPosNormalized);
}

void OnMouseWheelChangedPerFrame(float deltaTime)
{
	float curFrameWheelDelta = g_realtimeWheelDelta;
	g_pCamera->ApplyMouseWheelDelta(curFrameWheelDelta, deltaTime);

	g_realtimeWheelDelta = 0;
}

/*
struct VertexPosColor
{
	XMFLOAT3 Position;
	XMFLOAT3 Color;
};*/

// the vertices and indices of an untextured cube
/*
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
*/