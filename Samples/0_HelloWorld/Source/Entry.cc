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

#include <cstdio>
#include <array>
#include <cassert>
#include <iostream>

#include <d3d11.h>
#include <HelperMacro.h>

#include <FWindowsPlatform.h>
#include <PLowInputMousePos.h>

std::unique_ptr<dy::APlatformBase> platform = nullptr;

int WINAPI WinMain(
  [[maybe_unused]] HINSTANCE hInstance, 
  [[maybe_unused]] HINSTANCE hPrevInstance, 
  [[maybe_unused]] LPSTR lpCmdLine, 
  [[maybe_unused]] int nCmdShow)
{
  platform = std::make_unique<dy::FWindowsPlatform>();
  platform->InitPlatform();
  platform->CreateConsoleWindow();

#ifdef _WIN32
  #ifdef CreateWindow
    #undef CreateWindow
  #endif
#endif

  dy::PWindowCreationDescriptor desc = {};
  desc.mIsWindowFullScreen = false;
  desc.mIsWindowResizable = true;
  desc.mIsWindowShouldFocus = true;
  desc.mIsWindowVisible = true;
  desc.mWindowName = "D3D11 0_HelloWorld";
  desc.mWindowWidth = 800;
  desc.mWindowHeight = 600;

  auto optRes = platform->CreateWindow(desc);
  assert(optRes.has_value() == true);

  auto& input = platform->GetInputManager();
  input.SetMousePosFeatureState(dy::base::ELowMousePosState::Normal);

  // Crate D3D11 Device & Context
  UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  D3D_FEATURE_LEVEL featureLevel;
  ID3D11Device* mD3DDevice;
  ID3D11DeviceContext* mD3DImmediateContext;

  // Make device. (physical? logical?)
  HRESULT hr = D3D11CreateDevice(
    0, // Default Adapter
    D3D_DRIVER_TYPE_HARDWARE,
    0, // No Software device.
    createDeviceFlags,
    0, // No feature level (default)
    0, 
    D3D11_SDK_VERSION,
    &mD3DDevice,
    &featureLevel,
    &mD3DImmediateContext
  );
  
  // Error checking.
  if (FAILED(hr))
  {
    platform->GetDebugManager().OnAssertionFailed(
      "D3D11CreateDevice Failed.\n", __FUNCTION__, __FILE__, __LINE__
    );
    return 3;
  }

  if (featureLevel != D3D_FEATURE_LEVEL_11_0)
  {
    platform->GetDebugManager().OnAssertionFailed(
      "Direct3D Feature Level 11 unsupported.\n", __FUNCTION__, __FILE__, __LINE__
    );
    return 3;
  }

  // Check 4x MSAA (Forwarding) Quality Support
  UINT m4xMsaaQuality;
  {
    const auto result = mD3DDevice->CheckMultisampleQualityLevels(
      DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);
    assert(m4xMsaaQuality > 0);
  }

  const auto width  = platform->GetWindowWidth(*optRes);
  const auto height = platform->GetWindowHeight(*optRes);

  // Describe Swap chain.
  DXGI_SWAP_CHAIN_DESC sd;
  {
    // Describe frame-buffer format.
    sd.BufferDesc.Width   = width;
    sd.BufferDesc.Height  = height;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format  = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // NO MSAA (Temporary)
    sd.SampleDesc.Count   = 1;
    sd.SampleDesc.Quality = 0;

    // Describe overall properties.
    sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount  = 1;
    sd.OutputWindow = static_cast<HWND>(platform->_GetHandleOf(*optRes));
    sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags        = 0;
  }

  // Create swap chain.
  IDXGISwapChain* mD3DSwapChain = nullptr;
  {
    // To call creating swap-chain function from DXGIFactory,
    // we must get DXGI device, adapter, and etc... using querying.

    IDXGIDevice* dxgiDevice = nullptr;
    HR(mD3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

    IDXGIAdapter* dxgiAdapter = nullptr;
    HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

    // Finally got the IDXGIFactory interface.
    IDXGIFactory* dxgiFactory = nullptr;
    HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

    // Create swap chain.
    HR(dxgiFactory->CreateSwapChain(mD3DDevice, &sd, &mD3DSwapChain));

    // Release COM interfaces.
    ReleaseCOM(dxgiFactory);
    ReleaseCOM(dxgiAdapter);
    ReleaseCOM(dxgiDevice);
  }

  // Create render target view (RTV).
  ID3D11RenderTargetView* mRenderTargetView = nullptr;
  ID3D11Texture2D* mBackBufferTexture = nullptr; // COM?
  mD3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&mBackBufferTexture);
  mD3DDevice->CreateRenderTargetView(mBackBufferTexture, nullptr, &mRenderTargetView);
  ReleaseCOM(mBackBufferTexture);
  
  // Descript Depth/Stencil Buffer and View descriptors.
  D3D11_TEXTURE2D_DESC mDepthStencilDesc;
  {
    // Depth/Stencil Render buffer (texture) descript.
    mDepthStencilDesc.Width = width;
    mDepthStencilDesc.Height = height;
    mDepthStencilDesc.MipLevels = 1;
    mDepthStencilDesc.ArraySize = 1;
    mDepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Depth 24, Stencil 8.

    // No MSAA
    mDepthStencilDesc.SampleDesc.Count = 1;
    mDepthStencilDesc.SampleDesc.Quality = 0;

    // Descript overall texture format.
    mDepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    mDepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    mDepthStencilDesc.CPUAccessFlags = 0;
    mDepthStencilDesc.MiscFlags = 0;
  }

  // Create Depth/Stencil View (DSV)
  ID3D11Texture2D* mDepthStencilBuffer = nullptr;
  ID3D11DepthStencilView* mDepthStencilView = nullptr;
  HR(mD3DDevice->CreateTexture2D(&mDepthStencilDesc, nullptr, &mDepthStencilBuffer));
  HR(mD3DDevice->CreateDepthStencilView(mDepthStencilBuffer, nullptr, &mDepthStencilView));

  // Bind the views (RTV, DSV) to the Output Merger Stage.
  mD3DImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

  // Set Viewport
  D3D11_VIEWPORT vp;
  vp.TopLeftX = 0.0f;
  vp.TopLeftY = 0.0f;
  vp.Width    = static_cast<float>(width);
  vp.Height   = static_cast<float>(height);
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  mD3DImmediateContext->RSSetViewports(1, &vp);

  // Loop
	while (platform->CanShutdown() == false)
	{
    platform->PollEvents();

    // D3D Routine
    static const float blue[4] = {0, 0, 1, 1};
    mD3DImmediateContext->ClearRenderTargetView(mRenderTargetView, blue);
    mD3DImmediateContext->ClearDepthStencilView(mDepthStencilView, 
      D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
      1.0f,
      0);

    // Present the back buffer to the screen.
    HR(mD3DSwapChain->Present(0, 0));
	}

  // Release device.


  platform->RemoveAllWindow();
  platform->RemoveConsoleWindow();
  platform->ReleasePlatform();
	return 0;
}
