#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "scene.h"

void Camera::Init() {
	auto trf = GetTransform();
	m_target = XMFLOAT3(0, 0, 0);
	trf->SetPosition(XMFLOAT3(0, -1.5f, -3.0f)); // 初期カメラ位置を少しずらす

	// カメラの回転クォータニオンの初期化
   // 初期状態では回転なし（単位クォータニオン）
	XMStoreFloat4(&m_cameraRotationQuaternion, XMQuaternionIdentity());
	m_cameraDistance = 25.0f; // 被写体からの初期距離
}
void Camera::UnInit() {
}

void Camera::Update() {


    // --- 回転クォータニオンの更新 ---
    XMVECTOR currentQuaternion = XMLoadFloat4(&m_cameraRotationQuaternion);

#ifdef _DEBUG
    // --- ユーザー入力によるカメラ回転・ズーム制御 (例: マウス入力) ---
    float rotationSpeed = 0.005f; // マウス感度調整用
    float zoomSpeed = 0.1f;       // ズーム速度の調整



    // デバッグ用
    float mouseDeltaX = 0.0f;
    float mouseDeltaY = 0.0f;
    float mouseWheelDelta = 0.0f;

    if (Input::GetKeyPress('K')) { // 右回転
        mouseDeltaX = 5.0f;
    }
    if (Input::GetKeyPress('H')) { // 左回転
        mouseDeltaX = -5.0f;
    }
    if (Input::GetKeyPress('U')) { // 上回転
        mouseDeltaY = -5.0f; // DirectXではY軸上方向が正、下方向が負なので、上を向くにはマイナス
    }
    if (Input::GetKeyPress('J')) { // 下回転
        mouseDeltaY = 5.0f;
    }

    if (Input::GetKeyPress('I')) { // ズームイン
        mouseWheelDelta += 1.0f;
    }
    if (Input::GetKeyPress('O')) { // ズームアウト
        mouseWheelDelta += -1.0f;
    }



    // Y軸（ワールドの上方向）周りの回転 (ヨー)
    // マウスのX移動量に対応
    XMVECTOR yawQuaternion = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), mouseDeltaX * rotationSpeed);

    // X軸（カメラのローカルX軸）周りの回転 (ピッチ)
    // マウスのY移動量に対応
    // ここが重要: ピッチ回転は、ワールドX軸ではなく、現在のカメラの「右」方向（ローカルX軸）周りに行う。
    // そのためには、現在のクォータニオンを回転行列に変換し、そのX軸ベクトルを取得するか、
    // または Yaw を適用した後のローカルX軸を回転軸とします。
    // 一般的なタンブルカメラでは、先にヨー回転でカメラの左右を定め、そのローカルX軸でピッチをかけるのが自然です。

    // 今回はシンプルに、ワールドX軸周りの回転を合成してみます。
    // ジンバルロックのリスクは減りますが、マウスY移動に対する反応が少し異なる場合があります。
    XMVECTOR pitchQuaternion = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), mouseDeltaY * rotationSpeed);


    // 回転クォータニオンの合成
    // 新しい回転を既存のクォータニオンに適用します。
    // XMQuaternionMultiply(Q1, Q2) は Q1 * Q2 で、Q2が先に適用される
    currentQuaternion = XMQuaternionMultiply(pitchQuaternion, currentQuaternion); // ピッチを先に適用
    currentQuaternion = XMQuaternionMultiply(currentQuaternion, yawQuaternion);   // ヨーを後に適用

    // クォータニオンの正規化 (浮動小数点誤差の蓄積を防ぐため)
    currentQuaternion = XMQuaternionNormalize(currentQuaternion);
    XMStoreFloat4(&m_cameraRotationQuaternion, currentQuaternion);

    // --- ズームイン/アウト (カメラ距離の変更) ---
    m_cameraDistance -= mouseWheelDelta * zoomSpeed;
#endif // !DEBUG

    m_cameraDistance = XMMin(m_cameraDistance, m_distance_range.y); // 最大距離
    m_cameraDistance = XMMax(m_cameraDistance, m_distance_range.x);   // 最小距離

    // --- カメラの位置の計算 ---
    XMVECTOR lookAt = XMLoadFloat3(&m_target);

    // 初期カメラ位置 (被写体からZ軸負方向にg_cameraDistance離れた位置)
    XMVECTOR initialCameraPosRelative = XMVectorSet(0.0f, 0.0f, -m_cameraDistance, 1.0f);

    // クォータニオンを使って、初期相対位置を回転
    // XMVector3Rotate(V, Q) はベクトル V をクォータニオン Q で回転させる
    XMVECTOR rotatedCameraPosRelative = XMVector3Rotate(initialCameraPosRelative, currentQuaternion);

    // 被写体の位置を考慮して、実際のワールド座標におけるカメラ位置を決定
    XMVECTOR cameraPos = rotatedCameraPosRelative + lookAt;

    // GetTransform() に位置を設定
    GetTransform()->SetPosition(XMFLOAT3(XMVectorGetX(cameraPos), XMVectorGetY(cameraPos), XMVectorGetZ(cameraPos)));
}

void Camera::Draw() {
	
     // ビューマトリクス設定
    XMVECTOR cameraPos = XMLoadFloat3(&GetTransform()->GetPosition());
    XMVECTOR lookAt = XMLoadFloat3(&m_target);
    XMVECTOR currentQuaternion = XMLoadFloat4(&m_cameraRotationQuaternion);

    // カメラの上方向ベクトルをクォータニオンで回転させて求める
    // ワールドY軸上方向 (0, 1, 0) を初期の上方向ベクトルとし、
    // 現在のカメラの回転クォータニオンで回転させます。
    XMVECTOR initialUpVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR up = XMVector3Rotate(initialUpVector, currentQuaternion);


    XMMATRIX viewMatrix = XMMatrixLookAtLH(cameraPos, lookAt, up);
    XMStoreFloat4x4(&m_viewmatrix, viewMatrix);

    // プロジェクションマトリクス設定 (変更なし)
    XMMATRIX projectionMatrix;
    projectionMatrix = XMMatrixPerspectiveFovLH(1.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f);

    Renderer::GetRenderer().SetWorldViewProjectionData3D(viewMatrix, projectionMatrix);
    Renderer::GetRenderer().SetViewMatrix(viewMatrix);
    Renderer::GetRenderer().SetProjectionMatrix(projectionMatrix);
    Renderer::GetRenderer().SetCameraPosition(GetTransform()->GetPosition());
}


void Camera::SetPosition(XMFLOAT3 camPos, XMFLOAT3 Target) {
    // XMVectorに変換
    XMVECTOR vec1 = XMLoadFloat3(&camPos);
    XMVECTOR vec2 = XMLoadFloat3(&Target);
    XMVECTOR diff = XMVectorSubtract(vec1, vec2);
    XMVECTOR lengthSq = XMVector3LengthSq(diff); // 距離の二乗を計算
    // 結果を取得
    float distance;
    XMStoreFloat(&distance, lengthSq);
    if (distance < 0.1f) {
		camPos.z -= 0.1f; // 距離が近すぎる場合は、カメラ位置を少しずらす
    }
	GetTransform()->SetPosition(camPos);
	m_target = Target;
}

void Camera::SetDistance(float distance) {
    m_cameraDistance = distance;
    // 距離のクランプ
    m_cameraDistance = XMMin(m_cameraDistance, m_distance_range.y);
    m_cameraDistance = XMMax(m_cameraDistance, m_distance_range.x);
}

void Camera::AddDistance(float deltaDistance) {
    m_cameraDistance += deltaDistance;
    // 距離のクランプ
    m_cameraDistance = XMMin(m_cameraDistance, m_distance_range.y);
    m_cameraDistance = XMMax(m_cameraDistance, m_distance_range.x);
}

// オイラー角の変化量をクォータニオンに変換して適用する便利な関数
void Camera::ApplyRotationDelta(float deltaPitch, float deltaYaw, float deltaRoll) {
    XMVECTOR currentQ = XMLoadFloat4(&m_cameraRotationQuaternion);

    // 新しい回転クォータニオンを生成
    XMVECTOR deltaQ = XMQuaternionRotationRollPitchYaw(deltaPitch, deltaYaw, deltaRoll);

    // 現在の回転に新しい回転を合成
    // deltaQ * currentQ の順序で乗算（deltaQがcurrentQのローカル空間で適用される）
    currentQ = XMQuaternionMultiply(deltaQ, currentQ);

    // 正規化
    currentQ = XMQuaternionNormalize(currentQ);
    XMStoreFloat4(&m_cameraRotationQuaternion, currentQ);
}

void Camera::SetRotationDelta(float deltaPitch, float deltaYaw, float deltaRoll) {
    // 新しい回転クォータニオンを生成
    XMVECTOR deltaQ = XMQuaternionRotationRollPitchYaw(deltaPitch, deltaYaw, deltaRoll);

    // 正規化
    deltaQ = XMQuaternionNormalize(deltaQ);
    XMStoreFloat4(&m_cameraRotationQuaternion, deltaQ);
}

// 直接クォータニオンを追加する関数
void Camera::ApplyRotationQuaternion(const XMFLOAT4& deltaQuaternion) {
    XMVECTOR currentQ = XMLoadFloat4(&m_cameraRotationQuaternion);  
    XMVECTOR deltaQ = XMLoadFloat4(&deltaQuaternion);

    // deltaQ * currentQ の順序で乗算
    currentQ = XMQuaternionMultiply(deltaQ, currentQ);

    // 正規化
    currentQ = XMQuaternionNormalize(currentQ);
    XMStoreFloat4(&m_cameraRotationQuaternion, currentQ);
}