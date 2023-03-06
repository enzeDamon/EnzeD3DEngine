#pragma once
#include "stdafx.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
// each object has different world matrix
struct ObjectConstants {
    
    DirectX::XMFLOAT4X4 World = MathHelper::Identity4X4();
    
};

struct PassConstants {
    DirectX::XMFLOAT4X4 ViewMatrix = MathHelper::Identity4X4();
    DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4X4();
    DirectX::XMFLOAT4X4 ProjMatrix = MathHelper::Identity4X4();
    DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4X4();
    DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4X4();
    DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4X4();
    DirectX::XMFLOAT3 EyePosW = {0.f, 0.f, 0.f};
    float NearZ = 0.f;
    float FarZ = 0.f;
    float Time = 0.f; 
    DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };
    Light Lights[MaxLights];
};
struct Vertex {
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT3 Normal;
};
class FrameResource
{
    public:
        FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount);
        FrameResource(const FrameResource& rhs) = delete;
        FrameResource& operator=(const FrameResource& rhs) = delete;
        ~FrameResource();
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;
        std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
        std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;
        std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
        UINT64 Fence = 0;
};