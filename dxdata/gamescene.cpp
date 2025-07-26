#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "scene.h"
#include "gamescene.h"
#include "score.h"
#include "skydome.h"
#include "terrain.h"
#include "stone.h"
#include "water.h"
#include "splasheEmitter.h"
#include "slingshot.h"
#include "shadow2D.h"
#include "verticalGauge.h"
#include "shadowvolume.h"
#include "titlescene.h"

void GameScene::Init() {
	auto cam = AddGameObject<Camera>(0);
	cam->ApplyRotationQuaternion(XMFLOAT4(0.13706708f, 0.0f, 0.0f, 0.990561783f));
	cam->SetDistance(70.0f);
	AddGameObject<SkyDome>(0);
	auto tr = AddGameObject<Terrain>(1);
	AddGameObject<Water>(1);
	tr->GetTransform()->SetPosition(0, -3.0f, 0);
	tr->MakeTerrainAndBlendTextureDDS("asset\\texture\\kawa.png", XMFLOAT3(256.0f, 15.0f, 128.0f),
		"asset\\texture\\kawazoko.dds", "asset\\texture\\field.dds", "asset\\texture\\blend.dds");
	AddGameObject<ShadowVolume>(1);
	AddGameObject<Shadow2D>(1);
	AddGameObject<VerticalGauge>(2);
	AddGameObject<SlingShot>(1);
}

void GameScene::Update() {

	if (!m_flag) {
 		Manager::GetManager().PreLoadScene<ResultScene>();
		m_flag = true;
	}

};
