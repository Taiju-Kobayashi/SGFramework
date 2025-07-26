#include "main.h"
#include "renderer.h"
#include "splasheEmitter.h"
#include "scene.h"
#include "camera.h"
#include "manager.h"
#include "input.h"
#include "xmExtended.h"

void splasheEmitter::Init() {

	VERTEX_3D vertex[4]{};

	vertex[0].Position = XMFLOAT3(-0.5f, 0.5f, 0);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(0, 0);

	vertex[1].Position = XMFLOAT3(0.5f, 0.5f, 0);
	vertex[1].Normal = XMFLOAT3(0, 1.0f, 0);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0);

	vertex[2].Position = XMFLOAT3(-0.5f, -0.5f, 0);
	vertex[2].Normal = XMFLOAT3(0, 1.0f, 0);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(0, 1.0f);

	vertex[3].Position = XMFLOAT3(0.5f, -0.5f, 0);
	vertex[3].Normal = XMFLOAT3(0, 1.0f, 0);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);


	//頂点バッファ生成　anime
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;//DEFALUT からDYNAMICに置き換える
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//データを書き込める状態にする
	//書き込みやすいが描画効率が下がる

	D3D11_SUBRESOURCE_DATA sd{};
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	Renderer::GetRenderer().GetDevice()->CreateBuffer(&bd, &sd, &m_vertex_buffer);

	//テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"asset\\texture\\particle.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(Renderer::GetRenderer().GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_texture);
	assert(m_texture);

	Renderer::GetRenderer().CreateVertexShader(&m_vertex_shader, &m_vertex_layout,
		"shader\\particleVS.cso");
	Renderer::GetRenderer().CreatePixelShader(&m_pixel_shader,
		"shader\\particlePS.cso");
	Renderer::GetRenderer().CreateSrvStructureBuffer(m_position_Buffer, m_positionSRV, sizeof(XMFLOAT4), m_particle_max);
	m_transform->SetScale(XMFLOAT3(10.0f, 10.0f, 10.0f));
}

void splasheEmitter::UnInit() {
	m_vertex_buffer->Release();
	m_texture->Release();

	m_vertex_layout->Release();
	m_vertex_shader->Release();
	m_pixel_shader->Release();
}

void splasheEmitter::Update() {
	if (!m_use)return;
	auto mPos = m_transform->GetPosition();
	//パーティクル発射
	for (int x = 0; x < m_particle_max; x++) {
		if (m_particle_array[x].Enable == false && m_particle_counter < m_particle_max) {
			m_particle_array[x].Enable = true;
			float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * XM_PI;
			m_particle_array[x].Enable = true;
			m_particle_array[x].Position.y = mPos.y;
			XMFLOAT3 vn(cos(angle) * m_radius, (rand() % 100 + 50) / 100.0f, sin(angle) * m_radius);
			m_particle_array[x].Position.x = mPos.x + vn.x;
			m_particle_array[x].Position.z = mPos.z + vn.z;
			float size = static_cast<float>(rand() % 10) * 0.1f;
			m_particle_array[x].MaxSize = XMFLOAT2(size, size);
			m_particle_array[x].MinSize = XMFLOAT2(size * 0.1f, size * 0.1f);
			vn = XMFloat3Normalize(vn);
			m_particle_array[x].Velocity.x = vn.x * m_particle_force;
			m_particle_array[x].Velocity.y = m_particle_force;
			m_particle_array[x].Velocity.z = vn.z * m_particle_force;
			m_particle_counter++;
			m_particle_array[x].life = m_particle_life_time;
			break;
		}
	}
	float dt = Manager::GetManager().GetDeltaTime();

	//パーティクル移動
	for (int x = 0; x < m_particle_max; x++) {
		if (m_particle_array[x].Enable != false) {
			m_particle_array[x].Velocity.y += -0.98f * dt;

			m_particle_array[x].Position.x += m_particle_array[x].Velocity.x * dt;
			m_particle_array[x].Position.y += m_particle_array[x].Velocity.y * dt;
			m_particle_array[x].Position.z += m_particle_array[x].Velocity.z * dt;
			m_particle_array[x].life--;
			m_particle_array[x].Size.x = std::lerp(m_particle_array[x].MinSize.x, m_particle_array[x].MaxSize.x, m_particle_array[x].life / m_particle_life_time);
			m_particle_array[x].Size.y = std::lerp(m_particle_array[x].MinSize.y, m_particle_array[x].MaxSize.y, m_particle_array[x].life / m_particle_life_time);
			if (m_particle_array[x].life <= 0) {
				m_particle_array[x].Enable = false;

				if (m_particle_max > m_particle_counter)
					return;
				SetDestroy();

			}

		}
	}
}

void splasheEmitter::Draw() {
	if (!m_use)return;

	//入力レイアウト
	Renderer::GetRenderer().GetDeviceContext()->IASetInputLayout(m_vertex_layout);

	//shader設定
	Renderer::GetRenderer().GetDeviceContext()->VSSetShader(m_vertex_shader, nullptr, 0);
	Renderer::GetRenderer().GetDeviceContext()->PSSetShader(m_pixel_shader, nullptr, 0);


	//ビルボード処理 //逆行列ver 天地行列の方法もあり

		//カメラのviewマトリクス取得
	Scene& _scene = Manager::GetManager().GetNowScene();
	Camera* cam = _scene.GetGameObject<Camera>();
	XMMATRIX view = cam->GetViewMatrix();

	//ビューの逆行列
	XMMATRIX inView;
	inView = XMMatrixInverse(nullptr, view);//逆行列
	//移動成分を無効化
	inView.r[3].m128_f32[0] = 0.0f;
	inView.r[3].m128_f32[1] = 0.0f;
	inView.r[3].m128_f32[2] = 0.0f;



	//頂点バッファ
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetRenderer().GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);


	//マテリアル
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(0.517f, 0.635f, 0.831f, 1.0f);//1色より他色を混ぜる事で明るく見せる
	material.TextureEnable = true;
	Renderer::GetRenderer().SetMaterial(material);

	//テクスチャ
	Renderer::GetRenderer().GetDeviceContext()->PSSetShaderResources(0, 1, &m_texture);

	//プリミティブトポロジー
	Renderer::GetRenderer().GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//zバッファ無効
	Renderer::GetRenderer().SetDepthEnable(false);

	//加算合成有効化
	Renderer::GetRenderer().SetBlendAddEnable(true);
	Renderer::GetRenderer().SetWorldViewProjection3D();


	XMFLOAT3 lscale = m_transform->GetScale();
	for (int v = 0; v < m_particle_max; v++)
	{
		//マトリクス //ループ
		if (m_particle_array[v].Enable == true) {
			XMMATRIX world, scale, rot, trans;
			scale = XMMatrixScaling(m_particle_array[v].Size.x * lscale.x, m_particle_array[v].Size.y * lscale.y, lscale.z);
			trans = XMMatrixTranslation(m_particle_array[v].Position.x, m_particle_array[v].Position.y, m_particle_array[v].Position.z);
			world = scale * inView * trans;
			Renderer::GetRenderer().SetWorldMatrix(world);

			//ポリゴン描画
			Renderer::GetRenderer().GetDeviceContext()->Draw(4, 0);
		}
	}
	Renderer::GetRenderer().SetBlendAddEnable(false);
	Renderer::GetRenderer().SetDepthEnable(true);
}
