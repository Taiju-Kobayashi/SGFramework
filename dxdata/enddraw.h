#pragma once
#ifndef _ENDDRAW_H_
#define _ENDDRAW_H_
#include "posteffectobject.h"

class EndDraw : public PostEffectObject
{
	ID3D11Buffer* m_vertex_buffer = NULL;

	//������	�V�F�[�_�[�֘A�̕ϐ���ǉ�
	ID3D11VertexShader* m_vertex_shader;//���_�V�F�[�_�[�I�u�W�F�N�g�|�C���^
	ID3D11PixelShader* m_pixel_shader;//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�|�C���^
	ID3D11InputLayout* m_vertex_layout;//���_���C�A�E�g�I�u�W�F�N�g
public:
	using PostEffectObject::PostEffectObject;
	EndDraw();
	~EndDraw();
	void Draw(ID3D11ShaderResourceView* ppTexture) override;
};
#endif // !_ENDDRAW_H_	