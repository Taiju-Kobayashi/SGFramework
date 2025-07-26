/**
 * @file   terrain.h
 * @brief  地形の作成、描画等を行うクラスのヘッダー
 * @author Taiju Kobayashi
 * @date   2024/11
 */
#pragma once
#include "gameObject.h"

/// @brief  地形の作成、描画等を行うクラス
class Terrain :public GameObject
{
	
public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;

	/// @brief 指定したGameObjectが地形を含むOBB内に存在するかどうかを判定します。
	/// @param Target 判定対象のGameObject
	/// @return true: OBB内に存在する, false: OBB内に存在しない
	bool CheckInTerrain(GameObject* Target);

	/// @brief 指定した座標における地面の高さを取得します。
	/// @param Position 座標 (XMFLOAT3)
	/// @return 地面の高さ (float)
	float GetHeight(XMFLOAT3& Position);

	/// @brief 指定した座標における地面の高さと法線ベクトルを取得します。
	/// @param Position 座標 (XMFLOAT3)
	/// @param RetNormal 対象ポリゴン法線戻り値用構造体
	/// @return 地面の高さ (float)
	float GetHeightAndNormal(XMFLOAT3& Position, XMFLOAT3& RetNormal);

	/// @brief 高さマップとテクスチャを使用して地形を生成します。
	/// @param hightmapPath 高さマップ画像のパス
	/// @param terrainSize 地形のサイズ (X, Y, Z)
	/// @param texturePath テクスチャ画像のパス
	/// @return true: 成功, false: 失敗
	bool MakeTerrain(const char* hightmapPath,const XMFLOAT3 terrainSize,const char* texturePath);
	
	/// @brief 高さマップとブレンドテクスチャを使用して地形を生成します。
	/// @param hightmapPath 高さマップ画像のパス
	/// @param terrainSize 地形のサイズ (X, Y, Z)
	/// @param textureOnePath 1番目のテクスチャ画像のパス
	/// @param textureTwoPath 2番目のテクスチャ画像のパス
	/// @param blendTexturePath ブレンドテクスチャ画像のパス
	/// @return true: 成功, false: 失敗
	bool MakeTerrainAndBlendTexture(const char* hightmapPath,const XMFLOAT3 terrainSize, const char* textureOnePath,const char*  textureTwoPath,const char* blendTexturePath);
	
	/// @brief 高さマップとブレンドテクスチャ(DDS)を使用して地形を生成します。
	/// @param hightmapPath 高さマップ画像のパス
	/// @param terrainSize 地形のサイズ (X, Y, Z)
	/// @param textureOnePath 1番目のテクスチャ画像のパス
	/// @param textureTwoPath 2番目のテクスチャ画像のパス
	/// @param blendTexturePath ブレンドテクスチャ画像のパス
	/// @return true: 成功, false: 失敗
	bool MakeTerrainAndBlendTextureDDS(const char* hightmapPath, const XMFLOAT3 terrainSize, const char* textureOnePath, const char* textureTwoPath, const char* blendTexturePath);

private:
	ID3D11Buffer* m_vertex_buffer{};
	ID3D11Buffer* m_index_buffer{};
	const static int m_num_texture = 3;//テクスチャ数
	ID3D11ShaderResourceView* m_texture[m_num_texture]{};
	ID3D11ShaderResourceView* m_bampmap_texture{};

	ID3D11VertexShader* m_vertex_shader{};
	ID3D11PixelShader* m_pixel_shader{};
	ID3D11InputLayout* m_vertex_layout{};

	const static int g_field_size = 128;

	VERTEX_3D* m_vertex_pointer = nullptr;

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

	/// @brief 8bitグレースケール画像から地形の頂点データとインデックスデータを生成
	/// @param image 地形生成に使用する画像データ。8bitグレースケール形式である必要がある
	/// @param TerrainSize 地形のサイズ（XMFLOAT3）。X, Zが水平方向のサイズ、Yが高さ方向のサイズ
	void MakeVertex(const Image& image, XMFLOAT3 TerrainSize);


};
