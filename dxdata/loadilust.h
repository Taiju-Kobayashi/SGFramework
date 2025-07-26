/**
 * @file   loadilust.h
 * @brief  ���[�h��ʂ̔w�i�摜��`�悷��N���X�̃w�b�_
 * @author Taiju Kobayashi
 * @date   2024/11
 */
#pragma once
#include "gameObject.h"

class Audio;
/// @brief ���[�h��ʂ̔w�i�摜��`�悷��N���X
class  LoadIlust :public GameObject
{
public:
	void Init()override;
	void PosInit(XMFLOAT2 Size, XMFLOAT2 Pos);
	void UnInit()override;
	void Update()override;
	void Draw()override;
private:
	ID3D11Buffer* m_vertex_buffer = NULL;
	ID3D11ShaderResourceView* m_texture = NULL;

	ID3D11VertexShader* m_vertex_shader{};
	ID3D11PixelShader* m_pixel_shader{};
	ID3D11InputLayout* m_vertex_layout{};
};