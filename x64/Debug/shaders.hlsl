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
    #define NUM_DIR_LIGHTS 3
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

float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
    float cosIncidentAngle = saturate(dot(normal, lightVec));

    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = R0 + (1.0f - R0)*(f0*f0*f0*f0*f0);

    return reflectPercent;
}


float3 reflectionCalculation(Light L[MAXLIGHTNUM], Material mat, float3 toEye, float3 normal)
{   float m = mat.Shininess * 256.0f;
    float3 result = 0.f;
    for (int i = 0; i < NUM_DIR_LIGHTS; i++) {
        float3 light_dir = -L[i].Direction;
        float3 halfVector = normalize(normal + toEye);
        // 乘法在GPU中比除法快，所以选择用0.125代替除以8
        float roughnessFactor = 0.125f * (m + 8.f) * pow(max(dot(halfVector, normal), 0.f), m);
        float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVector, light_dir);
        float3 specAlbedo = fresnelFactor * roughnessFactor;
        specAlbedo = specAlbedo / (specAlbedo + 1.f);
        float3 light_strength = L[i].Strength * max(dot(light_dir, normal), 0.f);
        result += specAlbedo * light_strength; 
    }
    return result;
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
    float Shininess = 1.f - Roughness;
    Material current = { DiffuseAlbedo, FresnelR0, Shininess };
    float3 reflection = reflectionCalculation(Lights, current, toEye, NormalW);
    float3 result = ambient.xyz + diffusion.xyz + reflection;
    // 下面就是开始计算 反射光，包含微表面模型了
    
    return float4(result, DiffuseAlbedo.a);
}
