#include "main.h"
#include "renderer.h"
#include "terrain.h"

void Terrain::Init() {

}

void Terrain::UnInit() {
	if (m_vertex_pointer == nullptr)return;
	m_vertex_buffer->Release();
	for (int i = 0; i < m_num_texture; i++)
	{
		if (m_texture[i]) {
			m_texture[i]->Release();
		}
	}
	m_bampmap_texture->Release();
	m_vertex_layout->Release();
	m_vertex_shader->Release();
	m_index_buffer->Release();
	m_pixel_shader->Release();
	delete[] m_vertex_pointer;

}

void Terrain::Update() {
}

void Terrain::Draw() {

	if (m_vertex_pointer == nullptr)return;

	//入力レイアウト
	Renderer::GetRenderer().GetDeviceContext()->IASetInputLayout(m_vertex_layout);

	//shader設定
	Renderer::GetRenderer().GetDeviceContext()->VSSetShader(m_vertex_shader, NULL, 0);
	Renderer::GetRenderer().GetDeviceContext()->PSSetShader(m_pixel_shader, NULL, 0);

	Renderer::GetRenderer().SetWorldMatrix(m_transform->GetWorldMatrix());

	//頂点バッファ
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetRenderer().GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

	//インデックスバッファ
	Renderer::GetRenderer().GetDeviceContext()->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);


	//マテリアル
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::GetRenderer().SetMaterial(material);

	Renderer::GetRenderer().GetDeviceContext()->PSSetShaderResources(0, 1, &m_texture[0]);

	if (m_texture[1]) {
		Renderer::GetRenderer().GetDeviceContext()->PSSetShaderResources(1, 1, &m_texture[1]);
	}
	if (m_texture[2]) {
		Renderer::GetRenderer().GetDeviceContext()->PSSetShaderResources(2, 1, &m_texture[2]);
	}
	if (m_bampmap_texture) {
		Renderer::GetRenderer().GetDeviceContext()->PSSetShaderResources(3, 1, &m_bampmap_texture);
	}
	//プリミティブトポロジー
	Renderer::GetRenderer().GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	//ポリゴン描画 
	Renderer::GetRenderer().GetDeviceContext()->DrawIndexed(((m_image_width + 1) * 2) * (m_image_height - 1) - 2, 0, 0);//indexbuffur指定の位置で描画
}

bool Terrain::CheckInTerrain(GameObject* Target) {
	//OBB
	XMFLOAT3 direction;
	auto tpos = Target->GetTransform()->GetPosition();
	auto mpos = m_transform->GetPosition();

	direction.x = tpos.x - mpos.x;
	direction.y = tpos.y - mpos.y;
	direction.z = tpos.z - mpos.z;

	//X分離軸
	XMFLOAT3 axisX = m_transform->GetRight();
	XMFLOAT3 axisY = m_transform->GetUp();
	XMFLOAT3 axisZ = m_transform->GetForward();

	//内積計算
	float dotx = (direction.x * axisX.x + direction.y * axisX.y + direction.z * axisX.z);
	float doty = (direction.x * axisY.x + direction.y * axisY.y + direction.z * axisY.z);
	float dotz = (direction.x * axisZ.x + direction.y * axisZ.y + direction.z * axisZ.z);

	if (-m_size.x < dotx && dotx < m_size.x &&
		-m_size.y < doty && doty < m_size.y &&
		-m_size.z < dotz && dotz < m_size.z) {
		return true;
	}


	return false;
}


float Terrain::GetHeight(XMFLOAT3& Position) {

	if (m_vertex_pointer == nullptr)
		return -1.0f;


	int x, z;
	//ブロック番号算出 //ブロックサイズから現在番地を計算する
	x = static_cast<int>((Position.x - m_offset.x) / m_block_size.x);
	z = static_cast<int>((Position.z - m_offset.y) / m_block_size.y);
	if (x < 0) {
		x *= -1;
	}
	if (z < 0) {
		z *= -1;
	}

	//プレイヤーが居る番地を囲む座標を取得
	XMFLOAT3 pos0, pos1, pos2, pos3;
	pos0 = (m_vertex_pointer + ((x * m_image_height) + z))->Position;//左上
	pos1 = (m_vertex_pointer + ((x + 1) * m_image_height + z))->Position;//右上
	pos2 = (m_vertex_pointer + (x * m_image_height + (z + 1)))->Position;//左下
	pos3 = (m_vertex_pointer + ((x + 1) * m_image_height + (z + 1)))->Position;//右下

	XMFLOAT3 v12{ pos2.x - pos1.x,pos2.y - pos1.y,pos2.z - pos1.z };
	XMFLOAT3 v1p{ Position.x - pos1.x, Position.y - pos1.y ,Position.z - pos1.z };

	//外積
	float cv = v12.z * v1p.x - v12.x * v1p.z;

	//マスを構成する2つのポリゴンの内、
	//どちらのポリゴン上に居るか計算
	float py;
	XMFLOAT3 n;
	if (cv > 0.0f) {
		//左上ポリゴン
		XMFLOAT3 v10{};
		v10.x = pos0.x - pos1.x;
		v10.y = pos0.y - pos1.y;
		v10.z = pos0.z - pos1.z;

		//外積
		n.x = v10.y * v12.z - v10.z * v12.y;
		n.y = v10.z * v12.x - v10.x * v12.z;
		n.z = v10.x * v12.y - v10.y * v12.x;
	}
	else
	{
		//右下ポリゴン
		XMFLOAT3 v13{};
		v13.x = pos3.x - pos1.x;
		v13.y = pos3.y - pos1.y;
		v13.z = pos3.z - pos1.z;

		//外積
		n.x = v12.y * v13.z - v12.z * v13.y;
		n.y = v12.z * v13.x - v12.x * v13.z;
		n.z = v12.x * v13.y - v12.y * v13.x;
	}

	//高さ取得
	py = -((Position.x - pos1.x) * n.x + (Position.z - pos1.z) * n.z) / n.y + pos1.y;
	return py;
}

float Terrain::GetHeightAndNormal(XMFLOAT3& Position, XMFLOAT3& RetNormal) {
	if (m_vertex_pointer == nullptr)
		return -1.0f;


	int x, z;
	//ブロック番号算出 //ブロックサイズから現在番地を計算する
	x = static_cast<int>((Position.x - m_offset.x) / m_block_size.x);
	z = static_cast<int>((Position.z - m_offset.y) / m_block_size.y);
	if (x < 0) {
		x *= -1;
	}
	if (z < 0) {
		z *= -1;
	}

	//プレイヤーが居る番地を囲む座標を取得
	XMFLOAT3 pos0, pos1, pos2, pos3;
	pos0 = (m_vertex_pointer + ((x * m_image_height) + z))->Position;//左上
	pos1 = (m_vertex_pointer + ((x + 1) * m_image_height + z))->Position;//右上
	pos2 = (m_vertex_pointer + (x * m_image_height + (z + 1)))->Position;//左下
	pos3 = (m_vertex_pointer + ((x + 1) * m_image_height + (z + 1)))->Position;//右下

	XMFLOAT3 v12{ pos2.x - pos1.x,pos2.y - pos1.y,pos2.z - pos1.z };
	XMFLOAT3 v1p{ Position.x - pos1.x, Position.y - pos1.y ,Position.z - pos1.z };

	//外積
	float cv = v12.z * v1p.x - v12.x * v1p.z;

	//マスを構成する2つのポリゴンの内、
	//どちらのポリゴン上に居るか計算
	float py;
	XMFLOAT3 n;
	if (cv > 0.0f) {
		//左上ポリゴン
		XMFLOAT3 v10{};
		v10.x = pos0.x - pos1.x;
		v10.y = pos0.y - pos1.y;
		v10.z = pos0.z - pos1.z;

		//外積
		n.x = v10.y * v12.z - v10.z * v12.y;
		n.y = v10.z * v12.x - v10.x * v12.z;
		n.z = v10.x * v12.y - v10.y * v12.x;
	}
	else
	{
		//右下ポリゴン
		XMFLOAT3 v13{};
		v13.x = pos3.x - pos1.x;
		v13.y = pos3.y - pos1.y;
		v13.z = pos3.z - pos1.z;

		//外積
		n.x = v12.y * v13.z - v12.z * v13.y;
		n.y = v12.z * v13.x - v12.x * v13.z;
		n.z = v12.x * v13.y - v12.y * v13.x;
	}

	//高さ取得
	py = -((Position.x - pos1.x) * n.x + (Position.z - pos1.z) * n.z) / n.y + pos1.y;

	// 法線ベクトルを正規化して返す
	float length = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
	RetNormal.x = n.x / length;
	RetNormal.y = n.y / length;
	RetNormal.z = n.z / length;
	return py;
}


bool Terrain::MakeTerrain(const char* hightmapPath, const XMFLOAT3 terrainSize, const char* texturePath) {
	m_size = terrainSize;
	{
		size_t len = strlen(hightmapPath) + 1;//終端分+1
		wchar_t* wcstring = new wchar_t[len];
		size_t word;
		mbstowcs_s(&word, wcstring, len, hightmapPath, _TRUNCATE);
		TexMetadata heightdata;
		ScratchImage heightimage;
		auto hr = LoadFromWICFile(wcstring, WIC_FLAGS_NONE, &heightdata, heightimage);
		if (FAILED(hr))
			return false;
		MakeVertex(*heightimage.GetImage(0, 0, 0), terrainSize);
	}
	//テクスチャ読み込み
	{
		size_t len = strlen(texturePath) + 1;//終端分+1
		wchar_t* wcstring = new wchar_t[len];
		size_t word;
		mbstowcs_s(&word, wcstring, len, texturePath, _TRUNCATE);
		TexMetadata metadata;
		ScratchImage image;
		auto hr = LoadFromWICFile(wcstring, WIC_FLAGS_NONE, &metadata, image);
		if (FAILED(hr))
			return false;
		CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture[0]);
		assert(m_texture);
	}
	Renderer::GetRenderer().CreateVertexShader(&m_vertex_shader, &m_vertex_layout,
		"shader\\vertexLightingVS.cso");
	Renderer::GetRenderer().CreatePixelShader(&m_pixel_shader,
		"shader\\vertexLightingPS.cso");
}

bool Terrain::MakeTerrainAndBlendTexture(const char* hightmapPath, const XMFLOAT3 terrainSize,
	const char* textureOnePath, const char* textureTwoPath, const char* blendTexturePath) {
	m_size = terrainSize;
	{
		size_t len = strlen(hightmapPath) + 1;//終端分+1
		wchar_t* wcstring = new wchar_t[len];
		size_t word;
		mbstowcs_s(&word, wcstring, len, hightmapPath, _TRUNCATE);
		TexMetadata heightdata;
		ScratchImage heightimage;
		auto hr = LoadFromWICFile(wcstring, WIC_FLAGS_NONE, &heightdata, heightimage);
		if (FAILED(hr))
			return false;
		MakeVertex(*heightimage.GetImage(0, 0, 0), terrainSize);
	}
	//テクスチャ1読み込み
	{
		size_t len = strlen(textureOnePath) + 1;//終端分+1
		wchar_t* wcstring = new wchar_t[len];
		size_t word;
		mbstowcs_s(&word, wcstring, len, textureOnePath, _TRUNCATE);
		TexMetadata metadata;
		ScratchImage image;
		auto hr = LoadFromWICFile(wcstring, WIC_FLAGS_NONE, &metadata, image);
		if (FAILED(hr))
			return false;
		CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture[0]);
		assert(m_texture[0]);
	}
	//テクスチャ2読み込み
	{
		size_t len = strlen(textureTwoPath) + 1;//終端分+1
		wchar_t* wcstring = new wchar_t[len];
		size_t word;
		mbstowcs_s(&word, wcstring, len, textureTwoPath, _TRUNCATE);
		TexMetadata metadata;
		ScratchImage image;
		auto hr = LoadFromWICFile(wcstring, WIC_FLAGS_NONE, &metadata, image);
		if (FAILED(hr))
			return false;
		CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture[1]);
		assert(m_texture[1]);
	}
	//テクスチャ割合用テクスチャ読み込み
	{
		size_t len = strlen(blendTexturePath) + 1;//終端分+1
		wchar_t* wcstring = new wchar_t[len];
		size_t word;
		mbstowcs_s(&word, wcstring, len, blendTexturePath, _TRUNCATE);
		TexMetadata metadata;
		ScratchImage image;
		auto hr = LoadFromWICFile(wcstring, WIC_FLAGS_NONE, &metadata, image);
		if (FAILED(hr))
			return false;
		CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture[2]);
		assert(m_texture[2]);
	}//m_HeightTexture


	Renderer::GetRenderer().CreateVertexShader(&m_vertex_shader, &m_vertex_layout,
		"shader\\vertexLightingVS.cso");
	Renderer::GetRenderer().CreatePixelShader(&m_pixel_shader,
		"shader\\blendTexPS.cso");	
}

bool Terrain::MakeTerrainAndBlendTextureDDS(const char* hightmapPath, const XMFLOAT3 terrainSize,
	const char* textureOnePath, const char* textureTwoPath, const char* blendTexturePath) {
	m_size = terrainSize;
	{
		size_t len = strlen(hightmapPath) + 1;//終端分+1
		wchar_t* wcstring = new wchar_t[len];
		size_t word;
		mbstowcs_s(&word, wcstring, len, hightmapPath, _TRUNCATE);
		TexMetadata heightdata;
		ScratchImage heightimage;
		auto hr = LoadFromWICFile(wcstring, WIC_FLAGS_NONE, &heightdata, heightimage);

		if (FAILED(hr))
			return false;
		MakeVertex(*heightimage.GetImage(0, 0, 0), terrainSize);
	}
	//テクスチャ1読み込み
	{
		size_t len = strlen(textureOnePath) + 1;//終端分+1
		wchar_t* wcstring = new wchar_t[len];
		size_t word;
		mbstowcs_s(&word, wcstring, len, textureOnePath, _TRUNCATE);
		TexMetadata metadata;
		ScratchImage image;
		auto hr = LoadFromDDSFile(wcstring, DDS_FLAGS_NONE, &metadata, image);
		if (FAILED(hr))
			return false;
		CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture[0]);
		assert(m_texture[0]);

	}
	//テクスチャ2読み込み
	{
		size_t len = strlen(textureTwoPath) + 1;//終端分+1
		wchar_t* wcstring = new wchar_t[len];
		size_t word;
		mbstowcs_s(&word, wcstring, len, textureTwoPath, _TRUNCATE);
		TexMetadata metadata;
		ScratchImage image;
		auto hr = LoadFromDDSFile(wcstring, DDS_FLAGS_NONE, &metadata, image);
		if (FAILED(hr))
			return false;
		CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture[1]);
		assert(m_texture[1]);
	}
	//テクスチャ割合用テクスチャ読み込み
	{

		size_t len = strlen(blendTexturePath) + 1;//終端分+1
		wchar_t* wcstring = new wchar_t[len];
		size_t word;
		mbstowcs_s(&word, wcstring, len, blendTexturePath, _TRUNCATE);
		TexMetadata metadata;
		ScratchImage image;
		auto hr = LoadFromDDSFile(wcstring, DDS_FLAGS_NONE, &metadata, image);
		if (FAILED(hr))
			return false;
		CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture[2]);
		assert(m_texture[2]);
	}

	{

		size_t len = strlen(hightmapPath) + 1;//終端分+1
		wchar_t* wcstring = new wchar_t[len];
		size_t word;
		mbstowcs_s(&word, wcstring, len, hightmapPath, _TRUNCATE);
		TexMetadata heightdata;
		ScratchImage heightimage;
		auto hr = LoadFromWICFile(wcstring, WIC_FLAGS_NONE, &heightdata, heightimage);

		if (FAILED(hr))
			return false;
		CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), heightimage.GetImages(), heightimage.GetImageCount(), heightdata, &m_bampmap_texture);
		assert(m_bampmap_texture);
	}
	Renderer::GetRenderer().CreateVertexShader(&m_vertex_shader, &m_vertex_layout,
		"shader\\unlitTextureVS.cso");
	Renderer::GetRenderer().CreatePixelShader(&m_pixel_shader,
		"shader\\blendTexPS.cso");
	return true;
}


void Terrain::MakeVertex(const Image& image, XMFLOAT3 TerrainSize) {

	//画像読み取り
	uint8_t* pixels = image.pixels;
	size_t rowPitch = image.rowPitch;

	// 画像の幅と高さ 
	m_image_width = image.width;
	m_image_height = image.height;
	float ValueHeight = TerrainSize.y / m_bit_value;
	m_vertex_pointer = new VERTEX_3D[m_image_width * m_image_height];

	auto mpos = m_transform->GetPosition();

	m_block_size.x = TerrainSize.x / m_image_width;
	m_block_size.y = TerrainSize.z / m_image_height;
	m_offset.x = mpos.x - (m_block_size.x * (m_image_width * 0.5f));
	m_offset.y = mpos.z + (m_block_size.y * (m_image_height * 0.5f));

	//頂点バッファ
	for (unsigned int x = 0; x < m_image_width; x++)
	{
		//外側の数字が1番目の数字
		for (int z = 0; z < m_image_height; z++) {
			size_t index = z * rowPitch + x;
			int node = static_cast<int>(pixels[index]);
			(m_vertex_pointer + ((x * m_image_height) + z))->Position = XMFLOAT3(m_offset.x + (x * m_block_size.x), mpos.y + (ValueHeight * static_cast<int>(pixels[index])), m_offset.y + (z * -m_block_size.y));
			(m_vertex_pointer + ((x * m_image_height) + z))->Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			(m_vertex_pointer + ((x * m_image_height) + z))->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			(m_vertex_pointer + ((x * m_image_height) + z))->TexCoord = XMFLOAT2(0.0f + (1.0f / m_image_width) * x, 0.0f + (1.0f / m_image_height) * z);
		}
	}

	{
		//法線ベクトル算出
		for (int x = 1; x < (m_image_width - 1); x++)
		{
			for (int z = 1; z < (m_image_height - 1); z++)
			{
				XMFLOAT3 vx, vz, vn;
				vx.x = (m_vertex_pointer + ((x + 1) * m_image_height + z))->Position.x - (m_vertex_pointer + ((x - 1) * m_image_height + z))->Position.x;
				vx.y = (m_vertex_pointer + ((x + 1) * m_image_height + z))->Position.y - (m_vertex_pointer + ((x - 1) * m_image_height + z))->Position.y;
				vx.z = (m_vertex_pointer + ((x + 1) * m_image_height + z))->Position.z - (m_vertex_pointer + ((x - 1) * m_image_height + z))->Position.z;

				vz.x = (m_vertex_pointer + (x * m_image_height + (z - 1)))->Position.x - (m_vertex_pointer + (x * m_image_height + (z + 1)))->Position.x;
				vz.y = (m_vertex_pointer + (x * m_image_height + (z - 1)))->Position.y - (m_vertex_pointer + (x * m_image_height + (z + 1)))->Position.y;
				vz.z = (m_vertex_pointer + (x * m_image_height + (z - 1)))->Position.z - (m_vertex_pointer + (x * m_image_height + (z + 1)))->Position.z;

				//外積
				vn.x = vz.y * vx.z - vz.z * vx.y;
				vn.y = vz.z * vx.x - vz.x * vx.z;
				vn.z = vz.x * vx.y - vz.y * vx.x;

				//正規化
				float len = sqrtf(vn.x * vn.x + vn.y * vn.y + vn.z * vn.z);
				vn.x /= len;
				vn.y /= len;
				vn.z /= len;

				(m_vertex_pointer + ((x * m_image_height) + z))->Normal = vn;
			}
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX_3D) * (m_image_width * m_image_height);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd{};
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = m_vertex_pointer;

		Renderer::GetRenderer().GetDevice()->CreateBuffer(&bd, &sd, &m_vertex_buffer);
	}

	{
		// インデックスバッファを格納する
		std::vector<unsigned int> index;
		for (int x = 0; x < (m_image_width - 1); x++)
		{
			for (int z = 0; z < m_image_height; z++)
			{
				index.emplace_back(x * m_image_height + z);
				index.emplace_back((x + 1) * m_image_height + z);
			}
			if (x == (m_image_width - 2)) break;
			index.emplace_back((x + 1) * m_image_height + (m_image_height - 1));
			index.emplace_back((x + 1) * m_image_height);
		}
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = static_cast<UINT>(sizeof(unsigned int) * index.size());
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA sd{};
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = index.data();
		Renderer::GetRenderer().GetDevice()->CreateBuffer(&bd, &sd, &m_index_buffer);
	}
}
