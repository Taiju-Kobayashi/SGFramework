/**
 * @file   gamescene.h
 * @brief  �Q�[���̃v���C�V�[���N���X�̃w�b�_�[
 * @author Taiju Kobayashi
 * @date   2024/11
 */
#pragma once
#include "scene.h"
#include "resultscene.h"
#include "input.h"

 /// @brief �Q�[���̃v���C�V�[���N���X
class GameScene : public Scene
{
public:
	void Init()override;
	void Update()override;
private:
	bool m_flag = false;
};