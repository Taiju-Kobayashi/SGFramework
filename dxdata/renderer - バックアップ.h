/**
 * @file   renderer.h
 * @brief  DirectX描画関連情報を記述したヘッダー
 * @date   2024/07　
 */

#pragma once

struct VERTEX_3D
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT4 Diffuse;
	XMFLOAT2 TexCoord;
	XMFLOAT4 Tangent;
};


struct MATERIAL
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	BOOL		TextureEnable;
	float		Dummy[2];
};

struct PARAMETER
{
	XMFLOAT4 ParameterOne;
	XMFLOAT4 ParameterTwo;
	XMFLOAT4 ParameterThree;
	XMFLOAT4 Parameterfour;
};


struct POINTLIGHT
{
	XMFLOAT3 Position; // 座標
	XMFLOAT3 PositionInView; // カメラ空間での座標
	XMFLOAT4 LightColor; // ライトのカラー
	float Angle; // ライトの影響を与える範囲
	float Dummy;
};

struct DIRECTIONLIGHT
{
	XMFLOAT4 LightColor;//ライトの色
	float  LightIntensity;//ライトの強さ
	XMFLOAT3 LightDirection;//ライトの向き
};


struct LIGHT
{
	BOOL		Enable;
	BOOL		Dummy[3];
	XMFLOAT4	Direction; // ライトの方向ベクトル
	XMFLOAT4	Diffuse;// ディフューズカラー（拡散反射光の色）
	XMFLOAT4 Ambient; // アンビエントカラー（環境光の色）
	XMFLOAT4 SkyColor;//半球色
	XMFLOAT4 GroundColor;//地面色
	XMFLOAT4 GroundNormal;//地面法線
	XMFLOAT4 Position;//光源座標
	XMFLOAT4 Angle;//スポットlight範囲　ラジアン
	XMFLOAT4 PointLightParam;//光源範囲
	XMMATRIX ViewMatrix; //ライトのビュー行列（カメラ行列）
	XMMATRIX ProjectionMatrix;// ライトのプロジェクション行列
};

struct  VIEWMATRIX
{
	XMFLOAT4X4 ViewMatrix;
	XMFLOAT4X4 InViewMatrix;
};

struct  PROJECTIONMATRIX
{
	XMFLOAT4X4 ProjectionMatrix;
	XMFLOAT4X4 InProjectionMatrix;
};

/// @brief　DirectX描画関連処理を提供するクラス
class Renderer
{
public:
	static Renderer& GetRenderer();

	static void DestroyRenderer();

	void Init();
	void Uninit();
	void Begin();
	void End();

	void SetDepthEnable(bool Enable);
	void SetATCEnable(bool Enable);
	void SetBlendAddEnable(bool Enable);
	void SetBlendMaskEnable(bool Enable);
	void SetStencilEnable(bool Enable);
	void SetCullEnable(bool Enable);
	void SetWorldViewProjection2D();
	void SetWorldViewProjectionData3D(XMMATRIX& ViewMatrix, XMMATRIX& ProjectionMatrix);
	void SetWorldViewProjection3D();
	void SetWorldMatrix(XMMATRIX WorldMatrix);
	void SetViewMatrix(XMMATRIX ViewMatrix);
	void SetProjectionMatrix(XMMATRIX ProjectionMatrix);
	void SetMaterial(MATERIAL Material);
	void SetLight(LIGHT Light);
	void SetCameraPosition(XMFLOAT3 position);
	/// @brief shaderに不特定用途で使用する値を設定する
	void SetParameter(XMFLOAT4 Parameter);
	void SetParameter(XMFLOAT4 Parameter, int slotnum);
	void OnDepthShadowLightMatrix();

	ID3D11Device* GetDevice(void) { return m_device; }
	ID3D11DeviceContext* GetDeviceContext(void) { return m_devicecontext; }



	void CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
	void CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName);
	void CreateGeometryShader(ID3D11GeometryShader** GeometryShader, const char* FileName);
	void CreateComputeShader(ID3D11ComputeShader** GeometryShader, const char* FileName);

	void CreateSrvStructureBuffer(ID3D11Buffer* buffer, ID3D11ShaderResourceView* srv, UINT size, UINT count);
	void CreateUavStructureBuffer(ID3D11Buffer* buffer, ID3D11UnorderedAccessView* uavout, UINT size, UINT count);


	static	ID3D11ShaderResourceView* GetPETexture(int n)//0920変更
	{
		return m_peshader_resourceview[n];//レンダリングテクスチャの取得
	}

	//レンダリングターゲットをテクスチャへ切り替える
	static	void	BeginPE(int n)
	{
		//レンダリングターゲットの切り替え
		m_devicecontext->OMSetRenderTargets(
			1,
			&m_perenderer_targetview[n],	//レンダリングテクスチャ
			m_depthstencil_view);		//Zバッファ

		if (n == 0)
		{
			//レンダリングテクスチャクリア
			float ClearColor[4] = { 1.0f, 1.0f, 1.0f, 0.0f };//＜＜＜＜白にする11/18
			m_devicecontext->ClearRenderTargetView(m_perenderer_targetview[n],
				ClearColor);
			//m_devicecontext->ClearDepthStencilView(m_depthstencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		}
		else if (n == 1)
		{
			//レンダリングテクスチャクリア
			float ClearColor[4] = { 0.0f, 0.5f, 0.5f, 1.0f };//緑
			m_devicecontext->ClearRenderTargetView(m_perenderer_targetview[n],
				ClearColor);
		}
		else
		{
			//レンダリングテクスチャクリア
			float ClearColor[4] = { 0.0f, 0.0f, 0.5f, 1.0f };//緑
			m_devicecontext->ClearRenderTargetView(m_perenderer_targetview[n],
				ClearColor);
		}


		//Zバッファのクリア
		m_devicecontext->ClearDepthStencilView(m_depthstencil_view,
			D3D11_CLEAR_DEPTH, 1.0f, 0);

	}

	//レンダリングターゲットをテクスチャ2枚へ切り替える
	static	void	BeginMultiPE(int n) {
		//レンダリングターゲットの切り替え
		m_devicecontext->OMSetRenderTargets(
			2,//MRT用に2枚のテクスチャをレンダリングターゲットにする
			&m_perenderer_targetview[n],	//レンダリングテクスチャ
			m_depthstencil_view);		//Zバッファ

		//レンダリングテクスチャクリア
		{
			float ClearColor[4] = { 0.0f, 0.5f, 0.0f, 1.0f };//緑
			m_devicecontext->ClearRenderTargetView(m_perenderer_targetview[n],
				ClearColor);
			m_devicecontext->ClearRenderTargetView(m_perenderer_targetview[n + 1],
				ClearColor);
		}
		//Zバッファのクリア
		m_devicecontext->ClearDepthStencilView(m_depthstencil_view,
			D3D11_CLEAR_DEPTH, 1.0f, 0);

	}


	static ID3D11ShaderResourceView* GetDepthShadowTexture()
	{
		return m_depthshadow_shader_resourceview;
	}

	static void BeginDepth()
	{
		//深度バッファをテクスチャに切り替え,内容を1.0でクリア
		m_devicecontext->OMSetRenderTargets(0, NULL, m_depthshadow_stencilview);
		m_devicecontext->ClearDepthStencilView(m_depthshadow_stencilview,
			D3D11_CLEAR_DEPTH, 1.0f, 0);

	}

private:
	static Renderer* m_instance;

	Renderer() = default;
	~Renderer() = default;

	static D3D_FEATURE_LEVEL       m_feature_level;

	static ID3D11Device* m_device;
	static ID3D11DeviceContext* m_devicecontext;
	static IDXGISwapChain* m_swapchain;
	static ID3D11RenderTargetView* m_rendertarget_view;
	static ID3D11DepthStencilView* m_depthstencil_view;

	static ID3D11Buffer* m_world_buffer;
	static ID3D11Buffer* m_view_buffer;
	static ID3D11Buffer* m_projection_buffer;
	static ID3D11Buffer* m_material_buffer;
	static ID3D11Buffer* m_light_buffer;
	static ID3D11Buffer* m_camera_buffer;
	static ID3D11Buffer* m_parameter_buffer;


	static ID3D11RasterizerState* m_rasterizerstate_cullback;
	static ID3D11RasterizerState* m_rasterizerstate_cullnone;

	static ID3D11DepthStencilState* m_depthstate_enable;
	static ID3D11DepthStencilState* m_depthstate_disable;

	static ID3D11DepthStencilState* m_depthState_read;
	static ID3D11DepthStencilState* m_depthState_write;

	static ID3D11BlendState* m_blendstate;
	static ID3D11BlendState* m_blendstate_add;//加算合成用ブレンドステート
	static ID3D11BlendState* m_blend_state_atc;
	static ID3D11BlendState* m_blend_state_mask;


	static	ID3D11RenderTargetView* m_perenderer_targetview[3];//1101変更
	static	ID3D11ShaderResourceView* m_peshader_resourceview[3];//1101変更

	static	ID3D11RenderTargetView* m_particlerenderer_targetview;//1101変更
	static	ID3D11ShaderResourceView* m_particleshader_resourceview;//1101変更


	static ID3D11DepthStencilView* m_depthshadow_stencilview;//1202
	static ID3D11ShaderResourceView* m_depthshadow_shader_resourceview;//1202


	//3D・2D 描画切り替え関数用変数
	static XMMATRIX m_viewmatrix;
	static XMMATRIX m_projectionmatrix;

	PARAMETER m_Parameter{};

	//ポストエフェクト処理オブジェクトポインタ
	class PostEffectObject* m_end_draw = nullptr;
	LIGHT m_Light{};


};
