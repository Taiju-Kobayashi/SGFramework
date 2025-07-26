/**
 * @file   modelrenderer.h
 * @brief  OBJ���f���̓Ǎ�,�`�揈����񋟂���w�b�_�[
 * @date   2024/09
 */
#pragma once

#include "component.h"
#include <string>
#include <unordered_map>


// �}�e���A���\����
struct MODEL_MATERIAL
{
	char						Name[256];
	MATERIAL					Material;
	char						TextureName[256];
	ID3D11ShaderResourceView* Texture;

};


// �`��T�u�Z�b�g�\����
struct SUBSET
{
	unsigned int	StartIndex;
	unsigned int	IndexNum;
	MODEL_MATERIAL	Material;
};


// ���f���\����
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

/// @brief�@OBJ���f���̓Ǎ�,�`�揈����񋟂���N���X
class ModelRenderer : public Component
{

public:

	static void Preload(const char* FileName);
	static void UnloadAll();

	using Component::Component;

	void Load(const char* FileName);

	void Draw() override;
	/// @brief ���f���̃T�C�Y���擾
	XMFLOAT3 GetSize();
	/// @brief ���f���̃T�C�Y�𔼕��ɂ����l���擾
	XMFLOAT3 GetHarfSize();

private:

	static std::unordered_map<std::string, MODEL*> m_ModelPool;

	static void LoadModel(const char* FileName, MODEL* Model);
	static void LoadObj(const char* FileName, MODEL_OBJ* ModelObj, XMFLOAT3* ModelSize);
	static void LoadMaterial(const char* FileName, MODEL_MATERIAL** MaterialArray, unsigned int* MaterialNum);

	MODEL* m_model{};

	XMFLOAT3 m_min_position{};
	XMFLOAT3 m_max_position{};
	/// @brief �ǂݍ��񂾃��f�����߂�AABB�̃T�C�Y���v�Z����B
	static XMFLOAT3 CalculateAABB(const VERTEX_3D* positions, unsigned int positionNum);
};