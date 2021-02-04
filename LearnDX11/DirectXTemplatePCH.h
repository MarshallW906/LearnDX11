#pragma once

// System Includes
#include <Windows.h>

// DX11
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

// STL
#include <iostream>
#include <string>

// 

// Link library dependencies
// I set them in project settings
// consider uncommenting them when there are problems
//#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "winmm.lib")

// Safely Release
template <typename T>
inline void SafeRelease(T& ptr)
{
	if (ptr != NULL)
	{
		ptr->Release();
		ptr = NULL;
	}
}