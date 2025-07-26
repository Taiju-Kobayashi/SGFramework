/**
 * @file   camera.h
 * @brief  �J��������ێ�����N���X�̃w�b�_
 * @author Taiju Kobayashi
 * @date   2024/07
 */
#pragma once
#include "gameObject.h"

 /// @brief �J��������ێ�����N���X
class Camera : public GameObject
{
public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;



	void SetDistance(float distance);
	void AddDistance(float deltaDistance);

	void ApplyRotationDelta(float deltaPitch, float deltaYaw, float deltaRoll); // �I�C���[�p�̕ω��ʂ��N�H�[�^�j�I���ɕϊ����ēK�p
	void SetRotationDelta(float deltaPitch, float deltaYaw, float deltaRoll); // �I�C���[�p�̕ω��ʂ��N�H�[�^�j�I���ɕϊ����ēK�p
	void ApplyRotationQuaternion(const XMFLOAT4& deltaQuaternion); // ���ڃN�H�[�^�j�I���ŉ�]��ǉ�


	//XMFloat4x4 �� XMMATRIX�ɕϊ����鏈��
	XMMATRIX GetViewMatrix() { return  XMLoadFloat4x4(&m_viewmatrix); }
	void SetPosition(XMFLOAT3 camPos, XMFLOAT3 Target);
private:
	XMFLOAT2 m_distance_range{0.0f,100.0f}; // �J�����̋����͈� (�ŏ�, �ő�)
	XMFLOAT3 m_target{};
	XMFLOAT4X4 m_viewmatrix{};

	// �N�H�[�^�j�I���ɂ���]����p
	XMFLOAT4 m_cameraRotationQuaternion; // �J�����̌��݂̉�]�N�H�[�^�j�I��

	float m_cameraDistance; // ��ʑ̂���̋���
};

