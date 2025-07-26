#include "common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    Out = (PS_IN) 0;
    matrix wvp; //行列変数を作成
    wvp = mul(World, View); //wvp = ワールド行列＊カメラ行列
    wvp = mul(wvp, Projection); //wvp = wvp *プロジェクション行列
    Out.Position = mul(In.Position, wvp); //変換結果を出力する

    //頂点法線をワールド行列で変換
    float4 worldNormal, normal;
    normal = float4(In.Normal.xyz, 0.0f);
    worldNormal = mul(normal, World);
    worldNormal = normalize(worldNormal);
    Out.Normal = worldNormal;
    
    Out.Diffuse = In.Diffuse;
    Out.TexCoord = In.TexCoord;

    Out.WorldPosition = mul(In.Position, World);
    
    float4 worldTangent, Tangent;
    Tangent = float4(In.Normal.xyz, 0.0f); //頂点法線を平行移動しないようにする
    worldTangent = mul(Tangent, World); //法線をその場で回転させる
    worldTangent = normalize(worldTangent);
    Out.Tangent = worldTangent;
    
    //バイノーマルを求める TangentとNormalの外積
    float4 worldBinormal;
    worldBinormal = 0; //cross 外積
    worldBinormal.xyz = cross(worldTangent.xyz, worldNormal.xyz);
    worldBinormal = normalize(worldBinormal);
    Out.BiNormal = worldBinormal;
}