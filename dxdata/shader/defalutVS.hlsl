//頂点シェーダ
//特に何もしない
//スプライト&テクスチャ付き
#include "common.hlsl" //必ずインクルード

void main(in VS_IN In, out PS_IN Out)
{
    Out = (PS_IN) 0;
 //頂点変換処理
    matrix wvp; //World View Projection行列
    wvp = mul(World, View); //wvp = World* View;
    wvp = mul(wvp, Projection); //wvp = wvp * Projection
   
    Out.Position = mul(In.Position, wvp); //頂点座標を変換して出力
    
    //頂点色を出力
    Out.Diffuse = In.Diffuse;
    
    //テクスチャ座標出力
    Out.TexCoord = In.TexCoord;
        
}