#pragma once
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

#include <optional>
#include <unordered_map>
#include <D3D11.h>
#include <ComWrapper/IComBorrow.h>
#include <ComWrapper/IComOwner.h>
#include <Math/Type/Micellanous/DUuid.h>
#include <Resource/DD3DResourceDevice.h>
#include <Resource/DD3D11Handle.h>

namespace dy
{
struct DWindowHandle;
class APlatformBase;
} /// ::dy namespace

/// @class MD3D11Resources
/// @brief Manager class of D3D11 Resources.
class MD3D11Resources final
{
public:
  /// @brief Create default device that supports D3D11.
  /// @param platform Platform base type instance.
  /// @return If successful, return device handle instance.
  [[nodiscard]] static std::optional<D11HandleDevice> 
  CreateD3D11DefaultDevice(dy::APlatformBase& platform);

  /// @brief Check device resource is valid and in container.
  /// @param handle Device handle.
  /// @return If find, return true. Otherwise, return false.
  [[nodiscard]] static bool HasDevice(const D11HandleDevice& handle) noexcept;

  /// @brief Get borrow type of Device resource safely. 
  /// This function does not check whether handle is valid or not and device resource is exist or not.
  /// That can be checkable for using D11HandleDevice::IsValid() and MD3D11Resources::HasDevice().
  /// @param handle Valid device handle.
  /// @return Return borrow type of actual D3D11 device resource.
  static IComBorrow<ID3D11Device> GetDevice(const D11HandleDevice& handle);

  /// @brief Get borrow type of Device context resource safely.
  /// This function does not check whether handle is valid or not and device resource is exist or not.
  /// That can be checkable for using D11HandleDevice::IsValid() and MD3D11Resources::HasDevice().
  /// @param handle Valid device handle.
  /// @return Return borrow type of actual D3D11 device context resource.
  static IComBorrow<ID3D11DeviceContext> GetDeviceContext(const D11HandleDevice& handle);

  /// @brief Remove device resource with handle.
  /// @param handle Valid device handle.
  /// @return If find, return true. If not find, return false.
  static bool RemoveDevice(const D11HandleDevice& handle);

  /// @brief Create swap-chain of given valid device.
  /// @param device Valid device handle.
  /// @param mainHwnd WIN32 main window handle.
  /// @param desc Descriptor of D3D11 swap-chain.
  /// @return If successful, return handle of swap chain resource.
  [[nodiscard]] static std::optional<D11SwapChainHandle>
  CreateSwapChain(const D11HandleDevice& device, const HWND mainHwnd, DXGI_SWAP_CHAIN_DESC& desc);

  /// @brief Check swap-chain resource is valid and in container.
  /// @param handle Valid swap-chain handle.
  /// @return If find, return true. Otherwise, return false.
  [[nodiscard]] static bool HasSwapChain(const D11SwapChainHandle& handle) noexcept;

  /// @brief 
  /// @param
  /// @return
  static IComBorrow<IDXGISwapChain> GetSwapChain(const D11SwapChainHandle& handle);

  /// @brief Remove swap-chain resource with handle.
  /// @param handle Valid swap-chain handle.
  /// @return If find, return true. If not find, return false.
  static bool RemoveSwapChain(const D11SwapChainHandle& handle);

  /// @brief

private:
  template <typename TValue>
  using THashMap = std::unordered_map<::dy::math::DUuid, TValue>;

  using TThis = MD3D11Resources;

  /// @brief 
  static THashMap<DD3DResourceDevice> mDevices; 
  /// @brief
  static THashMap<IComOwner<IDXGISwapChain>> mSwapChains;
};