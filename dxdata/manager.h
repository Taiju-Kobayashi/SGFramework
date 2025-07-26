/**
 * @file   manager.h
 * @brief  �Q�[���̃V�[���Ǘ��A���ԊǗ��A�f�[�^�Ǘ����s�����߂̃N���X���`�����w�b�_�[
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
/// @brief�@ �Q�[���̃V�[���Ǘ��A���ԊǗ��A�f�[�^�Ǘ����s�����߂̃N���X
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
		// ���[�h���������łɐi�s�����`�F�b�N
		{
			std::lock_guard<std::mutex> lock(m_state_mutex);
			if (isLoading()) {
				return;
			}
			// �O��̃��[�h���ʂ�G���[��Ԃ��N���A
			resetLoadState();
			m_scene_swap_flag = false; // �v�����[�h�͑����J�ڂł͂Ȃ�
		} // lock ���

		// �V�[���I�u�W�F�N�g�𐶐� (���Ԃ�������ꍇ�����邽�߃��b�N�O��)
		auto new_scene = std::make_unique<T>();

		// ���[�h�������J�n
		CallLoading(std::move(new_scene));

	};

	template<typename T>
	void LoadScene() {

		// ���[�h���������łɐi�s�����`�F�b�N
		{
			std::lock_guard<std::mutex> lock(m_state_mutex);
			if (isLoading()) {
				return;
			}
			// �O��̃��[�h���ʂ�G���[��Ԃ��N���A
			resetLoadState();
			m_scene_swap_flag = true; // ���[�h�����㑦���J��
		} // lock ���

		// �V�[���I�u�W�F�N�g�𐶐� (���Ԃ�������ꍇ�����邽�߃��b�N�O��)
		auto new_scene = std::make_unique<T>();

		// ���[�h�������J�n
		CallLoading(std::move(new_scene));

	};

	//Preload�����ꍇ�ɃV�[���J�ڂ��s��
	void ChangeScene() {
		std::lock_guard<std::mutex> lock(m_state_mutex);
		if (m_load_state == LoadState::Loaded || m_load_state == LoadState::Error) {
			// ���[�h�������܂��̓G���[�I�����Ă���ꍇ�̂݁A�J�ڗv���\
			m_scene_swap_flag = true;
		}
	}
	//Preload���Ƀ��[�h���I����Ă��邩�`�F�b�N
	bool CheckPreload() const {
		std::lock_guard<std::mutex> lock(m_state_mutex); // ��Ԃ̓ǂݎ������b�N
		// Loaded �܂��� Error ��Ԃł���΁A���[�h�����Ƃ��Ă͊����Ƃ݂Ȃ�
		return m_load_state == LoadState::Loaded || m_load_state == LoadState::Error;
	}

	float GetDeltaTime() { return m_delta_time; }
	double GetGameTime() { return m_game_time; }

	//���ɃZ�b�g����Ă�ۂ̓Z�b�g�o���Ȃ�
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

	//�Ώۂ̃C���X�^���X�łȂ��ꍇ�Anullptr��Ԃ�
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

	// ���[�h�������i�s�����`�F�b�N (Prepare, Loading ���)
	// ����: ���b�N���ŌĂяo���z��
	bool isLoading() const {
		return m_load_state == LoadState::Preparing || m_load_state == LoadState::Loading;
	}

	// ���[�h��ԂƃG���[�������Z�b�g
	// ����: ���b�N���ŌĂяo���z��
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
	double  m_game_time = 0.0;//�N������̌o�ߎ���

	// ���[�h��Ԃ������񋓌^
	enum class LoadState {
		Idle,       // ���[�h�����Ȃ�
		Preparing,  // ���[�h������ (�X���b�h�J�n�O)
		Loading,    // �w�i�X���b�h��Init���s��
		Loaded,     // ���[�h�����A���ʑ҂�
		Error       // ���[�h���s
	};

	std::unique_ptr<Scene> m_scene;              // ���݃A�N�e�B�u�ȃV�[��
	// m_next_scene �� CallLoading �ɓn���ꎞ�I�Ȓu����Ƃ��Ă̂ݎg�p�A�����o�Ƃ��Ă͎����Ȃ��݌v
	std::unique_ptr<Scene> m_loaded_scene_result; // ���[�h�����������V�[���� unique_ptr (����)

	bool m_scene_swap_flag;                     // �V�[���X���b�v�v���t���O
	LoadState m_load_state;                     // ���݂̃��[�h���
	bool m_has_error;                           // ���[�h���ɃG���[������������
	std::string m_error_message;                // �G���[���b�Z�[�W

	mutable std::mutex m_state_mutex;           // ���L��Ԃ�ی삷��~���[�e�b�N�X

	void LoadingTask(std::unique_ptr<Scene> scene_to_init);
#ifdef _DEBUG
	bool	m_frame_feed = false;
#endif // !_DEBUG

	void CallLoading(std::unique_ptr<Scene> scene_to_load);

};
