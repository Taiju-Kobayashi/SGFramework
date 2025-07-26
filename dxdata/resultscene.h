/**
 * @file   resultscene.h
 * @brief  ゲームのリザルト画面のヘッダー
 * @author Taiju Kobayashi
 * @date   2025/01
 */
#pragma once
#include "scene.h"

 /// @brief　ゲームのリザルトシーンのクラス
class ResultScene :
    public Scene
{
    void Init()override;
    void Update()override;
private:
    bool m_flag{};
    class  Score* m_score = nullptr;
};

