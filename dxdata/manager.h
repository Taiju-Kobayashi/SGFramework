/**
 * @file   manager.h
 * @brief  ゲームのシーン管理、時間管理、データ管理を行うためのクラスを定義したヘッダー
 * @author Taiju Kobayashi
 * @date   2024/07
 */
#pragma once
#include "main.h"
#include "scene.h"
#include <mutex>
#include <atomic> // Atomic flags could be used for simple cases, but mutex for overall state is safer here
#include <memory> // For unique_ptr
#include <string>
#include <stdexcept> // For exceptions
#include <thread>

class Scene;
class DataInstance;
/// @brief　 ゲームのシーン管理、時間管理、データ管理を行うためのクラス
class Manager
{


public:
	Manager();
	~Manager();
	static Manager& GetManager();
	static bool EndManager();
	Scene& GetNowScene();

	template<typename T>
	void PreLoadScene() {
		// ロード処理がすでに進行中かチェック
		{
			std::lock_guard<std::mutex> lock(m_state_mutex);
			if (isLoading()) {
				return;
			}
			// 前回のロード結果やエラー状態をクリア
			resetLoadState();
			m_scene_swap_flag = false; // プリロードは即時遷移ではない
		} // lock 解放

		// シーンオブジェクトを生成 (時間がかかる場合があるためロック外で)
		auto new_scene = std::make_unique<T>();

		// ロード処理を開始
		CallLoading(std::move(new_scene));

	};

	template<typename T>
	void LoadScene() {

		// ロード処理がすでに進行中かチェック
		{
			std::lock_guard<std::mutex> lock(m_state_mutex);
			if (isLoading()) {
				return;
			}
			// 前回のロード結果やエラー状態をクリア
			resetLoadState();
			m_scene_swap_flag = true; // ロード完了後即時遷移
		} // lock 解放

		// シーンオブジェクトを生成 (時間がかかる場合があるためロック外で)
		auto new_scene = std::make_unique<T>();

		// ロード処理を開始
		CallLoading(std::move(new_scene));

	};

	//Preloadした場合にシーン遷移を行う
	void ChangeScene() {
		std::lock_guard<std::mutex> lock(m_state_mutex);
		if (m_load_state == LoadState::Loaded || m_load_state == LoadState::Error) {
			// ロードが完了またはエラー終了している場合のみ、遷移要求可能
			m_scene_swap_flag = true;
		}
	}
	//Preload時にロードが終わっているかチェック
	bool CheckPreload() const {
		std::lock_guard<std::mutex> lock(m_state_mutex); // 状態の読み取りもロック
		// Loaded または Error 状態であれば、ロード処理としては完了とみなす
		return m_load_state == LoadState::Loaded || m_load_state == LoadState::Error;
	}

	float GetDeltaTime() { return m_delta_time; }
	double GetGameTime() { return m_game_time; }

	//既にセットされてる際はセット出来ない
	template<typename T>
	void SetGameInstance() {
		if (!m_datainstance) {
			m_datainstance = new T;
		}
	}

	void ClearGameInstance() {
		if (m_datainstance) {
			delete m_datainstance;
			m_datainstance = nullptr;
		}
	}

	//対象のインスタンスでない場合、nullptrを返す
	template<typename T>
	T* GetGameInstance() {
		T* ret = dynamic_cast<T*>(m_datainstance);
		if (ret != nullptr) {
			return ret;
		}
		return nullptr;
	}
	void Init();
	void Uninit();
	void Update();
	void Draw();
	void GameLoop(double deltatime);

	// ロード処理が進行中かチェック (Prepare, Loading 状態)
	// 注意: ロック内で呼び出す想定
	bool isLoading() const {
		return m_load_state == LoadState::Preparing || m_load_state == LoadState::Loading;
	}

	// ロード状態とエラー情報をリセット
	// 注意: ロック内で呼び出す想定
	void resetLoadState() {
		m_load_state = LoadState::Idle;
		m_has_error = false;
		m_error_message.clear();
	}

protected:
	static Manager* m_instance;
	void SwapScene();

	DataInstance* m_datainstance = nullptr;
private:
	bool m_scene_switch = false;
	float	m_delta_time = 1.0f / 60;
	float	m_local_time = 0.0f;
	float	m_time_step = 0.02f;
	double  m_game_time = 0.0;//起動からの経過時間

	// ロード状態を示す列挙型
	enum class LoadState {
		Idle,       // ロード処理なし
		Preparing,  // ロード準備中 (スレッド開始前)
		Loading,    // 背景スレッドでInit実行中
		Loaded,     // ロード成功、結果待ち
		Error       // ロード失敗
	};

	std::unique_ptr<Scene> m_scene;              // 現在アクティブなシーン
	// m_next_scene は CallLoading に渡す一時的な置き場としてのみ使用、メンバとしては持たない設計
	std::unique_ptr<Scene> m_loaded_scene_result; // ロードが完了したシーンの unique_ptr (結果)

	bool m_scene_swap_flag;                     // シーンスワップ要求フラグ
	LoadState m_load_state;                     // 現在のロード状態
	bool m_has_error;                           // ロード中にエラーが発生したか
	std::string m_error_message;                // エラーメッセージ

	mutable std::mutex m_state_mutex;           // 共有状態を保護するミューテックス

	void LoadingTask(std::unique_ptr<Scene> scene_to_init);
#ifdef _DEBUG
	bool	m_frame_feed = false;
#endif // !_DEBUG

	void CallLoading(std::unique_ptr<Scene> scene_to_load);

};
