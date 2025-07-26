/**
 * @file   polygon2D.h
 * @brief  2D画像表示等行うクラスのヘッダ
 * @author Taiju Kobayashi
 * @date   2024/07
 */
#pragma once
#include "gameObject.h"

 /// @brief 2D画像表示等行うクラス
class  Polygon2D :public Component
{

public:
	using Component::Component;
	void UnInit()override;
	void Update()override;
	void Draw()override;

	bool SetImage(const char* ImagePath, const XMFLOAT2 imageSize,const XMFLOAT2 position);
private:
	ID3D11Buffer* m_vertex_buffer = NULL;
	ID3D11ShaderResourceView* m_texture = NULL;

	ID3D11VertexShader* m_vertex_shader;
	ID3D11PixelShader* m_pixel_shader;
	ID3D11InputLayout* m_vertex_layout;

	bool m_is_draw = false;
};