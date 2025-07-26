/**
 * @file   renderer.h
 * @brief  DirectX�`��֘A�����L�q�����w�b�_�[
 * @date   2024/07�@
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
	XMFLOAT3 Position; // ���W
	XMFLOAT3 PositionInView; // �J������Ԃł̍��W
	XMFLOAT4 LightColor; // ���C�g�̃J���[
	float Angle; // ���C�g�̉e����^����͈�
	float Dummy;
};

struct DIRECTIONLIGHT
{
	XMFLOAT4 LightColor;//���C�g�̐F
	float  LightIntensity;//���C�g�̋���
	XMFLOAT3 LightDirection;//���C�g�̌���
};


struct LIGHT
{
	BOOL		Enable;
	BOOL		Dummy[3];
	XMFLOAT4	Direction; // ���C�g�̕����x�N�g��
	XMFLOAT4	Diffuse;// �f�B�t���[�Y�J���[�i�g�U���ˌ��̐F�j
	XMFLOAT4 Ambient; // �A���r�G���g�J���[�i�����̐F�j
	XMFLOAT4 SkyColor;//�����F
	XMFLOAT4 GroundColor;//�n�ʐF
	XMFLOAT4 GroundNormal;//�n�ʖ@��
	XMFLOAT4 Position;//�������W
	XMFLOAT4 Angle;//�X�|�b�glight�͈́@���W�A��
	XMFLOAT4 PointLightParam;//�����͈�
	XMMATRIX ViewMatrix; //���C�g�̃r���[�s��i�J�����s��j
	XMMATRIX ProjectionMatrix;// ���C�g�̃v���W�F�N�V�����s��
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

/// @brief�@DirectX�`��֘A������񋟂���N���X
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
	/// @brief shader�ɕs����p�r�Ŏg�p����l��ݒ肷��
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


	static	ID3D11ShaderResourceView* GetPETexture(int n)//0920�ύX
	{
		return m_peshader_resourceview[n];//�����_�����O�e�N�X�`���̎擾
	}

	//�����_�����O�^�[�Q�b�g���e�N�X�`���֐؂�ւ���
	static	void	BeginPE(int n)
	{
		//�����_�����O�^�[�Q�b�g�̐؂�ւ�
		m_devicecontext->OMSetRenderTargets(
			1,
			&m_perenderer_targetview[n],	//�����_�����O�e�N�X�`��
			m_depthstencil_view);		//Z�o�b�t�@

		if (n == 0)
		{
			//�����_�����O�e�N�X�`���N���A
			float ClearColor[4] = { 1.0f, 1.0f, 1.0f, 0.0f };//�����������ɂ���11/18
			m_devicecontext->ClearRenderTargetView(m_perenderer_targetview[n],
				ClearColor);
			//m_devicecontext->ClearDepthStencilView(m_depthstencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		}
		else if (n == 1)
		{
			//�����_�����O�e�N�X�`���N���A
			float ClearColor[4] = { 0.0f, 0.5f, 0.5f, 1.0f };//��
			m_devicecontext->ClearRenderTargetView(m_perenderer_targetview[n],
				ClearColor);
		}
		else
		{
			//�����_�����O�e�N�X�`���N���A
			float ClearColor[4] = { 0.0f, 0.0f, 0.5f, 1.0f };//��
			m_devicecontext->ClearRenderTargetView(m_perenderer_targetview[n],
				ClearColor);
		}


		//Z�o�b�t�@�̃N���A
		m_devicecontext->ClearDepthStencilView(m_depthstencil_view,
			D3D11_CLEAR_DEPTH, 1.0f, 0);

	}

	//�����_�����O�^�[�Q�b�g���e�N�X�`��2���֐؂�ւ���
	static	void	BeginMultiPE(int n) {
		//�����_�����O�^�[�Q�b�g�̐؂�ւ�
		m_devicecontext->OMSetRenderTargets(
			2,//MRT�p��2���̃e�N�X�`���������_�����O�^�[�Q�b�g�ɂ���
			&m_perenderer_targetview[n],	//�����_�����O�e�N�X�`��
			m_depthstencil_view);		//Z�o�b�t�@

		//�����_�����O�e�N�X�`���N���A
		{
			float ClearColor[4] = { 0.0f, 0.5f, 0.0f, 1.0f };//��
			m_devicecontext->ClearRenderTargetView(m_perenderer_targetview[n],
				ClearColor);
			m_devicecontext->ClearRenderTargetView(m_perenderer_targetview[n + 1],
				ClearColor);
		}
		//Z�o�b�t�@�̃N���A
		m_devicecontext->ClearDepthStencilView(m_depthstencil_view,
			D3D11_CLEAR_DEPTH, 1.0f, 0);

	}


	static ID3D11ShaderResourceView* GetDepthShadowTexture()
	{
		return m_depthshadow_shader_resourceview;
	}

	static void BeginDepth()
	{
		//�[�x�o�b�t�@���e�N�X�`���ɐ؂�ւ�,���e��1.0�ŃN���A
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
	static ID3D11BlendState* m_blendstate_add;//���Z�����p�u�����h�X�e�[�g
	static ID3D11BlendState* m_blend_state_atc;
	static ID3D11BlendState* m_blend_state_mask;


	static	ID3D11RenderTargetView* m_perenderer_targetview[3];//1101�ύX
	static	ID3D11ShaderResourceView* m_peshader_resourceview[3];//1101�ύX

	static	ID3D11RenderTargetView* m_particlerenderer_targetview;//1101�ύX
	static	ID3D11ShaderResourceView* m_particleshader_resourceview;//1101�ύX


	static ID3D11DepthStencilView* m_depthshadow_stencilview;//1202
	static ID3D11ShaderResourceView* m_depthshadow_shader_resourceview;//1202


	//3D�E2D �`��؂�ւ��֐��p�ϐ�
	static XMMATRIX m_viewmatrix;
	static XMMATRIX m_projectionmatrix;

	PARAMETER m_Parameter{};

	//�|�X�g�G�t�F�N�g�����I�u�W�F�N�g�|�C���^
	class PostEffectObject* m_end_draw = nullptr;
	LIGHT m_Light{};


};
