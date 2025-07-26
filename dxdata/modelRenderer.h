/**
 * @file   modelrenderer.h
 * @brief  OBJモデルの読込,描画処理を提供するヘッダー
 * @date   2024/09
 */
#pragma once

#include "component.h"
#include <string>
#include <unordered_map>


// マテリアル構造体
struct MODEL_MATERIAL
{
	char						Name[256];
	MATERIAL					Material;
	char						TextureName[256];
	ID3D11ShaderResourceView* Texture;

};


// 描画サブセット構造体
struct SUBSET
{
	unsigned int	StartIndex;
	unsigned int	IndexNum;
	MODEL_MATERIAL	Material;
};


// モデル構造体
struct MODEL_OBJ
{
	VERTEX_3D* VertexArray;
	unsigned int	VertexNum;

	unsigned int* IndexArray;
	unsigned int	IndexNum;

	SUBSET* SubsetArray;
	unsigned int	SubsetNum;
};

struct MODEL
{
	XMFLOAT3	  ModelSize{};
	XMFLOAT3	  HalfSize{};
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;

	SUBSET* SubsetArray;
	unsigned int	SubsetNum;
};

/// @brief　OBJモデルの読込,描画処理を提供するクラス
class ModelRenderer : public Component
{

public:

	static void Preload(const char* FileName);
	static void UnloadAll();

	using Component::Component;

	void Load(const char* FileName);

	void Draw() override;
	/// @brief モデルのサイズを取得
	XMFLOAT3 GetSize();
	/// @brief モデルのサイズを半分にした値を取得
	XMFLOAT3 GetHarfSize();

private:

	static std::unordered_map<std::string, MODEL*> m_ModelPool;

	static void LoadModel(const char* FileName, MODEL* Model);
	static void LoadObj(const char* FileName, MODEL_OBJ* ModelObj, XMFLOAT3* ModelSize);
	static void LoadMaterial(const char* FileName, MODEL_MATERIAL** MaterialArray, unsigned int* MaterialNum);

	MODEL* m_model{};

	XMFLOAT3 m_min_position{};
	XMFLOAT3 m_max_position{};
	/// @brief 読み込んだモデルを包めるAABBのサイズを計算する。
	static XMFLOAT3 CalculateAABB(const VERTEX_3D* positions, unsigned int positionNum);
};