/**
 * @file   slingshot.h
 * @brief  �΃I�u�W�F�N�g����,���ˏ������s���N���X�̃w�b�_
 * @author Taiju Kobayashi
 * @date   2024/12
 */
#pragma once
#include "gameObject.h"
#include "component.h"

/// @brief  �΃I�u�W�F�N�g����,���ˏ������s���N���X
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

	//�Γ��������p�ϐ�
	bool m_iskey_antouchd = false;
	float m_pressd_time = 0.0f;
	const float m_maxpress_time = 2.0f;//�`���[�W�̍ő剟������

	const float MAX_POWER_VALUE = 4.0f;
	const float GAUGE_DECREASE_PER_SEC = 1.0f;

	enum class SwingState
	{
		Idle,           // �ҋ@��
		ChargingThrow,       // �͂𗭂߂Ă���
		ChargingGauge,  // �Q�[�W�̂��߂̃`���[�W��
		Released,       // �������s�ς݁i���̃t���[���Łj
	};
	 SwingState m_swingState = SwingState::Idle;
	 float m_accumulatedPower = 0.0f;
	 int m_powerSampleCount = 0;
	 float m_currentMagnitude = 0.0f;
	 float m_previousMagnitude = 0.0f;
	 float m_gaugeValue = 0.0f;

};