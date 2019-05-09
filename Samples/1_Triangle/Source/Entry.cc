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

#include <examples/imgui_impl_win32.h>
#include <examples/imgui_impl_dx11.h>
#include <imgui.h>

#include <D3Dcompiler.h>
#include <D3D11.h>
#include <HelperMacro.h>
#include <XCommonDebug.h>
#include <ComWrapper/IComOwner.h>
#include <ComWrapper/IComBorrow.h>
#include <FD3D11Factory.h>
#include <MTimeChecker.h>
#include <MGuiManager.h>

#include <StringUtil/XUtility.h>
#include <FGuiHelloWorld.h>

#include <Math/Type/Math/DVector3.h>
#include <Math/Type/Math/DVector4.h>

#include <FWindowsPlatform.h>
#include <PLowInputMousePos.h>
#include <XTriangle.h>

// Win32 message handler
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::unique_ptr<dy::APlatformBase> platform = nullptr;

/// @brief
std::optional<dy::DWindowHandle>
CreateMainWindow(const std::string& titleName, unsigned width, unsigned height)
{
  auto value = MTimeChecker::CheckCpuTime("CreateMainWindow");

  dy::PWindowCreationDescriptor desc = {};
  desc.mIsWindowFullScreen = false;
  desc.mIsWindowResizable = true;
  desc.mIsWindowShouldFocus = true;
  desc.mIsWindowVisible = true;
  desc.mWindowName = "D3D11 1_Box";
  desc.mWindowWidth = 800;
  desc.mWindowHeight = 600;

  
#ifdef _WIN32
  #ifdef CreateWindow
    #undef CreateWindow
  #endif
#endif

  auto optRes = platform->CreateWindow(desc);
  if (optRes.has_value() == false)
  {
    return std::nullopt;
  }
    
#ifdef _WIN32
  #ifndef CreateWindow
    #define CreateWindow CreateWindowW
  #endif
#endif

  auto& input = platform->GetInputManager();
  input.SetMousePosFeatureState(dy::base::ELowMousePosState::Normal);
  return optRes;
}

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
  auto optRes = CreateMainWindow("D3D11 0_HelloWorld", 800, 600);
  assert(optRes.has_value() == true);

  const auto& checker = MTimeChecker::Get("CreateMainWindow");
  LOG("[%s] : %.4lf second.\n", "CreateMainWindow", checker.GetRecent().count());

  // Crate D3D11 Device & Context
  auto optDeviceContext = FD3D11Factory::CreateD3D11Device(*platform);
  assert(optDeviceContext.has_value() == true);

  auto mD3DDevice           = optDeviceContext.value().first.GetBorrow();
  auto mD3DImmediateContext = optDeviceContext.value().second.GetBorrow();

  // Get width and height of main window client region.
  const auto width  = platform->GetWindowWidth(*optRes);
  const auto height = platform->GetWindowHeight(*optRes);

  // Describe Swap chain.
  // https://bell0bytes.eu/the-swap-chain/
  DXGI_SWAP_CHAIN_DESC sd;
  {
    // Describe frame-buffer format.
    sd.BufferDesc.Width   = width;
    sd.BufferDesc.Height  = height;
    sd.BufferDesc.RefreshRate.Numerator   = 60; // Want 60 FPS
    sd.BufferDesc.RefreshRate.Denominator = 1;// When Numerator is not 1 or 0, Denominator must be 1.
    sd.BufferDesc.Format  = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; 
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    sd.SampleDesc.Count   = 1;
    sd.SampleDesc.Quality = 0;

    // Describe overall properties.
    sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount  = 1; // The count of `back buffer`.
    sd.OutputWindow = static_cast<HWND>(platform->_GetHandleOf(*optRes)); // HWND to display.
    sd.Windowed     = true;
    sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD; // Discard swapped old buffer.
    sd.Flags        = 0; 
  }

  // Create swap chain.
  IComOwner<IDXGISwapChain> mD3DSwapChain = nullptr;
  {
    // To call creating swap-chain function from DXGIFactory,
    // we must get DXGI device, adapter, and etc... using querying.
    IComOwner<IDXGIDevice> dxgiDevice = nullptr;
    HR(mD3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

    IComOwner<IDXGIAdapter> dxgiAdapter = nullptr;
    HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

    IComOwner<IDXGIFactory> dxgiFactory = nullptr;
    HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

    // Create swap chain.
    HR(dxgiFactory->CreateSwapChain(&mD3DDevice.Get(), &sd, mD3DSwapChain.GetAddressOf()));
    HR(dxgiFactory->MakeWindowAssociation(
      static_cast<HWND>(platform->_GetHandleOf(*optRes)),
      DXGI_MWA_NO_WINDOW_CHANGES));
  }

  // Create render target view (RTV).
  IComOwner<ID3D11RenderTargetView> mRenderTargetView = nullptr;
  {
    IComOwner<ID3D11Texture2D> mBackBufferTexture = nullptr; 
    // This function increase internal COM instance reference counting.
    mD3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&mBackBufferTexture);
    mD3DDevice->CreateRenderTargetView(mBackBufferTexture.GetPtr(), nullptr, mRenderTargetView.GetAddressOf());
  }
  
  // Descript Depth/Stencil Buffer and View descriptors.
  // https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ns-d3d11-d3d11_texture2d_desc
  D3D11_TEXTURE2D_DESC mDepthStencilDesc;
  {
    // Depth/Stencil Render buffer (texture) descript.
    mDepthStencilDesc.Width = width;
    mDepthStencilDesc.Height = height;
    mDepthStencilDesc.MipLevels = 1;
    mDepthStencilDesc.ArraySize = 1;
    mDepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Depth 24, Stencil 8.

    mDepthStencilDesc.SampleDesc.Count = 1;
    mDepthStencilDesc.SampleDesc.Quality = 0;

    mDepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    mDepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    mDepthStencilDesc.CPUAccessFlags = 0; 
    mDepthStencilDesc.MiscFlags = 0;
  }

  // Create Depth/Stencil View (DSV)
  IComOwner<ID3D11Texture2D> mDepthStencilBuffer = nullptr;
  IComOwner<ID3D11DepthStencilView> mDepthStencilView = nullptr;

  // Second parameter of CreateTexture2D is a pointer to initial data to fill the texture with.
  // We do not specify additional DESC to DSV, leave it inherits properties of Depth/Stencil Texture.
  HR(mD3DDevice->CreateTexture2D(&mDepthStencilDesc, nullptr, mDepthStencilBuffer.GetAddressOf()));
  HR(mD3DDevice->CreateDepthStencilView(mDepthStencilBuffer.GetPtr(), nullptr, mDepthStencilView.GetAddressOf()));
  mD3DImmediateContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.GetPtr());

  // Set Viewport
  D3D11_VIEWPORT vp;
  vp.TopLeftX = 0.0f; vp.TopLeftY = 0.0f;
  vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
  vp.Width    = static_cast<float>(width);
  vp.Height   = static_cast<float>(height);
  mD3DImmediateContext->RSSetViewports(1, &vp);

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

    HR(mD3DDevice->CreateBuffer(&vbDesc, &vbData, vBuffer.GetAddressOf()));
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

    HR(mD3DDevice->CreateBuffer(&ibDesc, &ibData, iBuffer.GetAddressOf()));    
  }

  // Compile Vertex Shader.
  // https://docs.microsoft.com/ko-kr/windows/desktop/api/d3dcompiler/nf-d3dcompiler-d3dcompile2
  // "VS" is entry point.
  IComOwner<ID3D11VertexShader> ownVertexShader = nullptr;
  IComOwner<ID3D11InputLayout> ownVsLayout = nullptr;
  {
    IComOwner<ID3DBlob> ownVsBlob = nullptr;
    HR(FD3D11Factory::CompileShaderFromFile(
      *platform,
      "../../Resource/Shader.fx", "VS", "vs_5_0", ownVsBlob.GetAddressOf()));

    HR(mD3DDevice->CreateVertexShader(
      ownVsBlob->GetBufferPointer(), ownVsBlob->GetBufferSize(), 
      nullptr, 
      ownVertexShader.GetAddressOf()));
   
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
      ownVsLayout.GetAddressOf()));

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
      "../../Resource/Shader.fx", "PS", "ps_5_0", ownPsBlob.GetAddressOf()));

    HR(mD3DDevice->CreatePixelShader(
      ownPsBlob->GetBufferPointer(), ownPsBlob->GetBufferSize(), 
      nullptr, 
      ownPsShader.GetAddressOf()));
    ownPsBlob.Release();
  }
 
  // Set topologies.
  mD3DImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  UINT stride = sizeof(DVertex);
  UINT offset = 0;
  mD3DImmediateContext->IASetVertexBuffers(0, 1, vBuffer.GetAddressOf(), &stride, &offset);
  mD3DImmediateContext->IASetIndexBuffer(iBuffer.GetPtr(), DXGI_FORMAT_R32_UINT, 0);

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

    HR(mD3DDevice->CreateRasterizerState(&rasterDesc, ownRasterState.GetAddressOf()));
  }
  mD3DImmediateContext->RSSetState(ownRasterState.GetPtr());

  // Set rect state.
  std::array<D3D11_RECT, 1> ownRectState =
  {
    D3D11_RECT
    {0, 0, 800, 600}
  };
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

    HR(mD3DDevice->CreateDepthStencilState(&dssd, ownDepthStencilState.GetAddressOf()));
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

    HR(mD3DDevice->CreateBlendState(&blendDesc, ownBlendState.GetAddressOf()));
  }
  const FLOAT blendFactor[4] = {0, 0, 0, 0}; 
  mD3DImmediateContext->OMSetBlendState(ownBlendState.GetPtr(), blendFactor, 0xFFFFFFFF);

  // Make timestamp queries (disjoint and start-end queries)
  // https://docs.microsoft.com/ko-kr/windows/desktop/api/d3d11/nf-d3d11-id3d11device-createquery
  IComOwner<ID3D11Query> ownDisjointQuery       = *FD3D11Factory::CreateTimestampQuery(mD3DDevice.Get(), true);
  IComOwner<ID3D11Query> ownGpuStartFrameQuery  = *FD3D11Factory::CreateTimestampQuery(mD3DDevice.Get(), false);
  IComOwner<ID3D11Query> ownGpuEndFrameQuery    = *FD3D11Factory::CreateTimestampQuery(mD3DDevice.Get(), false);

  // Setup Dear ImGui context
  MGuiManager::Initialize(
    [&]()
    {
      ImGui_ImplWin32_Init(platform->_GetHandleOf(*optRes));
      ImGui_ImplDX11_Init(mD3DDevice, mD3DImmediateContext);
      platform->SetPreProcessCallback(ImGui_ImplWin32_WndProcHandler);
    },
    [&]()
    {
      ImGui_ImplDX11_Shutdown();
      ImGui_ImplWin32_Shutdown();
      ImGui::DestroyContext();
    }
  );
  MGuiManager::SetRenderCallbacks(
    [] {
      // Start the Dear ImGui frame
      ImGui_ImplDX11_NewFrame();
      ImGui_ImplWin32_NewFrame();
      ImGui::NewFrame();
    },
    [] {
      ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
  );
  MGuiManager::CreateGui<FGuiHelloWorld>("Hello");

  // Loop
	while (platform->CanShutdown() == false)
	{
    platform->PollEvents();

    auto cpuTime = MTimeChecker::CheckCpuTime("CpuFrame");
    auto gpuTime = MTimeChecker::CheckGpuD3D11Time("GpuFrame", *ownDisjointQuery, mD3DImmediateContext.Get(), 
      false);

    {
      auto fragment = 
        gpuTime.CheckFragment("Overall", *ownGpuStartFrameQuery, *ownGpuEndFrameQuery);

      // D3D Routine
      mD3DImmediateContext->ClearRenderTargetView(mRenderTargetView.GetPtr(), colors[0].data());
      mD3DImmediateContext->ClearDepthStencilView(mDepthStencilView.GetPtr(), 
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0);

      //mD3DImmediateContext->RSSetState(&ownRasterState.Get());
      mD3DImmediateContext->VSSetShader(ownVertexShader.GetPtr(), nullptr, 0);
      mD3DImmediateContext->PSSetShader(ownPsShader.GetPtr(), nullptr, 0);
      mD3DImmediateContext->DrawIndexed(3, 0, 0);

      MGuiManager::Render();

      // Present the back buffer to the screen.
      HR(mD3DSwapChain->Present(0, 0));
    }
	}

  {
    const auto flag = MGuiManager::RemoveGui("Hello");
    assert(flag == true);
  }

  MGuiManager::Shutdown();
  platform->RemoveAllWindow();
  platform->RemoveConsoleWindow();
  platform->ReleasePlatform();
  WIN32_TRY_TURN_OFF_DEBUG();
	return 0;
}
