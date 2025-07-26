/**
 * @file   stone.h
 * @brief  石オブジェクトの描画,水面メッシュとの判定/反発処理等を行うクラスのヘッダ
 * @author Taiju Kobayashi
 * @date   2024/10
 */
#pragma once
#include "gameObject.h"
#include "component.h"

 /// @brief  石オブジェクトの描画,水面メッシュとの判定/反発処理等を行うクラス
class ReplayStone :public GameObject
{

public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;

	void AddForce(const XMFLOAT3& force);
	void AddRotForce(const XMFLOAT3& force);
	bool& GetReleaseFlag() {
		return	m_release;
	}
	void SetReleaseFlag(bool flag) {
		m_release = flag;
	}

	//リプレイ用
	void RestartPlay();
	bool& GetEndFlag() { return m_IsEnd; }

private:
	ID3D11VertexShader* m_vertex_shader{};
	ID3D11PixelShader* m_pixel_shader{};
	ID3D11InputLayout* m_VertexLayout{};
	ID3D11ShaderResourceView* m_texture = NULL;

	XMFLOAT3 m_old_position{};
	XMFLOAT3 m_velocity{};
	float m_reboundrate = 0.98f;

	class Audio* m_se{};
	Component* m_modelrenderer = nullptr;
	XMFLOAT3 m_rotation_speed{};
	bool m_release = false;
	bool m_iswater_touch = false;
	GameObject* m_shadow = nullptr;
	class Water* _field = nullptr;

	int m_bound_count = 0;
	XMFLOAT3 m_start_position{};
	bool m_IsEnd = false;
};