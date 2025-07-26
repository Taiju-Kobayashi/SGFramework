#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "modelRenderer.h"
#include "audio.h"
#include "scene.h"
#include "terrain.h"
#include "camera.h"
#include "input.h"
#include "slingshot.h"
#include "splasheEmitter.h"
#include "resultInstance.h"
#include "water.h"
#include "stone.h"
#include "shadowvolume.h"
#include "transform.h"
#include "verticalGauge.h"
#include "xmextended.h"

void SlingShot::Init() {

	Scene& scene = GetParentScene();

	m_stone = scene.AddGameObject<Stone>(1);
	m_velocity.z = 50.0f;
	scene.AddGameObject<ShadowVolume>(1);
	if (!m_gauge)m_gauge = scene.GetGameObject<VerticalGauge>();
}

void SlingShot::UnInit() {
}
int t_id = 0;
int g_id = 0;
bool g_sw = false;
bool ig_sw = false;
float t_mag = 0.0f;
float g_mag;
float g_threshold = 0.5f;
float g_old_mag = 0.0f;
float g_now_mag = 0.0f;

void SlingShot::Update() {
	if (((Stone*)m_stone)->GetReleaseFlag())return;

	auto& manager = Manager::GetManager();
	auto dt = manager.GetDeltaTime();
	XMFLOAT3 Right = m_transform->GetRight();

	XMFLOAT3 Forward = m_transform->GetForward();
	Transform* trf = m_stone->GetTransform();
	auto rot = trf->GetRotation();
	if (Input::GetKeyPress('W')) {
		trf->SetRotation(XMFLOAT3(rot.x + 1.0f * dt, rot.y, rot.z));

	}
	else if (Input::GetKeyPress('S')) {
		trf->SetRotation(XMFLOAT3(rot.x - 1.0f * dt, rot.y, rot.z));
	}

	rot = trf->GetRotation();
	if (Input::GetKeyPress('A')) {
		m_yaxis_rotation_speed += 0.01f;
		if (m_yaxis_rotation_speed > 30.0f) {
			m_yaxis_rotation_speed = 30.0f;
		}
	}
	else if (Input::GetKeyPress('D')) {
		m_yaxis_rotation_speed -= 0.01f;
		if (m_yaxis_rotation_speed < -30.0f) {
			m_yaxis_rotation_speed = -30.0f;
		}
	}
	XMFLOAT2 joystick{};
	if (Input::GetJoyStickRight(0, joystick.x, joystick.y)) {
		trf->SetRotation(XMFLOAT3(rot.x + joystick.y * dt, rot.y, rot.z));
		if (joystick.x < 0.0f) {
			m_yaxis_rotation_speed += (0.01f * joystick.x);
			if (m_yaxis_rotation_speed > 30.0f) {
				m_yaxis_rotation_speed = 30.0f;
			}
		}
		else if (0.0f < joystick.x) {
			m_yaxis_rotation_speed += (0.01f * joystick.x);
			if (m_yaxis_rotation_speed < -30.0f) {
				m_yaxis_rotation_speed = -30.0f;
			}
		}
	}

	m_velocity.x = m_yaxis_rotation_speed / 5;

	rot = trf->GetRotation();
	trf->SetRotation(XMFLOAT3(rot.x, rot.y + m_yaxis_rotation_speed * dt, rot.z));

	//力ゲージUI制御
	//投擲時スピード設定
	float deltaMagnitude = 0.0f;
	switch (Input::GetInputMode())
	{
	case INPUT_MODE::INPUT_MODE_CONTROLLER:		//ジョイコンの加速度センサーを使って、スリングショットの力を調整する
	case INPUT_MODE::INPUT_MODE_CONTROLLER_KEYBOARD:
		// 加速度を取得し、大きさを計算
		XMFLOAT3 accel;
		Input::GetJoyAccel(0, accel);
		m_currentMagnitude = XMFloat3Magnitude(accel);
		deltaMagnitude = m_currentMagnitude - m_previousMagnitude;
		// Rボタンの状態を取得
		if (Input::GetJoyButtonPress(0, JSMASK_R))
		{
			// 振りを検知したらチャージ開始
			if (deltaMagnitude > g_threshold)
			{
				m_swingState = SwingState::ChargingThrow;
				m_accumulatedPower += deltaMagnitude;
				m_powerSampleCount++;
			}
			// 振りが終わり、チャージ状態だったら投擲実行
			else if (m_swingState == SwingState::ChargingThrow)
			{
				float throwPower = 0.0f;
				if (m_powerSampleCount > 0)
				{
					throwPower = std::min((m_accumulatedPower / m_powerSampleCount), MAX_POWER_VALUE) / MAX_POWER_VALUE;
				}

				m_gauge->SetGaugeValue(throwPower);
				m_velocity.x = m_yaxis_rotation_speed * throwPower;
				m_velocity.z = m_velocity.z * throwPower;
				auto ins = manager.GetGameInstance<ResultInstance>();
				ins->m_stoneforce = m_velocity;
				ins->m_rotforce = XMFLOAT3(0.0f, m_yaxis_rotation_speed, 0.0f);
				ins->m_rotate = trf->GetRotation();
				((Stone*)m_stone)->AddForce(m_velocity);
				((Stone*)m_stone)->AddRotForce(XMFLOAT3(0.0f, m_yaxis_rotation_speed, 0.0f));
				((Stone*)m_stone)->SetReleaseFlag(true);

				m_swingState = SwingState::Released;
			}
		}
		else
		{
			// 直前まで投擲動作をしていた場合は、待機状態に戻す
			if (m_swingState == SwingState::Released || m_swingState == SwingState::ChargingThrow)
			{
				m_swingState = SwingState::Idle;
				m_accumulatedPower = 0.0f;
				m_powerSampleCount = 0;
			}

			// 振りを検知したら「ゲージチャージ」状態へ
			if (deltaMagnitude > g_threshold)
			{
				if (m_swingState != SwingState::ChargingGauge)
				{
					m_swingState = SwingState::ChargingGauge;
					m_accumulatedPower = 0.0f;
					m_powerSampleCount = 0;
				}
				m_accumulatedPower += deltaMagnitude;
				m_powerSampleCount++;
			}
			else // 振りが収まった、または振っていない場合
			{
				// ゲージチャージが終わった瞬間
				if (m_swingState == SwingState::ChargingGauge)
				{
					if (m_powerSampleCount > 0)
					{
						// 計算した力をゲージ値として確定
						m_gaugeValue = std::min((m_accumulatedPower / m_powerSampleCount), MAX_POWER_VALUE) / MAX_POWER_VALUE;
					}
					// 待機状態に戻す
					m_swingState = SwingState::Idle;
				}
				// 完全な待機状態なら、ゲージを自然減少させる
				else if (m_swingState == SwingState::Idle)
				{
					m_gaugeValue -= GAUGE_DECREASE_PER_SEC * dt;
					m_gaugeValue = std::max(m_gaugeValue, 0.0f);
				}
			}
			// 最終的なゲージ値をUIに反映
			m_gauge->SetGaugeValue(m_gaugeValue);
		}
		m_previousMagnitude = m_currentMagnitude;
		break;
	default:	//Rキー押下でゲージが溜まる方式
		if (Input::GetKeyPress('R') || Input::GetJoyButtonPress(0, JSMASK_R)) {
			if (!m_iskey_antouchd) {
				m_iskey_antouchd = true;
				m_pressd_time = manager.GetGameTime();
			}
			else
			{
				float nowTime = manager.GetGameTime();
				nowTime = std::clamp(nowTime - m_pressd_time, 0.0f, m_maxpress_time);
				m_gauge->SetGaugeValue(nowTime / m_maxpress_time);
			}
		}
		else if (m_iskey_antouchd)
		{
			float releaseTime = manager.GetGameTime();
			float powerLeng = std::clamp(releaseTime - m_pressd_time, 0.0f, m_maxpress_time);
			m_gauge->SetGaugeValue(powerLeng / m_maxpress_time);
			m_velocity.x = m_yaxis_rotation_speed * (powerLeng / m_maxpress_time);
			m_velocity.z = m_velocity.z * (powerLeng / m_maxpress_time);
			auto ins = manager.GetGameInstance<ResultInstance>();
			ins->m_stoneforce = m_velocity;
			ins->m_rotforce = XMFLOAT3(0.0f, m_yaxis_rotation_speed, 0.0f);
			ins->m_rotate = trf->GetRotation();
			((Stone*)m_stone)->AddForce(m_velocity);
			((Stone*)m_stone)->AddRotForce(XMFLOAT3(0.0f, m_yaxis_rotation_speed, 0.0f));
			((Stone*)m_stone)->SetReleaseFlag(true);
		}
		break;
	}

}

void SlingShot::Draw() {

}