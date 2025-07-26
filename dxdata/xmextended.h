/**
 * @file   XMExtended.h
 * @brief  DirectXMath �̊g���@�\��񋟂���w�b�_�[�t�@�C��
 * @author Taiju Kobayashi
 * @date   2024/12
 */

#pragma once
#ifndef _XMEXTENDED_H_
#define _XMEXTENDED_H_

#include <DirectXMath.h>
#include <cmath>



 /// @brief 2�� XMFLOAT2 �x�N�g���̃h�b�g�ς��v�Z���܂�
 /// @param v1 1�Ԗڂ̃x�N�g��
 /// @param v2 2�Ԗڂ̃x�N�g��
 /// @return �h�b�g�ς̌���
static float XMFloat2Dot(XMFLOAT2& In1, XMFLOAT2& In2) {
	float Out;
	XMStoreFloat(&Out, XMVector2Dot(XMLoadFloat2(&In1), XMLoadFloat2(&In2)));
	return Out;
}

/// @brief 2�� XMFLOAT3 �x�N�g���̃h�b�g�ς��v�Z���܂�
/// @param v1 1�Ԗڂ̃x�N�g��
/// @param v2 2�Ԗڂ̃x�N�g��
/// @return �h�b�g�ς̌���
static float XMFloat3Dot(XMFLOAT3& In1, XMFLOAT3& In2) {
	float Out;
	XMStoreFloat(&Out, XMVector3Dot(XMLoadFloat3(&In1), XMLoadFloat3(&In2)));
	return Out;
}

/// @brief 2�� XMFLOAT3 �x�N�g���̃N���X�ς��v�Z���܂�
/// @param v1 1�Ԗڂ̃x�N�g��
/// @param v2 2�Ԗڂ̃x�N�g��
/// @return �N���X�ς̌���
static XMFLOAT3 XMFloat3Cross(XMFLOAT3& In1, XMFLOAT3& In2) {
	XMFLOAT3 Out;
	XMStoreFloat3(&Out, XMVector3Cross(XMLoadFloat3(&In1), XMLoadFloat3(&In2)));
	return Out;
}

/// @brief XMFLOAT3���K��
/// @param In ���K������x�N�g��
/// @return ���K���̌���
static XMFLOAT3 XMFloat3Normalize(XMFLOAT3& In) {
	XMFLOAT3 Out;
	XMStoreFloat3(&Out, XMVector3Normalize(XMLoadFloat3(&In)));
	return Out;
};

/// @brief XMFLOAT4���K��
/// @param In ���K������x�N�g��
/// @return ���K���̌���
static XMFLOAT4 XMFloat4Normalize(XMFLOAT4& In) {
	XMFLOAT4 Out;
	XMStoreFloat4(&Out, XMVector4Normalize(XMLoadFloat4(&In)));
	return Out;
};

/// @brief XMFLOAT3 �x�N�g���̒������v�Z
/// @param v �x�N�g��
/// @return �x�N�g���̒���
static float XMFloat3Magnitude(const XMFLOAT3& In) {
	return XMVectorGetX(XMVector3Length(XMLoadFloat3(&In)));
}

/// @brief 2�� XMFLOAT3�̉��Z����
/// @param v1 1�Ԗڂ�XMFLOAT3 
/// @param v2 1�Ԗڂ�XMFLOAT3
/// @return ���Z����
static XMFLOAT3 XMFloat3Add(const XMFLOAT3& v1, const XMFLOAT3& v2) {
	XMFLOAT3 Out;
	XMStoreFloat3(&Out, XMVectorAdd(XMLoadFloat3(&v1), XMLoadFloat3(&v2)));
	return Out;
}

/// @brief 2�� XMFLOAT4�̉��Z����
/// @param v1 1�Ԗڂ�XMFLOAT4
/// @param v2 1�Ԗڂ�XMFLOAT4
/// @return ���Z����
static XMFLOAT4 XMFloat4Add(const XMFLOAT4& v1, const XMFLOAT4& v2) {
	XMFLOAT4 Out;
	XMStoreFloat4(&Out, XMVectorAdd(XMLoadFloat4(&v1), XMLoadFloat4(&v2)));
	return Out;
}

/// @brief 2�� XMFLOAT3�̌��Z����
/// @param v1 1�Ԗڂ�XMFLOAT3 ���������
/// @param v2 1�Ԗڂ�XMFLOAT3 ������
/// @return ���Z�̌���
static XMFLOAT3 XMFloat3Subtract(const XMFLOAT3& v1, const XMFLOAT3& v2) {
	XMFLOAT3 Out;
	XMStoreFloat3(&Out, XMVectorSubtract(XMLoadFloat3(&v1), XMLoadFloat3(&v2)));
	return Out;
}

/// @brief 2�� XMFLOAT4�̌��Z����
/// @param v1 1�Ԗڂ�XMFLOAT4 ���������
/// @param v2 1�Ԗڂ�XMFLOAT4 ������
/// @return ���Z�̌���
static XMFLOAT4 XMFloat4Subtract(const XMFLOAT4& v1, const XMFLOAT4& v2) {
	XMFLOAT4 Out;
	XMStoreFloat4(&Out, XMVectorSubtract(XMLoadFloat4(&v1), XMLoadFloat4(&v2)));
	return Out;
}


#endif // !_XMEXTENDED_H_

