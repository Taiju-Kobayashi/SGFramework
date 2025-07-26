#include "common.hlsl" //必ずインクルード

//C言語側で設定したサンプラー設定を受け取る
//高度な設定はシェーダ側では出来ない
SamplerState g_SamplerState : register(s0); //テクスチャサンプラー　0番

//テクスチャ情報を受け取る
Texture2D g_Texture2d : register(t0); //テクスチャ0番


void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
	//このピクセルに使うテクセルを取得
    outDiffuse = g_Texture2d.Sample(g_SamplerState, In.TexCoord);
    outDiffuse *= In.Diffuse; //アルファ値を掛け算
}
