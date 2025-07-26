/**
 * @file   titlescene.h
 * @brief  ゲームのタイトルシーンのヘッダー
 * @author Taiju Kobayashi
 * @date   2025/01
 */
#pragma once
#include "scene.h"

 /// @brief　ゲームのタイトルシーンのクラス
class TitleScene :
    public Scene
{

    void Init()override;
    void Update()override;
private:
    bool m_flag = false;

    // サンプリング周期
    float dt = 0.1f;
    // 状態変数
    XMVECTOR xEst = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f); // Roll, Pitch, 無用な2成分は0
    XMMATRIX PEst = XMMatrixIdentity() * (0.0174f * dt * dt);

    // ノイズ共分散行列
    XMMATRIX Q = XMMatrixIdentity() * (0.0174f * dt * dt);  // プロセスノイズ
    XMMATRIX R = XMMatrixIdentity() * (dt * dt);            // 観測ノイズ

    // 観測行列
    XMMATRIX H = XMMatrixIdentity();
};

