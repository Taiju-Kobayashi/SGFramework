/**
 * @file   animationModel.h
 * @brief  FBX���f���̓Ǎ�,�`��,�A�j���[�V�����Đ�������񋟂���N���X�̃w�b�_
 * @author Taiju Kobayashi
 * @date   2024/09
 *Open Asset Import Library (assimp)
 *Copyright (c) 2006-2021, assimp team
 *All rights reserved.
 *
 */
#pragma once

#include <unordered_map>

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#pragma comment (lib, "assimp-vc143-mt.lib")

#include "component.h"


 //�ό`�㒸�_�\����
struct DEFORM_VERTEX
{
	aiVector3D Position;
	aiVector3D Normal;
	int				BoneNum;
	std::string		BoneName[4];//�{���̓{�[���C���f�b�N�X�ŊǗ�����ׂ�
	float			BoneWeight[4];
};

//�{�[���\����
struct BONE
{
	aiMatrix4x4 Matrix;
	aiMatrix4x4 AnimationMatrix;
	aiMatrix4x4 OffsetMatrix;
};

/// @brief�@FBX���f���̓Ǎ�,�`��,�A�j���[�V�����Đ�������񋟂���N���X
class AnimationModel : public Component
{
public:
	using Component::Component;

	void Load(const char* FileName);
	void Load_NoBone(const char* FileName);
	void UnInit() override;
	void Update(const char* AnimationName1, int Frame1, const char* AnimationName2, int Frame2, float BlendRatio);
	void Draw() override;

	void LoadAnimation(const char* FileName, const char* Name);
	void CreateBone(aiNode* node);
	void UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix);
	XMFLOAT3& GetModelSize() { return m_model_size; }
	XMFLOAT3 GetHarfModelSize();

private:
	const aiScene* m_aiscene = nullptr;
	std::unordered_map<std::string, const aiScene*> m_animation;

	ID3D11Buffer** m_vertex_buffer;
	ID3D11Buffer** m_index_buffer;

	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_texture;

	std::vector<DEFORM_VERTEX>* m_deform_vertex;//�ό`�㒸�_�f�[�^
	std::unordered_map<std::string, BONE> m_bone;//�{�[���f�[�^�i���O�ŎQ�Ɓj

	XMFLOAT3 m_model_size{};
	XMFLOAT3 findSize(const aiScene* scene);
};