/**
 * @file   shadowvolume.h
 * @brief  �X�e���V���V���h�E�`��p�I�u�W�F�N�g�̃w�b�_
 * @author Taiju Kobayashi
 * @date   2024/11
 */
#pragma once
#include "gameObject.h"

 /// @brief �X�e���V���V���h�E�`��p�I�u�W�F�N�g�̃N���X
class ShadowVolume :
    public GameObject
{
public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;
private:
	ID3D11VertexShader* m_vertex_shader{};
	ID3D11PixelShader* m_pixel_shader{};
	ID3D11InputLayout* m_vertex_layout{};

	ID3D11Buffer* m_vertex_buffer{};
};

