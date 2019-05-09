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

MD3D11Resources::THashMap<DD3DResourceDevice> MD3D11Resources::mDevices; 

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
  HRESULT hr = D3D11CreateDevice(
    nullptr,              // Default Adapter (Primary)
    D3D_DRIVER_TYPE_HARDWARE, // Use hardware driver (Most optimal) 
    nullptr,              // No Software device because we use TYPE_HARDWARE (D3D11).
    createDeviceFlags,    // Set flags (DEBUG, SINGLETHREAD etc...)
    nullptr,              // If no flag is exist, just pick up the highest version of SDK.
    0,                    // Above argument brings the array of D3D_FEATURE, so we set it to 0 as nullptr. 
    D3D11_SDK_VERSION,    // Always specify this.
    mD3DDevice.GetAddressOf(),          // Output
    &featureLevel,                      // Output
    mD3DImmediateContext.GetAddressOf() // Output
  );
  
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

  auto [it, isSucceeded] = mDevices.try_emplace(
    ::dy::math::DUuid{true}, 
    std::move(mD3DDevice), std::move(mD3DImmediateContext));
  assert(isSucceeded == true);
  const auto& [uuid, pOwner] = *it;

  return D11HandleDevice{uuid};
}

bool MD3D11Resources::HasDevice(const D11HandleDevice& handle)
{
  return TThis::mDevices.find(handle.GetUuid()) != TThis::mDevices.end();
}

bool MD3D11Resources::RemoveDevice(const D11HandleDevice& handle)
{
  if (TThis::HasDevice(handle) == false) { return false; }

  TThis::mDevices.erase(handle.GetUuid());
  return true;
}
