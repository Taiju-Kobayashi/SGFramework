/**
 * @file   loadscene.h
 * @brief  読込画面シーンのヘッダー
 * @author Taiju Kobayashi
 * @date   2025/01
 */
#pragma once
#include "scene.h"


 /// @brief　読込画面シーンクラス
class LoadingScene :
    public Scene
{
    void Init()override;
    void Update()override;

private:
    bool m_flag = false;
};

