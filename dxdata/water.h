/**
 * @file   water.h
 * @brief  ���ʂƔg�������s���N���X�̃w�b�_�[
 * @author Taiju Kobayashi
 * @date   2024/11
 */

#pragma once
#include "gameObject.h"

 ///	@brief  ���ʂƔg�������s���N���X�̃w�b�_�[
class Water :
	public GameObject
{

public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;

	/// @brief �w�肵��GameObject�����ʂ��܂�OBB���ɑ��݂��邩�ǂ����𔻒肵�܂��B
	/// @param Target ����Ώۂ�GameObject
	/// @return true: ���ʓ��ɑ��݂���, false: ���ʓ��ɑ��݂��Ȃ�
	bool CheckInWater(GameObject* Target);
	/// @brief �w�肵�����W�ɂ����鐅�ʂ̍������擾���܂��B
	/// @param Position ���W (XMFLOAT3)
	/// @return ���ʂ̍��� (float)
	float GetHeight(XMFLOAT3& Position);
	/// @brief �w�肵�����W�ɂ����鐅�ʂ̍����Ɩ@���x�N�g�����擾���܂��B
	/// @param Position ���W (XMFLOAT3)
	/// @param RetNormal �Ώۃ|���S���@���߂�l�p�\����
	/// @return ���ʂ̍��� (float)
	float GetHeightAndNormal(XMFLOAT3& Position, XMFLOAT3& RetNormal);

	/// @brief �w�肵�����W�ɂ����鐅�ʂ̖@���x�N�g�����擾���܂��B
	/// @param Position ���W (XMFLOAT3)
	XMFLOAT3 CalculateWaveNormalFrom3Points(
		 XMFLOAT3& offset1_base_xz,
		 XMFLOAT3& offset2_base_xz,
		 XMFLOAT3& offset3_base_xz,
		 XMFLOAT3& object_position);
	/// @brief �g��ǉ����܂��B
	/// @param wave �ǉ�����g (WAVEMAKER)
	/// @return �ǉ����� (true) �܂��͎��s (false)
	bool AddWave(XMFLOAT3 Position);

	/// @brief �g�����������܂��B
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

	/// @brief ���_���u�����h���Đ��ʂ�g�������܂��B
	void BlendVertex();
	float m_threshold;
	float m_UvTime = 0.0f;

	//�g�̃p�����[�^
	// �g�̑��x�A���Ԃ̍��݁A�O���b�h��
	const float m_wave_force = 0.4f;
	float m_wave_dt = 1.0f / 60.0f; //60fps��z�肵������
	const float m_wave_width = 0.25f;
	float m_wave_cth = 0.0f;

};

