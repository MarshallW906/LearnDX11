#pragma once
#include "GameContextD3D11.h"

class VertexShader
{
public:
	VertexShader() = delete;
	VertexShader(const VertexShader&) = delete; 
	explicit VertexShader(GameContextD3D11* pGameContext);
	~VertexShader();

	// TODO: use CreateShaderResourceView() to automate the process of CreateInputLayout instead of hand-crafting the desc objects
	bool LoadFromFileAndCompile(const std::wstring& filename, const std::string& entryPoint, const std::string& _profile,
		D3D11_INPUT_ELEMENT_DESC* pVertexLayoutDesc, unsigned int inputLayoutElementCount);
	void BindInputLayoutAndShader();

	static std::string GetLatestProfile(GameContextD3D11* pGameContext);

private:
	GameContextD3D11* m_pGameContext;
	ID3D11VertexShader* m_pShader;
	// TODO: since many vertex shaders may have the same input layout
	// move pInputLayout outside to reduce unnecessary performance consumption
	ID3D11InputLayout* m_pInputLayout;	
};

