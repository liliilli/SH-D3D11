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
#include <IComOwner.h>
#include <IComBorrow.h>

#include <StringUtil/XUtility.h>

#include <Math/Type/Math/DVector3.h>
#include <Math/Type/Math/DVector4.h>

#include <FWindowsPlatform.h>
#include <PLowInputMousePos.h>

/// @brief Turn on memory leak detection feature and console window for logging.
void InitializeWin32Debug()
{
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
}

#if defined(_DEBUG) == true

#define WIN32_CRT_BREAKPOINT(Id)    _crtBreakAlloc(Id) 
#define WIN32_TRY_TURN_ON_DEBUG()   InitializeWin32Debug()
#define WIN32_TRY_TURN_OFF_DEBUG()  (void)0

#else

#define WIN32_CRT_BREAKPOINT(Id)    (void)0
#define WIN32_TRY_TURN_ON_DEBUG()   (void)0
#define WIN32_TRY_TURN_OFF_DEBUG()  (void)0

#endif

std::unique_ptr<dy::APlatformBase> platform = nullptr;

/// @brief
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
    
#ifdef _WIN32
  #ifndef CreateWindow
    #define CreateWindow CreateWindowW
  #endif
#endif

  auto& input = platform->GetInputManager();
  input.SetMousePosFeatureState(dy::base::ELowMousePosState::Normal);
  return optRes;
}

/// @brief
std::optional<
  std::pair<IComOwner<ID3D11Device>, IComOwner<ID3D11DeviceContext>>
>
CreateD3D11Device()
{
  IComOwner<ID3D11Device>         mD3DDevice            = nullptr;
  IComOwner<ID3D11DeviceContext>  mD3DImmediateContext  = nullptr;
  D3D_FEATURE_LEVEL               featureLevel;
  
  // Crate D3D11 Device & Context
  UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

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
    return std::nullopt;
  }
  if (featureLevel != D3D_FEATURE_LEVEL_11_0)
  {
    platform->GetDebugManager().OnAssertionFailed(
      "Direct3D Feature Level 11 unsupported.\n", __FUNCTION__, __FILE__, __LINE__
    );
    return std::nullopt;
  }

  return std::pair{std::move(mD3DDevice), std::move(mD3DImmediateContext)};
}

/// @brief OLD Style.
HRESULT CompileShaderFromFile(
  const std::string& szFilePath, 
  LPCSTR szEntryPoint, 
  LPCSTR szShaderModel, 
  ID3DBlob** ppBlobOut)
{
  DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
  // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
  // Setting this flag improves the shader debugging experience, but still allows 
  // the shaders to be optimized and to run exactly the way they will run in 
  // the release configuration of this program.
  dwShaderFlags |= D3DCOMPILE_DEBUG;
  // Disable optimizations to further improve shader debugging
  dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

  // Convert file path to wchar_t string.
  const auto wSzFilePath = dy::ConvertStringToWString(szFilePath);

  std::filesystem::path fsPath = szFilePath;
  if (std::filesystem::exists(fsPath) == false)
  {
    const auto absolutePath = std::filesystem::absolute(fsPath);
    platform->GetDebugManager().OnAssertionFailed(
      absolutePath.string().c_str(),
      __FUNCTION__, __FILE__, __LINE__
    );
    return E_FAIL;
  }

  // Create file descriptor.
  FILE* fdFile = fopen(szFilePath.c_str(), "r");
  if (fdFile == nullptr) 
  { 
    platform->GetDebugManager().OnAssertionFailed(
      "Failed to read shader file.", __FUNCTION__, __FILE__, __LINE__
    );
    return E_FAIL; 
  }

  // Read file buffer into fdBuffer.
  fseek(fdFile, 0, SEEK_END);
  const auto fdLength = ftell(fdFile);
  fseek(fdFile, 0, SEEK_SET);

  std::vector<char> fdBuffer(fdLength, 0);
  fread(fdBuffer.data(), sizeof(char), fdLength, fdFile);
  assert(feof(fdFile) != 0);

  // Close file descriptor.
  fclose(fdFile);

  // D3DCompileFromFile.
  // https://docs.microsoft.com/en-us/windows/desktop/api/d3dcompiler/nf-d3dcompiler-d3dcompilefromfile
  // https://docs.microsoft.com/ko-kr/windows/desktop/api/d3dcompiler/nf-d3dcompiler-d3dcompile2
  ID3DBlob* pErrorBlob = nullptr;
  HRESULT hr = D3DCompile2(
    fdBuffer.data(), fdBuffer.size(),
    nullptr,
    nullptr, nullptr,
    szEntryPoint, szShaderModel,
    dwShaderFlags, 0,
    0,
    nullptr, 0,
    ppBlobOut, &pErrorBlob
  );

  // Check error.
  if (FAILED(hr))
  {
    if (pErrorBlob != nullptr)
    {
      OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
    }
  }

  ReleaseCOM(pErrorBlob);
  return hr;
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

  // Crate D3D11 Device & Context
  auto optDeviceContext = CreateD3D11Device();
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
    HR(dxgiFactory->CreateSwapChain(&mD3DDevice.Get(), &sd, &mD3DSwapChain));
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
    mD3DDevice->CreateRenderTargetView(&mBackBufferTexture.Get(), nullptr, &mRenderTargetView);
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
  HR(mD3DDevice->CreateTexture2D(&mDepthStencilDesc, nullptr, &mDepthStencilBuffer));
  HR(mD3DDevice->CreateDepthStencilView(&mDepthStencilBuffer.Get(), nullptr, &mDepthStencilView));
  mD3DImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, &mDepthStencilView.Get());

  // Set Viewport
  D3D11_VIEWPORT vp;
  vp.TopLeftX = 0.0f;
  vp.TopLeftY = 0.0f;
  vp.Width    = static_cast<float>(width);
  vp.Height   = static_cast<float>(height);
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  mD3DImmediateContext->RSSetViewports(1, &vp);

  // * Build Geometry Buffers.
  using dy::math::DVector3;
  using dy::math::DVector4;
  using dy::math::TReal;

  // Create Vertex Buffer.
  struct DVertex final
  {
    DVector3<TReal> mPos;
    DVector4<TReal> mCol;
  };

  std::array<DVertex, 8> vertices =
  { 
    DVertex
    { {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
    { {+1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f} },
    { {+1.0f, -1.0f, +1.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
    { {-1.0f, -1.0f, +1.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },

    { {-1.0f, +1.0f, -1.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
    { {+1.0f, +1.0f, -1.0f}, {1.0f, 1.0f, 0.0f, 1.0f} },
    { {+1.0f, +1.0f, +1.0f}, {1.0f, 0.0f, 1.0f, 1.0f} },
    { {-1.0f, +1.0f, +1.0f}, {0.0f, 1.0f, 1.0f, 1.0f} },
  };

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

  // Create Index Buffer.
  std::array<unsigned, 36> indices =
  {
    0, 1, 2, 0, 2, 3,
    4, 5, 6, 4, 6, 7,
    
    1, 2, 6, 1, 6, 5,
    3, 0, 4, 3, 4, 7,

    0, 1, 5, 0, 5, 4,
    2, 6, 7, 2, 7, 3,
  };

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

  // Compile Vertex Shader.
  // https://docs.microsoft.com/ko-kr/windows/desktop/api/d3dcompiler/nf-d3dcompiler-d3dcompile2
  // "VS" is entry point.
  IComOwner<ID3DBlob> ownVsBlob = nullptr;
  HR(CompileShaderFromFile("../../Resource/Shader.fx", "VS", "vs_5_0", &ownVsBlob));

  IComOwner<ID3D11VertexShader> ownVertexShader = nullptr;
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

  IComOwner<ID3D11InputLayout> ownVsLayout = nullptr;
  HR(mD3DDevice->CreateInputLayout(
    vertexDesc.data(), static_cast<UINT>(vertexDesc.size()), 
    ownVsBlob->GetBufferPointer(), ownVsBlob->GetBufferSize(),
    &ownVsLayout));

  // Set layout into logical device context.
  mD3DImmediateContext->IASetInputLayout(&ownVsLayout.Get());
  ownVsBlob->Release();

  // Compile Pixel Shader 
  // "PS" is entry point.
  IComOwner<ID3DBlob> ownPsBlob = nullptr;
  HR(CompileShaderFromFile("../../Resource/Shader.fx", "PS", "ps_5_0", &ownPsBlob));

  IComOwner<ID3D11PixelShader> ownPsShader = nullptr;
  HR(mD3DDevice->CreatePixelShader(
    ownPsBlob->GetBufferPointer(), ownPsBlob->GetBufferSize(), 
    nullptr, 
    &ownPsShader));
  ownPsBlob->Release();

  mD3DImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  UINT stride = sizeof(DVertex);
  UINT offset = 0;
  mD3DImmediateContext->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
  mD3DImmediateContext->IASetIndexBuffer(&iBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

  IComOwner<ID3D11RasterizerState> ownRasterState = nullptr;
  {
    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = true;
    rasterDesc.DepthBias = 0;
    rasterDesc.SlopeScaledDepthBias = 0;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.ScissorEnable = true;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.AntialiasedLineEnable = false;

    mD3DDevice->CreateRasterizerState(&rasterDesc, &ownRasterState);
  }
  mD3DImmediateContext->RSSetState(&ownRasterState.Get());

  using TColor = std::array<float, 4>;
  const std::array<TColor, 4> colors =
  {
    TColor
    {0, 0, 1, 1}, {1, 0, 1, 1}, {1, 1, 0, 1}, {1, 0, 0, 1},
  };

  // Loop
	while (platform->CanShutdown() == false)
	{
    platform->PollEvents();

    // D3D Routine
    mD3DImmediateContext->ClearRenderTargetView(&mRenderTargetView.Get(), colors[0].data());
    mD3DImmediateContext->ClearDepthStencilView(&mDepthStencilView.Get(), 
      D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
      1.0f,
      0);
    mD3DImmediateContext->VSSetShader(&ownVertexShader.Get(), nullptr, 0);
    mD3DImmediateContext->PSSetShader(&ownPsShader.Get(), nullptr, 0);
    mD3DImmediateContext->DrawIndexed(36, 0, 0);

    // Present the back buffer to the screen.
    HR(mD3DSwapChain->Present(0, 0));
	}

  platform->RemoveAllWindow();
  platform->RemoveConsoleWindow();
  platform->ReleasePlatform();
  WIN32_TRY_TURN_OFF_DEBUG();
	return 0;
}
