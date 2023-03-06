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

PSInput VSMain(float3 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = mul(float4(position, 1.0f), gWorldViewProj);
    result.color = color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
