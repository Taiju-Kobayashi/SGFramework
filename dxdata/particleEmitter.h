/**
 * @file   particleEmitter.h
 * @brief  �p�[�e�B�N���̐���,�`����s���N���X�̃w�b�_
 * @author Taiju Kobayashi
 * @date   2024/09
 */
#pragma once
#include "component.h"


struct PARTICLESTATE
{
	float Duration = 1.0f;
	bool Looping = false;
	bool Prewarm = false;
	float LifeTime = 5.0f;//�p�[�e�B�N���̕\������
	float StartSpeed = 1.0f;//�p�[�e�B�N���̑��x
	float StartSize = 1.0f;//�p�[�e�B�N���̃T�C�Y
	XMFLOAT4 DiffuseColor{};
	float	GravityMultiplier;
	int MaxParticles = 100;
};


 /// @brief �p�[�e�B�N���̐���,�`����s���N���X
class particleEmitter :public Component
{
	ID3D11Buffer* m_vertex_buffer{};
	ID3D11ShaderResourceView* m_texture{};

	ID3D11VertexShader* m_vertex_shader{};
	ID3D11PixelShader* m_pixel_shader{};
	ID3D11InputLayout* m_vertex_layout{};
	
	struct  PARTICLE
	{
		XMFLOAT3 Position;
		XMFLOAT3 Velocity;
		float life;
		bool Enable;//�L�����H
	};
	static const int PARTICLE_MAX = 1000;
	PARTICLE g_Particles[PARTICLE_MAX]{};

	bool m_Enable = false;
public:
	using Component::Component;
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;

};

