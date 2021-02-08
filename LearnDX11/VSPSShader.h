#pragma once

#include "GameContextD3D11.h"

class VSPSShader
{
public:
	VSPSShader() = delete;
	VSPSShader(const VSPSShader&) = delete;
	VSPSShader(GameContextD3D11* pGameContext);
	~VSPSShader();

	void LoadAndCompileShaderWithInputLayout(
		LPCWSTR vsFile, LPCSTR vsEntryPoint, LPCSTR vsProfileVersion,
		LPCWSTR psFile, LPCSTR psEntryPoint, LPCSTR psProfileVersion,
		D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, UINT inputLayoutElementCount);
	void BindShaderAndLayout();

	// TODO: I tried to template this function but got an error: "C2910: cannot be explicitly specialized"
	static LPCSTR GetLatestProfileVS(GameContextD3D11* pGameContext);
	static LPCSTR GetLatestProfilePS(GameContextD3D11* pGameContext);

private:
	GameContextD3D11* m_pGameContext;

	ID3DBlob* m_pVSBlob;
	ID3DBlob* m_pPSBlob;
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	// TODO: this should be managed outside because multiple shaders could have the same input layouts
	ID3D11InputLayout* m_pInputLayout;
};

