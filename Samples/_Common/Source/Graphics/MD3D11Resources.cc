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

#include <Graphics/MD3D11Resources.h>
#include <APlatformBase.h>
#include <HelperMacro.h>
#include <d3dcompiler.h>

MD3D11Resources::THashMap<DD3DResourceDevice>         MD3D11Resources::mDevices; 
MD3D11Resources::THashMap<IComOwner<ID3D11Buffer>> MD3D11Resources::mBuffers;
MD3D11Resources::THashMap<IComOwner<IDXGISwapChain>>  MD3D11Resources::mSwapChains;
MD3D11Resources::THashMap<IComOwner<ID3D11RenderTargetView>> MD3D11Resources::mRTVs;
MD3D11Resources::THashMap<IComOwner<ID3D11DepthStencilView>> MD3D11Resources::mDSVs;
MD3D11Resources::THashMap<IComOwner<ID3D11RasterizerState>> MD3D11Resources::mRasterStates;
MD3D11Resources::THashMap<IComOwner<ID3D11DepthStencilState>> MD3D11Resources::mDepthStencilStates;
MD3D11Resources::THashMap<IComOwner<ID3D11BlendState>> MD3D11Resources::mBlendStates;
MD3D11Resources::THashMap<IComOwner<ID3D11Texture2D>> MD3D11Resources::mTexture2Ds;
MD3D11Resources::THashMap<IComOwner<ID3DBlob>> MD3D11Resources::mBlobs;
MD3D11Resources::THashMap<IComOwner<ID3D11Query>> MD3D11Resources::mQueries;

//!
//! Device
//! 

std::optional<D11HandleDevice> 
MD3D11Resources::CreateD3D11DefaultDevice(dy::APlatformBase& platform)
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

  // Insert.
  auto [it, isSucceeded] = mDevices.try_emplace(
    ::dy::math::DUuid{true}, 
    std::move(mD3DDevice), std::move(mD3DImmediateContext));
  assert(isSucceeded == true);
  const auto& [uuid, pOwner] = *it;

  return {uuid};
}

bool MD3D11Resources::HasDevice(const D11HandleDevice& handle) noexcept
{
  return TThis::mDevices.find(handle.GetUuid()) != TThis::mDevices.end();
}

IComBorrow<ID3D11Device> MD3D11Resources::GetDevice(const D11HandleDevice& handle)
{
  assert(TThis::HasDevice(handle) == true);
  
  auto& object = TThis::mDevices.at(handle.GetUuid());
  return object.mOwnDevice.GetBorrow();
}

IComBorrow<ID3D11DeviceContext> MD3D11Resources::GetDeviceContext(const D11HandleDevice& handle)
{
  assert(TThis::HasDevice(handle) == true);

  auto& object = TThis::mDevices.at(handle.GetUuid());
  return object.mOwnDc.GetBorrow();
}

bool MD3D11Resources::RemoveDevice(const D11HandleDevice& handle)
{
  // Validation check.
  if (TThis::HasDevice(handle) == false) { return false; }

  TThis::mDevices.erase(handle.GetUuid());
  return true;
}

//!
//! Swap chain
//!

std::optional<D11SwapChainHandle> 
MD3D11Resources::CreateSwapChain(
  const D11HandleDevice& handle, 
  const HWND mainHwnd, 
  DXGI_SWAP_CHAIN_DESC& desc)
{
  // Validation check.
  if (TThis::HasDevice(handle) == false) { return std::nullopt; };
  auto device = TThis::GetDevice(handle);

  // To call creating swap-chain function from DXGIFactory,
  // we must get DXGI device, adapter, and etc... using querying.
  IDXGIDevice* dxgiDevice = nullptr;
  HR(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

  IDXGIAdapter* dxgiAdapter = nullptr;
  HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

  IDXGIFactory* dxgiFactory = nullptr;
  HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

  // Create swap chain.
  IDXGISwapChain* pSch = nullptr;
  HR(dxgiFactory->CreateSwapChain(device.GetPtr(), &desc, &pSch));
  HR(dxgiFactory->MakeWindowAssociation(mainHwnd, DXGI_MWA_NO_WINDOW_CHANGES));

  // Release internal resources.
  ReleaseCOM(dxgiFactory);
  ReleaseCOM(dxgiAdapter);
  ReleaseCOM(dxgiDevice);

  // If failed, just return with nullopt.
  if (pSch == nullptr) { return std::nullopt; }

  // Insert.
  auto [it, isSucceeded] = TThis::mSwapChains.try_emplace(::dy::math::DUuid{true}, pSch);
  assert(isSucceeded == true);
  const auto& [uuid, pOwner] = *it;

  return {uuid};
}

bool MD3D11Resources::HasSwapChain(const D11SwapChainHandle& handle) noexcept
{
  return TThis::mSwapChains.find(handle.GetUuid()) != TThis::mSwapChains.end();
}

IComBorrow<IDXGISwapChain> 
MD3D11Resources::GetSwapChain(const D11SwapChainHandle& handle)
{
  assert(TThis::HasSwapChain(handle) == true);

  auto& object = TThis::mSwapChains.at(handle.GetUuid());
  return object.GetBorrow();  
}

bool MD3D11Resources::RemoveSwapChain(const D11SwapChainHandle& handle)
{
  // Validation check.
  if (TThis::HasSwapChain(handle) == false) { return false; }

  TThis::mSwapChains.erase(handle.GetUuid());
  return true;
}

//!
//! Rendering-Target View
//!

std::optional<D11HandleRTV>
MD3D11Resources::CreateRTVFromSwapChain(
  const D11HandleDevice& hDevice, const D11SwapChainHandle& hSwapChain, 
  D3D11_RENDER_TARGET_VIEW_DESC* pDesc)
{
  // Validation check.
  if (TThis::HasDevice(hDevice) == false) { return std::nullopt; }
  if (TThis::HasSwapChain(hSwapChain) == false) { return std::nullopt; }

  auto swapChain  = TThis::GetSwapChain(hSwapChain);
  auto device     = TThis::GetDevice(hDevice);

  // Create RTV resource.
  ID3D11RenderTargetView* pRtv = nullptr;
  {
    ID3D11Texture2D* mBackBufferTexture = nullptr; 

    // This function increase internal COM instance reference counting.
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&mBackBufferTexture);
    HR(device->CreateRenderTargetView(mBackBufferTexture, pDesc, &pRtv));
    ReleaseCOM(mBackBufferTexture);
  }

  // If failed, just return with nullopt.
  if (pRtv == nullptr) { return std::nullopt; }

  // Insert.
  auto [it, isSucceeded] = TThis::mRTVs.try_emplace(::dy::math::DUuid{true}, pRtv);
  assert(isSucceeded == true);
  const auto& [uuid, pOwner] = *it;

  return {uuid};
}

bool MD3D11Resources::HasRTV(const D11HandleRTV& handle) noexcept
{
  return TThis::mRTVs.find(handle.GetUuid()) != TThis::mRTVs.end();
}

IComBorrow<ID3D11RenderTargetView> MD3D11Resources::GetRTV(const D11HandleRTV& handle)
{
  assert(TThis::HasRTV(handle) == true);

  auto& object = TThis::mRTVs.at(handle.GetUuid());
  return object.GetBorrow();  
}

bool MD3D11Resources::RemoveRTV(const D11HandleRTV& handle)
{
  // Validation check.
  if (TThis::HasRTV(handle) == false) { return false; }

  TThis::mRTVs.erase(handle.GetUuid());
  return true;
}

//!
//! Depth-Stencil View
//!

std::optional<D11HandleDSV>
MD3D11Resources::CreateDSV(
  const D11HandleDevice& hDevice, const D11HandleTexture2D& hTexture2D,
  D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc)
{
  // Validation Check
  if (TThis::HasDevice(hDevice) == false) { return std::nullopt; }
  if (TThis::HasTexture2D(hTexture2D) == false) { return std::nullopt; }

  auto texture2D  = TThis::GetTexture2D(hTexture2D);
  auto device     = TThis::GetDevice(hDevice);

  // Create DSV resource.
  ID3D11DepthStencilView* pDsv = nullptr;
  HR(device->CreateDepthStencilView(texture2D.GetPtr(), pDesc, &pDsv));

  // If failed, just return with nullopt.
  if (pDsv == nullptr) { return std::nullopt; }

  // Insert.
  auto [it, isSucceeded] = TThis::mDSVs.try_emplace(::dy::math::DUuid{true}, pDsv);
  assert(isSucceeded == true);
  const auto& [uuid, pOwner] = *it;

  return {uuid};
}

bool MD3D11Resources::HasDSV(const D11HandleDSV& handle) noexcept
{
  return TThis::mDSVs.find(handle.GetUuid()) != TThis::mDSVs.end();
}

IComBorrow<ID3D11DepthStencilView> MD3D11Resources::GetDSV(const D11HandleDSV& handle)
{
  assert(TThis::HasDSV(handle) == true);

  auto& object = TThis::mDSVs.at(handle.GetUuid());
  return object.GetBorrow();  
}

bool MD3D11Resources::RemoveDSV(const D11HandleDSV& handle)
{
  // Validation check.
  if (TThis::HasDSV(handle) == false) { return false; }

  TThis::mDSVs.erase(handle.GetUuid());
  return true;
}

//!
//! Rasterizer-State
//!

std::optional<D11HandleRasterState>
MD3D11Resources::CreateRasterState(const D11HandleDevice& hDevice, const D3D11_RASTERIZER_DESC& desc)
{
  // Validation Check
  if (TThis::HasDevice(hDevice) == false) { return std::nullopt; }
  auto device = TThis::GetDevice(hDevice);

  // Create DSV resource.
  ID3D11RasterizerState* pRasterState = nullptr;
  HR(device->CreateRasterizerState(&desc, &pRasterState));

  // If failed, just return with nullopt.
  if (pRasterState == nullptr) { return std::nullopt; }

  // Insert.
  auto [it, isSucceeded] = TThis::mRasterStates.try_emplace(::dy::math::DUuid{true}, pRasterState);
  assert(isSucceeded == true);
  const auto& [uuid, pOwner] = *it;

  return {uuid};
}

bool MD3D11Resources::HasRasterState(const D11HandleRasterState& handle) noexcept
{
  return TThis::mRasterStates.find(handle.GetUuid()) != TThis::mRasterStates.end();
}

IComBorrow<ID3D11RasterizerState> 
MD3D11Resources::GetRasterState(const D11HandleRasterState& handle)
{
  assert(TThis::HasRasterState(handle) == true);

  auto& object = TThis::mRasterStates.at(handle.GetUuid());
  return object.GetBorrow(); 
}

bool MD3D11Resources::RemoveRasterState(const D11HandleRasterState& handle)
{
  // Validation check.
  if (TThis::HasRasterState(handle) == false) { return false; }

  TThis::mRasterStates.erase(handle.GetUuid());
  return true;
}

//!
//! Depth-Stencil State
//!

std::optional<D11HandleDepthStencilState>
MD3D11Resources::CreateDepthStencilState(
  const D11HandleDevice& hDevice, const D3D11_DEPTH_STENCIL_DESC& desc)
{
  // Validation Check
  if (TThis::HasDevice(hDevice) == false) { return std::nullopt; }
  auto device = TThis::GetDevice(hDevice);

  // Create DSV resource.
  ID3D11DepthStencilState* pDss = nullptr;
  HR(device->CreateDepthStencilState(&desc, &pDss));

  // If failed, just return with nullopt.
  if (pDss == nullptr) { return std::nullopt; }

  // Insert.
  auto [it, isSucceeded] = TThis::mDepthStencilStates.try_emplace(::dy::math::DUuid{true}, pDss);
  assert(isSucceeded == true);
  const auto& [uuid, pOwner] = *it;

  return {uuid};
}

bool MD3D11Resources::HasDepthStencilState(const D11HandleDepthStencilState& handle) noexcept
{
  return TThis::mDepthStencilStates.find(handle.GetUuid()) != TThis::mDepthStencilStates.end();
}

IComBorrow<ID3D11DepthStencilState> 
MD3D11Resources::GetDepthStencilState(const D11HandleDepthStencilState& handle)
{
  assert(TThis::HasDepthStencilState(handle) == true);

  auto& object = TThis::mDepthStencilStates.at(handle.GetUuid());
  return object.GetBorrow(); 
}

bool MD3D11Resources::RemoveDepthStencilState(const D11HandleDepthStencilState& handle)
{
  // Validation check.
  if (TThis::HasDepthStencilState(handle) == false) { return false; }

  TThis::mDepthStencilStates.erase(handle.GetUuid());
  return true;
}

//!
//! Blend State
//!

std::optional<D11HandleBlendState> 
MD3D11Resources::CreateBlendState(const D11HandleDevice& hDevice, const D3D11_BLEND_DESC& desc)
{
  // Validation Check
  if (TThis::HasDevice(hDevice) == false) { return std::nullopt; }
  auto device = TThis::GetDevice(hDevice);

  // Create DSV resource.
  ID3D11BlendState* pBlend = nullptr;
  HR(device->CreateBlendState(&desc, &pBlend));

  // If failed, just return with nullopt.
  if (pBlend == nullptr) { return std::nullopt; }

  // Insert.
  auto [it, isSucceeded] = TThis::mBlendStates.try_emplace(::dy::math::DUuid{true}, pBlend);
  assert(isSucceeded == true);
  const auto& [uuid, pOwner] = *it;

  return {uuid};
}

bool MD3D11Resources::HasBlendState(const D11HandleBlendState& handle) noexcept
{
  return TThis::mBlendStates.find(handle.GetUuid()) != TThis::mBlendStates.end();
}

IComBorrow<ID3D11BlendState> 
MD3D11Resources::GetBlendState(const D11HandleBlendState& handle)
{
  assert(TThis::HasBlendState(handle) == true);

  auto& object = TThis::mBlendStates.at(handle.GetUuid());
  return object.GetBorrow(); 
}

bool MD3D11Resources::RemoveBlendState(const D11HandleBlendState& handle)
{
  // Validation check.
  if (TThis::HasBlendState(handle) == false) { return false; }

  TThis::mBlendStates.erase(handle.GetUuid());
  return true;
}

//!
//! Texture2D
//!

std::optional<D11HandleTexture2D>
MD3D11Resources::CreateTexture2D(
  const D11HandleDevice& hDevice, 
  const D3D11_TEXTURE2D_DESC& desc, 
  const void* pInitBuffer)
{
  // Validation check.
  if (TThis::HasDevice(hDevice) == false) { return std::nullopt; }
  auto device = TThis::GetDevice(hDevice);

  // Create ID3D11Texture2D Resource.
  ID3D11Texture2D* pTexture2d = nullptr;
  if (pInitBuffer != nullptr)
  {
    D3D11_SUBRESOURCE_DATA subresource = {};
    subresource.pSysMem = pInitBuffer;
    
    HR(device->CreateTexture2D(&desc, &subresource, &pTexture2d));
  }
  else
  {
    HR(device->CreateTexture2D(&desc, nullptr, &pTexture2d));
  }

  // If pTexture2D is null, just return nullopt.
  if (pTexture2d == nullptr) { return std::nullopt; }

  // Insert.
  auto [it, isSucceeded] = TThis::mTexture2Ds.try_emplace(::dy::math::DUuid{true}, pTexture2d);
  assert(isSucceeded == true);
  const auto& [uuid, pOwner] = *it;

  return {uuid};
}

bool MD3D11Resources::HasTexture2D(const D11HandleTexture2D& handle)
{
  return TThis::mTexture2Ds.find(handle.GetUuid()) != TThis::mTexture2Ds.end();
}

IComBorrow<ID3D11Texture2D> MD3D11Resources::GetTexture2D(const D11HandleTexture2D& handle)
{
  assert(TThis::HasTexture2D(handle) == true);

  auto& object = TThis::mTexture2Ds.at(handle.GetUuid());
  return object.GetBorrow();  
}

bool MD3D11Resources::RemoveTexture2D(const D11HandleTexture2D& handle)
{
  // Validation check.
  if (TThis::HasTexture2D(handle) == false) { return false; }

  TThis::mTexture2Ds.erase(handle.GetUuid());
  return true;
}

//!
//! Buffer
//!

std::optional<D11HandleBuffer>
MD3D11Resources::CreateBuffer(
  const D11HandleDevice& hDevice, 
  const D3D11_BUFFER_DESC& desc, 
  const void* pInitBuffer)
{
  // Validation check.
  if (TThis::HasDevice(hDevice) == false) { return std::nullopt; }
  assert(desc.Usage == D3D11_USAGE_IMMUTABLE ? pInitBuffer != nullptr : true);

  auto device = TThis::GetDevice(hDevice);

  // Create ID3D11Buffer Resource.
  ID3D11Buffer* pBuffer = nullptr;
  if (pInitBuffer != nullptr)
  {
    D3D11_SUBRESOURCE_DATA subresource = {};
    subresource.pSysMem = pInitBuffer;
    
    HR(device->CreateBuffer(&desc, &subresource, &pBuffer));
  }
  else
  {
    HR(device->CreateBuffer(&desc, nullptr, &pBuffer));
  }

  // If pTexture2D is null, just return nullopt.
  if (pBuffer == nullptr) { return std::nullopt; }

  // Insert.
  auto [it, isSucceeded] = TThis::mBuffers.try_emplace(::dy::math::DUuid{true}, pBuffer);
  assert(isSucceeded == true);
  const auto& [uuid, pOwner] = *it;

  return {uuid}; 
}

bool MD3D11Resources::HasBuffer(const D11HandleBuffer& handle)
{
  return TThis::mBuffers.find(handle.GetUuid()) != TThis::mBuffers.end();
}

IComBorrow<ID3D11Buffer> MD3D11Resources::GetBuffer(const D11HandleBuffer& handle)
{
  assert(TThis::HasBuffer(handle) == true);

  auto& object = TThis::mBuffers.at(handle.GetUuid());
  return object.GetBorrow();  
}

bool MD3D11Resources::RemoveBuffer(const D11HandleBuffer& handle)
{
  // Validation check.
  if (TThis::HasBuffer(handle) == false) { return false; }

  TThis::mBuffers.erase(handle.GetUuid());
  return true;
}

//!
//! Blob
//!

std::optional<D11HandleBlob>
MD3D11Resources::CreateBlob(const D11HandleDevice& hDevice, const std::size_t byteSize)
{
  // Validation check.
  if (TThis::HasDevice(hDevice) == false) { return std::nullopt; }
  auto device = TThis::GetDevice(hDevice);
  
  // Create ID3D11Blob Resource.
  ID3DBlob* pBlob = nullptr;
  HR(D3DCreateBlob(byteSize, &pBlob));

  // If blob is null, just return nullopt.
  if (pBlob == nullptr) { return std::nullopt; }

  // Insert.
  auto [it, isSucceeded] = TThis::mBlobs.try_emplace(::dy::math::DUuid{true}, pBlob);
  assert(isSucceeded == true);
  const auto& [uuid, pOwner] = *it;

  return {uuid}; 
}

std::optional<D11HandleBlob>
MD3D11Resources::InsertRawBlob(ID3DBlob*& pRawBlob)
{
  // Validation check.
  if (pRawBlob == nullptr) { return std::nullopt; }

  // Insert.
  auto [it, isSucceeded] = TThis::mBlobs.try_emplace(::dy::math::DUuid{true}, pRawBlob);
  assert(isSucceeded == true);
  const auto& [uuid, pOwner] = *it;

  pRawBlob = nullptr;
  return {uuid}; 
}

bool MD3D11Resources::HasBlob(const D11HandleBlob& handle)
{
  return TThis::mBlobs.find(handle.GetUuid()) != TThis::mBlobs.end();
}

IComBorrow<ID3DBlob> MD3D11Resources::GetBlob(const D11HandleBlob& handle)
{
  assert(TThis::HasBlob(handle) == true);

  auto& object = TThis::mBlobs.at(handle.GetUuid());
  return object.GetBorrow();  
}

bool MD3D11Resources::RemoveBlob(const D11HandleBlob& handle)
{
  // Validation check.
  if (TThis::HasBlob(handle) == false) { return false; }

  TThis::mBlobs.erase(handle.GetUuid());
  return true;
}

//!
//! Query
//!

std::optional<D11HandleQuery>
MD3D11Resources::CreateQuery(const D11HandleDevice& hDevice, const D3D11_QUERY_DESC& desc)
{
  // Validation check.
  if (TThis::HasDevice(hDevice) == false) { return std::nullopt; }
  auto device = TThis::GetDevice(hDevice);

  // Create ID3D11Query Resource.
  ID3D11Query* pQuery = nullptr;
  HR(device->CreateQuery(&desc, &pQuery));

  // If pTexture2D is null, just return nullopt.
  if (pQuery == nullptr) { return std::nullopt; }

  // Insert.
  auto [it, isSucceeded] = TThis::mQueries.try_emplace(::dy::math::DUuid{true}, pQuery);
  assert(isSucceeded == true);
  const auto& [uuid, pOwner] = *it;

  return {uuid}; 
}

bool MD3D11Resources::HasQuery(const D11HandleQuery& handle)
{
  return TThis::mQueries.find(handle.GetUuid()) != TThis::mQueries.end();
}

IComBorrow<ID3D11Query> MD3D11Resources::GetQuery(const D11HandleQuery& handle)
{
  assert(TThis::HasQuery(handle) == true);

  auto& object = TThis::mQueries.at(handle.GetUuid());
  return object.GetBorrow();  
}

bool MD3D11Resources::RemoveQuery(const D11HandleQuery& handle)
{
  // Validation check.
  if (TThis::HasQuery(handle) == false) { return false; }

  TThis::mQueries.erase(handle.GetUuid());
  return true;
}
