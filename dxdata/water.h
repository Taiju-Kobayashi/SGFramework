/**
 * @file   water.h
 * @brief  水面と波処理を行うクラスのヘッダー
 * @author Taiju Kobayashi
 * @date   2024/11
 */

#pragma once
#include "gameObject.h"

 ///	@brief  水面と波処理を行うクラスのヘッダー
class Water :
	public GameObject
{

public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;

	/// @brief 指定したGameObjectが水面を含むOBB内に存在するかどうかを判定します。
	/// @param Target 判定対象のGameObject
	/// @return true: 水面内に存在する, false: 水面内に存在しない
	bool CheckInWater(GameObject* Target);
	/// @brief 指定した座標における水面の高さを取得します。
	/// @param Position 座標 (XMFLOAT3)
	/// @return 水面の高さ (float)
	float GetHeight(XMFLOAT3& Position);
	/// @brief 指定した座標における水面の高さと法線ベクトルを取得します。
	/// @param Position 座標 (XMFLOAT3)
	/// @param RetNormal 対象ポリゴン法線戻り値用構造体
	/// @return 水面の高さ (float)
	float GetHeightAndNormal(XMFLOAT3& Position, XMFLOAT3& RetNormal);

	/// @brief 指定した座標における水面の法線ベクトルを取得します。
	/// @param Position 座標 (XMFLOAT3)
	XMFLOAT3 CalculateWaveNormalFrom3Points(
		 XMFLOAT3& offset1_base_xz,
		 XMFLOAT3& offset2_base_xz,
		 XMFLOAT3& offset3_base_xz,
		 XMFLOAT3& object_position);
	/// @brief 波を追加します。
	/// @param wave 追加する波 (WAVEMAKER)
	/// @return 追加成功 (true) または失敗 (false)
	bool AddWave(XMFLOAT3 Position);

	/// @brief 波を初期化します。
	bool InitWave();

private:
	ID3D11Buffer* m_vertex_buffer{};
	ID3D11Buffer* m_indexbuffer{};
	ID3D11ShaderResourceView* m_texture{};

	ID3D11VertexShader* m_vertex_shader{};
	ID3D11PixelShader* m_pixel_shader{};
	ID3D11InputLayout* m_vertex_layout{};
	ID3D11ShaderResourceView* m_envtexture{};

	const static int g_surface_size = 100;
	int		m_now_index{};
	VERTEX_3D m_vertex_array[3][g_surface_size][g_surface_size]{};
	float m_time{};

	//ベクトルの長さを計算する
	float get_vector_length(XMFLOAT3 v) {
		return pow((v.x * v.x) + (v.y * v.y) + (v.z * v.z), 0.5);
	}

	//ベクトル内積
	float dot_product(XMFLOAT3 vl, XMFLOAT3 vr) {
		return vl.x * vr.x + vl.y * vr.y + vl.z * vr.z;
	}




	unsigned int m_image_width = 0;
	unsigned int m_image_height = 0;
	static const  int m_bit_value = 255;
	XMFLOAT2 m_image_size{};
	XMFLOAT2 m_block_size{};
	XMFLOAT2 m_offset{};

	/// @brief 頂点をブレンドして水面を波立たせます。
	void BlendVertex();
	float m_threshold;
	float m_UvTime = 0.0f;

	//波のパラメータ
	// 波の速度、時間の刻み、グリッド幅
	const float m_wave_force = 0.4f;
	float m_wave_dt = 1.0f / 60.0f; //60fpsを想定し初期化
	const float m_wave_width = 0.25f;
	float m_wave_cth = 0.0f;

};

