/**
 * @file   slingshot.h
 * @brief  �΃I�u�W�F�N�g����,���ˏ������s���N���X�̃w�b�_
 * @author Taiju Kobayashi
 * @date   2024/12
 */
#pragma once
#include "gameObject.h"

 /// @brief �΃I�u�W�F�N�g����,���ˏ������s���N���X
class LoadAnime :public GameObject
{
public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;
private:
	ID3D11Buffer* m_vertex_buffer{};
	ID3D11ShaderResourceView* m_texture{};

	ID3D11VertexShader* m_vertex_shader{};
	ID3D11PixelShader* m_pixel_shader{};
	ID3D11InputLayout* m_vertex_layout{};

	int m_counter{};
};

