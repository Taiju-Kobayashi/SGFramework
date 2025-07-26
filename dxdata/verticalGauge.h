/**
 * @file   verticalGauge.h
 * @brief  �c�����̃Q�[�W��\������N���X�̃w�b�_
 * @author Taiju Kobayashi
 * @date   2024/12
 */
#pragma once
#include "gameObject.h"


class Audio;
///	@brief  �c�����̃Q�[�W��\������N���X
class  VerticalGauge :public GameObject
{
public:
	void Init()override;
	void UnInit()override;
	void Update()override {};
	void Draw()override;
	/// @brief �Q�[�W�̕\�����銄����ݒ�
	void SetGaugeValue(float num) { m_gauge_value = num; }

private:

	ID3D11Buffer* m_vertex_buffer = NULL;
	ID3D11ShaderResourceView* m_texture = NULL;
	ID3D11ShaderResourceView* m_bg_texture = NULL;

	ID3D11VertexShader* m_vertex_shader{};
	ID3D11PixelShader* m_pixel_shader{};
	ID3D11PixelShader* m_bgpixel_shader{};

	ID3D11InputLayout* m_vertex_layout{};
	float m_gauge_value = 0.0f;
};