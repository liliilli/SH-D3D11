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

#include <d3d11.h>
#include <HelperMacro.h>

#include <FWindowsPlatform.h>
#include <PLowInputMousePos.h>

std::unique_ptr<dy::APlatformBase> platform = nullptr;

std::optional<dy::DWindowHandle>
CreateMainWindow(const std::string& titleName, unsigned width, unsigned height)
{
  dy::PWindowCreationDescriptor desc = {};
  desc.mIsWindowFullScreen = false;
  desc.mIsWindowResizable = true;
  desc.mIsWindowShouldFocus = true;
  desc.mIsWindowVisible = true;
  desc.mWindowName = "D3D11 0_HelloWorld";
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

  auto& input = platform->GetInputManager();
  input.SetMousePosFeatureState(dy::base::ELowMousePosState::Normal);
  return optRes;
}

/// @brief Returned list must be released explicitly.
std::vector<IDXGIAdapter*> GetAllAdapters()
{
  std::vector<IDXGIAdapter*> enumrableAdapters = {};
  IDXGIFactory* dxgiFactory = nullptr;
  HR(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

  UINT i = 0;
  IDXGIAdapter* adapter = nullptr;
  // i is index.
  // When the EnumAdapters method succeeds and fills the ppAdapter parameter 
  // with the address of the pointer to the adapter interface, 
  // EnumAdapters increments the adapter interface's reference count.
  while (dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
  {
    enumrableAdapters.emplace_back(adapter);
    ++i;
  }

  ReleaseCOM(dxgiFactory);
  return enumrableAdapters;
}

void ReleaseD3DAdapters(std::vector<IDXGIAdapter*>& adapters)
{
  for (auto& adapter : adapters)
  {
    ReleaseCOM(adapter);
  }
}

void ReleaseDXGIOutputs(std::vector<IDXGIOutput*>& outputs)
{
  for (auto& output : outputs)
  {
    ReleaseCOM(output);
  }
}

/// EXERCISE 2 : User the IDXGIFactory::EnumAdapters method to determine
/// how many adapters are on your system.
void DisplayAvailableAdapterCount()
{
  auto adapters = GetAllAdapters();
  // Output information of adapters of system.
  std::cout << "Available adapter count : " << adapters.size() << "\n";

  // Release IDXGIAdapters and IDXGIFactory.
  ReleaseD3DAdapters(adapters);
}

/// EXERCISE 3.
/// If you try to use CheckInterfaceSupport to check whether a Direct3D 11.x and later version 
/// interface is supported, CheckInterfaceSupport returns DXGI_ERROR_UNSUPPORTED. 
/// Therefore, do not use CheckInterfaceSupport. 
/// Instead, to verify whether the operating system supports a particular interface, 
/// try to create the interface. 
/// For example, if you call the ID3D11Device::CreateBlendState method and it fails, 
/// the operating system does not support the ID3D11BlendState interface.
std::vector<IDXGIAdapter**> GetAdapterSupportsD3D10(std::vector<IDXGIAdapter*>& adapters)
{
  std::vector<IDXGIAdapter**> result = {};
  for (auto& adapter : adapters)
  {
    LARGE_INTEGER umdVersion;
    const auto flag = adapter->CheckInterfaceSupport(__uuidof(ID3D10Device), &umdVersion);
    if (flag == S_OK)
    {
      result.emplace_back(&adapter);
    }
  }

  return result;
}

bool CheckSupportD3D11(IDXGIAdapter*& adapter)
{
  assert(adapter != nullptr);

  ID3D11Device* mD3DDevice;
  ID3D11DeviceContext* mD3DImmediateContext;
  D3D_FEATURE_LEVEL featureLevel;

  HRESULT hr = D3D11CreateDevice(
    adapter,              // Adapter to check.
    D3D_DRIVER_TYPE_UNKNOWN, // Must use `D3D_DRIVER_TYPE_UNKNOWN`, when adapter is non-null. 
    nullptr,              // No Software device because we use TYPE_HARDWARE (D3D11).
    0,                    // Set flags (DEBUG, SINGLETHREAD etc...)
    nullptr,              // If no flag is exist, just pick up the highest version of SDK.
    0,                    // Above argument brings the array of D3D_FEATURE, so we set it to 0 as nullptr. 
    D3D11_SDK_VERSION,    // Always specify this.
    &mD3DDevice,          // Output
    &featureLevel,        // Output
    &mD3DImmediateContext // Output
  );
  
  bool isSupport = (mD3DDevice == nullptr) ? false : true;
  ReleaseCOM(mD3DImmediateContext);
  ReleaseCOM(mD3DDevice);

  return isSupport;
}

/// EXERCISE 4
/// Use IDXGIAdapter::EnumOutputs to determine the number of outputs for adapter.
std::vector<IDXGIOutput*> GetOutputsFromAdapter(IDXGIAdapter*& validAdapter)
{
  assert(validAdapter != nullptr);

  std::vector<IDXGIOutput*> result = {};
  UINT i = 0;
  IDXGIOutput* output = nullptr;

  // i is index.
  // When the EnumOutputs method succeeds and fills the ppOutput parameter with 
  // the address of the pointer to the output interface, 
  // EnumOutputs increments the output interface's reference count. 
  while (validAdapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
  {
    result.emplace_back(output);
    ++i;
  }

  return result;
}

/// EXRECISE 5.
/// For each output of adapter, show the width, height and refresh rate of each display mode,
/// when the output supports for the DXGI_FORMAT_R8G8B8A8_UNORM format.
void DisplayDisplayModeInformationOfOutput(IDXGIOutput*& validOutput)
{
  assert(validOutput != nullptr);

  // https://docs.microsoft.com/en-us/windows/desktop/api/dxgi/nf-dxgi-idxgioutput-getdisplaymodelist
  UINT countOfDesc = 0;
  HR(validOutput->GetDisplayModeList(
    DXGI_FORMAT_R8G8B8A8_UNORM, 
    0, &countOfDesc, nullptr));

  std::vector<DXGI_MODE_DESC> desc(countOfDesc);
  HR(validOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &countOfDesc, desc.data()));

  for (UINT i = 0; i < countOfDesc; ++i)
  {
    std::printf("*** WIDTH = %d HEIGHT = %d REFRESH = %d/%d\n",
      desc[i].Width, desc[i].Height, 
      desc[i].RefreshRate.Numerator, desc[i].RefreshRate.Denominator);
  }
}

int WINAPI WinMain(
  [[maybe_unused]] HINSTANCE hInstance, 
  [[maybe_unused]] HINSTANCE hPrevInstance, 
  [[maybe_unused]] LPSTR lpCmdLine, 
  [[maybe_unused]] int nCmdShow)
{
  // Create base system.
  platform = std::make_unique<dy::FWindowsPlatform>();
  platform->InitPlatform();
  platform->CreateConsoleWindow();
  auto optRes = CreateMainWindow("D3D11 0_HelloWorld", 800, 600);
  assert(optRes.has_value() == true);

  // Crate D3D11 Device & Context
  UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  // Device can allocate resource (buffer memory) and check compatibilities. (Like a physical device?)
  ID3D11Device* mD3DDevice;
  // DeviceContext can set states of render and bind resource into logical context,
  // issue rendering commands.
  // Also, ID3D11Device::CreateDeferredContext can create Deferred Context (DeviceContext)
  // This function is used to render context on Multi-thread environment.
  ID3D11DeviceContext* mD3DImmediateContext;
  D3D_FEATURE_LEVEL featureLevel;

  /// EXERCISE 2.
  DisplayAvailableAdapterCount();

  /// EXERCISE 3.
  /// If you try to use CheckInterfaceSupport to check whether a Direct3D 11.x and later version 
  /// interface is supported, CheckInterfaceSupport returns DXGI_ERROR_UNSUPPORTED. 
  /// Therefore, do not use CheckInterfaceSupport. 
  /// Instead, to verify whether the operating system supports a particular interface, 
  /// try to create the interface. 
  /// For example, if you call the ID3D11Device::CreateBlendState method and it fails, 
  /// the operating system does not support the ID3D11BlendState interface.
  {
    auto adapters = GetAllAdapters();
    auto d3d11SupportedAdapters = GetAdapterSupportsD3D10(adapters);
    // We release adapters in adavance because we do not any process with given adapters.
    ReleaseD3DAdapters(adapters);

    if (d3d11SupportedAdapters.empty() == true)
    {
      platform->GetDebugManager().OnAssertionFailed(
        "Failed to initialize D3D11 Samples, None of adapters are not support ID3D11Device.",
        __FUNCTION__, __FILE__, __LINE__
      );

      return 3;
    }
  }

  /// EXERCISE 4.
  /// Use IDXGIAdapter::EnumOutputs to determine the number of outputs for adapter.
  {
    auto adapters = GetAllAdapters();
    auto pMainAdapter = adapters.front();

    auto outputs = GetOutputsFromAdapter(pMainAdapter);
    std::cout << "The number of outpus of main adapter. : " << outputs.size() << '\n';

    ReleaseDXGIOutputs(outputs);
    ReleaseD3DAdapters(adapters);
  }

  /// EXRECISE 5.
  /// For each output of adapter, show the width, height and refresh rate of each display mode,
  /// when the output supports for the DXGI_FORMAT_R8G8B8A8_UNORM format.
  {
    auto adapters = GetAllAdapters();
    std::cout << "*** NUM ADAPTERS = " << adapters.size() << '\n';

    for (std::size_t i = 0, size = adapters.size(); i < size; ++i)
    {
      if (CheckSupportD3D11(adapters[i]) == false) { continue; }
      std::cout << "*** D3D11 SUPPORTED FOR ADAPTER " << i << '\n';

      // Check.
      auto outputs = GetOutputsFromAdapter(adapters[i]);
      for (auto& output : outputs)
      {
        DisplayDisplayModeInformationOfOutput(output);
      }

      ReleaseDXGIOutputs(outputs);
    }

    ReleaseD3DAdapters(adapters);
  }

  // Make device. (physical? logical?)
  HRESULT hr = D3D11CreateDevice(
    nullptr,              // Default Adapter (Primary)
    D3D_DRIVER_TYPE_HARDWARE, // Use hardware driver (Most optimal) 
    nullptr,              // No Software device because we use TYPE_HARDWARE (D3D11).
    createDeviceFlags,    // Set flags (DEBUG, SINGLETHREAD etc...)
    nullptr,              // If no flag is exist, just pick up the highest version of SDK.
    0,                    // Above argument brings the array of D3D_FEATURE, so we set it to 0 as nullptr. 
    D3D11_SDK_VERSION,    // Always specify this.
    &mD3DDevice,          // Output
    &featureLevel,        // Output
    &mD3DImmediateContext // Output
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
  // https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/nf-d3d11-id3d11device-checkmultisamplequalitylevels
  // If successful, return value is H_OK or something except for H_ERROR.
  UINT m4xMsaaQuality;
  {
    // This function just check given DXGI_FORMAT and sample count is supported by hardware.
    // Output : Number of quality levels supported by the adapter. See remarks.
    const auto result = mD3DDevice->CheckMultisampleQualityLevels(
      DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);
    assert(m4xMsaaQuality > 0);
  }

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
    // the scanline drawing mode.
    // Usually use _UNSPECIFIED.
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; 
    // DXGI_MODE_SCALING_UNSPECIFIED, 
    // which means that our rendered images will just appear in the top-left corner of the window
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // NO MSAA (Temporary)
    // Both the back buffer and depth buffer must be created with the same multisampling settings.
    sd.SampleDesc.Count   = 1;
    // The image quality level. The higher the quality, the lower the performance. 
    // The valid range is between zero and one less than the level 
    // returned by ID3D10Device::CheckMultisampleQualityLevels 
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
  IDXGISwapChain* mD3DSwapChain = nullptr;
  {
    // To call creating swap-chain function from DXGIFactory,
    // we must get DXGI device, adapter, and etc... using querying.

    // https://docs.microsoft.com/en-us/windows/desktop/api/dxgi/nn-dxgi-idxgifactory
    // To use IDXGIFactory, we must get instance from ID3D11Device for dependency
    // using querying.
    IDXGIDevice* dxgiDevice = nullptr;
    HR(mD3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

    IDXGIAdapter* dxgiAdapter = nullptr;
    HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

    // Finally got the IDXGIFactory interface.
    // DXGIDevice -> DXGIAdapter -> DXGIFactory.
    IDXGIFactory* dxgiFactory = nullptr;
    HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

    // Create swap chain.
    HR(dxgiFactory->CreateSwapChain(mD3DDevice, &sd, &mD3DSwapChain));

    /// EXERCISES 1 : Disable ALT+ENTER using DXGI API.
    /// https://docs.microsoft.com/en-us/windows/desktop/api/dxgi/nf-dxgi-idxgifactory-makewindowassociation
    HR(dxgiFactory->MakeWindowAssociation(
      static_cast<HWND>(platform->_GetHandleOf(*optRes)),
      DXGI_MWA_NO_WINDOW_CHANGES));

    // Release COM interfaces.
    ReleaseCOM(dxgiFactory);
    ReleaseCOM(dxgiAdapter);
    ReleaseCOM(dxgiDevice);
  }

  // Create render target view (RTV).
  ID3D11RenderTargetView* mRenderTargetView = nullptr;
  ID3D11Texture2D* mBackBufferTexture = nullptr; 
  // This function increase internal COM instance reference counting.
  // So we need to release when given COMptr is not used anymore.
  mD3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&mBackBufferTexture);
  // If DESC is nullptr, just create RTV with texture's (IResource) ormat with mipmap lv 1.
  // Therefore, RTV is created with default texture2D's format of created chain-swap,
  // DXGI_FORMAT_R8G8B8A8_UNORM.
  mD3DDevice->CreateRenderTargetView(mBackBufferTexture, nullptr, &mRenderTargetView);
  // This macro does not release actual buffer resource
  // But just decrease the count of reference counting of COM.
  // If Reference counting is 0, COM instance automatically release in internal logic.
  ReleaseCOM(mBackBufferTexture); 
  
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

    // No MSAA
    // Both the back buffer and depth buffer must be created with the same multisampling settings.
    // RTV resource and DSV resource must have same SampleDesc properties.
    mDepthStencilDesc.SampleDesc.Count = 1;
    mDepthStencilDesc.SampleDesc.Quality = 0;

    // Descript overall texture format.
    // D3D11_USAGE_DEFAULT : GPU can read / write to given resource, but CPU cannot do both.
    mDepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    mDepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    // How the CPU will access the resource.
    // We do not use this flag because given texture resource is set up with D3D11_USAGE_DEFAULT.
    mDepthStencilDesc.CPUAccessFlags = 0; 
    mDepthStencilDesc.MiscFlags = 0;
  }

  // Create Depth/Stencil View (DSV)
  ID3D11Texture2D* mDepthStencilBuffer = nullptr;
  ID3D11DepthStencilView* mDepthStencilView = nullptr;
  // Second parameter of CreateTexture2D is a pointer to initial data to fill the texture with.
  HR(mD3DDevice->CreateTexture2D(&mDepthStencilDesc, nullptr, &mDepthStencilBuffer));
  // We do not specify additional DESC to DSV, leave it inherits properties of Depth/Stencil Texture.
  HR(mD3DDevice->CreateDepthStencilView(mDepthStencilBuffer, nullptr, &mDepthStencilView));

  // Bind the views (RTV, DSV) to the Output Merger Stage.
  // We can set an array of render target views but only one depth/stencil now.
  mD3DImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

  // Set Viewport
  D3D11_VIEWPORT vp;
  vp.TopLeftX = 000.0f;
  vp.TopLeftY = 000.0f;
  vp.Width    = 500.0f;
  vp.Height   = 400.0f;
#if 0
  vp.TopLeftX = 0.0f;
  vp.TopLeftY = 0.0f;
  vp.Width    = static_cast<float>(width);
  vp.Height   = static_cast<float>(height);
#endif
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
  ReleaseCOM(mDepthStencilView);
  ReleaseCOM(mDepthStencilBuffer);
  ReleaseCOM(mBackBufferTexture);
  ReleaseCOM(mRenderTargetView);
  ReleaseCOM(mD3DSwapChain);
  ReleaseCOM(mD3DImmediateContext);
  ReleaseCOM(mD3DDevice);

  platform->RemoveAllWindow();
  platform->RemoveConsoleWindow();
  platform->ReleasePlatform();
	return 0;
}
