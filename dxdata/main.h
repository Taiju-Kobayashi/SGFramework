#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <memory>


#define NOMINMAX
#include <windows.h>
#include <assert.h>
#include <functional>

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")


#include <dxgi.h>
#include <dxgi1_4.h>
#pragma comment (lib,	"dxgi.lib")


#include <DirectXMath.h>
using namespace DirectX;
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>

#include "DirectXTex.h"

#if _DEBUG
#pragma comment(lib,"DirectXTex_Debug.lib")
#else
#pragma comment(lib,"DirectXTex_Release.lib")
#endif

#pragma comment (lib, "winmm.lib")


//IMGUI用インクルード 25/02/18
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"


#define SCREEN_WIDTH	(1280)
#define SCREEN_HEIGHT	(720)


HWND GetWindow();
SIZE_T ShowMemoryUsage();
