/**
 * @file   stonechoice.h
 * @brief  �΃I�u�W�F�N�g�I���A�V�[���J�ڂ��s���N���X�̃w�b�_
 * @author Taiju Kobayashi
 * @date   2024/3
 */
#pragma once
#include "gameObject.h"
#include "component.h"

 /// @brief  �΃I�u�W�F�N�g�I���A�V�[���J�ڂ��s���N���X
class StoneChoice :public GameObject
{
	Component* m_stone_one = nullptr;
	Component* m_stone_two = nullptr;
	Component* m_stone_three = nullptr;

	//�΂̃��f���p�X
	static const int m_array_size = 3;
	const char* m_file_path[m_array_size]{};

	//�Ε`��
	ID3D11VertexShader* m_vertex_shader{};
	ID3D11PixelShader* m_pixel_shader{};
	ID3D11InputLayout* m_VertexLayout{};

	//�J�[�\���e�N�X�`��
	Component* m_cursor = nullptr;

	class Audio* m_choice_se{};
	int m_id = 0;
public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;
};