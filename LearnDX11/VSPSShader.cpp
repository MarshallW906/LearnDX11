#include "DirectXTemplatePCH.h"
#include "VSPSShader.h"

VSPSShader::VSPSShader(GameContextD3D11* pGameContext)
	: m_pGameContext(pGameContext)
	, m_pVSBlob(nullptr)
	, m_pPSBlob(nullptr)
	, m_pVertexShader(nullptr)
	, m_pPixelShader(nullptr)
	, m_pInputLayout(nullptr)
{
}

VSPSShader::~VSPSShader()
{
	SafeRelease(m_pVSBlob);
	SafeRelease(m_pPSBlob);
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexShader);
	SafeRelease(m_pPixelShader);
}

void VSPSShader::LoadAndCompileShaderWithInputLayout(
	LPCWSTR vsFile, LPCSTR vsEntryPoint, 
	LPCWSTR psFile, LPCSTR psEntryPoint, 
	D3D11_INPUT_ELEMENT_DESC *inputLayoutDesc, UINT inputLayoutElementCount)
{
	ID3DBlob* pErrorBlob;

	HRESULT hr = D3DCompileFromFile(
		vsFile, nullptr, nullptr, vsEntryPoint,
		GetLatestProfileVS(m_pGameContext),
		D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG,
		0,
		&m_pVSBlob,
		&pErrorBlob
	);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			std::string errorMessage = (char*)pErrorBlob->GetBufferPointer();
			OutputDebugStringA(errorMessage.c_str());

			SafeRelease(m_pVSBlob);
			SafeRelease(pErrorBlob);
		}
		return;
	}
	
	hr = m_pGameContext->m_d3dDevice->CreateVertexShader(m_pVSBlob->GetBufferPointer(), m_pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{
		return;
	}

	// pixel shader
	hr = D3DCompileFromFile(
		psFile, nullptr, nullptr, psEntryPoint,
		GetLatestProfilePS(m_pGameContext),
		D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG,
		0,
		&m_pPSBlob,
		&pErrorBlob
	);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			std::string errorMessage = (char*)pErrorBlob->GetBufferPointer();
			OutputDebugStringA(errorMessage.c_str());

			SafeRelease(m_pVSBlob);
			SafeRelease(pErrorBlob);
		}
		return;
	}
	hr = m_pGameContext->m_d3dDevice->CreatePixelShader(m_pPSBlob->GetBufferPointer(), m_pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	if (FAILED(hr))
	{
		return;
	}

	// input layout
	hr = m_pGameContext->m_d3dDevice->CreateInputLayout(inputLayoutDesc, inputLayoutElementCount, m_pVSBlob->GetBufferPointer(), m_pVSBlob->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
	{
		return;
	}
}

void VSPSShader::BindShaderAndLayout()
{
	assert(m_pGameContext->m_d3dDevice);
	assert(m_pGameContext->m_d3dDeviceContext);

	m_pGameContext->m_d3dDeviceContext->IASetInputLayout(m_pInputLayout);
	m_pGameContext->m_d3dDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pGameContext->m_d3dDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

LPCSTR VSPSShader::GetLatestProfileVS(GameContextD3D11* pGameContext)
{
	assert(pGameContext->m_d3dDevice);

	// Query the current feature level
	D3D_FEATURE_LEVEL featureLevel = pGameContext->m_d3dDevice->GetFeatureLevel();

	switch (featureLevel)
	{
	case D3D_FEATURE_LEVEL_11_1:
	case D3D_FEATURE_LEVEL_11_0:
	{
		return "vs_5_0";
	}
	break;
	case D3D_FEATURE_LEVEL_10_1:
	{
		return "vs_4_1";
	}
	break;
	case D3D_FEATURE_LEVEL_10_0:
	{
		return "vs_4_0";
	}
	break;
	case D3D_FEATURE_LEVEL_9_3:
	{
		return "vs_4_0_level_9_3";
	}
	break;
	case D3D_FEATURE_LEVEL_9_2:
	case D3D_FEATURE_LEVEL_9_1:
	{
		return "vs_4_0_level_9_1";
	}
	break;
	} // switch( featureLevel )

	return "";
}


LPCSTR VSPSShader::GetLatestProfilePS(GameContextD3D11* pGameContext)
{
	assert(pGameContext->m_d3dDevice);

	// Query the current feature level:
	D3D_FEATURE_LEVEL featureLevel = pGameContext->m_d3dDevice->GetFeatureLevel();
	switch (featureLevel)
	{
	case D3D_FEATURE_LEVEL_11_1:
	case D3D_FEATURE_LEVEL_11_0:
	{
		return "ps_5_0";
	}
	break;
	case D3D_FEATURE_LEVEL_10_1:
	{
		return "ps_4_1";
	}
	break;
	case D3D_FEATURE_LEVEL_10_0:
	{
		return "ps_4_0";
	}
	break;
	case D3D_FEATURE_LEVEL_9_3:
	{
		return "ps_4_0_level_9_3";
	}
	break;
	case D3D_FEATURE_LEVEL_9_2:
	case D3D_FEATURE_LEVEL_9_1:
	{
		return "ps_4_0_level_9_1";
	}
	break;
	}
	return "";
}