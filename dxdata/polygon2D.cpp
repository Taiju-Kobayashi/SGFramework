#include "main.h"
#include "renderer.h"
#include "polygon2D.h"
#include "polygon2D.h"

void Polygon2D::UnInit(){
	if (!m_is_draw)return;
	m_vertex_buffer->Release();
	m_texture->Release();

	m_vertex_layout->Release();
	m_vertex_shader->Release();
	m_pixel_shader->Release();
}

void Polygon2D::Update(){

}

void Polygon2D::Draw(){
	if (!m_is_draw)return;

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
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::GetRenderer().SetMaterial(material);

	//テクスチャ
	Render.GetDeviceContext()->PSSetShaderResources(0, 1, &m_texture);

	//プリミティブトポロジー
	Render.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ポリゴン描画
	Render.GetDeviceContext()->Draw(4, 0);
}

bool Polygon2D::SetImage(const char* ImagePath, const XMFLOAT2 imageSize, const XMFLOAT2 position) {
	auto& Render = Renderer::GetRenderer();

	//テクスチャ読み込み
	{
		size_t len = strlen(ImagePath) + 1;//終端分+1
		wchar_t* wcstring = new wchar_t[len];
		size_t word;
		mbstowcs_s(&word, wcstring, len, ImagePath, _TRUNCATE);
		TexMetadata metadata;
		ScratchImage image;
		auto hr = LoadFromWICFile(wcstring, WIC_FLAGS_NONE, &metadata, image);
		if (FAILED(hr))
			return false;
		CreateShaderResourceView(Render.GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture);
		assert(m_texture);

	}

	VERTEX_3D vertex[4];

	vertex[0].Position = XMFLOAT3(position.x - imageSize.x, position.y - imageSize.y, 0);
	vertex[0].Normal = XMFLOAT3(0, 0, 0);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

	vertex[1].Position = XMFLOAT3(position.x + imageSize.x, position.y - imageSize.y, 0);
	vertex[1].Normal = XMFLOAT3(0, 0, 0);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);

	vertex[2].Position = XMFLOAT3(position.x - imageSize.x, position.y + imageSize.y, 0);
	vertex[2].Normal = XMFLOAT3(0, 0, 0);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);

	vertex[3].Position = XMFLOAT3(position.x + imageSize.x, position.y + imageSize.y, 0);
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


	Render.GetDevice()->CreateBuffer(&bd, &sd, &m_vertex_buffer);


	Render.CreateVertexShader(&m_vertex_shader, &m_vertex_layout,
		"shader\\unlitTextureVS.cso");
	Render.CreatePixelShader(&m_pixel_shader,
		"shader\\unlitTexTurePS.cso");

	m_is_draw = true;
	return true;
}