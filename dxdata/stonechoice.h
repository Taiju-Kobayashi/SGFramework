/**
 * @file   stonechoice.h
 * @brief  石オブジェクト選択、シーン遷移を行うクラスのヘッダ
 * @author Taiju Kobayashi
 * @date   2024/3
 */
#pragma once
#include "gameObject.h"
#include "component.h"

 /// @brief  石オブジェクト選択、シーン遷移を行うクラス
class StoneChoice :public GameObject
{
	Component* m_stone_one = nullptr;
	Component* m_stone_two = nullptr;
	Component* m_stone_three = nullptr;

	//石のモデルパス
	static const int m_array_size = 3;
	const char* m_file_path[m_array_size]{};

	//石描画
	ID3D11VertexShader* m_vertex_shader{};
	ID3D11PixelShader* m_pixel_shader{};
	ID3D11InputLayout* m_VertexLayout{};

	//カーソルテクスチャ
	Component* m_cursor = nullptr;

	class Audio* m_choice_se{};
	int m_id = 0;
public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;
};