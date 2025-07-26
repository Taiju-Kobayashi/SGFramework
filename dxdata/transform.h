/**
 * @file   transform.h
 * @brief  �I�u�W�F�N�g�̕ό`�i�ړ��A��]�A�g��k���j���Ǘ����邽�߂̃N���X
 * @author Taiju Kobayashi
 * @date   2024/12�@
 */
#pragma once
#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_


#include "component.h"
#include <DirectXMath.h>
using namespace DirectX;

/// @brief�@�I�u�W�F�N�g�̕ό`�i�ړ��A��]�A�g��k���j���Ǘ����邽�߂̃N���X
class Transform : public Component
{

public:
	using Component::Component;
	~Transform() {};

	void Init()override;

	//	���݂̍��W���擾
	const XMFLOAT3& GetPosition();
	//	���W����ݒ肷��
	const XMFLOAT3& SetPosition(XMFLOAT3 pos);
	//	���W����ݒ肷��
	const XMFLOAT3& SetPosition(float x, float y, float  z);

	//	���݂̉�]��Ԃ��擾
	const XMFLOAT3& GetRotation();
	//	��]����ݒ肷��
	const XMFLOAT3& SetRotation(XMFLOAT3 rot);
	//	��]����ݒ肷��
	const XMFLOAT3& SetRotation(float x, float y, float  z);

	//	���݂̃X�P�[�������擾
	const XMFLOAT3& GetScale();
	//	�X�P�[������ݒ�
	const XMFLOAT3& SetScale(XMFLOAT3 scale);
	//	�X�P�[������ݒ�
	const XMFLOAT3& SetScale(float x, float y, float  z);

	// �X�P�[���s���ݒ�
	const void SetScaleMatrix(XMMATRIX& mat);
	// ��]�s���ݒ�
	const void SetRotateMatrix(XMMATRIX& mat);
	// �ړ��s���ݒ�
	const void SetTransMatrix(XMMATRIX& mat);
	// �X�P�[���s����擾
	const XMMATRIX& GetScaleMatrix();
	// ��]�s����擾
	const XMMATRIX& GetRotateMatrix();
	// �ړ��s����擾
	const XMMATRIX& GetTransMatrix();
	// ���[���h�s����擾
	const XMMATRIX GetWorldMatrix();

	// �N�H�[�^�j�I����ݒ�
	const void SetQuaternion(XMFLOAT4 quat);
	// �N�H�[�^�j�I�����擾
	const XMVECTOR GetQuaternion();


	//	Z��������vector�擾
	XMFLOAT3 GetForward();
	XMVECTOR GetVectorForward();
	//	X��������vector�擾
	XMFLOAT3 GetRight();
	XMVECTOR GetVectorRight();
	//	Y��������vector�擾
	XMFLOAT3 GetUp();
	XMVECTOR GetVectorUp();

private:
	XMFLOAT3 m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT4 m_quaternion = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);	//�N�H�[�^�j�I���������@w��1.0�ŏ����� ����]��Ԃ�\��
	XMFLOAT3 m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	XMMATRIX m_scale_matrix{};
	XMMATRIX m_rotate_matrix{};
	XMMATRIX m_trans_matrix{};

};
#endif // !_TRANSFORM_H_
