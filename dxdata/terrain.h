/**
 * @file   terrain.h
 * @brief  �n�`�̍쐬�A�`�擙���s���N���X�̃w�b�_�[
 * @author Taiju Kobayashi
 * @date   2024/11
 */
#pragma once
#include "gameObject.h"

/// @brief  �n�`�̍쐬�A�`�擙���s���N���X
class Terrain :public GameObject
{
	
public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;

	/// @brief �w�肵��GameObject���n�`���܂�OBB���ɑ��݂��邩�ǂ����𔻒肵�܂��B
	/// @param Target ����Ώۂ�GameObject
	/// @return true: OBB���ɑ��݂���, false: OBB���ɑ��݂��Ȃ�
	bool CheckInTerrain(GameObject* Target);

	/// @brief �w�肵�����W�ɂ�����n�ʂ̍������擾���܂��B
	/// @param Position ���W (XMFLOAT3)
	/// @return �n�ʂ̍��� (float)
	float GetHeight(XMFLOAT3& Position);

	/// @brief �w�肵�����W�ɂ�����n�ʂ̍����Ɩ@���x�N�g�����擾���܂��B
	/// @param Position ���W (XMFLOAT3)
	/// @param RetNormal �Ώۃ|���S���@���߂�l�p�\����
	/// @return �n�ʂ̍��� (float)
	float GetHeightAndNormal(XMFLOAT3& Position, XMFLOAT3& RetNormal);

	/// @brief �����}�b�v�ƃe�N�X�`�����g�p���Ēn�`�𐶐����܂��B
	/// @param hightmapPath �����}�b�v�摜�̃p�X
	/// @param terrainSize �n�`�̃T�C�Y (X, Y, Z)
	/// @param texturePath �e�N�X�`���摜�̃p�X
	/// @return true: ����, false: ���s
	bool MakeTerrain(const char* hightmapPath,const XMFLOAT3 terrainSize,const char* texturePath);
	
	/// @brief �����}�b�v�ƃu�����h�e�N�X�`�����g�p���Ēn�`�𐶐����܂��B
	/// @param hightmapPath �����}�b�v�摜�̃p�X
	/// @param terrainSize �n�`�̃T�C�Y (X, Y, Z)
	/// @param textureOnePath 1�Ԗڂ̃e�N�X�`���摜�̃p�X
	/// @param textureTwoPath 2�Ԗڂ̃e�N�X�`���摜�̃p�X
	/// @param blendTexturePath �u�����h�e�N�X�`���摜�̃p�X
	/// @return true: ����, false: ���s
	bool MakeTerrainAndBlendTexture(const char* hightmapPath,const XMFLOAT3 terrainSize, const char* textureOnePath,const char*  textureTwoPath,const char* blendTexturePath);
	
	/// @brief �����}�b�v�ƃu�����h�e�N�X�`��(DDS)���g�p���Ēn�`�𐶐����܂��B
	/// @param hightmapPath �����}�b�v�摜�̃p�X
	/// @param terrainSize �n�`�̃T�C�Y (X, Y, Z)
	/// @param textureOnePath 1�Ԗڂ̃e�N�X�`���摜�̃p�X
	/// @param textureTwoPath 2�Ԗڂ̃e�N�X�`���摜�̃p�X
	/// @param blendTexturePath �u�����h�e�N�X�`���摜�̃p�X
	/// @return true: ����, false: ���s
	bool MakeTerrainAndBlendTextureDDS(const char* hightmapPath, const XMFLOAT3 terrainSize, const char* textureOnePath, const char* textureTwoPath, const char* blendTexturePath);

private:
	ID3D11Buffer* m_vertex_buffer{};
	ID3D11Buffer* m_index_buffer{};
	const static int m_num_texture = 3;//�e�N�X�`����
	ID3D11ShaderResourceView* m_texture[m_num_texture]{};
	ID3D11ShaderResourceView* m_bampmap_texture{};

	ID3D11VertexShader* m_vertex_shader{};
	ID3D11PixelShader* m_pixel_shader{};
	ID3D11InputLayout* m_vertex_layout{};

	const static int g_field_size = 128;

	VERTEX_3D* m_vertex_pointer = nullptr;

	//�x�N�g���̒������v�Z����
	float get_vector_length(XMFLOAT3 v) {
		return pow((v.x * v.x) + (v.y * v.y) + (v.z * v.z), 0.5);
	}

	//�x�N�g������
	float dot_product(XMFLOAT3 vl, XMFLOAT3 vr) {
		return vl.x * vr.x + vl.y * vr.y + vl.z * vr.z;
	}

	unsigned int m_image_width = 0;
	unsigned int m_image_height = 0;
	static const  int m_bit_value = 255;
	XMFLOAT2 m_image_size{};
	XMFLOAT2 m_block_size{};
	XMFLOAT2 m_offset{};

	/// @brief 8bit�O���[�X�P�[���摜����n�`�̒��_�f�[�^�ƃC���f�b�N�X�f�[�^�𐶐�
	/// @param image �n�`�����Ɏg�p����摜�f�[�^�B8bit�O���[�X�P�[���`���ł���K�v������
	/// @param TerrainSize �n�`�̃T�C�Y�iXMFLOAT3�j�BX, Z�����������̃T�C�Y�AY�����������̃T�C�Y
	void MakeVertex(const Image& image, XMFLOAT3 TerrainSize);


};
