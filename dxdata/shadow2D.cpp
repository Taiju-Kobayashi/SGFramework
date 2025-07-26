#include "main.h"
#include "renderer.h"
#include "shadow2D.h"
#include "scene.h"

void Shadow2D::Init(){
	VERTEX_3D vertex[4];

	vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 0);
	vertex[0].Normal = XMFLOAT3(0, 0, 0);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(0, 0);

	vertex[1].Position = XMFLOAT3(SCREEN_WIDTH, 0, 0);
	vertex[1].Normal = XMFLOAT3(0, 0, 0);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0);

	vertex[2].Position = XMFLOAT3(0.0f, SCREEN_HEIGHT, 0);
	vertex[2].Normal = XMFLOAT3(0, 0, 0);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(0, 1.0f);

	vertex[3].Position = XMFLOAT3(SCREEN_WIDTH, SCREEN_HEIGHT, 0);
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
	LoadFromWICFile(L"asset\\texture\\incskies_019_16k.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(Render.GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture);
	assert(m_texture);

	Render.CreateVertexShader(&m_vertex_shader, &m_vertex_layout,
		"shader\\unlitTextureVS.cso");
	Render.CreatePixelShader(&m_pixel_shader,
		"shader\\unlitTexTurePS.cso");
}

void Shadow2D::UnInit(){
	m_vertex_buffer->Release();
	m_texture->Release();

	m_vertex_layout->Release();
	m_vertex_shader->Release();
	m_pixel_shader->Release();
}

void Shadow2D::Update(){

}

void Shadow2D::Draw(){

	auto& Render = Renderer::GetRenderer();

	//入力レイアウト
	Render.GetDeviceContext()->IASetInputLayout(m_vertex_layout);

	//shader設定
	Render.GetDeviceContext()->VSSetShader(m_vertex_shader, NULL, 0);
	Render.GetDeviceContext()->PSSetShader(m_pixel_shader, NULL, 0);

	//マトリクス
	Render.SetWorldViewProjection2D();

	//頂点バッファ
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Render.GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

	//マテリアル
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	material.TextureEnable = false;
	Renderer::GetRenderer().SetMaterial(material);

	////テクスチャ
	Render.GetDeviceContext()->PSSetShaderResources(0, 1, &m_texture);

	//プリミティブトポロジー
	Render.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	Render.SetStencilEnable(false);

	//ポリゴン描画
	Render.GetDeviceContext()->Draw(4, 0);

	Render.SetDepthEnable(true);

	auto cam = GetParentScene().GetGameObject<Camera>();
	if (cam)
		cam->Draw();
}
