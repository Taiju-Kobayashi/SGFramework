#include "common.hlsl"


void main(in VS_IN In, out PS_IN Out)
{
    Out = (PS_IN) 0;
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp);

	  //頂点法線のワールド変換
    float4 worldNormal, Normal;
    Normal = float4(In.Normal.xyz, 0.0f); //頂点法線を平行移動しないようにする
    worldNormal = mul(Normal, World); //法線をその場で回転させる
    worldNormal = normalize(worldNormal);
    Out.Normal = worldNormal;
    
	
    Out.TexCoord = In.TexCoord;
    Out.Diffuse = In.Diffuse * Material.Diffuse;
	 //ワールド変換した頂点座標
    Out.WorldPosition = mul(In.Position, World);
 
	   //タンジェントをワールド変換
    float4 worldTangent, Tangent;
    Tangent = float4(In.Normal.xyz, 0.0f); //頂点法線を平行移動しないようにする
    worldTangent = mul(Tangent, World); //法線をその場で回転させる
    worldTangent = normalize(worldTangent);
    Out.Tangent = worldTangent;
    
    //バイノーマルを求める TangentとNormalの外積
    float4 worldBinormal;
    worldBinormal.w = 0.0; //cross 外積
    worldBinormal.xyz = cross(worldTangent.xyz, worldNormal.xyz);
    worldBinormal = normalize(worldBinormal);
    Out.BiNormal = worldBinormal;
}

