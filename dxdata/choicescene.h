/**
 * @file   choicescene.h
 * @brief  �Q�[���̑I���V�[���̃w�b�_�[
 * @author Taiju Kobayashi
 * @date   2025/04
 */
#pragma once
#include "scene.h"

 /// @brief�@�Q�[���̃^�C�g���V�[���̃N���X
class ChoiceScene :
    public Scene
{

    void Init()override;
    void Update()override;
private:
    bool m_flag = false;
};

