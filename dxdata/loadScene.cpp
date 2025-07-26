#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "scene.h"
#include "loadScene.h"
#include "input.h"
#include "gamescene.h"
#include "loadilust.h"
#include "loadanim.h"

void LoadingScene::Init() {
	AddGameObject<Camera>(0)->SetPosition(XMFLOAT3(-1.0f, 19.0000458f, -68.0000000f), XMFLOAT3(0, 0, 0));
	AddGameObject<LoadIlust>(2);
}

void LoadingScene::Update() {
	if (!m_flag) {
		Manager::GetManager().PreLoadScene<GameScene>();
		m_flag = true;
	}

	if (Manager::GetManager().CheckPreload()) {
		Manager::GetManager().ChangeScene();
	}

}