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
#include "FrameResource.h"

using namespace DirectX;

// Note that while ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.
// An example of this can be found in the class method: OnDestroy().
using Microsoft::WRL::ComPtr;

// 3 frames  cpu to calculate
const int gNumFrameResources = 3;

struct RenderItem
{
	RenderItem() = default;

    // World matrix of the shape that describes the object's local space
    // relative to the world space, which defines the position, orientation,
    // and scale of the object in the world.
    XMFLOAT4X4 World = MathHelper::Identity4X4();

	// Dirty flag indicating the object data has changed and we need to update the constant buffer.
	// Because we have an object cbuffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify obect data we should set 
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
	UINT ObjCBIndex = -1;

    int NumFramesDirty = gNumFrameResources;
	MeshGeometry* Geo = nullptr;

    // Primitive topology.
    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // DrawIndexedInstanced parameters.
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    int BaseVertexLocation = 0;
};

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
    POINT m_LastMousePos;
    XMFLOAT3 m_EyePos = { 0.0f, 0.0f, 0.0f };
    XMFLOAT4X4 m_View = MathHelper::Identity4X4();
    XMFLOAT4X4 m_Proj = MathHelper::Identity4X4();
    float m_NearZ = 1.f;
    float m_FarZ = 1000.f;
    float m_Theta = 1.5f * XM_PI;
    float m_Phi = XM_PIDIV4;
    float m_Radius = 5.0f;
    
    static const UINT FrameCount = 2;


//  让 render 和 geometry进行分离。因为有些物体其实
// 形状是一样的，可以通过更改其objectcb来进行更改

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
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    UINT m_rtvDescriptorSize;
    UINT m_depthStencilDescriptorSize;
    UINT m_cbvDescriptorSize;
    DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    // App resources.
    std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_Geometries;
    std::vector<std::unique_ptr<RenderItem>> mAllRitems; 
    std::vector<RenderItem *>mOpaqueRitems;

    // get the upload pointer ready
    std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputElementDescs;
    ComPtr<ID3DBlob> m_vertexShader;
    ComPtr<ID3DBlob> m_pixelShader;


    // Synchronization objects.
    UINT m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue;
    // frames to use
    std::vector<std::unique_ptr<FrameResource>> mFrameResources;
    FrameResource* mCurrFrameResource = nullptr;
    int mCurrFrameResourceIndex = 0;

    void BuildRootSignature();
    void CreateSwapChainAndCommandThing();
    void CreateDescHeaps();
    void CreateRtvResources();
    void CreateDepthResources();
    void PopulateCommandList();
    void WaitForPreviousFrame();
    void DefineInputLayout();
    void CompileShader();
    void BuildPSO();
    void CreateCommandList();
    void UpdateObjectConstants();
    void UpdateMainPass(); 
    void UpdateCamera();
    void InitProjMatrix();
    void BuildCommonGeoMetry();
    void BuildRenderItems();
    void BuildFrameResources();
    void RenderGroupItems();
};
