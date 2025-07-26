/**
 * @file   particleEmitter.h
 * @brief  パーティクルの生成,描画を行うクラスのヘッダ
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
	float LifeTime = 5.0f;//パーティクルの表示時間
	float StartSpeed = 1.0f;//パーティクルの速度
	float StartSize = 1.0f;//パーティクルのサイズ
	XMFLOAT4 DiffuseColor{};
	float	GravityMultiplier;
	int MaxParticles = 100;
};


 /// @brief パーティクルの生成,描画を行うクラス
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
		bool Enable;//有効か？
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

