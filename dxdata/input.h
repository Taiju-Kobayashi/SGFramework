/**
 * @file   input.h
 * @brief  キー入力を受け取るクラスを定義したヘッダー
 * @date   2024/07
 */
#pragma once
#include "jsrhdr/JoyShockLibrary.h"

 /*
  *buttons は、以下のマスクでコントローラーのすべてのボタンの状態を含みます
  */
#define JSMASK_UP 0x00001//@brief 十字キー上
#define JSMASK_DOWN 0x00002//@brief 十字キー下
#define JSMASK_LEFT 0x00004//@brief 十字キー左
#define JSMASK_RIGHT 0x00008//@brief 十字キー右
#define JSMASK_PLUS 0x00010//@brief  Nintendoデバイスの＋ボタン、DS4のOptionsボタン
#define JSMASK_OPTIONS 0x00010//@brief  Nintendoデバイスの＋ボタン、DS4のOptionsボタン
#define JSMASK_MINUS 0x00020//@brief  Nintendoデバイスの－ボタン、DS4のShareボタン
#define JSMASK_SHARE 0x00020//@brief Nintendoデバイスの－ボタン、DS4のShareボタン
#define JSMASK_LCLICK 0x00040//@brief Nintendoデバイスの左スティック押し込み、DS4のL3ボタン
#define JSMASK_RCLICK 0x00080//@brief Nintendoデバイスの右スティック押し込み、DS4のR3ボタン
#define JSMASK_L 0x00100//@brief   NintendoデバイスのLボタン、DS4のL1ボタン
#define JSMASK_R 0x00200//@brief　NintendoデバイスのRボタン、DS4のR1ボタン
#define JSMASK_ZL 0x00400//@brief NintendoデバイスのZLボタン、DS4のL2ボタン
#define JSMASK_ZR 0x00800//@brief  NintendoデバイスのZRボタン、DS4のR2ボタン
#define JSMASK_S 0x01000//@brief 南側のフェイスボタン：NintendoデバイスのB、DS4の⨉ボタン
#define JSMASK_E 0x02000//@brief 東側のフェイスボタン：NintendoデバイスのA、DS4の○ボタン
#define JSMASK_W 0x04000//@brief 西側のフェイスボタン：NintendoデバイスのY、DS4の□ボタン
#define JSMASK_N 0x08000//@brief 北側のフェイスボタン：任天堂デバイスのX、DS4の△ボタン
#define JSMASK_HOME 0x10000//@brief ホームボタン
#define JSMASK_PS 0x10000//@brief PSボタン（DualShock 4では存在しない）
#define JSMASK_CAPTURE 0x20000//@brief スクリーンショットボタン（Switch Pro Controllerでは存在しない）
#define JSMASK_TOUCHPAD_CLICK 0x20000// @brief タッチパッドのクリック（DualShock 4では存在しない）
#define JSMASK_MIC 0x40000// @brief マイクボタン（DualShock 4では存在しない）
#define JSMASK_SL 0x40000// @brief スピーカーの左側のボタン（DualShock 4では存在しない）
#define JSMASK_SR 0x80000// @brief スピーカーの右側のボタン（DualShock 4では存在しない）


#define MAX_CONTROLLERS 4 //最大コントローラー数


enum INPUT_MODE {
	INPUT_MODE_KEYBOARD,// キーボードのみを使用
	INPUT_MODE_MOUSE, // マウスのみを使用
	INPUT_MODE_CONTROLLER,// コントローラーのみを使用
	INPUT_MODE_KEYBOARD_MOUSE, // キーボードとマウスの両方を使用
	INPUT_MODE_CONTROLLER_KEYBOARD, // コントローラーとキーボードの両方を使用
	INPUT_MODE_UNKNOWN
};

//接続されているコントローラーの種類
enum CONTROLLER_TYPE { N_SWITCH, S_DS4, S_DS, NONE };

/// @brief　キー入力を受け取るクラス
class Input
{

public:
	static void Init();
	static void Uninit();
	static void Update();

	//インプットモードの取得
	static INPUT_MODE GetInputMode() { return m_input_mode; }

	//キーボード入力
	// キーの押下状態を取得
	static bool GetKeyPress(BYTE KeyCode);
	// キーのトリガー状態を取得（前回から押されたかどうか）
	static bool GetKeyTrigger(BYTE KeyCode);

	//マウス入力
	// マウスの現在位置を取得
	static XMFLOAT2 GetMousePosition();
	// マウスの移動量を取得（前回位置からの変化量）
	static XMFLOAT2 GetMouseDelta();
	// マウス左ボタンの押下状態を取得
	static bool GetMouseLeftButtonPress();
	// マウス左ボタンのトリガー状態を取得
	static bool GetMouseLeftButtonTrigger();
	// マウス右ボタンの押下状態を取得
	static bool GetMouseRightButtonPress();
	// マウス右ボタンのトリガー状態を取得
	static bool GetMouseRightButtonTrigger();


	// Controller Common
	static bool InitJoyShockLibrary(); // JoyShockLibraryの初期化（コントローラー接続の確認）
	static int GetNumConnectedControllers();
	static CONTROLLER_TYPE GetControllerType(int controllerIndex); // コントローラーの種類を取得

	// Controller Buttons
	static JOY_SHOCK_STATE GetJoyState(int controllerIndex); // コントローラーのボタン・スティック・トリガー状態を取得
	static bool GetJoyButtonPress(int controllerIndex, int buttonOffset); // 例: JSOFFSET_S
	static bool GetJoyButtonTrigger(int controllerIndex, int buttonOffset);

	// Controller Sticks (値は -1.0f ～ 1.0f)
	static bool GetJoyStickLeft(int controllerIndex, float& x, float& y);
	static bool GetJoyStickRight(int controllerIndex, float& x, float& y);

	// Controller Triggers (Analog, 値は 0.0f ～ 1.0f)
	static bool GetJoyLTrigger(int controllerIndex, float& value);
	static bool GetJoyRTrigger(int controllerIndex, float& value);

	// Controller Gyro & Accelerometer
	static IMU_STATE GetJoyIMUState(int controllerIndex); // IMUの生の状態を取得
	static bool GetJoyGyro(int controllerIndex, XMFLOAT3& gyro); // ジャイロ値 (rad/s)
	static bool GetJoyAccel(int controllerIndex, XMFLOAT3& accel); // 加速度値 (g)
	//拡張: JoyShockLibraryのIMUデータをEKFで処理した値を取得する関数
	static bool GetEkfGyro(int controllerIndex, float& gyroX, float& gyroY, float& gyroZ);

	static bool GetJoyQuaternion(int controllerIndex, XMFLOAT4& quaternion); // クォータニオン値 (x, y, z, w)

private:
	static INPUT_MODE m_input_mode; // 現在の入力モード（キーボード、マウス、コントローラーなど）
	static BYTE m_old_key_state[256];// 前回のキー状態（押下中かどうか）
	static BYTE m_key_state[256];//キーの状態（押下中かどうか）
	static XMFLOAT2 m_mouse_position; // マウスの現在位置
	static XMFLOAT2 m_mouse_old_position; // マウスの移動量（前回位置からの変化量）
	static bool m_mouse_left_button[2]; // マウス左ボタンの状態（押下中かどうか）
	static bool m_mouse_right_button[2]; // マウス右ボタンの状態（押下中かどうか）

	// Controller
	static int m_connected_device_handles[MAX_CONTROLLERS];// 接続されているコントローラーのハンドル（JoyShockLibrary用）
	static int m_num_connected_controllers;// 接続されているコントローラーの数
	static JOY_SHOCK_STATE m_joy_state[MAX_CONTROLLERS];// コントローラーの状態（ボタン・スティック・トリガー）
	static JOY_SHOCK_STATE m_old_joy_state[MAX_CONTROLLERS];// 前回のコントローラー状態（ボタン・スティック・トリガー）
	static IMU_STATE m_imu_state[MAX_CONTROLLERS]; // IMUデータ用
	static MOTION_STATE m_motion_state[MAX_CONTROLLERS]; // モーションデータ用
	static CONTROLLER_TYPE m_controller_type[MAX_CONTROLLERS]; // コントローラータイプ用
	static bool m_jsl_active; // JoyShockLibraryがアクティブに使用可能か（1台でもコントローラーが接続されていればtrue）

	static XMVECTOR m_prevAngles; // 前回の角度（ジャイロ用）

	static XMVECTOR CalculateGyroAngles(); // ジャイロの角度を計算する関数（内部使用）

};
