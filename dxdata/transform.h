/**
 * @file   transform.h
 * @brief  オブジェクトの変形（移動、回転、拡大縮小）を管理するためのクラス
 * @author Taiju Kobayashi
 * @date   2024/12　
 */
#pragma once
#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_


#include "component.h"
#include <DirectXMath.h>
using namespace DirectX;

/// @brief　オブジェクトの変形（移動、回転、拡大縮小）を管理するためのクラス
class Transform : public Component
{

public:
	using Component::Component;
	~Transform() {};

	void Init()override;

	//	現在の座標を取得
	const XMFLOAT3& GetPosition();
	//	座標情報を設定する
	const XMFLOAT3& SetPosition(XMFLOAT3 pos);
	//	座標情報を設定する
	const XMFLOAT3& SetPosition(float x, float y, float  z);

	//	現在の回転状態を取得
	const XMFLOAT3& GetRotation();
	//	回転情報を設定する
	const XMFLOAT3& SetRotation(XMFLOAT3 rot);
	//	回転情報を設定する
	const XMFLOAT3& SetRotation(float x, float y, float  z);

	//	現在のスケール情報を取得
	const XMFLOAT3& GetScale();
	//	スケール情報を設定
	const XMFLOAT3& SetScale(XMFLOAT3 scale);
	//	スケール情報を設定
	const XMFLOAT3& SetScale(float x, float y, float  z);

	// スケール行列を設定
	const void SetScaleMatrix(XMMATRIX& mat);
	// 回転行列を設定
	const void SetRotateMatrix(XMMATRIX& mat);
	// 移動行列を設定
	const void SetTransMatrix(XMMATRIX& mat);
	// スケール行列を取得
	const XMMATRIX& GetScaleMatrix();
	// 回転行列を取得
	const XMMATRIX& GetRotateMatrix();
	// 移動行列を取得
	const XMMATRIX& GetTransMatrix();
	// ワールド行列を取得
	const XMMATRIX GetWorldMatrix();

	// クォータニオンを設定
	const void SetQuaternion(XMFLOAT4 quat);
	// クォータニオンを取得
	const XMVECTOR GetQuaternion();


	//	Z軸正方向vector取得
	XMFLOAT3 GetForward();
	XMVECTOR GetVectorForward();
	//	X軸正方向vector取得
	XMFLOAT3 GetRight();
	XMVECTOR GetVectorRight();
	//	Y軸正方向vector取得
	XMFLOAT3 GetUp();
	XMVECTOR GetVectorUp();

private:
	XMFLOAT3 m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT4 m_quaternion = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);	//クォータニオン初期化　wを1.0で初期化 無回転状態を表す
	XMFLOAT3 m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	XMMATRIX m_scale_matrix{};
	XMMATRIX m_rotate_matrix{};
	XMMATRIX m_trans_matrix{};

};
#endif // !_TRANSFORM_H_
