#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "scene.h"
#include "titlescene.h"
#include "input.h"
#include "gamescene.h"
#include "ilustTitle.h"
#include "terrain.h"
#include "stone.h"
#include "water.h"
#include "splasheEmitter.h"
#include "skydome.h"
#include "slingshot.h"
#include "camera.h"
#include "loadScene.h"
#include "shadow2D.h"
#include "shadowvolume.h"
#include "resultInstance.h"
#include "loadilust.h"
#include "choicescene.h"

void TitleScene::Init() {
	LIGHT light{};
	light.Enable = true;
	light.Direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	light.Position = XMFLOAT4(0.0f, 15.0f, 0.0f, 0.0f);
	light.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	light.Diffuse = XMFLOAT4(0.75f, 0.75f, 0.75f, 0.75f);
	light.PointLightParam = XMFLOAT4(10.0f, 0.5f, 0.5f, 0.5f);	
	Renderer::GetRenderer().SetLight(light);

	auto cam = AddGameObject<Camera>(0);
	cam->ApplyRotationQuaternion(XMFLOAT4(0.13706708f, 0.0f, 0.0f, 0.990561783f));
	cam->SetDistance(70.0f);
	AddGameObject<SkyDome>(0);
	auto tr = AddGameObject<Terrain>(1);
	tr->GetTransform()->SetPosition(XMFLOAT3(0, -3.0f, 0));
	AddGameObject<Water>(1);
	
	tr->MakeTerrainAndBlendTextureDDS("asset\\texture\\kawa.png", XMFLOAT3(256.0f, 15.0f, 128.0f),
		"asset\\texture\\kawazoko.dds", "asset\\texture\\field.dds", "asset\\texture\\blend.dds");

	AddGameObject<IlustTitle>(2);
	Manager::GetManager().SetGameInstance<ResultInstance>();
	auto* ins = Manager::GetManager().GetGameInstance<ResultInstance>();

}

void TitleScene::Update() {
	if (!m_flag) {
		Manager::GetManager().PreLoadScene<ChoiceScene>();
		m_flag = true;
	}

	if (Input::GetKeyTrigger(VK_SPACE) || Input::GetJoyButtonTrigger(0, JSMASK_R)) {

 		if (Manager::GetManager().CheckPreload()) {
 			Manager::GetManager().ChangeScene();
		}
	}
}