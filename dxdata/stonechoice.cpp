#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "audio.h"
#include "scene.h"
#include "camera.h"
#include "input.h"
#include "xmextended.h"
#include "resultInstance.h"
#include "animationModel.h"
#include "stonechoice.h"
#include "loadScene.h"
#include "polygon2D.h"

void StoneChoice::Init() {

	m_file_path[0] = "asset\\model\\stone.fbx";
	m_file_path[1] = "asset\\model\\bluerock.fbx";
	m_file_path[2] = "asset\\model\\redrock.fbx";

	m_cursor = new Polygon2D(this);
	((Polygon2D*)m_cursor)->SetImage("asset\\texture\\arrow_back.png",XMFLOAT2(SCREEN_WIDTH/2, SCREEN_HEIGHT / 2), XMFLOAT2(SCREEN_WIDTH / 2,SCREEN_HEIGHT/2));
	m_stone_one = new AnimationModel(this);
	m_stone_two = new AnimationModel(this);
	m_stone_three = new AnimationModel(this);

	((AnimationModel*)m_stone_one)->Load_NoBone("asset\\model\\stone.fbx");
	((AnimationModel*)m_stone_two)->Load_NoBone("asset\\model\\bluerock.fbx");
	((AnimationModel*)m_stone_three)->Load_NoBone("asset\\model\\redrock.fbx");

	m_choice_se = new Audio(this);
	m_choice_se->Load("asset\\sound\\maou_se_system27.wav");


	Renderer::GetRenderer().CreateVertexShader(&m_vertex_shader, &m_VertexLayout,
		"shader\\vertexLightingVS.cso");
	Renderer::GetRenderer().CreatePixelShader(&m_pixel_shader,
		"shader\\vertexLightingPS.cso");
	m_transform->SetScale(XMFLOAT3(0.5f, 0.5f, 0.5f));
	m_transform->SetPosition(XMFLOAT3(0.0f, 18.0f, -50.0f));
}

void StoneChoice::UnInit() {
	m_VertexLayout->Release();
	m_vertex_shader->Release();
	m_pixel_shader->Release();

	m_stone_one->UnInit();
 m_stone_two->UnInit();
	m_stone_three->UnInit();

	delete 	m_stone_one;
	delete m_stone_two;
	delete m_stone_three;
}

void StoneChoice::Update() {

	if (Input::GetKeyTrigger('A') || Input::GetJoyButtonTrigger(0, JSMASK_E)) {
		m_id = (m_id - 1) % m_array_size;
		if (m_id < 0) {
			m_id = m_array_size - 1; // 負の値の場合、配列の最大値に設定
		}
	}
	else if (Input::GetKeyTrigger('D') || Input::GetJoyButtonTrigger(0, JSMASK_W)) {
		m_id = (m_id + 1) % m_array_size;
	}
	if (Input::GetKeyTrigger(VK_SPACE) || Input::GetJoyButtonTrigger(0, JSMASK_R)) {
		m_choice_se->Play();
		ResultInstance* instance = Manager::GetManager().GetGameInstance<ResultInstance>();
		instance->m_choicestone_path = m_file_path[m_id];

		if (Manager::GetManager().CheckPreload()) {
			Manager::GetManager().ChangeScene();
		}
	}
}

void StoneChoice::Draw() {
	//入力レイアウト
	Renderer::GetRenderer().GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//shader設定
	Renderer::GetRenderer().GetDeviceContext()->VSSetShader(m_vertex_shader, NULL, 0);
	Renderer::GetRenderer().GetDeviceContext()->PSSetShader(m_pixel_shader, NULL, 0);

	//マトリクス設定
	Renderer::GetRenderer().SetWorldMatrix(m_transform->GetWorldMatrix());


	//マテリアル
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::GetRenderer().SetMaterial(material);

	switch (m_id)
	{
	case 0:
		((AnimationModel*)m_stone_one)->Draw();
		break;
	case 1:
		((AnimationModel*)m_stone_two)->Draw();
		break;
	case 2:
		((AnimationModel*)m_stone_three)->Draw();
		break;
	}
	//カーソル描画
	((Polygon2D*)m_cursor)->Draw();
}