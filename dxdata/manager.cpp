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

//@brief ��{FPS�@MS
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
	// �u���[�N�|�C���g���őO�t���[���ƌ��t���[���̌o�ߎ��Ԃ�
	// �傫�Ȓl�ɂȂ�ۂ�defalut�l�������ė\�����ʋ�����\�h�B
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
	Renderer::GetRenderer().Begin();//IMGUI�g�p�̈�
	m_scene->Update();
	m_scene->ParentUpdate();
}


void Manager::Draw()
{
	m_scene->Draw();	
	m_scene->PostDraw();
	Renderer::GetRenderer().End();
}





// �w�i�X���b�h�Ŏ��s�������ۂ̃��[�f�B���O�^�X�N
void Manager::LoadingTask(std::unique_ptr<Scene> scene_to_init) {
	// ���̊֐��X�R�[�v���� scene_to_init ���V�[���I�u�W�F�N�g�̏��L��������

	if (!scene_to_init) {
		// ��O�I�ȃP�[�X�����Anullptr���n���ꂽ�ꍇ�̏���
		std::lock_guard<std::mutex> lock(m_state_mutex);
		m_load_state = LoadState::Error;
		m_has_error = true;
		return; // unique_ptr �̓X�R�[�v�𔲂���Ƃ��ɔj�������
	}

	bool init_successful = false;
	try {
			scene_to_init->Init();
		init_successful = true;
	}
	catch (const std::exception& e) {
		// ���������ɗ�O�����������ꍇ
		std::lock_guard<std::mutex> lock(m_state_mutex); // �G���[��Ԃ̍X�V��ی�
		m_load_state = LoadState::Error;
		m_has_error = true;
		// scene_to_init �̓X�R�[�v�𔲂���Ƃ��ɔj�������iunique_ptr�̂������ň��S�j
		return; // �G���[�����Ń^�X�N�I��
	}
	catch (...) {
		// �s���ȗ�O�̏ꍇ
		std::lock_guard<std::mutex> lock(m_state_mutex); // �G���[��Ԃ̍X�V��ی�
		m_load_state = LoadState::Error;
		m_has_error = true;
		// scene_to_init �̓X�R�[�v�𔲂���Ƃ��ɔj�������
		return; // �G���[�����Ń^�X�N�I��
	}

	// Init() �����������ꍇ
	std::lock_guard<std::mutex> lock(m_state_mutex); // ���ʂ̏�ԍX�V��ی�
	if (init_successful) {
		m_load_state = LoadState::Loaded; // ���[�h������ԂɑJ��
		m_loaded_scene_result = std::move(scene_to_init); // �������ς݃V�[���̏��L�����}�l�[�W���[�Ɉڏ�
	}
} 




void Manager::SwapScene() {
	std::unique_ptr<Scene> old_scene_to_uninit; // �Â��V�[����UnInit/����p�̈ꎞ�ϐ�

	{ // mutex ���b�N �X�R�[�v
		std::lock_guard<std::mutex> lock(m_state_mutex);

		// �V�[���X���b�v�v��������A�����[�h������ (�����܂��̓G���[) ���Ă��邩�`�F�b�N
		if (!m_scene_swap_flag || (m_load_state != LoadState::Loaded && m_load_state != LoadState::Error)) {
			// �X���b�v�����𖞂����Ă��Ȃ�
			return;
		}

		if (m_load_state == LoadState::Loaded) {// ���[�h�����A�V�[���؂�ւ����s

			// ���݂̃V�[�����Â��V�[���Ƃ��Ĉꎞ�ϐ��Ɉړ�
			old_scene_to_uninit = std::move(m_scene);

			// ���[�h���������V�����V�[�������݂̃V�[���Ƃ��Đݒ�
			m_scene = std::move(m_loaded_scene_result); // m_loaded_scene_result �͂����� nullptr �ɂȂ�

			// ���[�h��ԂƃX���b�v�v���t���O�����Z�b�g
			resetLoadState(); // ���[�h��Ԃ�Idle�ɖ߂�
			m_scene_swap_flag = false;

		}
		else if (m_load_state == LoadState::Error) {
			// ���[�h���ɃG���[���������Ă����ꍇ
			// �G���[��Ԃ��N���A���A���[�h��Ԃ����Z�b�g
			resetLoadState(); // ���[�h��Ԃ�Idle�ɖ߂�
			m_scene_swap_flag = false; // �X���b�v�v�����L�����Z��
			// �G���[�V�[���ɑJ�ڂ���Ȃǂ̃��J�o�������������ɒǉ����邱�Ƃ��\
		}

	} // mutex ���b�N ���

	// mutex�����A�Â��V�[����UnInit���Ăяo�� (���Ԃ�������\�������邽��)
	if (old_scene_to_uninit) {
		old_scene_to_uninit->UnInit();
	}
};




void Manager::CallLoading(std::unique_ptr<Scene> scene_to_load) {
	// unique_ptr �̏��L���� std::thread �̃R���X�g���N�^�Ƀ��[�u�œn��
	// �w�i�X���b�h�Ŏ��s���郁���o�֐��Ƃ��̈��� (Manager*, unique_ptr<Scene>) ���w��
	std::thread loading_thread(&Manager::LoadingTask, this, std::move(scene_to_load));
	loading_thread.detach(); // �X���b�h���f�^�b�`���āAManager�̐������ԂƐ؂藣��

	{
		std::lock_guard<std::mutex> lock(m_state_mutex);
		m_load_state = LoadState::Loading; // ���[�h�J�n��ԂɑJ��
	}
}
