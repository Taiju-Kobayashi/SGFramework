/**
 * @file   shadow2D.h
 * @brief  �X�e���V���V���h�E�`����s���N���X�̃w�b�_
 * @author Taiju Kobayashi
 * @date   2024/11
 */
#pragma once
#include "gameObject.h"

 /// @brief �X�e���V���V���h�E�`����s���N���X
class  Shadow2D :public GameObject
{
public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;
private:

	ID3D11Buffer* m_vertex_buffer = NULL;
	ID3D11ShaderResourceView* m_texture = NULL;

	ID3D11VertexShader* m_vertex_shader;
	ID3D11PixelShader* m_pixel_shader;
	ID3D11InputLayout* m_vertex_layout;
};