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

#include <FD3D11Factory.h>

#include <cassert>
#include <filesystem>
#include <d3dcompiler.h>

#include <StringUtil/XUtility.h>
#include <Graphics/MD3D11Resources.h>
#include <APlatformBase.h>
#include <HelperMacro.h>

std::optional<std::pair<IComOwner<ID3D11Device>, IComOwner<ID3D11DeviceContext>>> 
FD3D11Factory::CreateD3D11Device(dy::APlatformBase& platform)
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
  decltype(mD3DDevice)::TPtrType pDevice = nullptr;
  decltype(mD3DImmediateContext)::TPtrType pDc = nullptr;
  HRESULT hr = D3D11CreateDevice(
    nullptr,              // Default Adapter (Primary)
    D3D_DRIVER_TYPE_HARDWARE, // Use hardware driver (Most optimal) 
    nullptr,              // No Software device because we use TYPE_HARDWARE (D3D11).
    createDeviceFlags,    // Set flags (DEBUG, SINGLETHREAD etc...)
    nullptr,              // If no flag is exist, just pick up the highest version of SDK.
    0,                    // Above argument brings the array of D3D_FEATURE, so we set it to 0 as nullptr. 
    D3D11_SDK_VERSION,    // Always specify this.
    &pDevice,             // Output
    &featureLevel,        // Output
    &pDc                  // Output
  );
  mD3DDevice = decltype(mD3DDevice){pDevice};
  mD3DImmediateContext = decltype(mD3DImmediateContext){pDc};
  
  // Error checking.
  if (FAILED(hr))
  {
    platform.GetDebugManager().OnAssertionFailed(
      "D3D11CreateDevice Failed.\n", __FUNCTION__, __FILE__, __LINE__
    );
    return std::nullopt;
  }
  if (featureLevel != D3D_FEATURE_LEVEL_11_0)
  {
    platform.GetDebugManager().OnAssertionFailed(
      "Direct3D Feature Level 11 unsupported.\n", __FUNCTION__, __FILE__, __LINE__
    );
    return std::nullopt;
  }

  return std::pair{std::move(mD3DDevice), std::move(mD3DImmediateContext)};
}

std::optional<D11DefaultHandles> FD3D11Factory::CreateDefaultFrameBuffer(
  dy::APlatformBase& platform, dy::DWindowHandle& hWnd)
{
  const auto width  = platform.GetWindowWidth(hWnd);
  const auto height = platform.GetWindowHeight(hWnd);
  HWND mainWindowHandle = static_cast<HWND>(platform._GetHandleOf(hWnd));

  auto deviceHandle = MD3D11Resources::CreateD3D11DefaultDevice(platform);
  assert(MD3D11Resources::HasDevice(*deviceHandle) == true);

  // Create swap chain.
  D11SwapChainHandle swapChainHandle = nullptr;
  {
    // Describe Swap chain.
    // https://bell0bytes.eu/the-swap-chain/
    DXGI_SWAP_CHAIN_DESC desc = FD3D11Factory::GetDefaultSwapChainDesc(width, height, mainWindowHandle);
    swapChainHandle = *MD3D11Resources::CreateSwapChain(*deviceHandle, mainWindowHandle, desc);
    assert(MD3D11Resources::HasSwapChain(swapChainHandle) == true);
  }

  // Create default RTV from swap-chain.
  D11HandleRTV defaultRtv = nullptr;
  {
    defaultRtv = *MD3D11Resources::CreateRTVFromSwapChain(*deviceHandle, swapChainHandle);
    assert(MD3D11Resources::HasRTV(defaultRtv) == true);
  }

  // Create depth-stencil texture and view.
  // Second parameter of CreateTexture2D is a pointer to initial data to fill the texture with.
  // We do not specify additional DESC to DSV, leave it inherits properties of Depth/Stencil Texture.
  D11HandleTexture2D depthStencilTexture = nullptr;
  {
    // Descript Depth/Stencil Buffer and View descriptors.
    // https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ns-d3d11-d3d11_texture2d_desc
    depthStencilTexture = *MD3D11Resources::CreateTexture2D(
      *deviceHandle,
      FD3D11Factory::GetDefaultDepthStencilDesc(width, height));
    assert(MD3D11Resources::HasTexture2D(depthStencilTexture) == true);
  }
  D11HandleDSV defaultDsv = nullptr;
  {
    defaultDsv = *MD3D11Resources::CreateDSV(*deviceHandle, depthStencilTexture);
    assert(MD3D11Resources::HasDSV(defaultDsv) == true);
  }
  
  // Create default rasterizer-state.
  D11HandleRasterState defaultRasterState = nullptr;
  {
    defaultRasterState = *MD3D11Resources::CreateRasterState(
      *deviceHandle, 
      FD3D11Factory::GetDefaultRasterStateDesc());
    assert(MD3D11Resources::HasRasterState(defaultRasterState) == true);
  }
  
  // Create default depth-stencil state.
  D11HandleDepthStencilState handleDepthStencilState = nullptr;
  {
    handleDepthStencilState = *MD3D11Resources::CreateDepthStencilState(
      *deviceHandle,
      FD3D11Factory::GetDefaultDepthStencilStateDesc());
    assert(MD3D11Resources::HasDepthStencilState(handleDepthStencilState) == true);
  }
  
  // Create default blend state.
  D11HandleBlendState handleBlendState = nullptr;
  {
    handleBlendState = *MD3D11Resources::CreateBlendState(
      *deviceHandle,
      FD3D11Factory::GetDefaultBlendStateDesc());
    assert(MD3D11Resources::HasBlendState(handleBlendState) == true);
  }

  D11DefaultHandles result;
  result.mWHwnd       = mainWindowHandle;
  result.mDevice      = *deviceHandle;
  result.mSwapChain   = swapChainHandle;
  result.mRTV         = defaultRtv;
  result.mDSTexture2D = depthStencilTexture;
  result.mDSV         = defaultDsv;
  result.mRasterState = defaultRasterState;
  result.mDepthStencilState = handleDepthStencilState;
  result.mBlendState  = handleBlendState;
  return result;
}

HRESULT 
FD3D11Factory::CompileShaderFromFile(
  dy::APlatformBase& platform,
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
    platform.GetDebugManager().OnAssertionFailed(
      absolutePath.string().c_str(),
      __FUNCTION__, __FILE__, __LINE__
    );
    return E_FAIL;
  }

  // Create file descriptor.
  FILE* fdFile = fopen(szFilePath.c_str(), "r");
  if (fdFile == nullptr) 
  { 
    platform.GetDebugManager().OnAssertionFailed(
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

std::optional<D11HandleBlob> FD3D11Factory::CompileShaderFromFile2(
  dy::APlatformBase& platform,
  const std::filesystem::path& filePath, 
  const std::string& entryPointFunc, 
  const std::string& shaderModel,
  HRESULT* outResult)
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

  // Check file is exist.
  if (std::filesystem::exists(filePath) == false)
  {
    const auto absolutePath = std::filesystem::absolute(filePath);
    platform.GetDebugManager().OnAssertionFailed(
      absolutePath.string().c_str(),
      __FUNCTION__, __FILE__, __LINE__
    );

    if (outResult != nullptr) { *outResult = E_FAIL; }
    return std::nullopt;
  }
  
  // Create file descriptor.
  FILE* fdFile = fopen(filePath.string().c_str(), "r");
  if (fdFile == nullptr) 
  { 
    platform.GetDebugManager().OnAssertionFailed(
      "Failed to read shader file.", __FUNCTION__, __FILE__, __LINE__
    );

    if (outResult != nullptr) { *outResult = E_FAIL; }
    return std::nullopt;
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
  ID3DBlob* pBlob = nullptr;
  ID3DBlob* pErrorBlob = nullptr;
  HRESULT hr = D3DCompile2(
    fdBuffer.data(), fdBuffer.size(),
    nullptr, nullptr, nullptr,
    entryPointFunc.c_str(), shaderModel.c_str(),
    dwShaderFlags, 0,
    0, nullptr, 0,
    &pBlob, &pErrorBlob
  );

  // Check error.
  D11HandleBlob resultBlob = nullptr;
  if (FAILED(hr))
  {
    if (pErrorBlob != nullptr)
    {
      OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
    }

    ReleaseCOM(pBlob);
    ReleaseCOM(pErrorBlob);
    if (outResult != nullptr) { *outResult = E_FAIL; }
    return std::nullopt;
  }
  else
  {
    resultBlob = *MD3D11Resources::InsertRawBlob(pBlob);
    ReleaseCOM(pErrorBlob);
    return resultBlob;
  }
}

DXGI_SWAP_CHAIN_DESC FD3D11Factory::GetDefaultSwapChainDesc(
  unsigned width,
  unsigned height,
  HWND pOutputWindowHandle)
{
  static bool isInitialized = false;
  static DXGI_SWAP_CHAIN_DESC desc = {};

  if (isInitialized == false)
  {
    // Describe frame-buffer format.
    desc.BufferDesc.Width   = 0;
    desc.BufferDesc.Height  = 0;
    desc.BufferDesc.RefreshRate.Numerator   = 60; // Want 60 FPS
    desc.BufferDesc.RefreshRate.Denominator = 1;// When Numerator is not 1 or 0, Denominator must be 1.
    desc.BufferDesc.Format  = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; 
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    desc.SampleDesc.Count   = 1;
    desc.SampleDesc.Quality = 0;

    // Describe overall properties.
    desc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount  = 1; // The count of `back buffer`.
    desc.OutputWindow = nullptr;
    desc.Windowed     = true;
    desc.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD; // Discard swapped old buffer.
    desc.Flags        = 0; 

    isInitialized = true;
  }

  desc.BufferDesc.Width = width;
  desc.BufferDesc.Width = height;
  desc.OutputWindow = pOutputWindowHandle;
  return desc;
}

HRESULT FD3D11Factory::CreateD3D11SwapChain(
  ID3D11Device& device, 
  HWND windowHandle,
  DXGI_SWAP_CHAIN_DESC& descriptor,
  IComOwner<IDXGISwapChain>& ownSwapChainRef)
{
  // To call creating swap-chain function from DXGIFactory,
  // we must get DXGI device, adapter, and etc... using querying.
  IComOwner<IDXGIDevice> dxgiDevice = nullptr;
  HR(device.QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

  IComOwner<IDXGIAdapter> dxgiAdapter = nullptr;
  HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

  IComOwner<IDXGIFactory> dxgiFactory = nullptr;
  HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

  // Create swap chain.
  {
    IDXGISwapChain* pSch = nullptr;
    HR(dxgiFactory->CreateSwapChain(&device, &descriptor, &pSch));
    HR(dxgiFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_WINDOW_CHANGES));
    ownSwapChainRef = IComOwner<IDXGISwapChain>{pSch};
  }

  return S_OK;
}

D3D11_TEXTURE2D_DESC FD3D11Factory::GetDefaultDepthStencilDesc(unsigned width, unsigned height)
{
  static bool isInitialized = false;
  static D3D11_TEXTURE2D_DESC desc = {};

  if (isInitialized == false)
  {
    // Depth/Stencil Render buffer (texture) descript.
    desc.Width = 0;
    desc.Height = 0;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Depth 24, Stencil 8.

    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0; 
    desc.MiscFlags = 0;

    isInitialized = true;
  }

  desc.Width = width;
  desc.Height = height;
  return desc;
}

D3D11_RASTERIZER_DESC FD3D11Factory::GetDefaultRasterStateDesc()
{
  static bool isInitialized = false;
  static D3D11_RASTERIZER_DESC desc = {};

  if (isInitialized == false)
  {
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_BACK;
    desc.FrontCounterClockwise = true;
    desc.DepthBias = INT(0);
    desc.DepthBiasClamp = FLOAT(0);
    desc.SlopeScaledDepthBias = 0;
    desc.DepthClipEnable = true;
    desc.ScissorEnable = false;
    desc.MultisampleEnable = false;
    desc.AntialiasedLineEnable = false;

    isInitialized = true;
  }

  return desc;
}

D3D11_DEPTH_STENCIL_DESC FD3D11Factory::GetDefaultDepthStencilStateDesc()
{
  static bool isInitialized = false;
  static D3D11_DEPTH_STENCIL_DESC desc = {};

  if (isInitialized == false)
  {
    desc.DepthEnable    = FALSE;
    desc.DepthFunc      = D3D11_COMPARISON_LESS;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    desc.StencilEnable  = FALSE;
    desc.StencilReadMask  = 0xff;
    desc.StencilWriteMask = 0xff;

    isInitialized = true;
  }

  return desc;
}

D3D11_BLEND_DESC FD3D11Factory::GetDefaultBlendStateDesc()
{
  static bool isInitialized = false;
  static D3D11_BLEND_DESC desc = {};

  if (isInitialized == false)
  {
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
      desc.RenderTarget[0] = rtBlendDesc; 
    }
    desc.AlphaToCoverageEnable = false;
    desc.IndependentBlendEnable = false;

    isInitialized = true;
  }

  return desc;
}

D3D11_QUERY_DESC FD3D11Factory::GetDefaultTimeStampDisjointQueryDesc()
{
  static bool isInitialized = false;
  static D3D11_QUERY_DESC desc = {};

  if (isInitialized == false)
  {
    D3D11_QUERY_DESC queryDesc;
    queryDesc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
    queryDesc.MiscFlags = 0;

    isInitialized = true;
  }

  return desc;
}

D3D11_QUERY_DESC FD3D11Factory::GetDefaultTimeStampFragmentQueryDesc()
{
  static bool isInitialized = false;
  static D3D11_QUERY_DESC desc = {};

  if (isInitialized == false)
  {
    D3D11_QUERY_DESC queryDesc;
    queryDesc.Query = D3D11_QUERY_TIMESTAMP;
    queryDesc.MiscFlags = 0;

    isInitialized = true;
  }

  return desc;
}

std::optional<IComOwner<ID3D11Query>> FD3D11Factory::CreateTimestampQuery(
  ID3D11Device& device,
  bool isDisjoint)
{
  D3D11_QUERY_DESC queryDesc;
  queryDesc.Query = isDisjoint == true ? D3D11_QUERY_TIMESTAMP_DISJOINT : D3D11_QUERY_TIMESTAMP;
  queryDesc.MiscFlags = 0;

  ID3D11Query* pQuery = nullptr;
  if (device.CreateQuery(&queryDesc, &pQuery) != S_OK)
  {
    if (pQuery != nullptr) { pQuery->Release(); }
    return std::nullopt;
  }

  return IComOwner<ID3D11Query>{pQuery};
}

std::optional<std::pair<IComOwner<ID3D11Query>, IComOwner<ID3D11Query>>>
FD3D11Factory::CreateTimestampQueryPair(ID3D11Device& device)
{
  IComOwner<ID3D11Query> start  = nullptr;
  IComOwner<ID3D11Query> end    = nullptr;

  D3D11_QUERY_DESC queryDesc;
  queryDesc.Query = D3D11_QUERY_TIMESTAMP;
  queryDesc.MiscFlags = 0;

  ID3D11Query* pStart,* pEnd;
  if (device.CreateQuery(&queryDesc, &pStart) != S_OK)
  {
    if (pStart != nullptr) { pStart->Release(); }
    return std::nullopt;
  }

  if (device.CreateQuery(&queryDesc, &pEnd) != S_OK)
  {
    if (pEnd != nullptr) { pEnd->Release(); }
    return std::nullopt;
  }

  return std::pair{IComOwner<ID3D11Query>{pStart}, IComOwner<ID3D11Query>{pEnd}};
}

std::optional<std::pair<D11HandleQuery, D11HandleQuery>> 
FD3D11Factory::CreateTimestampQueryPair2(const D11HandleDevice& hDevice)
{
  auto start = MD3D11Resources::CreateQuerySimple(hDevice, E11SimpleQueryType::TimeStampFragment);
  if (start.has_value() == false)
  {
    return std::nullopt;
  }
    
  auto end = MD3D11Resources::CreateQuerySimple(hDevice, E11SimpleQueryType::TimeStampFragment);
  if (end.has_value() == false)
  {
    return std::nullopt;
  }

  return std::pair{*start, *end};
}
