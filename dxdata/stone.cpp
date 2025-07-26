#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "modelRenderer.h"
#include "audio.h"
#include "scene.h"
#include "terrain.h"
#include "camera.h"
#include "input.h"
#include "stone.h"
#include "splasheEmitter.h"
#include "water.h"
#include "shadowvolume.h"
#include "xmextended.h"
#include "resultInstance.h"
#include "resultscene.h"
#include "animationModel.h"

namespace {
    constexpr float GRAVITY = -9.8f;
    constexpr float FRICTION = 0.5f;
    constexpr float ROTATION_IMPULSE_STRENGTH = 0.002f;
    constexpr float COLLISION_SPEED_THRESHOLD = 10.0f;
}

void Stone::Init() {
	m_modelrenderer = new AnimationModel(this);
	ResultInstance* instance = Manager::GetManager().GetGameInstance<ResultInstance>();

	if (!instance->m_choicestone_path)
		static_cast<AnimationModel*>(m_modelrenderer)->Load_NoBone("asset\\model\\stone.fbx");
	else
		static_cast<AnimationModel*>(m_modelrenderer)->Load_NoBone(instance->m_choicestone_path);

	m_size = static_cast<AnimationModel*>(m_modelrenderer)->GetHarfModelSize();

	m_se = new Audio(this);
	m_se->Load("asset\\sound\\pisya.wav");
	m_size = ((AnimationModel*)m_modelrenderer)->GetHarfModelSize();


	Renderer::GetRenderer().CreateVertexShader(&m_vertex_shader, &m_VertexLayout,
		"shader\\vertexLightingVS.cso");
	Renderer::GetRenderer().CreatePixelShader(&m_pixel_shader,
		"shader\\vertexLightingPS.cso");
	m_transform->SetScale(XMFLOAT3(0.5f, 0.5f, 0.5f));
	m_transform->SetPosition(XMFLOAT3(0.0f, 18.0f, -50.0f));
	m_size.x *= m_transform->GetScale().x;
	m_size.y *= m_transform->GetScale().y;
	m_size.z *= m_transform->GetScale().z;

	m_old_position = m_transform->GetPosition();
	m_start_position = m_transform->GetPosition();
	m_transform->SetRotation(XMFLOAT3(XMConvertToRadians(-20.0f), 0.0f, 0.0f));
	_field = GetParentScene().GetGameObject<Water>();
	m_shadow = GetParentScene().GetGameObject<ShadowVolume>();

	XMFLOAT3 shpos = m_old_position;
	shpos.y = _field->GetHeight(m_old_position);
	if (m_shadow) {
		m_shadow->GetTransform()->SetPosition(shpos);
	}
}

void Stone::UnInit() {
    if (m_modelrenderer) {
        m_modelrenderer->UnInit();
        delete m_modelrenderer;
        m_modelrenderer = nullptr;
    }
    if (m_VertexLayout) {
        m_VertexLayout->Release();
        m_VertexLayout = nullptr;
    }
    if (m_vertex_shader) {
        m_vertex_shader->Release();
        m_vertex_shader = nullptr;
    }
    if (m_pixel_shader) {
        m_pixel_shader->Release();
        m_pixel_shader = nullptr;
    }
}

//ベクトルの長さを計算する
static float get_vector_length(XMFLOAT3 v) {
	return pow((v.x * v.x) + (v.y * v.y) + (v.z * v.z), 0.5);
}


static float IsParallel(const XMFLOAT3& vector1, const XMFLOAT3& vector2) {
	float dotx = (vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z);
	float cos = dotx / (get_vector_length(vector1) * get_vector_length(vector2));
	return cos;//XMConvertToDegrees(acosf(cos)); // 小さな閾値で平行判定
}

void Stone::Update() {

	if (!m_release)return;

	float dt = Manager::GetManager().GetDeltaTime();
	Scene& scene = Manager::GetManager().GetNowScene();

	XMFLOAT3 position = m_transform->GetPosition();


	//=====いったん消す
		//摩擦
	m_velocity.x -= (m_velocity.x * FRICTION) * dt;
	m_velocity.z -= (m_velocity.z * FRICTION) * dt;

	m_rotation_speed.x -= (m_rotation_speed.x * FRICTION) * dt;
	m_rotation_speed.y -= (m_rotation_speed.y * FRICTION) * dt;
	m_rotation_speed.z -= (m_rotation_speed.z * FRICTION) * dt;

	//重力加速度
	m_velocity.y += GRAVITY * dt;

	//移動処理
	XMFLOAT3 mscale = m_transform->GetScale();
	XMFLOAT3 oldposition = position;
	XMFLOAT3 terrpos = position; // terrposをpositionのコピーとして初期化

	// 回転処理（既に存在する回転速度を適用）
	XMFLOAT3 rot = m_transform->GetRotation();
	// m_rotation_speedがオイラー角の差分であると仮定
	rot.x += m_rotation_speed.x * dt;
	rot.y += m_rotation_speed.y * dt;
	rot.z += m_rotation_speed.z * dt;
	m_transform->SetRotation(rot); // 回転速度による回転を適用
	XMFLOAT3 fnormal;

	// --- ここから新しい法線計算ロジックの適用 ---
	XMFLOAT3 calculatedFnormal; // 新しい法線計算結果を格納する変数

	// 3点のオフセットを定義
	XMFLOAT3 offset_center = XMFLOAT3(0.0f, 0.0f, m_size.z); // OBBの中心
	XMFLOAT3 offset_bow = XMFLOAT3(m_size.x, 0.0f, 0.0f);   // 左舷方向（+x方向）
	XMFLOAT3 offset_starboard = XMFLOAT3(-m_size.x, 0.0f, 0.0f); // 右舷方向（-X方向）

	// 3点から平面の法線を計算
	fnormal = _field->CalculateWaveNormalFrom3Points(
		offset_center, offset_bow, offset_starboard,position);
	
	// --- 新しい法線計算ロジックここまで ---

	// ここでterrposは波の高さと法線を取得するための参照点として使用
	// _field->GetHeightAndNormalがpositionに基づいてfnormalを更新してくれることを期待
	// fnormalは、オブジェクトの重心位置における波の法線として利用されます。
	// 戦艦のように大きい場合は、重心位置だけでなく、接地している範囲の平均的な法線を計算する方が良いです。
	// 現状のGetHeightAndNormalはpositionの点における局所的な法線なので、大きなオブジェクトには問題が生じます。
	// ここでは、一旦fnormalがオブジェクト全体にとって妥当な波の法線であると仮定して進めます。
	terrpos.y = _field->GetHeight(position); // terrpos.yが波の高さになり、fnormalが波の法線になる

	if (m_shadow) {
		if (position.y < terrpos.y) {
			m_shadow->GetTransform()->SetPosition(position); // シャドウの位置を現在のオブジェクト位置に合わせる
		}
		else {
			m_shadow->GetTransform()->SetPosition(position); // 上記if/elseブロックは同じ処理なので簡略化可能
		}
	}

	if (m_shadow) {
		m_shadow->GetTransform()->SetPosition(position);
	}

	// 衝突判定は速度が速い場合のみ行う
	if (XMFloat3Magnitude(m_velocity) > COLLISION_SPEED_THRESHOLD) {
		// OBBと波（またはテレイン）との衝突判定
		// directionはterrpos（波の表面の高さ）とposition（オブジェクトのY位置）の差から計算される
		XMFLOAT3 direction = XMFloat3Subtract(terrpos, position);

		float length = get_vector_length(direction); // オブジェクトの中心から波の表面までの距離

		// OBBの軸取得
		XMFLOAT3 axisX = m_transform->GetRight();
		XMFLOAT3 axisY = m_transform->GetUp();
		XMFLOAT3 axisZ = m_transform->GetForward();

		// 分離軸の原理に基づいた内積計算（OBBの軸と法線の投影長）
		// radは、法線方向へのOBBの投影半径に相当
		float rad = 0.0f;
		XMFLOAT3 dox(axisX.x * m_size.x, axisX.y * m_size.x, axisX.z * m_size.x);
		rad += fabs(XMFloat3Dot(dox, fnormal));
		dox = XMFLOAT3(axisY.x * m_size.y, axisY.y * m_size.y, axisY.z * m_size.y);
		rad += fabs(XMFloat3Dot(dox, fnormal));
		dox = XMFLOAT3(axisZ.x * m_size.z, axisZ.y * m_size.z, axisZ.z * m_size.z);
		rad += fabs(XMFloat3Dot(dox, fnormal));

		// 衝突判定: オブジェクトの中心から波の表面までの距離がOBBの投影半径よりも小さい場合
		if (fabs(length) < rad) {
			XMFLOAT3 outNormal, outAxis;
			outNormal = XMFloat3Normalize(fnormal); // 衝突した面の法線（波の法線）
			outAxis = XMFloat3Normalize(axisZ);     // オブジェクトのフォワード軸

			float Culout = fabsf(IsParallel(outAxis, outNormal)); // 衝突角度の平行度合い？

			// --- ここから回転に関するロジックを追加/修正 ---

			// 衝突による反発速度の計算（既存）
			XMFLOAT3 velo{};
			velo.x = m_velocity.x - (1 + (m_reboundrate - Culout)) * XMFloat3Dot(fnormal, m_velocity) * fnormal.x;
			velo.y = m_velocity.y - (1 + (m_reboundrate - Culout)) * XMFloat3Dot(fnormal, m_velocity) * fnormal.y;
			velo.z = m_velocity.z - (1 + (m_reboundrate - Culout)) * XMFloat3Dot(fnormal, m_velocity) * fnormal.z;

			// オブジェクトの現在の速度と衝突法線fnormalを使用して、衝突による回転（角運動量）を計算
			// 簡略化された物理モデル
			XMVECTOR currentVelocity = XMLoadFloat3(&m_velocity);
			XMVECTOR collisionNormal = XMLoadFloat3(&fnormal);

			// 衝突点とオブジェクト重心間の相対位置ベクトル（簡略化：OBBの底面中心から重心へ）
			// 実際には衝突したOBBの最も低い点から重心へのベクトルが理想
			// ここでは便宜上、水平方向の衝撃に注目し、衝突点と重心の垂直方向のズレを無視します。
			// もしm_transform->GetPosition()が重心であれば、positionとcollisionPoint(接触した点)の差を用いるべき。
			// ここでは、衝突法線と水平速度の外積をトルクとして扱う。
			XMVECTOR horizontalVelocity = XMVectorSet(currentVelocity.m128_f32[0], 0.0f, currentVelocity.m128_f32[2], 0.0f);

			// 衝突による回転軸の方向を計算（トルクの方向）
			// 水平速度ベクトルと衝突法線の外積が、回転軸の方向を示す。
			// 例：右から左へぶつかり、水面が奥に傾いている場合、手前に回転させるトルクが発生する
			XMVECTOR impulseRotationAxis = XMVector3Normalize(XMVector3Cross(horizontalVelocity, collisionNormal));

			// 衝突による回転速度の大きさ（スカラー）
			// 衝突の強さ（法線方向の速度成分）と、水平方向の速度、オブジェクトのサイズなどを考慮
			float impactSpeed = XMFloat3Dot(fnormal, m_velocity); // 法線方向の衝突速度
			float impulseMagnitude = XMVector3Length(horizontalVelocity).m128_f32[0] * fabs(impactSpeed);

			// ここで調整パラメータを設定。数値が小さいほど回転しにくい
			// ROTATION_IMPULSE_STRENGTHを使用
			float rotationImpulseStrength = ROTATION_IMPULSE_STRENGTH; // 定数を使用

			// 角速度の増分
			// m_rotation_speedはオイラー角の回転速度と仮定されているため、
			// このトルクを直接m_rotation_speedに変換するのは少しトリッキー。
			// 最も簡単な方法は、impulseRotationAxisをm_rotation_speedに追加すること。
			// ただし、XMVECTORをXMFLOAT3に変換して追加する必要がある。

			XMFLOAT3 addedRotationSpeed;
			XMStoreFloat3(&addedRotationSpeed, impulseRotationAxis * impulseMagnitude * rotationImpulseStrength);

			// 既存のm_rotation_speedに加算
			// 既存のm_rotation_speedがX,Y,Z軸周りの回転速度を意味していると仮定
			m_rotation_speed.x += addedRotationSpeed.x;
			m_rotation_speed.y += addedRotationSpeed.y;
			m_rotation_speed.z += addedRotationSpeed.z;

			// --- 回転に関するロジック終了 ---

			m_velocity = velo; // 更新された速度を適用
			m_se->Play(); // 効果音再生

			if (!m_iswater_touch) {
				auto emit = scene.AddGameObject<splasheEmitter>(1);
				emit->GetTransform()->SetPosition(position);
				emit->SetForse(fabs(velo.y) / 3);
				emit->SetUse(true);
				m_bound_count++;
				_field->AddWave(position); // 水面に波紋を追加
			}
			m_iswater_touch = true;
		}
		else {
			m_iswater_touch = false;
		}
	}

	// 移動処理（更新された速度で位置を更新）
	position.x += m_velocity.x * dt;
	position.y += m_velocity.y * dt;
	position.z += m_velocity.z * dt;
	m_transform->SetPosition(position); // ここでオブジェクトの位置を更新

	// 地面（または水面）との接地判定と調整
	Terrain* _field_check = GetParentScene().GetGameObject<Terrain>(); // 新しい変数名で衝突防止
	if (_field_check) {
		if (!_field_check->CheckInTerrain(this)) return;

		// オブジェクトの現在の位置における波の高さを再取得
		// ここで取得するfnormalは、オブジェクトのY位置合わせに使う
		// 回転用には上記の衝突判定ブロックで計算されたfnormalが使われます。
		// 戦艦のような大きなオブジェクトの場合、positionの単一の点ではなく、
		// 船体の広がりを考慮した平均的な波の法線を別途計算し、
		// それにオブジェクトの回転を合わせる方が自然です。
		// 現状のGetHeightAndNormalは局所的な値なので、後述の「長期的な傾き」セクションを参照してください。
		XMFLOAT3 currentSurfaceNormal; // この時点の法線は、オブジェクトのY位置合わせに使う
		float GroundHeight = _field_check->GetHeightAndNormal(position, currentSurfaceNormal) + (mscale.y * 0.5f);

		if (position.y <= GroundHeight) {
			position.y = GroundHeight; // 波の表面にY座標を固定
			m_velocity.y = 0.0f;       // Y速度をゼロに

			ResultInstance* instance = Manager::GetManager().GetGameInstance<ResultInstance>();
			instance->m_bound = m_bound_count;
			instance->m_length = get_vector_length(XMFloat3Subtract(m_transform->GetPosition(), m_start_position));
			Manager::GetManager().ChangeScene();
		}
	}
	// 最終的な位置の適用（上記ifブロックでposition.yが変更された場合を考慮）
	m_transform->SetPosition(position);
}

void Stone::Draw() {
	//入力レイアウト
	Renderer::GetRenderer().GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//shader設定
	Renderer::GetRenderer().GetDeviceContext()->VSSetShader(m_vertex_shader, NULL, 0);
	Renderer::GetRenderer().GetDeviceContext()->PSSetShader(m_pixel_shader, NULL, 0);

	//マトリクス設定
	Renderer::GetRenderer().SetWorldMatrix(m_transform->GetWorldMatrix());


	//マテリアル
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::GetRenderer().SetMaterial(material);

	m_modelrenderer->Draw();
}

void Stone::AddForce(const XMFLOAT3& force) {
	m_velocity = XMFloat3Add(m_velocity, force);
}

void Stone::AddRotForce(const XMFLOAT3& force) {
	m_rotation_speed = force;
}
