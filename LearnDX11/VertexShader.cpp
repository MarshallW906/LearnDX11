#include "DirectXTemplatePCH.h"
#include "VertexShader.h"

std::string VertexShader::GetLatestProfile(GameContextD3D11* pGameContext)
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

VertexShader::VertexShader(GameContextD3D11* pGameContext)
	: m_pGameContext(pGameContext)
	, m_pShader(nullptr)
	, m_pInputLayout(nullptr)
{
}

VertexShader::~VertexShader()
{
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pShader);
}

bool VertexShader::LoadFromFileAndCompile(const std::wstring& filename, const std::string& entryPoint, const std::string& _profile,
	D3D11_INPUT_ELEMENT_DESC* pVertexLayoutDesc, unsigned int inputLayoutElementCount)
{
	// read .vs file to blob
	ID3DBlob* pShaderBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;
	ID3D11VertexShader* pShader = nullptr;

	std::string profile = _profile;
	if (profile == "latest")
	{
		profile = GetLatestProfile(m_pGameContext);
	}

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	// TODO: Use a pathbuilder to avoid having to specify paths of parent folders
	OutputDebugStringA("TODO: Use a pathbuilder to avoid having to specify paths of parent folders");
	HRESULT hr = D3DCompileFromFile(filename.c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), profile.c_str(),
		flags, 0, &pShaderBlob, &pErrorBlob);

	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			std::string errorMessage = (char*)pErrorBlob->GetBufferPointer();
			OutputDebugStringA(errorMessage.c_str());

			SafeRelease(pShaderBlob);
			SafeRelease(pErrorBlob);
		}
		return false;
	}

	// create vertex shader
	assert(m_pGameContext->m_d3dDevice);
	assert(pShaderBlob);
	hr = m_pGameContext->m_d3dDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &pShader);
	if (FAILED(hr))
	{
		return false;
	}

	m_pShader = pShader;

	// create input layout
	OutputDebugStringA("TODO: Here we hand-crafted the inputLayoutDesc. Use CreateShaderResourceView() later to automate this process");
	hr = m_pGameContext->m_d3dDevice->CreateInputLayout(pVertexLayoutDesc, inputLayoutElementCount, pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
	{
		return false;
	}

	// release objects that are no longer used
	SafeRelease(pShaderBlob);
	SafeRelease(pErrorBlob);

	return true;
}

void VertexShader::BindInputLayoutAndShader()
{
	m_pGameContext->m_d3dDeviceContext->IASetInputLayout(m_pInputLayout);
	// TODO: figure out how exactly the "classInstance" is used
	m_pGameContext->m_d3dDeviceContext->VSSetShader(m_pShader, nullptr, 0);
}