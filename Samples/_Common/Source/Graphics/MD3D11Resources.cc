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

MD3D11Resources::THashMap<DD3DResourceDevice>         MD3D11Resources::mDevices; 
MD3D11Resources::THashMap<IComOwner<IDXGISwapChain>>  MD3D11Resources::mSwapChains;

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

bool MD3D11Resources::RemoveSwapChain(const D11SwapChainHandle& handle)
{
  // Validation check.
  if (TThis::HasSwapChain(handle) == false) { return false; }

  TThis::mSwapChains.erase(handle.GetUuid());
  return true;
}
