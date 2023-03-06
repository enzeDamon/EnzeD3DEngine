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

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
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
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    
};

// 由于HLSL 是列向量乘法，所以vector都是在前的。同时也是为什么要先乘世界矩阵
// 再处理投影矩阵的原因
PSInput VSMain(float3 position : POSITION, float4 color : COLOR)
{
    PSInput result;
    float4 tempPosition = mul(float4(position, 1.0f), gWorldViewProj);
    result.position = mul(tempPosition, ViewProj);
    result.color = color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
