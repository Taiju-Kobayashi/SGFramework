#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "scene.h"
#include "titlescene.h"
#include "modelRenderer.h"
#include "audio.h"
#include "dataInstance.h"
#include "freePrefs.h"

#define SAVE_PREFS_FILE "rom/pref/freepref.dat"

//@brief 基本FPS　MS
const static int g_framerate = 60;
const static float g_defalut_frameMS = (1.0f / g_framerate);

Manager* Manager::m_instance = nullptr;

Manager::Manager():m_load_state(LoadState::Idle), 
m_scene_swap_flag(false), m_has_error(false) 
{
	Renderer::GetRenderer().Init();
	Audio::InitMaster();
}

Manager::~Manager() {
	Uninit();
}

Manager& Manager::GetManager() {
	if (m_instance == nullptr) {
		m_instance = new Manager();
	}
	return *m_instance;
}
bool Manager::EndManager() {
	if (m_instance == nullptr) {
		return false;
	}
	delete m_instance;
	return true;
}

Scene& Manager::GetNowScene() {

	return *m_scene;
}



void Manager::Init()
{
	m_scene.reset(new TitleScene());
	m_scene->Init();
	FreePrefs::Init(SAVE_PREFS_FILE);
}


void Manager::Uninit()
{
	m_scene->UnInit();
	FreePrefs::Uninit();
	Audio::UninitMaster();
	ModelRenderer::UnloadAll();
	Input::Uninit();
	Renderer::GetRenderer().Uninit();
}

void Manager::GameLoop(double deltatime) {
	
	//@brief 
	// ブレークポイント等で前フレームと現フレームの経過時間が
	// 大きな値になる際にdefalut値を代入して予期せぬ挙動を予防。
	if (g_defalut_frameMS < deltatime)
		m_delta_time = g_defalut_frameMS;
	else
		m_delta_time = deltatime;

	m_game_time += m_delta_time;
#ifdef _DEBUG
	if (m_frame_feed)
	{
		return;
	}
#endif // _DEBUG

	if (m_scene) {
		Update();
	}

	SwapScene();
	m_local_time = 0.0f;

	m_local_time += m_delta_time;
}

void Manager::Update()
{

	Input::Update();
	Renderer::GetRenderer().Begin();//IMGUI使用の為
	m_scene->Update();
	m_scene->ParentUpdate();
}


void Manager::Draw()
{
	m_scene->Draw();	
	m_scene->PostDraw();
	Renderer::GetRenderer().End();
}





// 背景スレッドで実行される実際のローディングタスク
void Manager::LoadingTask(std::unique_ptr<Scene> scene_to_init) {
	// この関数スコープ内で scene_to_init がシーンオブジェクトの所有権を持つ

	if (!scene_to_init) {
		// 例外的なケースだが、nullptrが渡された場合の処理
		std::lock_guard<std::mutex> lock(m_state_mutex);
		m_load_state = LoadState::Error;
		m_has_error = true;
		return; // unique_ptr はスコープを抜けるときに破棄される
	}

	bool init_successful = false;
	try {
			scene_to_init->Init();
		init_successful = true;
	}
	catch (const std::exception& e) {
		// 初期化中に例外が発生した場合
		std::lock_guard<std::mutex> lock(m_state_mutex); // エラー状態の更新を保護
		m_load_state = LoadState::Error;
		m_has_error = true;
		// scene_to_init はスコープを抜けるときに破棄される（unique_ptrのおかげで安全）
		return; // エラー発生でタスク終了
	}
	catch (...) {
		// 不明な例外の場合
		std::lock_guard<std::mutex> lock(m_state_mutex); // エラー状態の更新を保護
		m_load_state = LoadState::Error;
		m_has_error = true;
		// scene_to_init はスコープを抜けるときに破棄される
		return; // エラー発生でタスク終了
	}

	// Init() が成功した場合
	std::lock_guard<std::mutex> lock(m_state_mutex); // 結果の状態更新を保護
	if (init_successful) {
		m_load_state = LoadState::Loaded; // ロード完了状態に遷移
		m_loaded_scene_result = std::move(scene_to_init); // 初期化済みシーンの所有権をマネージャーに移譲
	}
} 




void Manager::SwapScene() {
	std::unique_ptr<Scene> old_scene_to_uninit; // 古いシーンのUnInit/解放用の一時変数

	{ // mutex ロック スコープ
		std::lock_guard<std::mutex> lock(m_state_mutex);

		// シーンスワップ要求があり、かつロードが完了 (成功またはエラー) しているかチェック
		if (!m_scene_swap_flag || (m_load_state != LoadState::Loaded && m_load_state != LoadState::Error)) {
			// スワップ条件を満たしていない
			return;
		}

		if (m_load_state == LoadState::Loaded) {// ロード成功、シーン切り替え実行

			// 現在のシーンを古いシーンとして一時変数に移動
			old_scene_to_uninit = std::move(m_scene);

			// ロード完了した新しいシーンを現在のシーンとして設定
			m_scene = std::move(m_loaded_scene_result); // m_loaded_scene_result はここで nullptr になる

			// ロード状態とスワップ要求フラグをリセット
			resetLoadState(); // ロード状態をIdleに戻す
			m_scene_swap_flag = false;

		}
		else if (m_load_state == LoadState::Error) {
			// ロード中にエラーが発生していた場合
			// エラー状態をクリアし、ロード状態をリセット
			resetLoadState(); // ロード状態をIdleに戻す
			m_scene_swap_flag = false; // スワップ要求もキャンセル
			// エラーシーンに遷移するなどのリカバリ処理をここに追加することも可能
		}

	} // mutex ロック 解放

	// mutex解放後、古いシーンのUnInitを呼び出す (時間がかかる可能性があるため)
	if (old_scene_to_uninit) {
		old_scene_to_uninit->UnInit();
	}
};




void Manager::CallLoading(std::unique_ptr<Scene> scene_to_load) {
	// unique_ptr の所有権は std::thread のコンストラクタにムーブで渡す
	// 背景スレッドで実行するメンバ関数とその引数 (Manager*, unique_ptr<Scene>) を指定
	std::thread loading_thread(&Manager::LoadingTask, this, std::move(scene_to_load));
	loading_thread.detach(); // スレッドをデタッチして、Managerの生存期間と切り離す

	{
		std::lock_guard<std::mutex> lock(m_state_mutex);
		m_load_state = LoadState::Loading; // ロード開始状態に遷移
	}
}
