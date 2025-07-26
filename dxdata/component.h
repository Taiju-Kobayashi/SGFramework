/**
 * @file   component.h
 * @brief  �R���|�[�l���g�̊��N���X�̃w�b�_�[
 * @author Taiju Kobayashi
 * @date   2024/12
 */
#pragma once
#ifndef  _COMPONENT_H_
#define _COMPONENT_H_

 /// @brief�@�R���|�[�l���g�̊��N���X
class Component
{
protected:
	class  GameObject* m_game_object = nullptr;

public:
	Component() = delete;
	Component(GameObject* Object):m_game_object(Object){ 
	}
	virtual ~Component(){}

	virtual void Init() {};//���̉��z�֐��ƒ��g�̂Ȃ��֐�
	virtual void UnInit() {};//��������K�v���Ȃ��N���X�ŃI�[�o���C�h����K�v�������Ȃ�
	virtual void Update() {};
	virtual void Draw() {};
};


#endif //  _COMPONENT_H_
