#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "scene.h"
#include "input.h"
#include "gamescene.h"
#include "resultscene.h"
#include "titlescene.h"
#include "polygon2D.h"
#include "skydome.h"
#include "score.h"
#include "resultInstance.h"
#include "camera.h"
#include "terrain.h"
#include "water.h"
#include "resultilust.h"
#include "replayStone.h"
#include "freePrefs.h"

void ResultScene::Init() {
	AddGameObject<ResultIlust>(2);
	auto cam = AddGameObject<Camera>(0);
	cam->ApplyRotationQuaternion(XMFLOAT4(0.552079618f, 0.0104215816f, 0.00690138433, 0.833697736f));
	cam->SetDistance(95.0f);
	AddGameObject<SkyDome>(0);
	auto tr = AddGameObject<Terrain>(1);
	tr->GetTransform()->SetPosition(XMFLOAT3(0, -3.0f, 0));
	AddGameObject<Water>(1);
	tr->MakeTerrainAndBlendTextureDDS("asset\\texture\\kawa.png", XMFLOAT3(256.0f, 15.0f, 128.0f),
		"asset\\texture\\kawazoko.dds", "asset\\texture\\field.dds", "asset\\texture\\blend.dds");
	AddGameObject<ReplayStone>(1);
	m_score = AddGameObject<Score>(2);	
}

void ResultScene::Update() {
	auto rs = GetGameObject<ReplayStone>();
	if (!m_flag) {
		Manager::GetManager().PreLoadScene<TitleScene>();
		m_flag = true;
		ResultInstance* instance = Manager::GetManager().GetGameInstance<ResultInstance>();
		m_score->AddScore((instance->m_length * instance->m_bound) / 10);
		//FreePrefs::SetInt("Score", (instance->m_length * instance->m_bound) / 10);
		//int x = FreePrefs::GetInt("Score");
		rs->RestartPlay();
	}
	if (rs->GetEndFlag()) {
		auto wave = GetGameObject<Water>();
		wave->InitWave();
		rs->RestartPlay();
	}
	if (Input::GetKeyTrigger(VK_SPACE) || Input::GetJoyButtonTrigger(0, JSMASK_R)) {
		if (Manager::GetManager().CheckPreload()) {
			//FreePrefs::PrefsSave();
			Manager::GetManager().ChangeScene();
		}
	}
}
