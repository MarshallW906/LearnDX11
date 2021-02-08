#pragma once

// System Includes
#include <Windows.h>
#include <windowsx.h>

// DX11
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

// DirectXTK (installed by NuGet Package Manager)
// TODO: try install it by project-to-project reference so that we will have more control
// project-to-project: sources are here https://github.com/Microsoft/DirectXTK/releases

//#include "SimpleMath.h" // TODO: change all XMVECTOR usage to SimpleMath until I ultimately understand XMVECTOR well enough

#include "BufferHelpers.h"
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PostProcess.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"

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