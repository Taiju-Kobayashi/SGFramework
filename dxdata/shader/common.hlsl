


cbuffer WorldBuffer : register(b0)
{
	matrix World;
}
cbuffer ViewBuffer : register(b1)
{
	matrix View;
    matrix InView;
}
cbuffer ProjectionBuffer : register(b2)
{
	matrix Projection;
    matrix InProjection;
}
cbuffer CameraBuffer : register(b5)
{
    float4 CameraPosition;
}

cbuffer ParamBuffer : register(b7)
{
    float4 ParameterOne;
    float4 ParameterTwo;
    float4 ParameterThree;
    float4 Parameterfour;
}


struct MATERIAL
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Emission;
	float Shininess;
	bool TextureEnable;
	float2 Dummy;
};

cbuffer MaterialBuffer : register(b3)
{
	MATERIAL Material;
}


struct POINTLIGHT
{
    float3 Position; // ���W
    float3 PositionInView; // �J������Ԃł̍��W
    float3 LightColor; // ���C�g�̃J���[
    float Angle; // ���C�g�̉e����^����͈�
    float2 Dummy;
};

struct DIRECTIONLIGHT
{
    float4 LightColor;//���C�g�̐F
    float  LightIntensity;//���C�g�̋���
    float3 LightDirection;//���C�g�̌���
};


struct LIGHT
{
	bool Enable;
	bool3 Dummy;
	float4 Direction;
	float4 Diffuse;
	float4 Ambient;
    float4 SkyColor; //�����F
    float4 GroundColor; //�n�ʐF
    float4 GroundNormal; //�n�ʖ@��
    float4 Position; //�������W
    float4 Angle; //�X�|�b�glight�͈́@���W�A��
    float4 PointLightParam; //�����͈�
    matrix ViewMatrix; //���C�g�J�����p//	11/18
    matrix ProjectonMatrix; //���C�g�v���W�F�N�V�����p	11/18
};

cbuffer LightBuffer : register(b4)
{
	LIGHT Light;
}


struct CS_IN
{
    uint3 groupThread : SV_GroupThreadID;
    uint3 group : SV_GroupID;
    uint groupIndex : SV_GroupIndex;
    uint3 dispatch : SV_DispatchThreadID;
};



struct VS_IN
{
	float4 Position		: POSITION0;
	float4 Normal		: NORMAL0;
	float4 Diffuse		: COLOR0;
	float2 TexCoord		: TEXCOORD0;
    float4 Tangent : TANGENT0;
    uint InstanceID : SV_InstanceID;
};


struct PS_IN
{
	float4 Position		: SV_POSITION;
	float4 Diffuse		: COLOR0;
	float2 TexCoord		: TEXCOORD0;
    float4 Normal : NORMAL0;
    float4 BiNormal : BINORMAL0;
    float4 Tangent : TANGENT0;
	float4 WorldPosition : POSITION0;
    float4 LightPosition : POSITION1; //���C�g��ԍ��W//11/18	
};
