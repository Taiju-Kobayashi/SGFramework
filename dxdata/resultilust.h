/**
 * @file   resultilust.h
 * @brief  ƒŠƒUƒ‹ƒg‰æ–Ê‚Ì”wŒi‰æ‘œ‚ğ•`‰æ‚·‚éƒNƒ‰ƒX‚Ìƒwƒbƒ_
 * @author Taiju Kobayashi
 * @date   2024/12
 */
#pragma once
#include "gameObject.h"

class Audio;
/// @brief ƒŠƒUƒ‹ƒg‰æ–Ê‚Ì”wŒi‰æ‘œ‚ğ•`‰æ‚·‚éƒNƒ‰ƒX
class  ResultIlust :public GameObject
{

public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;
private:
	ID3D11Buffer* m_vertex_buffer = NULL;
	ID3D11ShaderResourceView* m_textTexture = NULL;

	ID3D11VertexShader* m_vertex_shader{};
	ID3D11PixelShader* m_pixel_shader{};
	ID3D11InputLayout* m_vertex_layout{};
};