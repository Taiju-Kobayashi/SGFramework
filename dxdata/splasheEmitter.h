/**
 * @file   splasheEmitter.h
 * @brief  水しぶき表現を行うパーティクルのヘッダ
 * @author Taiju Kobayashi
 * @date   2024/12
 */
#pragma once
#include "gameObject.h"

class splasheEmitter :public GameObject
{
	
public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;

	void SetUse(bool use) { m_use = use; }
	void SetForse(float force) { m_particle_force = force; }

private:
	ID3D11Buffer* m_vertex_buffer{};
	ID3D11ShaderResourceView* m_texture{};

	ID3D11VertexShader* m_vertex_shader{};
	ID3D11PixelShader* m_pixel_shader{};
	ID3D11GeometryShader* m_geometry_shader;	//ジオメトリシェーダー
	ID3D11InputLayout* m_vertex_layout{};

	ID3D11Buffer* m_position_Buffer;
	ID3D11ShaderResourceView* m_positionSRV;

	struct  PARTICLE
	{
		XMFLOAT3 Position;
		XMFLOAT3 Velocity;
		XMFLOAT2 Size;
		XMFLOAT2 MaxSize;
		XMFLOAT2 MinSize;
		float life;
		bool Enable;//有効か？
	};
	static const int m_particle_max = 100;
	int m_particle_counter = 0;
	PARTICLE m_particle_array[m_particle_max]{};
	float m_particle_life_time = 600.0f;
	float m_radius = 5.0f;
	float m_particle_force = 4.0f;
	bool m_use = false;

};

