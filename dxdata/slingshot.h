/**
 * @file   slingshot.h
 * @brief  石オブジェクト生成,発射処理を行うクラスのヘッダ
 * @author Taiju Kobayashi
 * @date   2024/12
 */
#pragma once
#include "gameObject.h"
#include "component.h"

/// @brief  石オブジェクト生成,発射処理を行うクラス
class SlingShot :public GameObject
{
	
public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;

private:
	XMFLOAT3 m_old_position{};
	XMFLOAT3 m_velocity{};
	float	m_mass = 1.0f;
	float m_reboundrate = 1.0f;

	GameObject* m_stone = nullptr;
	float m_yaxis_rotation_speed = 0;
	bool m_is_release = false;

	class VerticalGauge* m_gauge = nullptr;

	//石投げ処理用変数
	bool m_iskey_antouchd = false;
	float m_pressd_time = 0.0f;
	const float m_maxpress_time = 2.0f;//チャージの最大押し時間

	const float MAX_POWER_VALUE = 4.0f;
	const float GAUGE_DECREASE_PER_SEC = 1.0f;

	enum class SwingState
	{
		Idle,           // 待機中
		ChargingThrow,       // 力を溜めている
		ChargingGauge,  // ゲージのためのチャージ中
		Released,       // 投擲実行済み（このフレームで）
	};
	 SwingState m_swingState = SwingState::Idle;
	 float m_accumulatedPower = 0.0f;
	 int m_powerSampleCount = 0;
	 float m_currentMagnitude = 0.0f;
	 float m_previousMagnitude = 0.0f;
	 float m_gaugeValue = 0.0f;

};