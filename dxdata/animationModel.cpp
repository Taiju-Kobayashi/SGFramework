#include "main.h"
#include "renderer.h"
#include "animationModel.h"

void AnimationModel::Draw()
{
	// プリミティブトポロジ設定
	Renderer::GetRenderer().GetRenderer().GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::GetRenderer().SetMaterial(material);

	for (unsigned int m = 0; m < m_aiscene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_aiscene->mMeshes[m];


		// マテリアル設定
		aiString texture;
		aiColor3D diffuse;
		float opacity;

		aiMaterial* aimaterial = m_aiscene->mMaterials[mesh->mMaterialIndex];
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture);
		aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		aimaterial->Get(AI_MATKEY_OPACITY, opacity);

		if (texture == aiString(""))
		{
			material.TextureEnable = false;
		}
		else
		{
			Renderer::GetRenderer().GetDeviceContext()->PSSetShaderResources(0, 1, &m_texture[texture.data]);
			material.TextureEnable = true;
		}

		material.Diffuse = XMFLOAT4(diffuse.r, diffuse.g, diffuse.b, opacity);
		material.Ambient = material.Diffuse;
		Renderer::GetRenderer().SetMaterial(material);


		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		Renderer::GetRenderer().GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertex_buffer[m], &stride, &offset);

		// インデックスバッファ設定
		Renderer::GetRenderer().GetDeviceContext()->IASetIndexBuffer(m_index_buffer[m], DXGI_FORMAT_R32_UINT, 0);

		// ポリゴン描画
		Renderer::GetRenderer().GetDeviceContext()->DrawIndexed(mesh->mNumFaces * 3, 0, 0);
	}
}

void AnimationModel::Load(const char* FileName)
{
	const std::string modelPath(FileName);

	m_aiscene = aiImportFile(FileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	assert(m_aiscene);

	m_vertex_buffer = new ID3D11Buffer * [m_aiscene->mNumMeshes];
	m_index_buffer = new ID3D11Buffer * [m_aiscene->mNumMeshes];


	//変形後頂点配列生成
	m_deform_vertex = new std::vector<DEFORM_VERTEX>[m_aiscene->mNumMeshes];
	m_model_size = findSize(m_aiscene);

	//再帰的にボーン生成
	//CreateBone(m_aiscene->mRootNode);

	for (unsigned int m = 0; m < m_aiscene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_aiscene->mMeshes[m];

		// 頂点バッファ生成
		{
			VERTEX_3D* vertex = new VERTEX_3D[mesh->mNumVertices];

			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				vertex[v].Position = XMFLOAT3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
				vertex[v].Normal = XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
				vertex[v].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX_3D) * mesh->mNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = vertex;

			Renderer::GetRenderer().GetDevice()->CreateBuffer(&bd, &sd,
				&m_vertex_buffer[m]);

			delete[] vertex;
		}


		// インデックスバッファ生成
		{
			unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

			for (unsigned int f = 0; f < mesh->mNumFaces; f++)
			{
				const aiFace* face = &mesh->mFaces[f];

				assert(face->mNumIndices == 3);

				index[f * 3 + 0] = face->mIndices[0];
				index[f * 3 + 1] = face->mIndices[1];
				index[f * 3 + 2] = face->mIndices[2];
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = index;

			Renderer::GetRenderer().GetDevice()->CreateBuffer(&bd, &sd, &m_index_buffer[m]);

			delete[] index;
		}



		//変形後頂点データ初期化
		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			DEFORM_VERTEX deformVertex;
			deformVertex.Position = mesh->mVertices[v];
			deformVertex.Normal = mesh->mNormals[v];
			deformVertex.BoneNum = 0;

			for (unsigned int b = 0; b < 4; b++)
			{
				deformVertex.BoneName[b] = "";
				deformVertex.BoneWeight[b] = 0.0f;
			}

			m_deform_vertex[m].push_back(deformVertex);
		}


		//////ボーンデータ初期化
		//for (unsigned int b = 0; b < mesh->mNumBones; b++)
		//{
		//	aiBone* bone = mesh->mBones[b];

		//	m_bone[bone->mName.C_Str()].OffsetMatrix = bone->mOffsetMatrix;

		//	//変形後頂点にボーンデータ格納
		//	for (unsigned int w = 0; w < bone->mNumWeights; w++)
		//	{
		//		aiVertexWeight weight = bone->mWeights[w];

		//		int num = m_deform_vertex[m][weight.mVertexId].BoneNum;

		//		m_deform_vertex[m][weight.mVertexId].BoneWeight[num] = weight.mWeight;
		//		m_deform_vertex[m][weight.mVertexId].BoneName[num] = bone->mName.C_Str();
		//		m_deform_vertex[m][weight.mVertexId].BoneNum++;

		//		assert(m_deform_vertex[m][weight.mVertexId].BoneNum <= 4);
		//	}
		//}
	}



	//テクスチャ読み込み
	for (int i = 0; i < m_aiscene->mNumTextures; i++)
	{
		aiTexture* aitexture = m_aiscene->mTextures[i];

		ID3D11ShaderResourceView* texture;

		// テクスチャ読み込み
		TexMetadata metadata;
		ScratchImage image;
		LoadFromWICMemory(aitexture->pcData, aitexture->mWidth, WIC_FLAGS_NONE, &metadata, image);
		CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
		assert(texture);

		m_texture[aitexture->mFilename.data] = texture;
	}



}


void AnimationModel::Load_NoBone(const char* FileName)
{
	const std::string modelPath(FileName);

	m_aiscene = aiImportFile(FileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	assert(m_aiscene);

	m_vertex_buffer = new ID3D11Buffer * [m_aiscene->mNumMeshes];
	m_index_buffer = new ID3D11Buffer * [m_aiscene->mNumMeshes];


	//変形後頂点配列生成
	m_deform_vertex = new std::vector<DEFORM_VERTEX>[m_aiscene->mNumMeshes];
	m_model_size = findSize(m_aiscene);


	for (unsigned int m = 0; m < m_aiscene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_aiscene->mMeshes[m];

		// 頂点バッファ生成
		{
			VERTEX_3D* vertex = new VERTEX_3D[mesh->mNumVertices];

			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				vertex[v].Position = XMFLOAT3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
				vertex[v].Normal = XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
				vertex[v].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX_3D) * mesh->mNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = vertex;

			Renderer::GetRenderer().GetDevice()->CreateBuffer(&bd, &sd,
				&m_vertex_buffer[m]);

			delete[] vertex;
		}


		// インデックスバッファ生成
		{
			unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

			for (unsigned int f = 0; f < mesh->mNumFaces; f++)
			{
				const aiFace* face = &mesh->mFaces[f];

				assert(face->mNumIndices == 3);

				index[f * 3 + 0] = face->mIndices[0];
				index[f * 3 + 1] = face->mIndices[1];
				index[f * 3 + 2] = face->mIndices[2];
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = index;

			Renderer::GetRenderer().GetDevice()->CreateBuffer(&bd, &sd, &m_index_buffer[m]);

			delete[] index;
		}



		//変形後頂点データ初期化
		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			DEFORM_VERTEX deformVertex;
			deformVertex.Position = mesh->mVertices[v];
			deformVertex.Normal = mesh->mNormals[v];
			deformVertex.BoneNum = 0;

			for (unsigned int b = 0; b < 4; b++)
			{
				deformVertex.BoneName[b] = "";
				deformVertex.BoneWeight[b] = 0.0f;
			}

			m_deform_vertex[m].push_back(deformVertex);
		}
	}



	//テクスチャ読み込み
	for (int i = 0; i < m_aiscene->mNumTextures; i++)
	{
		aiTexture* aitexture = m_aiscene->mTextures[i];

		ID3D11ShaderResourceView* texture;

		// テクスチャ読み込み
		TexMetadata metadata;
		ScratchImage image;
		LoadFromWICMemory(aitexture->pcData, aitexture->mWidth, WIC_FLAGS_NONE, &metadata, image);
		CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
		assert(texture);

		m_texture[aitexture->mFilename.data] = texture;
	}



}


void AnimationModel::UnInit()
{
	for (unsigned int m = 0; m < m_aiscene->mNumMeshes; m++)
	{
		m_vertex_buffer[m]->Release();
		m_index_buffer[m]->Release();
	}

	delete[] m_vertex_buffer;
	delete[] m_index_buffer;

	delete[] m_deform_vertex;


	for (std::pair<const std::string, ID3D11ShaderResourceView*> pair : m_texture)
	{
		pair.second->Release();
	}



	aiReleaseImport(m_aiscene);


	for (std::pair<const std::string, const aiScene*> pair : m_animation)
	{
		aiReleaseImport(pair.second);
	}
}



void AnimationModel::LoadAnimation(const char* FileName, const char* Name)
{

	m_animation[Name] = aiImportFile(FileName, aiProcess_ConvertToLeftHanded);
	assert(m_animation[Name]);

}

void AnimationModel::CreateBone(aiNode* node) {
	BONE bone;
	m_bone[node->mName.C_Str()] = bone;

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		CreateBone(node->mChildren[i]);
	}
}



void AnimationModel::Update(const char* AnimationName1, int Frame1, const char* AnimationName2, int Frame2, float BlendRatio) {

	if (m_animation.count(AnimationName1) == 0)
		return;

	if (m_animation.count(AnimationName2) == 0)
		return;

	if (!m_animation[AnimationName1]->HasAnimations())
		return;

	if (!m_animation[AnimationName2]->HasAnimations())
		return;

	//アニメーションデータからボーンマトリクス算出
	aiAnimation* animation1 = m_animation[AnimationName1]->mAnimations[0];
	aiAnimation* animation2 = m_animation[AnimationName2]->mAnimations[0];

	for (auto pair : m_bone)
	{
		BONE* bone = &m_bone[pair.first];

		aiNodeAnim* nodeAnim1 = nullptr;

		for (unsigned int c = 0; c < animation1->mNumChannels; c++)
		{
			if (animation1->mChannels[c]->mNodeName == aiString(pair.first)) {
				nodeAnim1 = animation1->mChannels[c];
				break;
			}
		}		
		
		aiNodeAnim* nodeAnim2 = nullptr;
		for (unsigned int c = 0; c < animation2->mNumChannels; c++)
		{
			if (animation2->mChannels[c]->mNodeName == aiString(pair.first)) {
				nodeAnim2 = animation2->mChannels[c];
				break;
			}
		}

		int f;
		aiQuaternion rot1;
		aiVector3D pos1;

		if (nodeAnim1) {
			f = Frame1 % nodeAnim1->mNumRotationKeys;//簡易実装
			rot1 = nodeAnim1->mRotationKeys[f].mValue;

			f = Frame1 % nodeAnim1->mNumPositionKeys;//簡易実装
			pos1 = nodeAnim1->mPositionKeys[f].mValue;
		}

		aiQuaternion rot2;
		aiVector3D pos2;

		if (nodeAnim2) {
			f = Frame2 % nodeAnim1->mNumRotationKeys;//簡易実装
			rot2 = nodeAnim2->mRotationKeys[f].mValue;

			f = Frame2 % nodeAnim2->mNumPositionKeys;//簡易実装
			pos2
				= nodeAnim2->mPositionKeys[f].mValue;
		}
		aiVector3D Pos;
		Pos = pos1 * (1.0f - BlendRatio) + pos2 * BlendRatio;//線形補間 エルミート補間
		aiQuaternion Rot;
		aiQuaternion::Interpolate(Rot,rot1, rot2,BlendRatio);

		bone->AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), Rot, Pos);

	}
	//再帰的にボーンマトリクス更新
	//aiMatrix4x4 rootMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), aiQuaternion((float)AI_MATH_PI, 0.0f, 0.0f), aiVector3D(0.0f, 0.0f, 0.0f));
		aiMatrix4x4 rootMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), aiQuaternion((float)AI_MATH_PI, 0.0f, 0.0f), aiVector3D(0.0f, 0.0f, 0.0f));

	UpdateBoneMatrix(m_aiscene->mRootNode, rootMatrix);

	//m_AiScene->mNumMeshes
	//mesh->mNumVertices
	//頂点変換(CPUスキニング)

	for (unsigned int m = 0; m < m_aiscene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_aiscene->mMeshes[m];

		D3D11_MAPPED_SUBRESOURCE ms;
		Renderer::GetRenderer().GetDeviceContext()->Map(m_vertex_buffer[m], 0,
			D3D11_MAP_WRITE_DISCARD, 0, &ms);

		VERTEX_3D* vertex = (VERTEX_3D*)ms.pData;

		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			DEFORM_VERTEX* deformVertex = &m_deform_vertex[m][v];

			aiMatrix4x4 matrix[4];
			aiMatrix4x4 outMatrix;
			matrix[0] = m_bone[deformVertex->BoneName[0]].Matrix;
			matrix[1] = m_bone[deformVertex->BoneName[1]].Matrix;
			matrix[2] = m_bone[deformVertex->BoneName[2]].Matrix;
			matrix[3] = m_bone[deformVertex->BoneName[3]].Matrix;

			//ウェイトを考慮してマトリクス計算
		/*	outMatrix = matrix[0] * deformVertex->BoneWeight[0]
				+ matrix[1] * deformVertex->BoneWeight[1]
				+ matrix[2] * deformVertex->BoneWeight[2]
				+ matrix[3] * deformVertex->BoneWeight[3];*/
			{
				outMatrix.a1 =
					matrix[0].a1 * deformVertex->BoneWeight[0]
					+ matrix[1].a1 * deformVertex->BoneWeight[1]
					+ matrix[2].a1 * deformVertex->BoneWeight[2]
					+ matrix[3].a1 * deformVertex->BoneWeight[3];

				outMatrix.a2 =
					matrix[0].a2 * deformVertex->BoneWeight[0]
					+ matrix[1].a2 * deformVertex->BoneWeight[1]
					+ matrix[2].a2 * deformVertex->BoneWeight[2]
					+ matrix[3].a2 * deformVertex->BoneWeight[3];

				outMatrix.a3 =
					matrix[0].a3 * deformVertex->BoneWeight[0]
					+ matrix[1].a3 * deformVertex->BoneWeight[1]
					+ matrix[2].a3 * deformVertex->BoneWeight[2]
					+ matrix[3].a3 * deformVertex->BoneWeight[3];

				outMatrix.a4 =
					matrix[0].a4 * deformVertex->BoneWeight[0]
					+ matrix[1].a4 * deformVertex->BoneWeight[1]
					+ matrix[2].a4 * deformVertex->BoneWeight[2]
					+ matrix[3].a4 * deformVertex->BoneWeight[3];


				outMatrix.b1 =
					matrix[0].b1 * deformVertex->BoneWeight[0]
					+ matrix[1].b1 * deformVertex->BoneWeight[1]
					+ matrix[2].b1 * deformVertex->BoneWeight[2]
					+ matrix[3].b1 * deformVertex->BoneWeight[3];

				outMatrix.b2 =
					matrix[0].b2 * deformVertex->BoneWeight[0]
					+ matrix[1].b2 * deformVertex->BoneWeight[1]
					+ matrix[2].b2 * deformVertex->BoneWeight[2]
					+ matrix[3].b2 * deformVertex->BoneWeight[3];

				outMatrix.b3 =
					matrix[0].b3 * deformVertex->BoneWeight[0]
					+ matrix[1].b3 * deformVertex->BoneWeight[1]
					+ matrix[2].b3 * deformVertex->BoneWeight[2]
					+ matrix[3].b3 * deformVertex->BoneWeight[3];

				outMatrix.b4 =
					matrix[0].b4 * deformVertex->BoneWeight[0]
					+ matrix[1].b4 * deformVertex->BoneWeight[1]
					+ matrix[2].b4 * deformVertex->BoneWeight[2]
					+ matrix[3].b4 * deformVertex->BoneWeight[3];


				outMatrix.c1 = matrix[0].c1 * deformVertex->BoneWeight[0]
					+ matrix[1].c1 * deformVertex->BoneWeight[1]
					+ matrix[2].c1 * deformVertex->BoneWeight[2]
					+ matrix[3].c1 * deformVertex->BoneWeight[3];

				outMatrix.c2 = matrix[0].c2 * deformVertex->BoneWeight[0]
					+ matrix[1].c2 * deformVertex->BoneWeight[1]
					+ matrix[2].c2 * deformVertex->BoneWeight[2]
					+ matrix[3].c2 * deformVertex->BoneWeight[3];

				outMatrix.c3 = matrix[0].c3 * deformVertex->BoneWeight[0]
					+ matrix[1].c3 * deformVertex->BoneWeight[1]
					+ matrix[2].c3 * deformVertex->BoneWeight[2]
					+ matrix[3].c3 * deformVertex->BoneWeight[3];

				outMatrix.c4 = matrix[0].c4 * deformVertex->BoneWeight[0]
					+ matrix[1].c4 * deformVertex->BoneWeight[1]
					+ matrix[2].c4 * deformVertex->BoneWeight[2]
					+ matrix[3].c4 * deformVertex->BoneWeight[3];


				outMatrix.d1 = matrix[0].d1 * deformVertex->BoneWeight[0]
					+ matrix[1].d1 * deformVertex->BoneWeight[1]
					+ matrix[2].d1 * deformVertex->BoneWeight[2]
					+ matrix[3].d1 * deformVertex->BoneWeight[3];


				outMatrix.d2 = matrix[0].d2 * deformVertex->BoneWeight[0]
					+ matrix[1].d2 * deformVertex->BoneWeight[1]
					+ matrix[2].d2 * deformVertex->BoneWeight[2]
					+ matrix[3].d2 * deformVertex->BoneWeight[3];


				outMatrix.d3 = matrix[0].d3 * deformVertex->BoneWeight[0]
					+ matrix[1].d3 * deformVertex->BoneWeight[1]
					+ matrix[2].d3 * deformVertex->BoneWeight[2]
					+ matrix[3].d3 * deformVertex->BoneWeight[3];


				outMatrix.d4 = matrix[0].d4 * deformVertex->BoneWeight[0]
					+ matrix[1].d4 * deformVertex->BoneWeight[1]
					+ matrix[2].d4 * deformVertex->BoneWeight[2]
					+ matrix[3].d4 * deformVertex->BoneWeight[3];

			}
			deformVertex->Position = mesh->mVertices[v];
			deformVertex->Position *= outMatrix;

			//法線変換用に移動成分を除去
			outMatrix.a4 = 0.0f;
			outMatrix.b4 = 0.0f;
			outMatrix.c4 = 0.0f;

			deformVertex->Normal = mesh->mNormals[v];
			deformVertex->Normal *= outMatrix;

			//頂点バッファ書き込み
			vertex[v].Position.x = deformVertex->Position.x;
			vertex[v].Position.y = deformVertex->Position.y;
			vertex[v].Position.z = deformVertex->Position.z;

			vertex[v].Normal.x = deformVertex->Normal.x;
			vertex[v].Normal.y = deformVertex->Normal.y;
			vertex[v].Normal.z = deformVertex->Normal.z;


			vertex[v].TexCoord.x = mesh->mTextureCoords[0][v].x;
			vertex[v].TexCoord.y = mesh->mTextureCoords[0][v].y;

			vertex[v].Diffuse = XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f };
		}
		Renderer::GetRenderer().GetDeviceContext()->Unmap(m_vertex_buffer[m], 0);
	}


}

void AnimationModel::UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix) {
	BONE* bone = &m_bone[node->mName.C_Str()];

	//マトリクスの乗算順番に注意
	aiMatrix4x4 worldMatrix;
	worldMatrix *= matrix;
	worldMatrix *= bone->AnimationMatrix;

	bone->Matrix = worldMatrix;
	bone->Matrix *= bone->OffsetMatrix;

	for (unsigned int n = 0; n < node->mNumChildren; n++)
	{
		UpdateBoneMatrix(node->mChildren[n], worldMatrix);
	}
}

XMFLOAT3 AnimationModel::findSize(const aiScene* scene) {
	XMFLOAT3 min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	XMFLOAT3 max(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		const aiMesh* mesh = scene->mMeshes[i];
		for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
			const aiVector3D& vertex = mesh->mVertices[j];
			min.x = std::min(min.x, vertex.x);
			min.y = std::min(min.y, vertex.y);
			min.z = std::min(min.z, vertex.z);
			max.x = std::max(max.x, vertex.x);
			max.y = std::max(max.y, vertex.y);
			max.z = std::max(max.z, vertex.z);
		}
	}
	return  XMFLOAT3(max.x - min.x, max.y - min.y, max.z - min.z);
}

XMFLOAT3 AnimationModel::GetHarfModelSize() {
	XMFLOAT3 size{};
	size.x = m_model_size.x * 0.5f;
	size.y = m_model_size.y * 0.5f;
	size.z = m_model_size.z * 0.5f;
	return size;
}
