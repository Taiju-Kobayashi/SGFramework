/**
 * @file   shadowvolume.h
 * @brief  ステンシルシャドウ描画用オブジェクトのヘッダ
 * @author Taiju Kobayashi
 * @date   2024/11
 */
#pragma once
#include "gameObject.h"

 /// @brief ステンシルシャドウ描画用オブジェクトのクラス
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

