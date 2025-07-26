/**
 * @file   XMExtended.h
 * @brief  DirectXMath の拡張機能を提供するヘッダーファイル
 * @author Taiju Kobayashi
 * @date   2024/12
 */

#pragma once
#ifndef _XMEXTENDED_H_
#define _XMEXTENDED_H_

#include <DirectXMath.h>
#include <cmath>



 /// @brief 2つの XMFLOAT2 ベクトルのドット積を計算します
 /// @param v1 1番目のベクトル
 /// @param v2 2番目のベクトル
 /// @return ドット積の結果
static float XMFloat2Dot(XMFLOAT2& In1, XMFLOAT2& In2) {
	float Out;
	XMStoreFloat(&Out, XMVector2Dot(XMLoadFloat2(&In1), XMLoadFloat2(&In2)));
	return Out;
}

/// @brief 2つの XMFLOAT3 ベクトルのドット積を計算します
/// @param v1 1番目のベクトル
/// @param v2 2番目のベクトル
/// @return ドット積の結果
static float XMFloat3Dot(XMFLOAT3& In1, XMFLOAT3& In2) {
	float Out;
	XMStoreFloat(&Out, XMVector3Dot(XMLoadFloat3(&In1), XMLoadFloat3(&In2)));
	return Out;
}

/// @brief 2つの XMFLOAT3 ベクトルのクロス積を計算します
/// @param v1 1番目のベクトル
/// @param v2 2番目のベクトル
/// @return クロス積の結果
static XMFLOAT3 XMFloat3Cross(XMFLOAT3& In1, XMFLOAT3& In2) {
	XMFLOAT3 Out;
	XMStoreFloat3(&Out, XMVector3Cross(XMLoadFloat3(&In1), XMLoadFloat3(&In2)));
	return Out;
}

/// @brief XMFLOAT3正規化
/// @param In 正規化するベクトル
/// @return 正規化の結果
static XMFLOAT3 XMFloat3Normalize(XMFLOAT3& In) {
	XMFLOAT3 Out;
	XMStoreFloat3(&Out, XMVector3Normalize(XMLoadFloat3(&In)));
	return Out;
};

/// @brief XMFLOAT4正規化
/// @param In 正規化するベクトル
/// @return 正規化の結果
static XMFLOAT4 XMFloat4Normalize(XMFLOAT4& In) {
	XMFLOAT4 Out;
	XMStoreFloat4(&Out, XMVector4Normalize(XMLoadFloat4(&In)));
	return Out;
};

/// @brief XMFLOAT3 ベクトルの長さを計算
/// @param v ベクトル
/// @return ベクトルの長さ
static float XMFloat3Magnitude(const XMFLOAT3& In) {
	return XMVectorGetX(XMVector3Length(XMLoadFloat3(&In)));
}

/// @brief 2つの XMFLOAT3の加算処理
/// @param v1 1番目のXMFLOAT3 
/// @param v2 1番目のXMFLOAT3
/// @return 加算結果
static XMFLOAT3 XMFloat3Add(const XMFLOAT3& v1, const XMFLOAT3& v2) {
	XMFLOAT3 Out;
	XMStoreFloat3(&Out, XMVectorAdd(XMLoadFloat3(&v1), XMLoadFloat3(&v2)));
	return Out;
}

/// @brief 2つの XMFLOAT4の加算処理
/// @param v1 1番目のXMFLOAT4
/// @param v2 1番目のXMFLOAT4
/// @return 加算結果
static XMFLOAT4 XMFloat4Add(const XMFLOAT4& v1, const XMFLOAT4& v2) {
	XMFLOAT4 Out;
	XMStoreFloat4(&Out, XMVectorAdd(XMLoadFloat4(&v1), XMLoadFloat4(&v2)));
	return Out;
}

/// @brief 2つの XMFLOAT3の減算処理
/// @param v1 1番目のXMFLOAT3 引かれる情報
/// @param v2 1番目のXMFLOAT3 引く数
/// @return 減算の結果
static XMFLOAT3 XMFloat3Subtract(const XMFLOAT3& v1, const XMFLOAT3& v2) {
	XMFLOAT3 Out;
	XMStoreFloat3(&Out, XMVectorSubtract(XMLoadFloat3(&v1), XMLoadFloat3(&v2)));
	return Out;
}

/// @brief 2つの XMFLOAT4の減算処理
/// @param v1 1番目のXMFLOAT4 引かれる情報
/// @param v2 1番目のXMFLOAT4 引く数
/// @return 減算の結果
static XMFLOAT4 XMFloat4Subtract(const XMFLOAT4& v1, const XMFLOAT4& v2) {
	XMFLOAT4 Out;
	XMStoreFloat4(&Out, XMVectorSubtract(XMLoadFloat4(&v1), XMLoadFloat4(&v2)));
	return Out;
}


#endif // !_XMEXTENDED_H_

