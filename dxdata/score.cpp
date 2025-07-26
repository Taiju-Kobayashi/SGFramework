#include "main.h"
#include "renderer.h"
#include "score.h"

void Score::Init() {
	VERTEX_3D vertex[4];

	vertex[0].Position = XMFLOAT3(100.0f, 100.0f, 0);
	vertex[0].Normal = XMFLOAT3(0, 0, 0);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(0, 0);

	vertex[1].Position = XMFLOAT3(500.0f, 100.0f, 0);
	vertex[1].Normal = XMFLOAT3(0, 0, 0);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0);

	vertex[2].Position = XMFLOAT3(100.0f, 500.0f, 0);
	vertex[2].Normal = XMFLOAT3(0, 0, 0);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(0, 1.0f);

	vertex[3].Position = XMFLOAT3(500.0, 500.0f, 0);
	vertex[3].Normal = XMFLOAT3(0, 0, 0);
	vertex[3].Diffuse = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);


	m_transform->SetPosition(XMFLOAT3(SCREEN_WIDTH/3,SCREEN_HEIGHT/3,0.0f));

	//���_�o�b�t�@�����@anime
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;//DEFALUT ����DYNAMIC�ɒu��������
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//�f�[�^���������߂��Ԃɂ���
	//�������݂₷�����`�������������

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	auto& Render = Renderer::GetRenderer();

	Render.GetDevice()->CreateBuffer(&bd, &sd, &m_vertex_buffer);

	//�e�N�X�`���ǂݍ���
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"asset\\texture\\syodonumber.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(Render.GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture);
	assert(m_texture);

	Render.CreateVertexShader(&m_vertex_shader, &m_vertex_layout,
		"shader\\unlitTextureVS.cso");
	Render.CreatePixelShader(&m_pixel_shader,
		"shader\\unlitTexTurePS.cso");
}


void Score::UnInit() {
	m_vertex_buffer->Release();
	m_texture->Release();

	m_vertex_layout->Release();
	m_vertex_shader->Release();
	m_pixel_shader->Release();
}

void Score::Update() {

}

void Score::Draw() {

	//
	auto& Render = Renderer::GetRenderer();

	

	//���̓��C�A�E�g
	Render.GetDeviceContext()->IASetInputLayout(m_vertex_layout);

	//shader�ݒ�
	Render.GetDeviceContext()->VSSetShader(m_vertex_shader, NULL, 0);
	Render.GetDeviceContext()->PSSetShader(m_pixel_shader, NULL, 0);

	//�}�g���N�X
	Render.SetWorldViewProjection2D();
	Render.SetWorldMatrix(m_transform->GetWorldMatrix());

	//���_�o�b�t�@
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Render.GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

	//�}�e���A��
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Render.SetMaterial(material);

	//�e�N�X�`��
	Render.GetDeviceContext()->PSSetShaderResources(0, 1, &m_texture);

	//�v���~�e�B�u�g�|���W�[
	Render.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	//===================
	{

		int One = m_Counter / 10;
		//�A�j���[�V��������
		float x = One % 5 * (1.0f / 5);
		float y = One / 5 * (1.0f / 5);

		//���_�f�[�^����
		D3D11_MAPPED_SUBRESOURCE msr;
		Render.GetDeviceContext()->Map(m_vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 0);
		vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = XMFLOAT2(x, y);

		vertex[1].Position = XMFLOAT3(200.0f, 0.0f, 0);
		vertex[1].Normal = XMFLOAT3(0, 1.0f, 0);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = XMFLOAT2(x + 0.2f, y);

		vertex[2].Position = XMFLOAT3(0.0f, 200.0f, 0);
		vertex[2].Normal = XMFLOAT3(0, 1.0f, 0);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = XMFLOAT2(x, y + 0.2f);

		vertex[3].Position = XMFLOAT3(200.0f, 200.0f, 0);
		vertex[3].Normal = XMFLOAT3(0, 1.0f, 0);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = XMFLOAT2(x + 0.2f, y + 0.2f);

		Render.GetDeviceContext()->Unmap(m_vertex_buffer, 0);	
		
		//�|���S���`��
		Render.GetDeviceContext()->Draw(4, 0);
	}

	{
		int Two = m_Counter % 10;
		//�A�j���[�V��������
		float x = Two % 5 * (1.0f / 5);
		float y = Two / 5 * (1.0f / 5);


		//���_�f�[�^����
		D3D11_MAPPED_SUBRESOURCE msr;
		Render.GetDeviceContext()->Map(m_vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		vertex[0].Position = XMFLOAT3(200.0f, 0.0f, 0);
		vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = XMFLOAT2(x, y);

		vertex[1].Position = XMFLOAT3(400.0f, 0.0f, 0);
		vertex[1].Normal = XMFLOAT3(0, 1.0f, 0);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = XMFLOAT2(x + 0.2f, y);

		vertex[2].Position = XMFLOAT3(200.0f, 200.0f, 0);
		vertex[2].Normal = XMFLOAT3(0, 1.0f, 0);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = XMFLOAT2(x, y + 0.2f);

		vertex[3].Position = XMFLOAT3(400.0f, 200.0f, 0);
		vertex[3].Normal = XMFLOAT3(0, 1.0f, 0);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = XMFLOAT2(x + 0.2f, y + 0.2f);

		Render.GetDeviceContext()->Unmap(m_vertex_buffer, 0);
		//===================

		//���̓��C�A�E�g
		Render.GetDeviceContext()->IASetInputLayout(m_vertex_layout);

		//shader�ݒ�
		Render.GetDeviceContext()->VSSetShader(m_vertex_shader, NULL, 0);
		Render.GetDeviceContext()->PSSetShader(m_pixel_shader, NULL, 0);

		//�}�g���N�X
		Render.SetWorldViewProjection2D();
		Render.SetWorldMatrix(m_transform->GetWorldMatrix());

		//���_�o�b�t�@
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		Render.GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

		//�}�e���A��
		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		material.TextureEnable = true;
		Render.SetMaterial(material);

		//�e�N�X�`��
		Render.GetDeviceContext()->PSSetShaderResources(0, 1, &m_texture);

		//�v���~�e�B�u�g�|���W�[
		Render.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		//�|���S���`��
		Render.GetDeviceContext()->Draw(4, 0);


	}
}
