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

#pragma once

#include "DXSample.h"
#include "MathHelper.h"
#include "UploadBuffer.h"

using namespace DirectX;

// Note that while ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.
// An example of this can be found in the class method: OnDestroy().
using Microsoft::WRL::ComPtr;

class EnzeApp : public DXSample
{
public:
    EnzeApp(UINT width, UINT height, std::wstring name);

    virtual void OnInit();
    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnDestroy();
    virtual void OnMouseDown(WPARAM btnState, int x, int y);
    virtual void OnMouseUp(WPARAM btnState, int x, int y);
    virtual void OnMouseMove(WPARAM btnState, int x, int y);
private:
    POINT mLastMousePos;
    XMFLOAT4X4 mWorld = MathHelper::Identity4X4();
    XMFLOAT4X4 mView = MathHelper::Identity4X4();
    XMFLOAT4X4 mProj = MathHelper::Identity4X4();
    float mTheta = 1.5f*XM_PI;
    float mPhi = XM_PIDIV4;
    float mRadius = 5.0f;
    
    static const UINT FrameCount = 2;
    struct Vertex {
        XMFLOAT3 pos;
        XMFLOAT4 col;
    };

    struct ObjectConstants{
        XMFLOAT4X4 WorldViewProject = MathHelper::Identity4X4();
    };

    // Pipeline objects.
    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    ComPtr<ID3D12Resource> m_depthStencilBuffer;
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12DescriptorHeap> m_depthStencilHeap;
    ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    UINT m_rtvDescriptorSize;
    UINT m_depthStencilDescriptorSize;
    UINT m_cbvDescriptorSize;
    DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    // App resources.
    ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    ComPtr<ID3D12Resource> m_indexBuffer;
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
    std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;
    // get the upload pointer ready
    std::unique_ptr<UploadBuffer<ObjectConstants>> m_objectCB = nullptr;
    std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputElementDescs;
    ComPtr<ID3DBlob> m_vertexShader;
    ComPtr<ID3DBlob> m_pixelShader;


    // Synchronization objects.
    UINT m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue;

    void LoadPipeline();
    void BuildRootSignature();
    void CreateSwapChainAndCommandThing();
    void CreateDescHeaps();
    void CreateRtvResources();
    void CreateDepthResources();
    void CreateConstantBuffer();
    void LoadAssets();
    void PopulateCommandList();
    void WaitForPreviousFrame();
    void DefineInputLayout();
    void CompileShader();
    void BuildPSO();
    void CreateCommandList();
    void BuildBoxGeometry();
    

    
    
};
