/**
 * @file   score.h
 * @brief  スコア表示用ポリゴン描画クラスのヘッダ
 * @author Taiju Kobayashi
 * @date   2024/12
 */
#pragma once
#include "gameObject.h"

 /// @brief スコア表示用ポリゴン描画クラス
class  Score :public GameObject
{
public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;

	void AddScore(int value) { m_Counter += value; }
private:
	ID3D11Buffer* m_vertex_buffer = NULL;
	ID3D11ShaderResourceView* m_texture = NULL;

	ID3D11VertexShader* m_vertex_shader;
	ID3D11PixelShader* m_pixel_shader;
	ID3D11InputLayout* m_vertex_layout;

	int m_Counter = 0;
};