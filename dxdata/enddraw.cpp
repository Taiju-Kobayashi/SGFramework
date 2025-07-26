#include "enddraw.h"
#include "main.h"
#include "renderer.h"

EndDraw::EndDraw() {

	VERTEX_3D vertex[4];

	vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

	vertex[1].Position = XMFLOAT3(SCREEN_WIDTH, 0.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);

	vertex[2].Position = XMFLOAT3(0.0f, SCREEN_HEIGHT, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);

	vertex[3].Position = XMFLOAT3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;
	auto& Render = Renderer::GetRenderer();

	Render.GetDevice()->CreateBuffer(&bd, &sd, &m_vertex_buffer);
	Render.CreateVertexShader(&m_vertex_shader, &m_vertex_layout,
		"shader\\defalutVS.cso");
	Render.CreatePixelShader(&m_pixel_shader,
		"shader\\defalutPS.cso");
}

EndDraw::~EndDraw() {
	m_vertex_buffer->Release();

	m_vertex_layout->Release();
	m_vertex_shader->Release();
	m_pixel_shader->Release();
}

void EndDraw::Draw(ID3D11ShaderResourceView* ppTexture) {
	Renderer& Render = Renderer::GetRenderer();

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

	////マテリアル
	//MATERIAL material;
	//ZeroMemory(&material, sizeof(material));
	//material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//material.TextureEnable = true;
	//Renderer::GetRenderer().SetMaterial(material);

	//テクスチャ
	Render.GetDeviceContext()->PSSetShaderResources(0, 1, &ppTexture);

	//プリミティブトポロジー
	Render.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



	//zバッファ無効
	//Renderer::GetRenderer().SetDepthEnable(false);
	
	//Renderer::GetRenderer().SetWorldViewProjection3D();

	//ポリゴン描画
	Render.GetDeviceContext()->Draw(4, 0);

	//Renderer::GetRenderer().SetDepthEnable(true);
}