#include "main.h"
#include "renderer.h"
#include "loadanim.h"
#include "scene.h"
#include "camera.h"
#include "manager.h"

void LoadAnime::Init() {
	m_transform->SetPosition(XMFLOAT3(SCREEN_WIDTH/2,SCREEN_HEIGHT/2,0.0f));

	VERTEX_3D vertex[4]{};

	vertex[0].Position = XMFLOAT3(0.0f,  0.0f, 0);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(0, 0);

	vertex[1].Position = XMFLOAT3(200.0f, 0.0f, 0);
	vertex[1].Normal = XMFLOAT3(0, 1.0f, 0);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(0.25f, 0);

	vertex[2].Position = XMFLOAT3(0.0f,200.0f, 0);
	vertex[2].Normal = XMFLOAT3(0, 1.0f, 0);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(0, 0.25f);

	vertex[3].Position = XMFLOAT3( 200.0f,  200.0f, 0);
	vertex[3].Normal = XMFLOAT3(0, 1.0f, 0);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(0.25f, 0.25f);


	//頂点バッファ生成　anime
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;//DEFALUT からDYNAMICに置き換える
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//データを書き込める状態にする
	//書き込みやすいが描画効率が下がる

	D3D11_SUBRESOURCE_DATA sd{};
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	Renderer::GetRenderer().GetDevice()->CreateBuffer(&bd, &sd, &m_vertex_buffer);

	//テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"asset\\texture\\mizukiriloadanim.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture);
	assert(m_texture);

	Renderer::GetRenderer().CreateVertexShader(&m_vertex_shader, &m_vertex_layout,
		"shader\\unlitTextureVS.cso");
	Renderer::GetRenderer().CreatePixelShader(&m_pixel_shader,
		"shader\\unlitTexTurePS.cso");
}

void LoadAnime::UnInit() {
	m_vertex_buffer->Release();
	m_texture->Release();

	m_vertex_layout->Release();
	m_vertex_shader->Release();
	m_pixel_shader->Release();
}

void LoadAnime::Update() {
	m_counter++;
	
}

void LoadAnime::Draw() {
	//===================
	//アニメーション処理
	float x = m_counter % 3 * (1.0f / 3);
	float y = m_counter / 2 * (1.0f / 2);

	//頂点データ書換
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetRenderer().GetDeviceContext()->Map(m_vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 0);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(x, y);

	vertex[1].Position = XMFLOAT3(200.0f, 0.0f, 0);
	vertex[1].Normal = XMFLOAT3(0, 1.0f, 0);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(x + 0.25f, y);

	vertex[2].Position = XMFLOAT3(0.0f, 200.0f, 0);
	vertex[2].Normal = XMFLOAT3(0, 1.0f, 0);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(x, y + 0.25f);

	vertex[3].Position = XMFLOAT3(200.0f, 200.0f, 0);
	vertex[3].Normal = XMFLOAT3(0, 1.0f, 0);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(x + 0.25f, y + 0.25f);

	Renderer::GetRenderer().GetDeviceContext()->Unmap(m_vertex_buffer, 0);
	//===================
		//入力レイアウト
	Renderer::GetRenderer().GetDeviceContext()->IASetInputLayout(m_vertex_layout);

	//shader設定
	Renderer::GetRenderer().GetDeviceContext()->VSSetShader(m_vertex_shader, NULL, 0);
	Renderer::GetRenderer().GetDeviceContext()->PSSetShader(m_pixel_shader, NULL, 0);

	//マトリクス
	Renderer::GetRenderer().SetWorldViewProjection2D();
	Renderer::GetRenderer().SetWorldMatrix(m_transform->GetWorldMatrix());
	//===========================================

	//頂点バッファ
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetRenderer().GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

	//マテリアル
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::GetRenderer().SetMaterial(material);

	//テクスチャ
	Renderer::GetRenderer().GetDeviceContext()->PSSetShaderResources(0, 1, &m_texture);

	//プリミティブトポロジー
	Renderer::GetRenderer().GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ポリゴン描画
	Renderer::GetRenderer().GetDeviceContext()->Draw(4, 0);
}