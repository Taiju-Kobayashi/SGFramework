
#include "main.h"
#include "manager.h"
#include "input.h"

INPUT_MODE Input::m_input_mode = INPUT_MODE::INPUT_MODE_KEYBOARD; // 初期入力モードはキーボード
BYTE Input::m_old_key_state[256];
BYTE Input::m_key_state[256];

// Controller static members
int Input::m_connected_device_handles[MAX_CONTROLLERS];
int Input::m_num_connected_controllers = 0;
JOY_SHOCK_STATE Input::m_joy_state[MAX_CONTROLLERS];
JOY_SHOCK_STATE Input::m_old_joy_state[MAX_CONTROLLERS];
IMU_STATE Input::m_imu_state[MAX_CONTROLLERS];
CONTROLLER_TYPE Input::m_controller_type[MAX_CONTROLLERS];
MOTION_STATE Input::m_motion_state[MAX_CONTROLLERS];
bool Input::m_jsl_active = false;
XMVECTOR Input::m_prevAngles{}; // 前回の角度（ジャイロ用）
bool Input::m_mouse_left_button[2] = { false,false };//0 最新 1 前回
bool Input::m_mouse_right_button[2] = { false,false };
XMFLOAT2 Input::m_mouse_position{}; // マウスの位置
XMFLOAT2 Input::m_mouse_old_position{}; // 前回のマウス位置


void Input::Init()
{
	memset(m_old_key_state, 0, 256);
	memset(m_key_state, 0, 256);

	// Initialize Controller related members
	memset(m_connected_device_handles, 0, sizeof(m_connected_device_handles));
	memset(m_joy_state, 0, sizeof(m_joy_state));
	memset(m_old_joy_state, 0, sizeof(m_old_joy_state));
	memset(m_imu_state, 0, sizeof(m_imu_state));

	m_num_connected_controllers = 0;
	m_jsl_active = false;

	// Initialize JoyShockLibrary
	JslConnectDevices(); // デバイスをスキャンして接続
	int current_num_devices = JslGetConnectedDeviceHandles(m_connected_device_handles, MAX_CONTROLLERS);
	m_num_connected_controllers = current_num_devices;

	if (m_num_connected_controllers > 0) {
		m_jsl_active = true;
		// printf("%d controller(s) found during Init.\n", m_num_connected_controllers);
		for (int i = 0; i < m_num_connected_controllers; ++i) {
			m_joy_state[i] = JslGetSimpleState(m_connected_device_handles[i]);
			m_old_joy_state[i] = m_joy_state[i]; // 最初は現在の状態と同じ
			m_imu_state[i] = JslGetIMUState(m_connected_device_handles[i]);
			m_motion_state[i] = JslGetMotionState(m_connected_device_handles[i]);
			// Fix: Cast the return value of JslGetControllerType to CONTROLLER_TYPE  
			switch (JslGetControllerType(m_connected_device_handles[i]))
			{
			case CONTROLLER_TYPE::S_DS4:
				m_controller_type[i] = CONTROLLER_TYPE::S_DS4;
				break;
			case CONTROLLER_TYPE::S_DS:
				m_controller_type[i] = CONTROLLER_TYPE::S_DS;
				break;
			case CONTROLLER_TYPE::N_SWITCH:
				m_controller_type[i] = CONTROLLER_TYPE::N_SWITCH;
				JslSetAutomaticCalibration(m_connected_device_handles[i], true);// 自動キャリブレーションを有効化
				break;
			default:
				m_controller_type[i] = CONTROLLER_TYPE::NONE;
				break;
			}
		}
		m_input_mode = INPUT_MODE::INPUT_MODE_CONTROLLER_KEYBOARD; // コントローラーが接続されている場合はコントローラーモードに設定
	}
	else {
		m_input_mode = INPUT_MODE::INPUT_MODE_KEYBOARD; // コントローラーがない場合はキーボードモードに設定
	}
}

void Input::Uninit()
{
	// JoyShockLibraryの終了処理
	JslDisconnectAndDisposeAll(); // 全てのデバイスを切断し、リソースを解放
	m_jsl_active = false;
	m_num_connected_controllers = 0;

}

void Input::Update()
{

	memcpy(m_old_key_state, m_key_state, 256);
	bool ch = GetKeyboardState(m_key_state);

	m_mouse_old_position = m_mouse_position; // 前回のマウス位置を保存
	POINT m_mouse_pos{};
	GetCursorPos(&m_mouse_pos); // マウスの位置を取得
	m_mouse_position = XMFLOAT2(static_cast<float>(m_mouse_pos.x), static_cast<float>(m_mouse_pos.y));

	// マウスの左ボタンと右ボタンの状態を更新
	m_mouse_left_button[1] = m_mouse_left_button[0]; // 前回の状態を保存
	m_mouse_right_button[1] = m_mouse_right_button[0]; // 前回の状態を保存
	m_mouse_left_button[0] = (GetKeyState(VK_LBUTTON) & 0x8000) != 0; // 左ボタンの状態を取得
	m_mouse_right_button[0] = (GetKeyState(VK_RBUTTON) & 0x8000) != 0; // 右ボタンの状態を取得

	
	if (m_input_mode != INPUT_MODE_CONTROLLER && m_input_mode != INPUT_MODE_CONTROLLER_KEYBOARD)return;

	if (m_num_connected_controllers > 0) {
		m_jsl_active = true;
		for (int i = 0; i < m_num_connected_controllers; ++i) {

			m_old_joy_state[i] = m_joy_state[i]; // 前フレームのJoyStateを保存
			m_joy_state[i] = JslGetSimpleState(m_connected_device_handles[i]);
			m_imu_state[i] = JslGetIMUState(m_connected_device_handles[i]);
			m_motion_state[i] = JslGetMotionState(m_connected_device_handles[i]);
		}
	}
	else {
		m_jsl_active = false;
	}
}

bool Input::GetKeyPress(BYTE KeyCode)
{
	return (m_key_state[KeyCode] & 0x80);
}

bool Input::GetKeyTrigger(BYTE KeyCode)
{
	return ((m_key_state[KeyCode] & 0x80) && !(m_old_key_state[KeyCode] & 0x80));
}

XMFLOAT2 Input::GetMousePosition() {
	return m_mouse_position;
}

XMFLOAT2 Input::GetMouseDelta() {
	return XMFLOAT2(m_mouse_position.x - m_mouse_old_position.x, m_mouse_position.y - m_mouse_old_position.y);
}

bool Input::GetMouseLeftButtonPress() {
	return (m_mouse_left_button[0] && m_mouse_left_button[1]);
} 

bool Input::GetMouseLeftButtonTrigger() {
	return (m_mouse_left_button[0] && !m_mouse_left_button[1]);
}

bool Input::GetMouseRightButtonPress() {
	return (m_mouse_right_button[0] && m_mouse_right_button[1]);
} 

bool Input::GetMouseRightButtonTrigger() {
	return (m_mouse_right_button[0] && !m_mouse_right_button[1]);
} 

bool Input::InitJoyShockLibrary() {
	// Update Controller
	JslConnectDevices(); // 新規接続や再接続を検出
	int current_num_devices = JslGetConnectedDeviceHandles(m_connected_device_handles, MAX_CONTROLLERS);
	if( current_num_devices <= 0) {
		m_jsl_active = false; // 接続されていない場合は無効化
		m_input_mode = INPUT_MODE::INPUT_MODE_KEYBOARD; // 入力モードをキーボードに戻す
		return false; // エラーではなく、単に接続されていない状態
	}

	// コントローラー数が変更された場合、タイプを再取得
	if (current_num_devices != m_num_connected_controllers) {
		// printf("Controller count changed: %d -> %d\n", m_num_connected_controllers, current_num_devices);
		for (int i = 0; i < current_num_devices; ++i) {
			// 新しいデバイスのタイプを取得 (既に存在するデバイスのタイプも上書きされるが、通常は変わらない)
			int type = JslGetControllerType(m_connected_device_handles[i]);
			switch (type)
			{
			case CONTROLLER_TYPE::S_DS4:
				m_controller_type[i] = CONTROLLER_TYPE::S_DS4;
				break;
			case CONTROLLER_TYPE::S_DS:
				m_controller_type[i] = CONTROLLER_TYPE::S_DS;
				break;
			case CONTROLLER_TYPE::N_SWITCH:
				m_controller_type[i] = CONTROLLER_TYPE::N_SWITCH;
				break;
			default:
				m_controller_type[i] = CONTROLLER_TYPE::NONE;
				break;
			}
		}
		// デバイス数が減った場合、古い情報が配列の後方に残るがアクセス範囲で制御
	}
	m_num_connected_controllers = current_num_devices; // 常に最新の接続数に更新
	m_input_mode = INPUT_MODE::INPUT_MODE_CONTROLLER_KEYBOARD; // コントローラーが接続されている場合はコントローラーモードに設定
	return true;
}

// --- Controller Common ---
int Input::GetNumConnectedControllers()
{
	return m_num_connected_controllers;
}

CONTROLLER_TYPE Input::GetControllerType(int controllerIndex)
{
	if (m_jsl_active && controllerIndex >= 0 && controllerIndex < m_num_connected_controllers) {
		return m_controller_type[controllerIndex];
	}
	return NONE; // 不明またはエラー
}

// --- Controller Buttons ---
JOY_SHOCK_STATE Input::GetJoyState(int controllerIndex)
{
	if (m_jsl_active && controllerIndex >= 0 && controllerIndex < m_num_connected_controllers) {
		return m_joy_state[controllerIndex];
	}
	JOY_SHOCK_STATE empty_state = { 0 };
	return empty_state;
}

bool Input::GetJoyButtonPress(int controllerIndex, int buttonOffset)
{
	if (!m_jsl_active || controllerIndex < 0 || controllerIndex >= m_num_connected_controllers) {
		return false;
	}
	return (m_joy_state[controllerIndex].buttons & buttonOffset);
}

bool Input::GetJoyButtonTrigger(int controllerIndex, int buttonOffset)
{
	if (!m_jsl_active || controllerIndex < 0 || controllerIndex >= m_num_connected_controllers) {
		return false;
	}
	bool current_press = (m_joy_state[controllerIndex].buttons & buttonOffset);
	bool old_press = (m_old_joy_state[controllerIndex].buttons & buttonOffset);
	return current_press && !old_press;
}

// --- Controller Sticks ---
bool Input::GetJoyStickLeft(int controllerIndex, float& x, float& y)
{
	x = 0.0f;
	y = 0.0f;
	if (!m_jsl_active || controllerIndex < 0 || controllerIndex >= m_num_connected_controllers) {
		return false;
	}
	x = m_joy_state[controllerIndex].stickLX;
	y = m_joy_state[controllerIndex].stickLY;
	return true;
}

bool Input::GetJoyStickRight(int controllerIndex, float& x, float& y)
{
	x = 0.0f;
	y = 0.0f;
	if (!m_jsl_active || controllerIndex < 0 || controllerIndex >= m_num_connected_controllers) {
		return false;
	}
	x = m_joy_state[controllerIndex].stickRX;
	y = m_joy_state[controllerIndex].stickRY;
	return true;
}

// --- Controller Triggers (Analog) ---
bool Input::GetJoyLTrigger(int controllerIndex, float& value)
{
	value = 0.0f;
	if (!m_jsl_active || controllerIndex < 0 || controllerIndex >= m_num_connected_controllers) {
		return false;
	}
	value = m_joy_state[controllerIndex].lTrigger;
	return true;
}

bool Input::GetJoyRTrigger(int controllerIndex, float& value)
{
	value = 0.0f;
	if (!m_jsl_active || controllerIndex < 0 || controllerIndex >= m_num_connected_controllers) {
		return false;
	}
	value = m_joy_state[controllerIndex].rTrigger;
	return true;
}

// --- Controller Gyro & Accelerometer ---
IMU_STATE Input::GetJoyIMUState(int controllerIndex)
{
	if (m_jsl_active && controllerIndex >= 0 && controllerIndex < m_num_connected_controllers) {
		return m_imu_state[controllerIndex];
	}
	IMU_STATE empty_state = { 0 };
	return empty_state;
}

bool Input::GetJoyGyro(int controllerIndex, XMFLOAT3& gyro)
{

	if (!m_jsl_active || controllerIndex < 0 || controllerIndex >= m_num_connected_controllers) {
		return false;
	}
	gyro.x = m_imu_state[controllerIndex].gyroX;
	gyro.y = m_imu_state[controllerIndex].gyroY;
	gyro.z = m_imu_state[controllerIndex].gyroZ;
	return true;
}

bool Input::GetJoyAccel(int controllerIndex, XMFLOAT3& accel)
{
	if (!m_jsl_active || controllerIndex < 0 || controllerIndex >= m_num_connected_controllers) {
		return false;
	}
	accel.x = m_imu_state[controllerIndex].accelX;
	accel.y = m_imu_state[controllerIndex].accelY;
	accel.z = m_imu_state[controllerIndex].accelZ;
	return true;
}

bool Input::GetEkfGyro(int controllerIndex, float& gyroX, float& gyroY, float& gyroZ) {
	return false;
}

XMVECTOR Input::CalculateGyroAngles() {
	if (!m_jsl_active) return XMVECTOR{ 0.0f, 0.0f, 0.0f, 0.0f };
	// ジャイロデータの各成分を抽出XMVECTOR gyroData
	float gx = m_imu_state->gyroX; // x軸のジャイロ
	float gy = m_imu_state->gyroY; // y軸のジャイロ
	float gz = m_imu_state->gyroZ; // z軸のジャイロ

	// 前回の角度（ロールとピッチ）を取得
	float prevRoll = XMVectorGetX(m_prevAngles); // 前回のロール角
	float prevPitch = XMVectorGetY(m_prevAngles); // 前回のピッチ角
	float prevYaw = XMVectorGetZ(m_prevAngles); // 前回のピッチ角

	// ジャイロデータを使った角度変化量を計算
	float droll = gx + sin(prevRoll) * tan(prevPitch) * gy + cos(prevRoll) * tan(prevPitch) * gz;
	float dpitch = cos(prevRoll) * gy - sin(prevRoll) * gz;
	float dyaw = sin(prevRoll) / cos(prevPitch) * gy + cos(prevRoll) / cos(prevPitch) * gz;

	auto dt = Manager::GetManager().GetDeltaTime(); // マネージャーからデルタタイムを取得

	// 新しい角度を計算
	float rollGyro = prevRoll + droll * dt;
	float pitchGyro = prevPitch + dpitch * dt;
	float yawGyro = prevYaw + dyaw * dt; // yawは前回のyawに加算

	// 結果を XMVECTOR で返す
	m_prevAngles = XMVectorSet(rollGyro, pitchGyro, yawGyro, 0.0f); // ZとW成分は0
	return m_prevAngles;
}

bool Input::GetJoyQuaternion(int controllerIndex, XMFLOAT4& quaternion)
{
	if (!m_jsl_active || controllerIndex < 0 || controllerIndex >= m_num_connected_controllers) {
		return false;
	}
	quaternion.x = m_motion_state[controllerIndex].quatX;
	quaternion.y = m_motion_state[controllerIndex].quatY;
	quaternion.z = m_motion_state[controllerIndex].quatZ;
	quaternion.w = m_motion_state[controllerIndex].quatW;
	return true;
}