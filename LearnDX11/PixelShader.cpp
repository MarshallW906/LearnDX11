#include "DirectXTemplatePCH.h"
#include "PixelShader.h"

PixelShader::PixelShader(GameContextD3D11* pGameContext)
	: m_pGameContext(pGameContext)
	, m_pShader(nullptr)
{
}

PixelShader::~PixelShader()
{
	SafeRelease(m_pShader);
}

bool PixelShader::LoadFromFileAndCompile(const std::wstring& filename, const std::string& entryPoint, const std::string& _profile)
{
	ID3DBlob* pShaderBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;
	ID3D11PixelShader* pShader = nullptr;

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

	hr = m_pGameContext->m_d3dDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &pShader);
	if (FAILED(hr))
	{
		return false;
	}

	SafeRelease(pShaderBlob);
	SafeRelease(pErrorBlob);

	m_pShader = pShader;

	return true;
}

void PixelShader::BindPixelShader()
{
	// TODO: figure out how exactly the "classInstance" is used
	m_pGameContext->m_d3dDeviceContext->PSSetShader(m_pShader, nullptr, 0);
}

std::string PixelShader::GetLatestProfile(GameContextD3D11* pGameContext)
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