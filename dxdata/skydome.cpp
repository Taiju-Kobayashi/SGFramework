#include "main.h"
#include "manager.h"
#include "renderer.h"
#include	"modelRenderer.h";
#include "camera.h"
#include "skydome.h"
#include "input.h"
#include "scene.h"

void SkyDome::Init() {

	auto l_Compo = new ModelRenderer(this);
	l_Compo->Load("asset\\model\\sky.obj");
	m_Components.push_back(l_Compo);

	Renderer::GetRenderer().CreateVertexShader(&m_vertex_shader, &m_vertex_layout,
		"shader\\unlitTextureVS.cso");
	Renderer::GetRenderer().CreatePixelShader(&m_pixel_shader,
		"shader\\unlitTexturePS.cso");
	////テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"asset\\model\\sky.jpg", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture);
	assert(m_texture);

	m_transform->SetScale(500.0f, 500.0f, 500.0f);
}

void SkyDome::UnInit() {
	m_texture->Release();
	m_vertex_layout->Release();
	m_vertex_shader->Release();
	m_pixel_shader->Release();
}

void SkyDome::Update() {
	for (auto* pm : m_Components)
	{
		pm->Update();
	}	

}

void SkyDome::Draw() {
	//入力レイアウト
	Renderer::GetRenderer().GetDeviceContext()->IASetInputLayout(m_vertex_layout);

	//shader設定
	Renderer::GetRenderer().GetDeviceContext()->VSSetShader(m_vertex_shader, nullptr, 0);
	Renderer::GetRenderer().GetDeviceContext()->PSSetShader(m_pixel_shader, nullptr, 0);

	//マトリクス
	Renderer::GetRenderer().SetWorldMatrix(m_transform->GetWorldMatrix());

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

	for (auto* pm : m_Components)
	{
		pm->Draw();
	}
}