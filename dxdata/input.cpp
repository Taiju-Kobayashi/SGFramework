
#include "main.h"
#include "manager.h"
#include "input.h"

INPUT_MODE Input::m_input_mode = INPUT_MODE::INPUT_MODE_KEYBOARD; // �������̓��[�h�̓L�[�{�[�h
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
XMVECTOR Input::m_prevAngles{}; // �O��̊p�x�i�W���C���p�j
bool Input::m_mouse_left_button[2] = { false,false };//0 �ŐV 1 �O��
bool Input::m_mouse_right_button[2] = { false,false };
XMFLOAT2 Input::m_mouse_position{}; // �}�E�X�̈ʒu
XMFLOAT2 Input::m_mouse_old_position{}; // �O��̃}�E�X�ʒu


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
	JslConnectDevices(); // �f�o�C�X���X�L�������Đڑ�
	int current_num_devices = JslGetConnectedDeviceHandles(m_connected_device_handles, MAX_CONTROLLERS);
	m_num_connected_controllers = current_num_devices;

	if (m_num_connected_controllers > 0) {
		m_jsl_active = true;
		// printf("%d controller(s) found during Init.\n", m_num_connected_controllers);
		for (int i = 0; i < m_num_connected_controllers; ++i) {
			m_joy_state[i] = JslGetSimpleState(m_connected_device_handles[i]);
			m_old_joy_state[i] = m_joy_state[i]; // �ŏ��͌��݂̏�ԂƓ���
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
				JslSetAutomaticCalibration(m_connected_device_handles[i], true);// �����L�����u���[�V������L����
				break;
			default:
				m_controller_type[i] = CONTROLLER_TYPE::NONE;
				break;
			}
		}
		m_input_mode = INPUT_MODE::INPUT_MODE_CONTROLLER_KEYBOARD; // �R���g���[���[���ڑ�����Ă���ꍇ�̓R���g���[���[���[�h�ɐݒ�
	}
	else {
		m_input_mode = INPUT_MODE::INPUT_MODE_KEYBOARD; // �R���g���[���[���Ȃ��ꍇ�̓L�[�{�[�h���[�h�ɐݒ�
	}
}

void Input::Uninit()
{
	// JoyShockLibrary�̏I������
	JslDisconnectAndDisposeAll(); // �S�Ẵf�o�C�X��ؒf���A���\�[�X�����
	m_jsl_active = false;
	m_num_connected_controllers = 0;

}

void Input::Update()
{

	memcpy(m_old_key_state, m_key_state, 256);
	bool ch = GetKeyboardState(m_key_state);

	m_mouse_old_position = m_mouse_position; // �O��̃}�E�X�ʒu��ۑ�
	POINT m_mouse_pos{};
	GetCursorPos(&m_mouse_pos); // �}�E�X�̈ʒu���擾
	m_mouse_position = XMFLOAT2(static_cast<float>(m_mouse_pos.x), static_cast<float>(m_mouse_pos.y));

	// �}�E�X�̍��{�^���ƉE�{�^���̏�Ԃ��X�V
	m_mouse_left_button[1] = m_mouse_left_button[0]; // �O��̏�Ԃ�ۑ�
	m_mouse_right_button[1] = m_mouse_right_button[0]; // �O��̏�Ԃ�ۑ�
	m_mouse_left_button[0] = (GetKeyState(VK_LBUTTON) & 0x8000) != 0; // ���{�^���̏�Ԃ��擾
	m_mouse_right_button[0] = (GetKeyState(VK_RBUTTON) & 0x8000) != 0; // �E�{�^���̏�Ԃ��擾

	
	if (m_input_mode != INPUT_MODE_CONTROLLER && m_input_mode != INPUT_MODE_CONTROLLER_KEYBOARD)return;

	if (m_num_connected_controllers > 0) {
		m_jsl_active = true;
		for (int i = 0; i < m_num_connected_controllers; ++i) {

			m_old_joy_state[i] = m_joy_state[i]; // �O�t���[����JoyState��ۑ�
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
	JslConnectDevices(); // �V�K�ڑ���Đڑ������o
	int current_num_devices = JslGetConnectedDeviceHandles(m_connected_device_handles, MAX_CONTROLLERS);
	if( current_num_devices <= 0) {
		m_jsl_active = false; // �ڑ�����Ă��Ȃ��ꍇ�͖�����
		m_input_mode = INPUT_MODE::INPUT_MODE_KEYBOARD; // ���̓��[�h���L�[�{�[�h�ɖ߂�
		return false; // �G���[�ł͂Ȃ��A�P�ɐڑ�����Ă��Ȃ����
	}

	// �R���g���[���[�����ύX���ꂽ�ꍇ�A�^�C�v���Ď擾
	if (current_num_devices != m_num_connected_controllers) {
		// printf("Controller count changed: %d -> %d\n", m_num_connected_controllers, current_num_devices);
		for (int i = 0; i < current_num_devices; ++i) {
			// �V�����f�o�C�X�̃^�C�v���擾 (���ɑ��݂���f�o�C�X�̃^�C�v���㏑������邪�A�ʏ�͕ς��Ȃ�)
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
		// �f�o�C�X�����������ꍇ�A�Â���񂪔z��̌���Ɏc�邪�A�N�Z�X�͈͂Ő���
	}
	m_num_connected_controllers = current_num_devices; // ��ɍŐV�̐ڑ����ɍX�V
	m_input_mode = INPUT_MODE::INPUT_MODE_CONTROLLER_KEYBOARD; // �R���g���[���[���ڑ�����Ă���ꍇ�̓R���g���[���[���[�h�ɐݒ�
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
	return NONE; // �s���܂��̓G���[
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
	// �W���C���f�[�^�̊e�����𒊏oXMVECTOR gyroData
	float gx = m_imu_state->gyroX; // x���̃W���C��
	float gy = m_imu_state->gyroY; // y���̃W���C��
	float gz = m_imu_state->gyroZ; // z���̃W���C��

	// �O��̊p�x�i���[���ƃs�b�`�j���擾
	float prevRoll = XMVectorGetX(m_prevAngles); // �O��̃��[���p
	float prevPitch = XMVectorGetY(m_prevAngles); // �O��̃s�b�`�p
	float prevYaw = XMVectorGetZ(m_prevAngles); // �O��̃s�b�`�p

	// �W���C���f�[�^���g�����p�x�ω��ʂ��v�Z
	float droll = gx + sin(prevRoll) * tan(prevPitch) * gy + cos(prevRoll) * tan(prevPitch) * gz;
	float dpitch = cos(prevRoll) * gy - sin(prevRoll) * gz;
	float dyaw = sin(prevRoll) / cos(prevPitch) * gy + cos(prevRoll) / cos(prevPitch) * gz;

	auto dt = Manager::GetManager().GetDeltaTime(); // �}�l�[�W���[����f���^�^�C�����擾

	// �V�����p�x���v�Z
	float rollGyro = prevRoll + droll * dt;
	float pitchGyro = prevPitch + dpitch * dt;
	float yawGyro = prevYaw + dyaw * dt; // yaw�͑O���yaw�ɉ��Z

	// ���ʂ� XMVECTOR �ŕԂ�
	m_prevAngles = XMVectorSet(rollGyro, pitchGyro, yawGyro, 0.0f); // Z��W������0
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