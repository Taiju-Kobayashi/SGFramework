/**
 * @file   camera.h
 * @brief  カメラ情報を保持するクラスのヘッダ
 * @author Taiju Kobayashi
 * @date   2024/07
 */
#pragma once
#include "gameObject.h"

 /// @brief カメラ情報を保持するクラス
class Camera : public GameObject
{
public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;



	void SetDistance(float distance);
	void AddDistance(float deltaDistance);

	void ApplyRotationDelta(float deltaPitch, float deltaYaw, float deltaRoll); // オイラー角の変化量をクォータニオンに変換して適用
	void SetRotationDelta(float deltaPitch, float deltaYaw, float deltaRoll); // オイラー角の変化量をクォータニオンに変換して適用
	void ApplyRotationQuaternion(const XMFLOAT4& deltaQuaternion); // 直接クォータニオンで回転を追加


	//XMFloat4x4 を XMMATRIXに変換する処理
	XMMATRIX GetViewMatrix() { return  XMLoadFloat4x4(&m_viewmatrix); }
	void SetPosition(XMFLOAT3 camPos, XMFLOAT3 Target);
private:
	XMFLOAT2 m_distance_range{0.0f,100.0f}; // カメラの距離範囲 (最小, 最大)
	XMFLOAT3 m_target{};
	XMFLOAT4X4 m_viewmatrix{};

	// クォータニオンによる回転制御用
	XMFLOAT4 m_cameraRotationQuaternion; // カメラの現在の回転クォータニオン

	float m_cameraDistance; // 被写体からの距離
};

