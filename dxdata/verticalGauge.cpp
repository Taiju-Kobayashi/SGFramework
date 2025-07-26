#include "main.h"
#include "renderer.h"
#include "verticalGauge.h"
#include "audio.h"
#include "input.h"

void VerticalGauge::Init(){
	m_size.x = 100.0f;
	m_size.y = 300.0f;
	//m_Transform->SetPosition(XMFLOAT3(540.0f, 0.0f, 0.0f));
	VERTEX_3D vertex[4];

	vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 0);
	vertex[0].Normal = XMFLOAT3(0, 0, 0);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(0, 0);

	vertex[1].Position = XMFLOAT3(m_size.x, 0.0f, 0);
	vertex[1].Normal = XMFLOAT3(0, 0, 0);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0);

	vertex[2].Position = XMFLOAT3(0.0f, m_size.y, 0);
	vertex[2].Normal = XMFLOAT3(0, 0, 0);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(0, 1.0f);

	vertex[3].Position = XMFLOAT3(m_size.x, m_size.y, 0);
	vertex[3].Normal = XMFLOAT3(0, 0, 0);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);


	//頂点バッファ
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	auto& Render = Renderer::GetRenderer();

	Render.GetDevice()->CreateBuffer(&bd, &sd, &m_vertex_buffer);

	//テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"asset\\texture\\nakawaku.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(Render.GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture);
	assert(m_texture);
	LoadFromWICFile(L"asset\\texture\\WakuBlack_bg.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(Render.GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_bg_texture);
	assert(m_bg_texture);

	Render.CreateVertexShader(&m_vertex_shader, &m_vertex_layout,
		"shader\\unlitTextureVS.cso");
	Render.CreatePixelShader(&m_bgpixel_shader,
		"shader\\unlitTexTurePS.cso");
	Render.CreatePixelShader(&m_pixel_shader,
		"shader\\verticallGaugePS.cso");
}


void VerticalGauge::UnInit() {
	m_vertex_buffer->Release();
	m_texture->Release();
	m_bg_texture->Release();
	m_bgpixel_shader->Release();
	m_vertex_layout->Release();
	m_vertex_shader->Release();
	m_pixel_shader->Release();
}

void VerticalGauge::Draw(){

	auto& Render = Renderer::GetRenderer();

	//入力レイアウト
	Render.GetDeviceContext()->IASetInputLayout(m_vertex_layout);

	//shader設定
	Render.GetDeviceContext()->VSSetShader(m_vertex_shader, NULL, 0);
	Render.GetDeviceContext()->PSSetShader(m_bgpixel_shader, NULL, 0);

	//マトリクス
	Render.SetWorldViewProjection2D();

	Renderer::GetRenderer().SetWorldMatrix(m_transform->GetWorldMatrix());

	//頂点バッファ
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Render.GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

	//マテリアル
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::GetRenderer().SetMaterial(material);

	//テクスチャ
	Render.GetDeviceContext()->PSSetShaderResources(0, 1, &m_bg_texture);

	//プリミティブトポロジー
	Render.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ポリゴン描画
	Render.GetDeviceContext()->Draw(4, 0);

	//ゲージ描画
	Render.GetDeviceContext()->PSSetShader(m_pixel_shader, NULL, 0);
	Render.GetDeviceContext()->PSSetShaderResources(0, 1, &m_texture);
	Render.SetParameter(XMFLOAT4(m_gauge_value, 0, 0, 0),1);
	Render.GetDeviceContext()->Draw(4, 0);
	Render.SetWorldViewProjection3D();
}
