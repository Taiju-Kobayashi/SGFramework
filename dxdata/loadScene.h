/**
 * @file   loadscene.h
 * @brief  �Ǎ���ʃV�[���̃w�b�_�[
 * @author Taiju Kobayashi
 * @date   2025/01
 */
#pragma once
#include "scene.h"


 /// @brief�@�Ǎ���ʃV�[���N���X
class LoadingScene :
    public Scene
{
    void Init()override;
    void Update()override;

private:
    bool m_flag = false;
};

