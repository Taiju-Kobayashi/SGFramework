/**
 * @file   choicescene.h
 * @brief  ゲームの選択シーンのヘッダー
 * @author Taiju Kobayashi
 * @date   2025/04
 */
#pragma once
#include "scene.h"

 /// @brief　ゲームのタイトルシーンのクラス
class ChoiceScene :
    public Scene
{

    void Init()override;
    void Update()override;
private:
    bool m_flag = false;
};

