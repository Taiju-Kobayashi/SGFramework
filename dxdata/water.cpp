#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "water.h"
#include "manager.h"
#include "scene.h"
#include "input.h"
#include "xmextended.h"

void Water::Init() {
	XMFLOAT3 pos = m_transform->GetPosition();
	m_size.x = 250.0f;
	pos.y += 2.0f;
	m_size.z = 250.0f;


	{
		XMFLOAT2 localsize(m_size.x / g_surface_size, m_size.z / g_surface_size);
		m_block_size = localsize;
		int center = static_cast<int>(g_surface_size * 0.5f) - 1;
		for (int x = 0; x < g_surface_size; x++)
		{

			for (int z = 0; z < g_surface_size; z++) {

				float offsetX = (x - center) * localsize.x;
				float offsetZ = (z - center) * -localsize.y;
				m_vertex_array[0][x][z].Position = XMFLOAT3(pos.x + offsetX, pos.y, pos.z + offsetZ);
				m_vertex_array[0][x][z].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				m_vertex_array[0][x][z].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				m_vertex_array[0][x][z].TexCoord = XMFLOAT2(0.0f + (1.0f / g_surface_size) * x, 0.0f + (1.0f / g_surface_size) * z);
			}
		}
		//法線ベクトル算出
		for (int x = 1; x < (g_surface_size - 1); x++)
		{
			for (int z = 1; z < (g_surface_size - 1); z++) {

				XMFLOAT3 vz, vn;
				vn = XMFloat3Subtract(m_vertex_array[0][x + 1][z].Position, m_vertex_array[0][x - 1][z].Position);
				vz = XMFloat3Subtract(m_vertex_array[0][x][z - 1].Position, m_vertex_array[0][x][z + 1].Position);
				vn = XMFloat3Cross(vn, vz);
				m_vertex_array[0][x][z].Normal = XMFloat3Normalize(vn);
			}
		}
		// Tangent Vectorの算出
		for (int x = 1; x < (g_surface_size - 1); x++)
		{
			for (int z = 1; z < (g_surface_size - 1); z++) {
				XMFLOAT4 tangent{};
				tangent.w = 0.0f;
				XMFLOAT3 edge1, edge2;
				XMFLOAT2 deltaUV1{}, deltaUV2{};

				// 頂点の位置とテクスチャ座標の差分を計算
				edge1 = XMFloat3Subtract(m_vertex_array[0][x + 1][z].Position, m_vertex_array[0][x][z].Position);
				edge2 = XMFloat3Subtract(m_vertex_array[0][x][z + 1].Position, m_vertex_array[0][x][z].Position);

				deltaUV1.x = m_vertex_array[0][x + 1][z].TexCoord.x - m_vertex_array[0][x][z].TexCoord.x;
				deltaUV1.y = m_vertex_array[0][x + 1][z].TexCoord.y - m_vertex_array[0][x][z].TexCoord.y;

				deltaUV2.x = m_vertex_array[0][x][z + 1].TexCoord.x - m_vertex_array[0][x][z].TexCoord.x;
				deltaUV2.y = m_vertex_array[0][x][z + 1].TexCoord.y - m_vertex_array[0][x][z].TexCoord.y;

				// Tangentの計算
				float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

				tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
				tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
				tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

				m_vertex_array[0][x][z].Tangent = XMFloat4Normalize(tangent);
			}
		}

		for (int x = 0; x < g_surface_size; x++)
		{
			for (int z = 0; z < g_surface_size; z++) {
				m_vertex_array[2][x][z] = m_vertex_array[1][x][z] = m_vertex_array[0][x][z];
			}
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * (g_surface_size * g_surface_size);
		bd.BindFlags = D3D11_MAP_WRITE_DISCARD;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA sd{};
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = m_vertex_array;

		Renderer::GetRenderer().GetDevice()->CreateBuffer(&bd, &sd, &m_vertex_buffer);
	}

	{
		//インデックスバッファ生成
		unsigned int index[((g_surface_size + 1) * 2) * (g_surface_size - 1) - 2];
		int i = 0;
		for (int x = 0; x < (g_surface_size - 1); x++)
		{
			for (int z = 0; z < g_surface_size; z++)
			{
				index[i] = x * g_surface_size + z;
				i++;
				index[i] = (x + 1) * g_surface_size + z;
				i++;
			}
			if (x == (g_surface_size - 2)) break;
			index[i] = (x + 1) * g_surface_size + (g_surface_size - 1);
			i++;

			index[i] = (x + 1) * g_surface_size;
			i++;
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(unsigned int) * (((g_surface_size + 1) * 2) * (g_surface_size - 1) - 2);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA sd{};
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = index;

		Renderer::GetRenderer().GetDevice()->CreateBuffer(&bd, &sd, &m_indexbuffer);
		m_now_index = 0;
	}


	//DDSテクスチャ読み込み
	TexMetadata metadata;//
	ScratchImage image;
	LoadFromDDSFile(L"asset\\texture\\clear-sea-water-512x512.dds", DDS_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture);
	assert(m_texture);

	//テクスチャ読み込み
	TexMetadata lmetadata;
	ScratchImage limage;
	LoadFromWICFile(L"asset\\texture\\gray-sea-water-512x512.png", WIC_FLAGS_NONE, &lmetadata, limage);
	CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), limage.GetImages(), limage.GetImageCount(), lmetadata, &m_envtexture);
	assert(m_envtexture);

	Renderer::GetRenderer().CreateVertexShader(&m_vertex_shader, &m_vertex_layout,
		"shader\\waveVS.cso");
	Renderer::GetRenderer().CreatePixelShader(&m_pixel_shader,
		"shader\\wavePS.cso");



	m_offset.x = pos.x - m_size.x * 0.5f;
	m_offset.y = pos.z + m_size.z * 0.5f;
	m_transform->SetPosition(pos);

}

void Water::UnInit() {
	m_vertex_buffer->Release();
	m_indexbuffer->Release();
	m_texture->Release();

	m_vertex_layout->Release();
	m_vertex_shader->Release();
	m_pixel_shader->Release();
	m_envtexture->Release();
}

void Water::Update() {

	m_UvTime += Manager::GetManager().GetDeltaTime();
	Renderer::GetRenderer().SetParameter(XMFLOAT4(m_UvTime, 0.01f, 512.0f, 512.0f));

	BlendVertex();
}

void Water::Draw() {
	//入力レイアウト
	Renderer::GetRenderer().GetDeviceContext()->IASetInputLayout(m_vertex_layout);

	//shader設定
	Renderer::GetRenderer().GetDeviceContext()->VSSetShader(m_vertex_shader, NULL, 0);
	Renderer::GetRenderer().GetDeviceContext()->PSSetShader(m_pixel_shader, NULL, 0);

	//マトリクス
	Renderer::GetRenderer().SetWorldMatrix(m_transform->GetWorldMatrix());


	//頂点バッファ
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetRenderer().GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

	//インデックスバッファ
	Renderer::GetRenderer().GetDeviceContext()->IASetIndexBuffer(m_indexbuffer, DXGI_FORMAT_R32_UINT, 0);


	//マテリアル
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Diffuse = XMFLOAT4(0.517f, 0.635f, 0.831f, 1.0f);
	material.TextureEnable = true;
	Renderer::GetRenderer().SetMaterial(material);

	//テクスチャ
	Renderer::GetRenderer().GetDeviceContext()->PSSetShaderResources(0, 1, &m_texture);
	//テクスチャ
	Renderer::GetRenderer().GetDeviceContext()->PSSetShaderResources(1, 1, &m_envtexture);

	//プリミティブトポロジー
	Renderer::GetRenderer().GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	//ポリゴン描画 
	Renderer::GetRenderer().GetDeviceContext()->DrawIndexed(((g_surface_size + 1) * 2) * (g_surface_size - 1) - 2, 0, 0);//indexbuffur指定の位置で描画

}

bool Water::CheckInWater(GameObject* Target) {
	//OBB
	XMFLOAT3 direction;
	XMFLOAT3 tpos = Target->GetTransform()->GetPosition();
	XMFLOAT3 mpos = m_transform->GetPosition();

	direction.x = tpos.x - mpos.x;
	direction.y = tpos.y - mpos.y;
	direction.z = tpos.z - mpos.z;

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

float Water::GetHeight(XMFLOAT3& Position) {


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
	pos0 = m_vertex_array[m_now_index][x][z].Position;//左上
	pos1 = m_vertex_array[m_now_index][x + 1][z].Position;//右上
	pos2 = m_vertex_array[m_now_index][x][z + 1].Position;//左下
	pos3 = m_vertex_array[m_now_index][x + 1][z + 1].Position;//右下

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
		v10 = XMFloat3Subtract(pos0, pos1);

		//外積
		n = XMFloat3Cross(v10, v12);
	}
	else
	{
		//右下ポリゴン
		XMFLOAT3 v13{};
		v13 = XMFloat3Subtract(pos3, pos1);

		//外積
		n = XMFloat3Cross(v12, v13);
	}

	//高さ取得
	py = -((Position.x - pos1.x) * n.x + (Position.z - pos1.z) * n.z) / n.y + pos1.y;
	return py;
}


float Water::GetHeightAndNormal(XMFLOAT3& Position, XMFLOAT3& RetNormal) {


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
	pos0 = m_vertex_array[m_now_index][x][z].Position;//左上
	pos1 = m_vertex_array[m_now_index][x + 1][z].Position;//右上
	pos2 = m_vertex_array[m_now_index][x][z + 1].Position;//左下
	pos3 = m_vertex_array[m_now_index][x + 1][z + 1].Position;//右下

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
		//v10.x = pos0.x - pos1.x;
		//v10.y = pos0.y - pos1.y;
		//v10.z = pos0.z - pos1.z;
		v10 = XMFloat3Subtract(pos0, pos1);

		//外積
		//n.x = v10.y * v12.z - v10.z * v12.y;
		//n.y = v10.z * v12.x - v10.x * v12.z;
		//n.z = v10.x * v12.y - v10.y * v12.x;
		n = XMFloat3Cross(v10, v12);
	}
	else
	{
		//右下ポリゴン
		XMFLOAT3 v13{};
		//v13.x = pos3.x - pos1.x;
		//v13.y = pos3.y - pos1.y;
		//v13.z = pos3.z - pos1.z;
		v13 = XMFloat3Subtract(pos3, pos1);

		//外積
		//n.x = v12.y * v13.z - v12.z * v13.y;
		//n.y = v12.z * v13.x - v12.x * v13.z;
		//n.z = v12.x * v13.y - v12.y * v13.x;
		n = XMFloat3Cross(v12, v13);
	}

	//高さ取得
	py = -((Position.x - pos1.x) * n.x + (Position.z - pos1.z) * n.z) / n.y + pos1.y;

	// 法線ベクトルを正規化して返す
	//float length = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
	//RetNormal.x = n.x / length;
	//RetNormal.y = n.y / length;
	//RetNormal.z = n.z / length;
	RetNormal = XMFloat3Normalize(n);
	return py;
}

XMFLOAT3 Water::CalculateWaveNormalFrom3Points(
	 XMFLOAT3& offset1_base_xz,
	 XMFLOAT3& offset2_base_xz,
	 XMFLOAT3& offset3_base_xz,
	 XMFLOAT3& object_position
	) {
	// 1. 各オフセット点に対するワールドX,Z座標を計算
	XMFLOAT3 p1_world_xz = XMFLOAT3(object_position.x + offset1_base_xz.x, 0.0f, object_position.z + offset1_base_xz.z);
	XMFLOAT3 p2_world_xz = XMFLOAT3(object_position.x + offset2_base_xz.x, 0.0f, object_position.z + offset2_base_xz.z);
	XMFLOAT3 p3_world_xz = XMFLOAT3(object_position.x + offset3_base_xz.x, 0.0f, object_position.z + offset3_base_xz.z);

	// 2. 各ワールドX,Z座標における波の高さを取得
	// _field->GetHeight()はXMFLOAT3(X,Y,Z)を受け取ってYを返すか、XMFLOAT3(X,Z)を受け取る関数を想定
	// ここでは、GetNormalAndHeightByXZ(const XMFLOAT3& xz_pos)が波のYと法線を返すことを想定
	// もしGetHeightしかなく、法線を別に計算できない場合は、GetHeightを3回呼び出してY座標を取得します。

	// 仮に_field->GetHeight(XMFLOAT3 pos_xz) が波の変位後のY座標を返すとする
	float y1 = GetHeight(p1_world_xz);
	float y2 = GetHeight(p2_world_xz);
	float y3 = GetHeight(p3_world_xz);

	// 3. 3つの3D座標点を構築 (X, Y, Z)
	XMVECTOR P1 = XMVectorSet(p1_world_xz.x, y1, p1_world_xz.z, 1.0f);
	XMVECTOR P2 = XMVectorSet(p2_world_xz.x, y2, p2_world_xz.z, 1.0f);
	XMVECTOR P3 = XMVectorSet(p3_world_xz.x, y3, p3_world_xz.z, 1.0f);

	// 4. 2つの辺ベクトルを計算 (P2-P1, P3-P1)
	XMVECTOR vecA = XMVectorSubtract(P2, P1);
	XMVECTOR vecB = XMVectorSubtract(P3, P1);

	// 5. 外積を計算して法線ベクトルを求める
	// 法線の向きが上方向になるように、ベクトルの順序を調整 (右手の法則)
	// 例えば、P1-P2-P3が時計回りなら vecA x vecB, 反時計回りなら vecB x vecA
	XMVECTOR normalVec = XMVector3Cross(vecB, vecA); // 例: P1-P2-P3が反時計回りに並ぶように設定

	// 6. 正規化
	// エラー防止のため、ゼロベクトルチェックを追加
	if (XMVector3NearEqual(normalVec, XMVectorZero(), XMVectorReplicate(FLT_EPSILON))) {
		// 法線がゼロベクトルになる場合（例: 3点が一直線上にあるか、全て同じ点）
		// デフォルトの上方向を返すか、エラーログを出す
		return XMFLOAT3(0.0f, 1.0f, 0.0f); // デフォルトのY軸上方向
	}
	normalVec = XMVector3Normalize(normalVec);

	XMFLOAT3 resultNormal;
	XMStoreFloat3(&resultNormal, normalVec);
	return resultNormal;
}



bool Water::AddWave(XMFLOAT3 Position) {

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

	m_vertex_array[m_now_index][x][z].Position.y += -0.25f;


	//頂点データ書換
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetRenderer().GetDeviceContext()->Map(m_vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;
	memcpy(vertex, m_vertex_array, sizeof(VERTEX_3D) * g_surface_size * g_surface_size);

	Renderer::GetRenderer().GetDeviceContext()->Unmap(m_vertex_buffer, 0);

	return true;
}



void Water::BlendVertex() {
	Manager& mn = Manager::GetManager();


	// 直前、直後の頂点取得
	int prev = (m_now_index + 2) % 3;
	int next = (m_now_index + 1) % 3;
	// 波の速度、時間の刻み、グリッド幅
	m_wave_dt = mn.GetDeltaTime();
	m_wave_cth = (m_wave_force * m_wave_force * m_wave_dt * m_wave_dt) / (m_wave_width * m_wave_width);

	//波の頂点を更新
	for (int x = 1; x < g_surface_size - 1; x++)
	{
		//外側の数字が1番目の数字
		for (int z = 1; z < g_surface_size - 1; z++) {
			m_vertex_array[next][x][z].Position.y = (m_wave_cth * (m_vertex_array[m_now_index][x + 1][z].Position.y +
				m_vertex_array[m_now_index][x - 1][z].Position.y + m_vertex_array[m_now_index][x][z + 1].Position.y +
				m_vertex_array[m_now_index][x][z - 1].Position.y)) + (2.0f - 4.0f * m_wave_cth) * m_vertex_array[m_now_index][x][z].Position.y - m_vertex_array[prev][x][z].Position.y;
		}
	}

	//法線ベクトル算出
	for (int x = 1; x < (g_surface_size - 1); x++)
	{
		for (int z = 1; z < (g_surface_size - 1); z++) {
			XMFLOAT3 vx, vz, vn;
			vx = XMFloat3Subtract(m_vertex_array[next][x + 1][z].Position, m_vertex_array[next][x - 1][z].Position);
			vz = XMFloat3Subtract(m_vertex_array[next][x][z - 1].Position, m_vertex_array[next][x][z + 1].Position);
			vn = XMFloat3Cross(vz, vx);
			m_vertex_array[next][x][z].Normal = XMFloat3Normalize(vn);
		}
	}
	m_now_index = next;
	//頂点データ書換
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetRenderer().GetDeviceContext()->Map(m_vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;
	memcpy(vertex, m_vertex_array[next], sizeof(VERTEX_3D) * g_surface_size * g_surface_size);

	Renderer::GetRenderer().GetDeviceContext()->Unmap(m_vertex_buffer, 0);

}

bool Water::InitWave() {
	XMFLOAT3 pos = m_transform->GetPosition();
	XMFLOAT2 localsize(m_size.x / g_surface_size, m_size.z / g_surface_size);
	m_block_size = localsize;
	int center = static_cast<int>(g_surface_size * 0.5f) - 1;
	for (int x = 0; x < g_surface_size; x++)
	{

		for (int z = 0; z < g_surface_size; z++) {

			float offsetX = (x - center) * localsize.x;
			float offsetZ = (z - center) * -localsize.y;
			m_vertex_array[0][x][z].Position = XMFLOAT3(pos.x + offsetX, pos.y, pos.z + offsetZ);
			m_vertex_array[0][x][z].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_vertex_array[0][x][z].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			m_vertex_array[0][x][z].TexCoord = XMFLOAT2(0.0f + (1.0f / g_surface_size) * x, 0.0f + (1.0f / g_surface_size) * z);
		}
	}
	//法線ベクトル算出
	for (int x = 1; x < (g_surface_size - 1); x++)
	{
		for (int z = 1; z < (g_surface_size - 1); z++) {

			XMFLOAT3 vz, vn;
			vn = XMFloat3Subtract(m_vertex_array[0][x + 1][z].Position, m_vertex_array[0][x - 1][z].Position);
			vz = XMFloat3Subtract(m_vertex_array[0][x][z - 1].Position, m_vertex_array[0][x][z + 1].Position);
			vn = XMFloat3Cross(vn, vz);
			m_vertex_array[0][x][z].Normal = XMFloat3Normalize(vn);
		}
	}
	// Tangent Vectorの算出
	for (int x = 1; x < (g_surface_size - 1); x++)
	{
		for (int z = 1; z < (g_surface_size - 1); z++) {
			XMFLOAT4 tangent{};
			tangent.w = 0.0f;
			XMFLOAT3 edge1, edge2;
			XMFLOAT2 deltaUV1{}, deltaUV2{};

			// 頂点の位置とテクスチャ座標の差分を計算
			edge1 = XMFloat3Subtract(m_vertex_array[0][x + 1][z].Position, m_vertex_array[0][x][z].Position);
			edge2 = XMFloat3Subtract(m_vertex_array[0][x][z + 1].Position, m_vertex_array[0][x][z].Position);

			deltaUV1.x = m_vertex_array[0][x + 1][z].TexCoord.x - m_vertex_array[0][x][z].TexCoord.x;
			deltaUV1.y = m_vertex_array[0][x + 1][z].TexCoord.y - m_vertex_array[0][x][z].TexCoord.y;

			deltaUV2.x = m_vertex_array[0][x][z + 1].TexCoord.x - m_vertex_array[0][x][z].TexCoord.x;
			deltaUV2.y = m_vertex_array[0][x][z + 1].TexCoord.y - m_vertex_array[0][x][z].TexCoord.y;

			// Tangentの計算
			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

			m_vertex_array[0][x][z].Tangent = XMFloat4Normalize(tangent);
		}
	}

	for (int x = 0; x < g_surface_size; x++)
	{
		for (int z = 0; z < g_surface_size; z++) {
			m_vertex_array[2][x][z] = m_vertex_array[1][x][z] = m_vertex_array[0][x][z];
		}
	}

	//頂点データ書換
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetRenderer().GetDeviceContext()->Map(m_vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;
	memcpy(vertex, m_vertex_array[0], sizeof(VERTEX_3D) * g_surface_size * g_surface_size);

	Renderer::GetRenderer().GetDeviceContext()->Unmap(m_vertex_buffer, 0);
	return true;
}