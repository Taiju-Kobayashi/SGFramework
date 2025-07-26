#pragma once
#ifndef _ENDDRAW_H_
#define _ENDDRAW_H_
#include "posteffectobject.h"

class EndDraw : public PostEffectObject
{
	ID3D11Buffer* m_vertex_buffer = NULL;

	//ここに	シェーダー関連の変数を追加
	ID3D11VertexShader* m_vertex_shader;//頂点シェーダーオブジェクトポインタ
	ID3D11PixelShader* m_pixel_shader;//ピクセルシェーダーオブジェクトポインタ
	ID3D11InputLayout* m_vertex_layout;//頂点レイアウトオブジェクト
public:
	using PostEffectObject::PostEffectObject;
	EndDraw();
	~EndDraw();
	void Draw(ID3D11ShaderResourceView* ppTexture) override;
};
#endif // !_ENDDRAW_H_	