#include "main.h"
#include "renderer.h"
#include "particleEmitter.h"
#include "scene.h"
#include "camera.h"
#include "manager.h"
#include "input.h"

void particleEmitter::Init() {
	if (!m_Enable)return;

	VERTEX_3D vertex[4]{};

	vertex[0].Position = XMFLOAT3(-5.0f, 5.0f, 0);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(0, 0);

	vertex[1].Position = XMFLOAT3(5.0f, 5.0f, 0);
	vertex[1].Normal = XMFLOAT3(0, 1.0f, 0);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0);

	vertex[2].Position = XMFLOAT3(-5.0f, -5.0f, 0);
	vertex[2].Normal = XMFLOAT3(0, 1.0f, 0);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(0, 1.0f);

	vertex[3].Position = XMFLOAT3(5.0f, -5.0f, 0);
	vertex[3].Normal = XMFLOAT3(0, 1.0f, 0);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);


	//���_�o�b�t�@�����@anime
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;//DEFALUT ����DYNAMIC�ɒu��������
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//�f�[�^���������߂��Ԃɂ���
	//�������݂₷�����`�������������

	D3D11_SUBRESOURCE_DATA sd{};
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	Renderer::GetRenderer().GetDevice()->CreateBuffer(&bd, &sd, &m_vertex_buffer);

	//�e�N�X�`���ǂݍ���
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"asset\\texture\\particle.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture);
	assert(m_texture);

	Renderer::GetRenderer().CreateVertexShader(&m_vertex_shader, &m_vertex_layout,
		"shader\\unlitTextureVS.cso");
	Renderer::GetRenderer().CreatePixelShader(&m_pixel_shader,
		"shader\\unlitTexTurePS.cso");
}

void particleEmitter::UnInit() {
	if (!m_Enable)return;

	m_vertex_buffer->Release();
	m_texture->Release();

	m_vertex_layout->Release();
	m_vertex_shader->Release();
	m_pixel_shader->Release();
	delete[] g_Particles;

}

void particleEmitter::Update() {
	if (!m_Enable)return;

	XMFLOAT3 mpos = m_game_object->GetTransform()->GetPosition();
	//�p�[�e�B�N������
	for (int x = 0; x < PARTICLE_MAX; x++) {
		if (g_Particles[x].Enable == false) {
			g_Particles[x].Enable = true;
			float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * XM_PI;
			g_Particles[x].Enable = true;
			g_Particles[x].Position.y = mpos.y;
			XMFLOAT3 vn(cos(angle) * 1.0f, (rand() % 100 + 50) / 100.0f, sin(angle) * 1.0f);
			g_Particles[x].Position.x = mpos.x + vn.x;
			g_Particles[x].Position.z = mpos.z + vn.z;
			//g_Particles[x].Position = mpos;//���쐳���
	/*		g_Particles[x].Velocity.x = (rand() % 100 - 50) / 100.0f;
			g_Particles[x].Velocity.y = (rand() % 100 + 50) / 100.0f;
			g_Particles[x].Velocity.z = (rand() % 100 - 50) / 100.0f;*/

			//�e�X�g
			float len = sqrtf(vn.x * vn.x + vn.y * vn.y + vn.z * vn.z);
			vn.x /= len;
			vn.y /= len;
			vn.z /= len;
			g_Particles[x].Velocity.x = vn.x * 5.0f;
			g_Particles[x].Velocity.y = vn.y * 5.0f;
			g_Particles[x].Velocity.z = vn.z * 5.0f;
			g_Particles[x].life = 300.0f;
			break;
		}
	}
	float dt = Manager::GetManager().GetDeltaTime();

	//�p�[�e�B�N���ړ�
	for (int x = 0; x < PARTICLE_MAX; x++) {
		if (g_Particles[x].Enable != false) {
			g_Particles[x].Velocity.y += -0.08f * dt;

			g_Particles[x].Position.x += g_Particles[x].Velocity.x * dt;
			g_Particles[x].Position.y += g_Particles[x].Velocity.y * dt;
			g_Particles[x].Position.z += g_Particles[x].Velocity.z * dt;
			g_Particles[x].life--;
			if (g_Particles[x].life <= 0) {
				g_Particles[x].Enable = false;
			}

		}
	}
}

void particleEmitter::Draw() {

	if (!m_Enable)return;

	//���̓��C�A�E�g
	Renderer::GetRenderer().GetDeviceContext()->IASetInputLayout(m_vertex_layout);

	//shader�ݒ�
	Renderer::GetRenderer().GetDeviceContext()->VSSetShader(m_vertex_shader, NULL, 0);
	Renderer::GetRenderer().GetDeviceContext()->PSSetShader(m_pixel_shader, NULL, 0);


	//�r���{�[�h���� //�t�s��ver �V�n�s��̕��@������

		//�J������view�}�g���N�X�擾
	Scene& _scene = Manager::GetManager().GetNowScene();
	Camera* cam = _scene.GetGameObject<Camera>();
	XMMATRIX view = cam->GetViewMatrix();

	//�r���[�̋t�s��
	XMMATRIX inView;
	inView = XMMatrixInverse(nullptr, view);//�t�s��
	//�ړ������𖳌���
	inView.r[3].m128_f32[0] = 0.0f;
	inView.r[3].m128_f32[1] = 0.0f;
	inView.r[3].m128_f32[2] = 0.0f;



	//���_�o�b�t�@
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetRenderer().GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);


	//�}�e���A��
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(0.1f, 0.1f, 0.6f, 1.0f);//1�F��葼�F�������鎖�Ŗ��邭������
	material.TextureEnable = true;
	Renderer::GetRenderer().SetMaterial(material);

	//�e�N�X�`��
	Renderer::GetRenderer().GetDeviceContext()->PSSetShaderResources(0, 1, &m_texture);

	//�v���~�e�B�u�g�|���W�[
	Renderer::GetRenderer().GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//z�o�b�t�@����
	Renderer::GetRenderer().SetDepthEnable(false);

	//���Z�����L����
	Renderer::GetRenderer().SetBlendAddEnable(true);

	for (int v = 0; v < PARTICLE_MAX; v++)
	{
		//�}�g���N�X //���[�v
		if (g_Particles[v].Enable == true) {
			XMMATRIX world, scale, trans;
			scale = m_game_object->GetTransform()->GetScaleMatrix();
			trans = XMMatrixTranslation(g_Particles[v].Position.x, g_Particles[v].Position.y, g_Particles[v].Position.z);
			world = scale * inView * trans;
			Renderer::GetRenderer().SetWorldMatrix(world);

			//�|���S���`��
			Renderer::GetRenderer().GetDeviceContext()->Draw(4, 0);
		}
	}
	Renderer::GetRenderer().SetBlendAddEnable(false);
	Renderer::GetRenderer().SetDepthEnable(true);
}