#pragma once
#include "GameContextD3D11.h"

class PixelShader
{
public:
	PixelShader() = delete;
	PixelShader(const PixelShader&) = delete;
	explicit PixelShader(GameContextD3D11* pGameContext);
	~PixelShader();

	bool LoadFromFileAndCompile(const std::wstring& filename, const std::string& entryPoint, const std::string& _profile);
	void BindPixelShader();

	static std::string GetLatestProfile(GameContextD3D11* pGameContext);

private:
	GameContextD3D11* m_pGameContext;
	ID3D11PixelShader* m_pShader;
};

