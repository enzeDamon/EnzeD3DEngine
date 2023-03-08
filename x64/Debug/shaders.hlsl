//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
// off and end is used to calculate attenuation
#define MAXLIGHTNUM (16)
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 2
#endif
struct Light
{
    float3 Strength;
    float FalloffStart; // point/spot light only
    float3 Direction;   // directional/spot light only
    float FalloffEnd;   // point/spot light only
    float3 Position;    // point light only
    float SpotPower;    // spot light only
};
struct Material
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Shininess;

};
cbuffer cbPerObject : register(b0)
{
	float4x4 WorldMatrix; 
};

cbuffer cbPerPass : register(b1)
{
        float4x4 ViewMatrix;
        float4x4 InvView;
        float4x4 ProjMatrix;
        float4x4 InvProj;
        float4x4 ViewProj;
        float4x4 InvViewProj;
        float3 EyePosW;
        float NearZ;
        float FarZ;
        float Time; 
        // 两个对齐位
        float pad_0;
        float pad_1;
        float4 AmbientLight;
        Light Lights[MAXLIGHTNUM];
};

cbuffer cbPerMaterial: register(b2)
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Roughness;
    float4x4 MatTransform;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 positionWorld: POSITION;
    
};

// 由于HLSL 是列向量乘法，所以vector都是在前的。同时也是为什么要先乘世界矩阵
// 再处理投影矩阵的原因
PSInput VSMain(float3 position : POSITION, float3 normal : NORMAL)
{
    PSInput result;
    float4 tempPosition = mul(float4(position, 1.0f), WorldMatrix);
    result.position = mul(tempPosition, ViewProj);
    result.normal = normal;
    result.positionWorld = tempPosition.xyz;
    return result;
}
float4 diffusionCalCulation(Light L[MAXLIGHTNUM], float4 diffuseAlbedo, float3 normal)
{
    float3 result = 0.f ;

    for(int i = 0; i < NUM_DIR_LIGHTS; ++i) {
        //  因为都以该平面的法线为主
        float3 light_dir = - L[i].Direction;
        float3 strength = L[i].Strength;
        // 投影下来的光照强度
        result += strength * max(dot(light_dir, normal), 0.f) * diffuseAlbedo.xyz;
    }
    return float4(result, 0.f);

}

float4 PSMain(PSInput input) : SV_TARGET
{
    //线性插值可能让其大于1
    float3 NormalW = normalize(input.normal);
    float3 toEye = normalize(EyePosW - input.positionWorld);
    // Blinn-Phong 模型分为三部分，先把环境光部分搞定
    // 这里用位乘法, 这里games101其实用的是另外一种算法，
    // 用的另外的ka
    float4 ambient = AmbientLight * DiffuseAlbedo;
    // 下面来计算diffusion
    float4 diffusion = diffusionCalCulation(Lights, DiffuseAlbedo, NormalW);
    float3 result = ambient.xyz + diffusion.xyz;
    // return DiffuseAlbedo;
    return float4(diffusion.xyz, DiffuseAlbedo.a);
}
