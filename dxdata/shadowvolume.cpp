#include "main.h"
#include "renderer.h"
#include "modelRenderer.h"
#include "camera.h"
#include "shadowvolume.h"
#include "manager.h"
#include "scene.h"

void ShadowVolume::Init() {

	auto l_Compo = new ModelRenderer(this);
	l_Compo->Load("asset\\model\\sphere_smooth.obj");
	m_Components.push_back(l_Compo);

	Renderer::GetRenderer().CreateVertexShader(&m_vertex_shader, &m_vertex_layout,
		"shader\\envMappingVS.cso");
	Renderer::GetRenderer().CreatePixelShader(&m_pixel_shader,
		"shader\\envMappingPS.cso");

	auto pos = m_transform->GetPosition();
	pos.y = 10000.0f;
	m_transform->SetPosition(pos);
	m_transform->SetScale(5.0f, 5.0f, 5.0f);
}

void ShadowVolume::UnInit() {

	m_vertex_layout->Release();
	m_vertex_shader->Release();
	m_pixel_shader->Release();
}

void ShadowVolume::Update() {

}

void ShadowVolume::Draw() {
	//入力レイアウト
	Renderer::GetRenderer().GetDeviceContext()->IASetInputLayout(m_vertex_layout);

	//shader設定
	Renderer::GetRenderer().GetDeviceContext()->VSSetShader(m_vertex_shader, NULL, 0);
	Renderer::GetRenderer().GetDeviceContext()->PSSetShader(m_pixel_shader, NULL, 0);


	//マトリクス
	Renderer::GetRenderer().SetWorldMatrix(m_transform->GetWorldMatrix());


	Renderer::GetRenderer().SetBlendMaskEnable(true);

	Renderer::GetRenderer().SetStencilEnable(true);
	Renderer::GetRenderer().SetCullEnable(false);

	for (auto* pm : m_Components)
	{

		pm->Draw();


	}
	Renderer::GetRenderer().SetBlendMaskEnable(false);

	Renderer::GetRenderer().SetDepthEnable(true);

	Renderer::GetRenderer().SetCullEnable(true);
}