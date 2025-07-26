/**
 * @file   resultscene.h
 * @brief  �Q�[���̃��U���g��ʂ̃w�b�_�[
 * @author Taiju Kobayashi
 * @date   2025/01
 */
#pragma once
#include "scene.h"

 /// @brief�@�Q�[���̃��U���g�V�[���̃N���X
class ResultScene :
    public Scene
{
    void Init()override;
    void Update()override;
private:
    bool m_flag{};
    class  Score* m_score = nullptr;
};

