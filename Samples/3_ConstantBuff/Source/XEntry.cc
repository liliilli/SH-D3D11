///
/// MIT License
/// Copyright (c) 2018-2019 Jongmin Yun
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include <cassert>
#include <cstdio>
#include <array>
#include <iostream>
#include <optional>
#include <vector>
#include <filesystem>

#include <D3Dcompiler.h>
#include <D3D11.h>
#include <HelperMacro.h>
#include <XCommonDebug.h>
#include <ComWrapper/IComOwner.h>
#include <ComWrapper/IComBorrow.h>
#include <FD3D11Factory.h>
#include <MTimeChecker.h>
#include <MGuiManager.h>
#include <XTriangle.h>
#include <XCBuffer.h>

#include <StringUtil/XUtility.h>
#include <FGuiWindow.h>

#include <FWindowsPlatform.h>
#include <PLowInputMousePos.h>
#include <XPlatform.h>
#include <XLocalCommon.h>

int WINAPI WinMain(
  [[maybe_unused]] HINSTANCE hInstance, 
  [[maybe_unused]] HINSTANCE hPrevInstance, 
  [[maybe_unused]] LPSTR lpCmdLine, 
  [[maybe_unused]] int nCmdShow)
{
  WIN32_TRY_TURN_ON_DEBUG();

  // Create base system.
  platform = std::make_unique<dy::FWindowsPlatform>();
  platform->InitPlatform();
  platform->CreateConsoleWindow();
  auto optRes = CreateMainWindow("D3D11 3_ConstantBuff", 1280, 720);
  assert(optRes.has_value() == true);

  //!
  //! D3D11 Setting up
  //!

  // Crate D3D11 Device & Context
  auto optDeviceContext = FD3D11Factory::CreateD3D11Device(*platform);
  assert(optDeviceContext.has_value() == true);

  auto mD3DDevice           = optDeviceContext.value().first.GetBorrow();
  auto mD3DImmediateContext = optDeviceContext.value().second.GetBorrow();

  // Get width and height of main window client region.
  const auto width  = platform->GetWindowWidth(*optRes);
  const auto height = platform->GetWindowHeight(*optRes);
  HWND mainWindowHandle = static_cast<HWND>(platform->_GetHandleOf(*optRes));

  // Describe Swap chain.
  // https://bell0bytes.eu/the-swap-chain/
  DXGI_SWAP_CHAIN_DESC sd = FD3D11Factory::GetDefaultSwapChainDesc(width, height, mainWindowHandle);

  // Create swap chain.
  IComOwner<IDXGISwapChain> mD3DSwapChain = nullptr;
  FD3D11Factory::CreateD3D11SwapChain(
    mD3DDevice.Get(), 
    static_cast<HWND>(platform->_GetHandleOf(*optRes)),
    sd,
    mD3DSwapChain);

  // Create render target view (RTV).
  IComOwner<ID3D11RenderTargetView> mRenderTargetView = nullptr;
  {
    IComOwner<ID3D11Texture2D> mBackBufferTexture = nullptr; 
    // This function increase internal COM instance reference counting.
    mD3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&mBackBufferTexture);
    mD3DDevice->CreateRenderTargetView(mBackBufferTexture.GetPtr(), nullptr, &mRenderTargetView);
  }
  
  // Descript Depth/Stencil Buffer and View descriptors.
  // https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ns-d3d11-d3d11_texture2d_desc
  D3D11_TEXTURE2D_DESC mDepthStencilDesc = FD3D11Factory::GetDefaultDepthStencilDesc(width, height);

  // Create Depth/Stencil View (DSV)
  IComOwner<ID3D11Texture2D> mDepthStencilBuffer = nullptr;
  IComOwner<ID3D11DepthStencilView> mDepthStencilView = nullptr;

  // Second parameter of CreateTexture2D is a pointer to initial data to fill the texture with.
  // We do not specify additional DESC to DSV, leave it inherits properties of Depth/Stencil Texture.
  HR(mD3DDevice->CreateTexture2D(&mDepthStencilDesc, nullptr, &mDepthStencilBuffer));
  HR(mD3DDevice->CreateDepthStencilView(mDepthStencilBuffer.GetPtr(), nullptr, &mDepthStencilView));
  mD3DImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView.GetPtr());

  // Set Viewport
  D3D11_VIEWPORT vp;
  vp.TopLeftX = 0.0f; vp.TopLeftY = 0.0f;
  vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
  vp.Width    = static_cast<float>(width);
  vp.Height   = static_cast<float>(height);
  mD3DImmediateContext->RSSetViewports(1, &vp);

  // Set raster state (RS)
  IComOwner<ID3D11RasterizerState> ownRasterState = nullptr;
  {
    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = true;
    rasterDesc.DepthBias = INT(0);
    rasterDesc.DepthBiasClamp = FLOAT(0);
    rasterDesc.SlopeScaledDepthBias = 0;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.ScissorEnable = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.AntialiasedLineEnable = false;

    HR(mD3DDevice->CreateRasterizerState(&rasterDesc, &ownRasterState));
  }
  mD3DImmediateContext->RSSetState(ownRasterState.GetPtr());

  // Set rect state.
  std::array<D3D11_RECT, 1> ownRectState = { D3D11_RECT {0, 0, 1280, 720} };
  mD3DImmediateContext->RSSetScissorRects(1, ownRectState.data());

  // Set depth stencil state
  IComOwner<ID3D11DepthStencilState> ownDepthStencilState = nullptr;
  {
    D3D11_DEPTH_STENCIL_DESC dssd = {};
    dssd.DepthEnable    = FALSE;
    dssd.DepthFunc      = D3D11_COMPARISON_LESS;
    dssd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dssd.StencilEnable  = FALSE;
    dssd.StencilReadMask  = 0xff;
    dssd.StencilWriteMask = 0xff;

    HR(mD3DDevice->CreateDepthStencilState(&dssd, &ownDepthStencilState));
  }
  mD3DImmediateContext->OMSetDepthStencilState(ownDepthStencilState.GetPtr(), 0x00);

  // Set blend state
  IComOwner<ID3D11BlendState> ownBlendState = nullptr;
  {
    D3D11_BLEND_DESC blendDesc = {};
    {
      D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
      rtBlendDesc.BlendEnable   = TRUE;
      rtBlendDesc.BlendOp       = D3D11_BLEND_OP_ADD;
      rtBlendDesc.BlendOpAlpha  = D3D11_BLEND_OP_ADD;
      rtBlendDesc.SrcBlend      = D3D11_BLEND_SRC_ALPHA;
      rtBlendDesc.DestBlend     = D3D11_BLEND_INV_SRC_ALPHA;
      rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
      rtBlendDesc.DestBlendAlpha= D3D11_BLEND_ZERO;
      rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; 
      blendDesc.RenderTarget[0] = rtBlendDesc; 
    }
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;

    HR(mD3DDevice->CreateBlendState(&blendDesc, &ownBlendState));
  }
  const FLOAT blendFactor[4] = {0, 0, 0, 0}; 
  mD3DImmediateContext->OMSetBlendState(ownBlendState.GetPtr(), blendFactor, 0xFFFFFFFF);

  //!
  //! Shader Compilation 
  //!

  IComOwner<ID3D11VertexShader> ownVertexShader = nullptr;
  IComOwner<ID3D11InputLayout> ownVsLayout = nullptr;
  {
    IComOwner<ID3DBlob> ownVsBlob = nullptr;
    HR(FD3D11Factory::CompileShaderFromFile(
      *platform,
      "../../Resource/Shader.fx", "VS", "vs_5_0", &ownVsBlob));

    HR(mD3DDevice->CreateVertexShader(
      ownVsBlob->GetBufferPointer(), ownVsBlob->GetBufferSize(), 
      nullptr, 
      &ownVertexShader));
   
    // Create Vertex shader input layout.
    // https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ns-d3d11-d3d11_input_element_desc
    std::array<D3D11_INPUT_ELEMENT_DESC, 2> vertexDesc =
    {
      decltype(vertexDesc)::value_type
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA , 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    HR(mD3DDevice->CreateInputLayout(
      vertexDesc.data(), static_cast<UINT>(vertexDesc.size()), 
      ownVsBlob->GetBufferPointer(), ownVsBlob->GetBufferSize(),
      &ownVsLayout));

    ownVsBlob.Release();
  }
  // Set layout into logical device context.
  mD3DImmediateContext->IASetInputLayout(ownVsLayout.GetPtr());

  // Compile Pixel Shader 
  // "PS" is entry point.
  IComOwner<ID3D11PixelShader> ownPsShader = nullptr;
  {
    IComOwner<ID3DBlob> ownPsBlob = nullptr;
    HR(FD3D11Factory::CompileShaderFromFile(
      *platform, 
      "../../Resource/Shader.fx", "PS", "ps_5_0", &ownPsBlob));

    HR(mD3DDevice->CreatePixelShader(
      ownPsBlob->GetBufferPointer(), ownPsBlob->GetBufferSize(), 
      nullptr, 
      &ownPsShader));
    ownPsBlob.Release();
  }

  //!
  //! Mesh Buffer Setting up
  //!

  // * Build Geometry Buffers.
  IComOwner<ID3D11Buffer> vBuffer = nullptr;
  {
    D3D11_BUFFER_DESC vbDesc;
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = 0;
    vbDesc.MiscFlags = 0;
    vbDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vbData;
    vbData.pSysMem = vertices.data();

    HR(mD3DDevice->CreateBuffer(&vbDesc, &vbData, &vBuffer));
  }

  IComOwner<ID3D11Buffer> iBuffer = nullptr;
  {
    D3D11_BUFFER_DESC ibDesc;
    ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
    ibDesc.ByteWidth = sizeof(indices);
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;
    ibDesc.MiscFlags = 0;
    ibDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA ibData;
    ibData.pSysMem = indices.data();

    HR(mD3DDevice->CreateBuffer(&ibDesc, &ibData, &iBuffer));    
  }

  IComOwner<ID3D11Buffer> cBuffer = nullptr;
  {
    D3D11_BUFFER_DESC desc;
    desc.Usage      = D3D11_USAGE_DEFAULT;
    desc.ByteWidth  = sizeof(DCbScale);
    desc.BindFlags  = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    DCbScale initScale;
    initScale.mScale = 0.5f;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = &initScale;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    HR(mD3DDevice->CreateBuffer(&desc, &initData, &cBuffer));
  }
   
  // Set topologies.
  mD3DImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  UINT stride = sizeof(DVertex);
  UINT offset = 0;
  mD3DImmediateContext->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
  mD3DImmediateContext->IASetIndexBuffer(iBuffer.GetPtr(), DXGI_FORMAT_R32_UINT, 0);
  mD3DImmediateContext->VSSetConstantBuffers(0, 1, &cBuffer);

  //!
  //! Queries & ImGui Setting up
  //!

  // Make timestamp queries (disjoint and start-end queries)
  // https://docs.microsoft.com/ko-kr/windows/desktop/api/d3d11/nf-d3d11-id3d11device-createquery
  IComOwner<ID3D11Query> ownDisjointQuery = *FD3D11Factory::CreateTimestampQuery(mD3DDevice.Get(), true);
  auto [ownGpuStartFrameQuery, ownGpuEndFrameQuery] = *FD3D11Factory::CreateTimestampQueryPair(mD3DDevice.Get());
  auto [ownStartCbUpdate, ownEndCbUpdate] = *FD3D11Factory::CreateTimestampQueryPair(mD3DDevice.Get());
  auto [ownStartDraw, ownEndDraw] = *FD3D11Factory::CreateTimestampQueryPair(mD3DDevice.Get());

  // Setup Dear ImGui context
  SetupGuiSettings(*optRes, *(*optDeviceContext).first, *(*optDeviceContext).second);
  auto& windowModel = *MGuiManager::CreateSharedModel<DModelWindow>("Window");
  MGuiManager::CreateGui<FGuiWindow>("Window", std::ref(windowModel));

  // Loop
	while (platform->CanShutdown() == false)
	{
    // Update Routine
    TIME_CHECK_CPU("CpuFrame");
    //auto cpuTime = MTimeChecker::CheckCpuTime("CpuFrame");
    platform->PollEvents();
  
    MGuiManager::Update();

    // Render Routine
    TIME_CHECK_D3D11_STALL(gpuTime, "GpuFrame", *ownDisjointQuery, mD3DImmediateContext.Get());
    {
      TIME_CHECK_FRAGMENT(gpuTime, "Overall", *ownGpuStartFrameQuery, *ownGpuEndFrameQuery);

      mD3DImmediateContext->ClearRenderTargetView(
        mRenderTargetView.GetPtr(), 
        std::array<FLOAT, 4>{0, 0, 0, 1}.data());
      mD3DImmediateContext->ClearDepthStencilView(mDepthStencilView.GetPtr(), 
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0);

      // https://bell0bytes.eu/shader-data/
      {
        TIME_CHECK_FRAGMENT(gpuTime, "CBuffer", *ownStartCbUpdate, *ownEndCbUpdate);

        DCbScale scale;
        scale.mScale = windowModel.mScale;
        mD3DImmediateContext->UpdateSubresource(cBuffer.GetPtr(), 0, nullptr, &scale, 0, 0);
      }

      mD3DImmediateContext->VSSetShader(ownVertexShader.GetPtr(), nullptr, 0);
      mD3DImmediateContext->PSSetShader(ownPsShader.GetPtr(), nullptr, 0);

      {
        TIME_CHECK_FRAGMENT(gpuTime, "Draw", *ownStartDraw, *ownEndDraw);
        mD3DImmediateContext->DrawIndexed(3, 0, 0);
      }

      MGuiManager::Render();
      // Present the back buffer to the screen.
      HR(mD3DSwapChain->Present(1, 0));
    }
	}

  MGuiManager::Shutdown();
  platform->RemoveAllWindow();
  platform->RemoveConsoleWindow();
  platform->ReleasePlatform();
  WIN32_TRY_TURN_OFF_DEBUG();
	return 0;
}