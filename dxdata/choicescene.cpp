#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "scene.h"
#include "choicescene.h"
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
#include "stonechoice.h"

void ChoiceScene::Init() {
	auto cam = AddGameObject<Camera>(0);
	cam->ApplyRotationQuaternion(XMFLOAT4(0.13706708f, 0.0f, 0.0f, 0.990561783f));
	cam->SetDistance(70.0f);
	AddGameObject<SkyDome>(0);
	auto tr = AddGameObject<Terrain>(1);
	tr->GetTransform()->SetPosition(XMFLOAT3(0, -3.0f, 0));
	AddGameObject<Water>(1);
	tr->MakeTerrainAndBlendTextureDDS("asset\\texture\\kawa.png", XMFLOAT3(256.0f, 15.0f, 128.0f),
		"asset\\texture\\kawazoko.dds", "asset\\texture\\field.dds", "asset\\texture\\blend.dds");
	AddGameObject<StoneChoice>(1);
}

void ChoiceScene::Update() {
	if (!m_flag) {
		Manager::GetManager().PreLoadScene<LoadingScene>();
		m_flag = true;
	}
}