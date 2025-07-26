/**
 * @file   skydome.h
 * @brief  �X�J�C�h�[���I�u�W�F�N�g��`�悷��N���X�̃w�b�_
 * @author Taiju Kobayashi
 * @date   2024/09
 */
#pragma once
#include "gameObject.h"
#include "component.h"

 /// @brief�@�X�J�C�h�[���I�u�W�F�N�g��`�悷��N���X
class SkyDome :
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

	ID3D11ShaderResourceView* m_texture = nullptr;

	XMFLOAT3 m_velocity{};

};

