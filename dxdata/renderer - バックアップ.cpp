#include "main.h"
#include "renderer.h"
#include <io.h>
#include "posteffectobject.h"
#include "enddraw.h"

Renderer* Renderer::m_instance = nullptr;

D3D_FEATURE_LEVEL   Renderer::m_feature_level = D3D_FEATURE_LEVEL_11_0;

ID3D11Device* Renderer::m_device{};
ID3D11DeviceContext* Renderer::m_devicecontext{};
IDXGISwapChain* Renderer::m_swapchain{};
ID3D11RenderTargetView* Renderer::m_rendertarget_view{};
ID3D11DepthStencilView* Renderer::m_depthstencil_view{};

ID3D11Buffer* Renderer::m_world_buffer{};
ID3D11Buffer* Renderer::m_view_buffer{};
ID3D11Buffer* Renderer::m_projection_buffer{};
ID3D11Buffer* Renderer::m_material_buffer{};
ID3D11Buffer* Renderer::m_light_buffer{};
ID3D11Buffer* Renderer::m_camera_buffer{};
ID3D11Buffer* Renderer::m_parameter_buffer{};


ID3D11DepthStencilState* Renderer::m_depthstate_enable{};
ID3D11DepthStencilState* Renderer::m_depthstate_disable{};
ID3D11DepthStencilState* Renderer::m_depthState_read{};
ID3D11DepthStencilState* Renderer::m_depthState_write{};

ID3D11RasterizerState* Renderer::m_rasterizerstate_cullback{};
ID3D11RasterizerState* Renderer::m_rasterizerstate_cullnone{};

ID3D11BlendState* Renderer::m_blendstate{};
ID3D11BlendState* Renderer::m_blendstate_add{};
ID3D11BlendState* Renderer::m_blend_state_atc{};
ID3D11BlendState* Renderer::m_blend_state_mask{};

XMMATRIX  Renderer::m_viewmatrix{};
XMMATRIX  Renderer::m_projectionmatrix{};

ID3D11RenderTargetView* Renderer::m_perenderer_targetview[3] = { NULL,NULL,NULL };//11/01変更
ID3D11ShaderResourceView* Renderer::m_peshader_resourceview[3] = { NULL, NULL,NULL };//1101

ID3D11RenderTargetView* Renderer::m_particlerenderer_targetview;//11/01変更
ID3D11ShaderResourceView* Renderer::m_particleshader_resourceview;//1101


ID3D11DepthStencilView* Renderer::m_depthshadow_stencilview;//1202
ID3D11ShaderResourceView* Renderer::m_depthshadow_shader_resourceview;//1202


Renderer& Renderer::GetRenderer() {
	if (m_instance != nullptr) {
		return *m_instance;
	}
	m_instance = new Renderer();
	return *m_instance;
}

void Renderer::DestroyRenderer() {
	delete m_instance;
}


void Renderer::Init()
{
	HRESULT hr = S_OK;

	//GPUの選択
	IDXGIAdapter1* iAdapter = NULL;
	IDXGIFactory1* iFactory = NULL;
	DXGI_ADAPTER_DESC iDesc;

	int maxmemory = 0;
	unsigned int l_Index = 0;

	CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&iFactory);
	for (unsigned int index = 0;; index++)
	{
		HRESULT ret = iFactory->EnumAdapters1(index, &iAdapter);
		if (ret == DXGI_ERROR_NOT_FOUND) {
			break;
		}

		HRESULT hl = iAdapter->GetDesc(&iDesc);
		if (hl != DXGI_ERROR_NOT_FOUND) {
			int memory = iDesc.DedicatedVideoMemory;
			if (memory > maxmemory) {
				l_Index = index;
				maxmemory = memory;
			}
		}
	}
	iFactory->EnumAdapters1(l_Index, &iAdapter);
	iFactory->Release();


	// デバイス、スワップチェーン作成
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = SCREEN_WIDTH;
	swapChainDesc.BufferDesc.Height = SCREEN_HEIGHT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = GetWindow();
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;

	hr = D3D11CreateDeviceAndSwapChain(iAdapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		0,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&m_swapchain,
		&m_device,
		&m_feature_level,
		&m_devicecontext);


	iAdapter->Release();



	// レンダーターゲットビュー作成
	ID3D11Texture2D* renderTarget{};
	m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&renderTarget);
	m_device->CreateRenderTargetView(renderTarget, NULL, &m_rendertarget_view);
	renderTarget->Release();


	// デプスステンシルバッファ作成
	ID3D11Texture2D* depthStencile{};
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = swapChainDesc.BufferDesc.Width;
	textureDesc.Height = swapChainDesc.BufferDesc.Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	//textureDesc.Format = DXGI_FORMAT_D16_UNORM;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.SampleDesc = swapChainDesc.SampleDesc;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	m_device->CreateTexture2D(&textureDesc, NULL, &depthStencile);

	// デプスステンシルビュー作成
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = textureDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilViewDesc.Flags = 0;
	m_device->CreateDepthStencilView(depthStencile, &depthStencilViewDesc, &m_depthstencil_view);


	m_devicecontext->OMSetRenderTargets(1, &m_rendertarget_view, m_depthstencil_view);

	depthStencile->Release();




	// ビューポート設定
	D3D11_VIEWPORT viewport{};
	viewport.Width = (FLOAT)SCREEN_WIDTH;
	viewport.Height = (FLOAT)SCREEN_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	m_devicecontext->RSSetViewports(1, &viewport);



	// ラスタライザステート設定
	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerstate_cullback);
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerstate_cullnone);

	m_devicecontext->RSSetState(m_rasterizerstate_cullback);




	// ブレンドステート設定
	D3D11_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;//前景*a
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;//背景*(1-a)
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_device->CreateBlendState(&blendDesc, &m_blendstate);


	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;//背景*1
	m_device->CreateBlendState(&blendDesc, &m_blendstate_add);
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0;
	m_device->CreateBlendState(&blendDesc, &m_blend_state_mask);
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = TRUE;
	m_device->CreateBlendState(&blendDesc, &m_blend_state_atc);

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_devicecontext->OMSetBlendState(m_blendstate, blendFactor, 0xffffffff);





	// デプスステンシルステート設定
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;

	m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthstate_enable);//深度有効ステート

	//depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthstate_disable);//深度無効ステート

	m_devicecontext->OMSetDepthStencilState(m_depthstate_enable, NULL);

	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depthStencilDesc.StencilEnable = TRUE;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthState_write);

	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_LESS;

	m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthState_read);


	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc{};
	//samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;//
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;//Mipmap有効化
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;//どの段階迄loD処理をするか

	ID3D11SamplerState* samplerState{};
	m_device->CreateSamplerState(&samplerDesc, &samplerState);

	m_devicecontext->PSSetSamplers(0, 1, &samplerState);



	// 定数バッファ生成
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(XMFLOAT4X4);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	m_device->CreateBuffer(&bufferDesc, NULL, &m_world_buffer);
	m_devicecontext->VSSetConstantBuffers(0, 1, &m_world_buffer);

	bufferDesc.ByteWidth = sizeof(VIEWMATRIX);
	m_device->CreateBuffer(&bufferDesc, NULL, &m_view_buffer);
	m_devicecontext->VSSetConstantBuffers(1, 1, &m_view_buffer);

	bufferDesc.ByteWidth = sizeof(PROJECTIONMATRIX);
	m_device->CreateBuffer(&bufferDesc, NULL, &m_projection_buffer);
	m_devicecontext->VSSetConstantBuffers(2, 1, &m_projection_buffer);


	bufferDesc.ByteWidth = sizeof(MATERIAL);
	m_device->CreateBuffer(&bufferDesc, NULL, &m_material_buffer);
	m_devicecontext->VSSetConstantBuffers(3, 1, &m_material_buffer);
	m_devicecontext->PSSetConstantBuffers(3, 1, &m_material_buffer);


	bufferDesc.ByteWidth = sizeof(LIGHT);

	m_device->CreateBuffer(&bufferDesc, NULL, &m_light_buffer);
	m_devicecontext->VSSetConstantBuffers(4, 1, &m_light_buffer);
	m_devicecontext->PSSetConstantBuffers(4, 1, &m_light_buffer);


	bufferDesc.ByteWidth = sizeof(XMFLOAT4);
	m_device->CreateBuffer(&bufferDesc, NULL, &m_camera_buffer);
	m_devicecontext->PSSetConstantBuffers(5, 1, &m_camera_buffer);

	bufferDesc.ByteWidth = sizeof(PARAMETER);
	m_device->CreateBuffer(&bufferDesc, NULL, &m_parameter_buffer);
	m_devicecontext->PSSetConstantBuffers(7, 1, &m_parameter_buffer);
	m_devicecontext->VSSetConstantBuffers(7, 1, &m_parameter_buffer);


	// ライト初期化
	LIGHT light{};
	light.Enable = true;
	light.Direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	light.Ambient = XMFLOAT4(0.2f, 0.1f, 0.1f, 1.0f);
	light.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetLight(light);

	// マテリアル初期化
	MATERIAL material{};
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = false;//テクスチャ有効化
	SetMaterial(material);


	//レンダリグテクスチャ0作成
	{
		ID3D11Texture2D* ppTexture = NULL;
		D3D11_TEXTURE2D_DESC	td;//テクスチャ作成用デスクリプタ構造体
		ZeroMemory(&td, sizeof(td));//構造体を0クリア

		td.Width = swapChainDesc.BufferDesc.Width;	//バックバッファサイズ
		td.Height = swapChainDesc.BufferDesc.Height;

		td.MipLevels = 1;	//作成するミップマップの数
		td.ArraySize = 1;

		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//ピクセルフォーマット
		td.SampleDesc = swapChainDesc.SampleDesc;
		td.Usage = D3D11_USAGE_DEFAULT;

		//レンダリングターゲット用の設定
		td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;

		//テクスチャの作成
		m_device->CreateTexture2D(&td, NULL, &ppTexture);
		//レンダ―ターゲットビュー
		D3D11_RENDER_TARGET_VIEW_DESC	rtvd;
		ZeroMemory(&rtvd, sizeof(rtvd));
		rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		m_device->CreateRenderTargetView(ppTexture,
			&rtvd,
			&m_perenderer_targetview[0]);



		//シェーダーリソースビュー
		D3D11_SHADER_RESOURCE_VIEW_DESC		srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		srvd.Texture2D.MipLevels = 1;
		m_device->CreateShaderResourceView(ppTexture,
			&srvd,
			&m_peshader_resourceview[0]);
	}

	//レンダリグテクスチャ1作成 0920
	{
		ID3D11Texture2D* ppTexture = NULL;
		D3D11_TEXTURE2D_DESC	td;//テクスチャ作成用デスクリプタ構造体
		ZeroMemory(&td, sizeof(td));//構造体を0クリア

		td.Width = swapChainDesc.BufferDesc.Width;	//バックバッファサイズ
		td.Height = swapChainDesc.BufferDesc.Height;

		td.MipLevels = 1;	//作成するミップマップの数
		td.ArraySize = 1;

		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//ピクセルフォーマット
		td.SampleDesc = swapChainDesc.SampleDesc;
		td.Usage = D3D11_USAGE_DEFAULT;

		//レンダリングターゲット用の設定
		td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;

		//テクスチャの作成
		m_device->CreateTexture2D(&td, NULL, &ppTexture);
		//レンダ―ターゲットビュー
		D3D11_RENDER_TARGET_VIEW_DESC	rtvd;
		ZeroMemory(&rtvd, sizeof(rtvd));
		rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		m_device->CreateRenderTargetView(ppTexture,
			&rtvd,
			&m_perenderer_targetview[1]);

		//シェーダーリソースビュー
		D3D11_SHADER_RESOURCE_VIEW_DESC		srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;
		m_device->CreateShaderResourceView(ppTexture,
			&srvd,
			&m_peshader_resourceview[1]);
	}


	//レンダリグテクスチャ2作成 1101
	{
		ID3D11Texture2D* ppTexture = NULL;
		D3D11_TEXTURE2D_DESC	td;//テクスチャ作成用デスクリプタ構造体
		ZeroMemory(&td, sizeof(td));//構造体を0クリア

		td.Width = swapChainDesc.BufferDesc.Width;	//バックバッファサイズ
		td.Height = swapChainDesc.BufferDesc.Height;

		td.MipLevels = 1;	//作成するミップマップの数
		td.ArraySize = 1;

		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//ピクセルフォーマット
		td.SampleDesc = swapChainDesc.SampleDesc;
		td.Usage = D3D11_USAGE_DEFAULT;

		//レンダリングターゲット用の設定
		td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;

		//テクスチャの作成
		m_device->CreateTexture2D(&td, NULL, &ppTexture);
		//レンダ―ターゲットビュー
		D3D11_RENDER_TARGET_VIEW_DESC	rtvd;
		ZeroMemory(&rtvd, sizeof(rtvd));
		rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		m_device->CreateRenderTargetView(ppTexture,
			&rtvd,
			&m_perenderer_targetview[2]);

		//シェーダーリソースビュー
		D3D11_SHADER_RESOURCE_VIEW_DESC		srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;
		m_device->CreateShaderResourceView(ppTexture,
			&srvd,
			&m_peshader_resourceview[2]);
	}

	//レンダリグテクスチャ2作成 1101
	{
		ID3D11Texture2D* ppTexture = NULL;
		D3D11_TEXTURE2D_DESC	td;//テクスチャ作成用デスクリプタ構造体
		ZeroMemory(&td, sizeof(td));//構造体を0クリア

		td.Width = swapChainDesc.BufferDesc.Width;	//バックバッファサイズ
		td.Height = swapChainDesc.BufferDesc.Height;

		td.MipLevels = 1;	//作成するミップマップの数
		td.ArraySize = 1;

		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//ピクセルフォーマット
		td.SampleDesc = swapChainDesc.SampleDesc;
		td.Usage = D3D11_USAGE_DEFAULT;

		//レンダリングターゲット用の設定
		td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;

		//テクスチャの作成
		m_device->CreateTexture2D(&td, NULL, &ppTexture);
		//レンダ―ターゲットビュー
		D3D11_RENDER_TARGET_VIEW_DESC	rtvd;
		ZeroMemory(&rtvd, sizeof(rtvd));
		rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		m_device->CreateRenderTargetView(ppTexture,
			&rtvd,
			&m_particlerenderer_targetview);

		//シェーダーリソースビュー
		D3D11_SHADER_RESOURCE_VIEW_DESC		srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;
		m_device->CreateShaderResourceView(ppTexture,
			&srvd,
			&m_particleshader_resourceview);
	}

	//深度バッファテクスチャ　1202
	{
		ID3D11Texture2D* ppTexture = NULL;
		D3D11_TEXTURE2D_DESC	td;//テクスチャ作成用デスクリプタ構造体
		ZeroMemory(&td, sizeof(td));//構造体を0クリア

		td.Width = swapChainDesc.BufferDesc.Width;	//バックバッファサイズ
		td.Height = swapChainDesc.BufferDesc.Height;

		td.MipLevels = 1;	//作成するミップマップの数
		td.ArraySize = 1;

		td.Format = DXGI_FORMAT_R32_TYPELESS;//ピクセルフォーマット 32bitの自由形式　1202追加
		td.SampleDesc = swapChainDesc.SampleDesc;
		td.Usage = D3D11_USAGE_DEFAULT;

		//レンダリングターゲット用の設定 //1202
		td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;

		//テクスチャの作成
		m_device->CreateTexture2D(&td, NULL, &ppTexture);
		//デプスステンシルターゲットビュー //1202
		D3D11_DEPTH_STENCIL_VIEW_DESC	rtvd;
		ZeroMemory(&rtvd, sizeof(rtvd));
		rtvd.Format = DXGI_FORMAT_D32_FLOAT;//32bit float
		rtvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		m_device->CreateDepthStencilView(ppTexture,
			&rtvd,
			&m_depthshadow_stencilview);

		//シェーダーリソースビュー
		D3D11_SHADER_RESOURCE_VIEW_DESC		srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_R32_FLOAT;//1202
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;
		auto hr = m_device->CreateShaderResourceView(ppTexture,
			&srvd,
			&m_depthshadow_shader_resourceview);

	}
#ifdef  _DEBUG
	//imgui追加 25/02/12
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(GetWindow());
	ImGui_ImplDX11_Init(m_device, m_devicecontext);
#endif //  _DEBUG
	m_end_draw = new EndDraw();
}



void Renderer::Uninit()
{
#ifdef _DEBUG
	//imgui追加 25/02/12
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif // _DEBUG

	m_world_buffer->Release();
	m_view_buffer->Release();
	m_projection_buffer->Release();
	m_light_buffer->Release();
	m_material_buffer->Release();
	m_camera_buffer->Release();
	m_parameter_buffer->Release();


	m_devicecontext->ClearState();
	m_rendertarget_view->Release();
	m_perenderer_targetview[0]->Release();
	m_perenderer_targetview[1]->Release();
	m_perenderer_targetview[2]->Release();
	m_depthstencil_view->Release();
	m_peshader_resourceview[0]->Release();
	m_peshader_resourceview[1]->Release();
	m_peshader_resourceview[2]->Release();
	m_depthshadow_stencilview->Release();//1202
	m_depthshadow_shader_resourceview->Release();//1202
	m_swapchain->Release();
	m_devicecontext->Release();
	m_device->Release();

}




void Renderer::Begin()
{
	//レンダリングターゲットの切り替え
	m_devicecontext->OMSetRenderTargets(
		1,
		&m_rendertarget_view,	//レンダリングテクスチャ
		m_depthstencil_view);		//Zバッファ

	float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_devicecontext->ClearRenderTargetView(m_rendertarget_view, clearColor);
	m_devicecontext->ClearDepthStencilView(m_depthstencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


#ifdef _DEBUG
	//imgui追加 25/02/19
	// (Your code process and dispatch Win32 messages)
		// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow(); // Show demo window! :)

	//レンダリングデバッグウィンドウ
	ImGui::Begin("Renderer Data");
	ImGui::Text("%.1f FPS/%.1f Ms", ImGui::GetIO().Framerate, ImGui::GetIO().DeltaTime * 1000.0f);

	static float value[180];
	for (int i = 0; i < 179; i++)
	{
		value[i] = value[i + 1];
	}
	value[179] = ImGui::GetIO().DeltaTime * 1000.0f;

	//## <-label非表示化識別子
	ImGui::PlotLines("##workload", value, sizeof(value) / sizeof(float),//要素数を求める処理
		0, NULL, 0.0f, 100.0f, ImVec2(0, 50));

	SIZE_T memorysizeKB = ShowMemoryUsage() / 1024;
	ImGui::Text("Memory Usage: %zu KB", memorysizeKB);
	ImGui::Text("Memory Usage: %zu MB", memorysizeKB / 1024);

	static float ramvalue[180];
	for (int i = 0; i < 179; i++)
	{
		ramvalue[i] = ramvalue[i + 1];
	}
	ramvalue[179] = (float)(memorysizeKB / 1024);

	ImGui::PlotLines("##Ramworkload", ramvalue, sizeof(ramvalue) / sizeof(float),//要素数を求める処理
		0, NULL, 0.0f, 5000.0f, ImVec2(0, 50));

	ImGui::End();
#endif
}



void Renderer::End()
{
#ifdef _DEBUG
	//imgui追加　25/02/12
		// Rendering
		// (Your code clears your framebuffer, renders your other stuff etc.)
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	// (Your code calls swapchain's Present() function)
#endif
	m_swapchain->Present(1, 0);//第一引数が垂直同期のフラグ
}




void Renderer::SetDepthEnable(bool Enable)
{
	if (Enable)
		m_devicecontext->OMSetDepthStencilState(m_depthstate_enable, NULL);
	else
		m_devicecontext->OMSetDepthStencilState(m_depthstate_disable, NULL);

}

void Renderer::SetBlendAddEnable(bool Enable)
{
	if (Enable)
		m_devicecontext->OMSetBlendState(m_blendstate_add, NULL, 0xffffffff);
	else
		m_devicecontext->OMSetBlendState(m_blendstate, NULL, 0xffffffff);

}


void Renderer::SetBlendMaskEnable(bool Enable) {
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	if (Enable)
		m_devicecontext->OMSetBlendState(m_blend_state_mask, blendFactor, 0xffffffff);
	else
		m_devicecontext->OMSetBlendState(m_blendstate, blendFactor, 0xffffffff);

}

void Renderer::SetStencilEnable(bool Enable) {
	if (Enable)
		m_devicecontext->OMSetDepthStencilState(m_depthState_write, NULL);
	else
		m_devicecontext->OMSetDepthStencilState(m_depthState_read, NULL);
}

void Renderer::SetCullEnable(bool Enable) {
	if (Enable)
		m_devicecontext->RSSetState(m_rasterizerstate_cullback);
	else
		m_devicecontext->RSSetState(m_rasterizerstate_cullnone);

}

void Renderer::SetATCEnable(bool Enable)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	if (Enable)
		m_devicecontext->OMSetBlendState(m_blend_state_atc, blendFactor, 0xffffffff);
	else
		m_devicecontext->OMSetBlendState(m_blendstate, blendFactor, 0xffffffff);

}

void Renderer::SetWorldViewProjection2D()
{
	SetWorldMatrix(XMMatrixIdentity());
	SetViewMatrix(XMMatrixIdentity());

	XMMATRIX projection;
	projection = XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	SetProjectionMatrix(projection);
}

void Renderer::SetWorldViewProjectionData3D(XMMATRIX& ViewMatrix, XMMATRIX& ProjectionMatrix) {
	m_viewmatrix = ViewMatrix;
	m_projectionmatrix = ProjectionMatrix;
}

void Renderer::SetWorldViewProjection3D() {
	SetViewMatrix(m_viewmatrix);
	SetProjectionMatrix(m_projectionmatrix);
}


void Renderer::SetWorldMatrix(XMMATRIX WorldMatrix)
{
	XMFLOAT4X4 worldf;
	XMStoreFloat4x4(&worldf, XMMatrixTranspose(WorldMatrix));
	m_devicecontext->UpdateSubresource(m_world_buffer, 0, NULL, &worldf, 0, 0);
}

void Renderer::SetViewMatrix(XMMATRIX ViewMatrix)
{
	VIEWMATRIX viewmat;
	XMStoreFloat4x4(&viewmat.ViewMatrix, XMMatrixTranspose(ViewMatrix));
	XMStoreFloat4x4(&viewmat.InViewMatrix, XMMatrixInverse(NULL,ViewMatrix));
	m_devicecontext->UpdateSubresource(m_view_buffer, 0, NULL, &viewmat, 0, 0);
}

void Renderer::SetProjectionMatrix(XMMATRIX ProjectionMatrix)
{
	//XMFLOAT4X4 projectionf;
	PROJECTIONMATRIX projectionmat;
	XMStoreFloat4x4(&projectionmat.ProjectionMatrix, XMMatrixTranspose(ProjectionMatrix));
	XMStoreFloat4x4(&projectionmat.InProjectionMatrix, XMMatrixInverse(NULL, ProjectionMatrix));
	m_devicecontext->UpdateSubresource(m_projection_buffer, 0, NULL, &projectionmat, 0, 0);

}

void Renderer::SetCameraPosition(XMFLOAT3 position) {
	XMFLOAT4 dta = XMFLOAT4(position.x, position.y, position.z, 0.0f);
	m_devicecontext->UpdateSubresource(m_camera_buffer, 0, NULL, &dta, 0, 0);
}

void Renderer::SetParameter(XMFLOAT4 Parameter)
{
	m_Parameter.ParameterOne = Parameter;
	m_devicecontext->UpdateSubresource(m_parameter_buffer, 0, NULL, &m_Parameter, 0, 0);
}
void Renderer::SetParameter(XMFLOAT4 Parameter, int slotnum) {
	switch (slotnum)
	{
	case 0:
		m_Parameter.ParameterOne = Parameter;

		break;
	case 1:
		m_Parameter.ParameterTwo = Parameter;

		break;
	case 2:
		m_Parameter.ParameterThree = Parameter;
		break;
	case 3:
		m_Parameter.Parameterfour = Parameter;
		break;
	default:
		m_Parameter.ParameterOne = Parameter;
		break;
	}
	m_devicecontext->UpdateSubresource(m_parameter_buffer, 0, NULL, &m_Parameter, 0, 0);
}


void Renderer::SetMaterial(MATERIAL Material)
{
	m_devicecontext->UpdateSubresource(m_material_buffer, 0, NULL, &Material, 0, 0);
}

void Renderer::SetLight(LIGHT Light)
{
	m_Light = Light;
	m_devicecontext->UpdateSubresource(m_light_buffer, 0, NULL, &Light, 0, 0);
}

void Renderer::OnDepthShadowLightMatrix() {
	SetViewMatrix(m_Light.ViewMatrix);
	SetProjectionMatrix(m_Light.ProjectionMatrix);
}



void Renderer::CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName)
{

	FILE* file;
	long int fsize;

	file = fopen(FileName, "rb");
	assert(file);

	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	m_device->CreateVertexShader(buffer, fsize, NULL, VertexShader);


	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 10, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0,DXGI_FORMAT_R32G32B32_FLOAT,0,4 * 12,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	m_device->CreateInputLayout(layout,
		numElements,
		buffer,
		fsize,
		VertexLayout);

	delete[] buffer;
}



void Renderer::CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName)
{
	FILE* file;
	long int fsize;

	file = fopen(FileName, "rb");
	assert(file);

	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	m_device->CreatePixelShader(buffer, fsize, NULL, PixelShader);

	delete[] buffer;
}

void Renderer::CreateGeometryShader(ID3D11GeometryShader** GeometryShader, const char* FileName) {
	FILE* file;
	long int fsize;

	file = fopen(FileName, "rb");
	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	m_device->CreateGeometryShader(buffer, fsize, NULL, GeometryShader);

	delete[] buffer;
}

void Renderer::CreateComputeShader(ID3D11ComputeShader** GeometryShader, const char* FileName) {
	FILE* file;
	long int fsize;

	file = fopen(FileName, "rb");
	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	m_device->CreateComputeShader(buffer, fsize, NULL, GeometryShader);

	delete[] buffer;
}


void Renderer::CreateSrvStructureBuffer(ID3D11Buffer* buffer, ID3D11ShaderResourceView* idsrv, UINT size, UINT count) {
	//構造化バッファを作成
	D3D11_BUFFER_DESC bd{};
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;//バッファをシェーダステージにバインド
	bd.ByteWidth = size * count;
	bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;//構造化バッファとして作成
	bd.StructureByteStride = size;//構造化バッファのサイズ(バイト単位)
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &buffer);

	//構造化バッファからシェーダリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvd.BufferEx.FirstElement = 0;
	srvd.Format = DXGI_FORMAT_UNKNOWN;
	srvd.BufferEx.NumElements = count;//リソース内要素数
	//構造化バッファをもとにシェーダリソースビュー作成
	GetDevice()->CreateShaderResourceView(buffer, &srvd, &idsrv);
}

void Renderer::CreateUavStructureBuffer(ID3D11Buffer* buffer, ID3D11UnorderedAccessView* uavout, UINT size, UINT count) {
	//構造化バッファを作成
	D3D11_BUFFER_DESC bd{};
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	//アンオーダードアクセスリソースをバインド
	bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;//バッファをシェーダステージにバインド
	bd.ByteWidth = size * count;
	bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;//構造化バッファとして作成
	bd.StructureByteStride = size;//構造化バッファのサイズ(バイト単位)
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	GetDevice()->CreateBuffer(&bd, NULL, &buffer);

	//構造化バッファからシェーダリソースビュー作成
	D3D11_UNORDERED_ACCESS_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	srvd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	srvd.Buffer.FirstElement = 0;
	srvd.Format = DXGI_FORMAT_UNKNOWN;
	srvd.Buffer.NumElements = count;//リソース内要素数
	//構造化バッファをもとにシェーダリソースビュー作成
	GetDevice()->CreateUnorderedAccessView(buffer, &srvd, &uavout);
}
