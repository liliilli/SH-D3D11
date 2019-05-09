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

  //!
  //! Swap-Chain
  //!

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

  /// @brief Get borrow type of Swap-chain context resource safely.
  /// This function does not check whether handle is valid or not and swap-chain resource is exist or not.
  /// That can be checkable for using D11HandleSwapChain::IsValid() and MD3D11Resources::HasSwapChain().
  /// @param handle Valid swap-chain handle.
  /// @return Return borrow type of actual D3D11 swap-chain resource.
  static IComBorrow<IDXGISwapChain> GetSwapChain(const D11SwapChainHandle& handle);

  /// @brief Remove swap-chain resource with handle.
  /// @param handle Valid swap-chain handle.
  /// @return If find, return true. If not find, return false.
  static bool RemoveSwapChain(const D11SwapChainHandle& handle);

  //!
  //! Render-Target View
  //!

  /// @brief Create render-target-view of given valid device and swap-chain.
  /// @param hDevice Valid device handle.
  /// @param hSwapChain Valid swap-chain handle.
  /// @param pDesc [Optional] Additional render-target-view descriptor.
  /// @return If successful, retrun handle of Render-target-view resource.
  [[nodiscard]] static std::optional<D11HandleRTV>
  CreateRTVFromSwapChain(
    const D11HandleDevice& hDevice, const D11SwapChainHandle& hSwapChain, 
    D3D11_RENDER_TARGET_VIEW_DESC* pDesc = nullptr);

  /// @brief Check RTV resource is valid and in container.
  /// @param handle Valid RTV handle.
  /// @return If find, return true. Otherwise, return false.
  [[nodiscard]] static bool HasRTV(const D11HandleRTV& handle) noexcept;

  /// @brief Get borrow type of RTV resource safely.
  /// This function does not check whether handle is valid or not and RTV resource is exist or not.
  /// That can be checkable for using D11HandleRTV::IsValid() and MD3D11Resources::HasRTV().
  /// @param handle Valid RTV handle.
  /// @return Return borrow type of actual D3D11 RTV resource.
  static IComBorrow<ID3D11RenderTargetView> GetRTV(const D11HandleRTV& handle);

  /// @brief Remove RTV resource with handle.
  /// @param handle Valid RTV handle.
  /// @return If find, return true. If not find, return false.
  static bool RemoveRTV(const D11HandleRTV& handle);

  //!
  //! Depth-Stencil View
  //!

  /// @brief Create Depth-Stencil-View from given valid device and texture2D resource.
  /// @param hDevice Valid device handle.
  /// @param hTexture2D Valid Texture2D handle.
  /// @param pDesc Optional DSV descriptor.
  /// @return If successful, return handle of DSV resource.
  [[nodiscard]] static std::optional<D11HandleDSV>
  CreateDSV(
    const D11HandleDevice& hDevice, const D11HandleTexture2D& hTexture2D,
    D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc = nullptr);

  /// @brief Check DSV resource is valid and in container.
  /// @param handle Valid DSV handle.
  /// @return If find, return true. Otherwise, return false.
  [[nodiscard]] static bool HasDSV(const D11HandleDSV& handle) noexcept;

  /// @brief Get borrow type of DSV resource safely.
  /// This function does not check whether handle is valid or not and DSV resource is exist or not.
  /// That can be checkable for using D11HandleDSV::IsValid() and MD3D11Resources::HasDSV().
  /// @param handle Valid DSV handle.
  /// @return Return borrow type of actual D3D11 DSV resource.
  static IComBorrow<ID3D11RenderTargetView> GetDSV(const D11HandleDSV& handle);

  /// @brief Remove DSV resource with handle.
  /// @param handle Valid DSV handle.
  /// @return If find, return true. If not find, return false.
  static bool RemoveDSV(const D11HandleDSV& handle);

  //!
  //! Rasterizer-State
  //!

  /// @brief Create Rasterizer-State from given valid device and desciptor.
  /// @param hDevice Valid device handle.
  /// @param desc Descriptor for Rasterizer state.
  /// @return If successful, return handle of Raster-State resource.
  [[nodiscard]] static std::optional<D11HandleRasterState>
  CreateRasterState(const D11HandleDevice& hDevice, const D3D11_RASTERIZER_DESC& desc); 

  /// @brief Check RasterState resource is valid and in container.
  /// @param handle Valid RasterState handle.
  /// @return If find, return true. Otherwise, return false.
  [[nodiscard]] static bool HasRasterState(const D11HandleRasterState& handle) noexcept;

  /// @brief Get borrow type of RasterState resource safely.
  /// This function does not check whether handle is valid or not and RasterState resource is exist or not.
  /// That can be checkable for using D11HandleRasterState::IsValid() and MD3D11Resources::HasRasterState().
  /// @param handle Valid RasterState handle.
  /// @return Return borrow type of actual D3D11 RasterState resource.
  static IComBorrow<ID3D11RasterizerState> GetRasterState(const D11HandleRasterState& handle);

  /// @brief Remove RasterState resource with handle.
  /// @param handle Valid RasterState handle.
  /// @return If find, return true. If not find, return false.
  static bool RemoveRasterState(const D11HandleRasterState& handle);

  //!
  //! Depth-Stencil State
  //!

  /// @brief Create Depth-Stencil State from given valid device and desciptor.
  /// @param hDevice Valid device handle.
  /// @param desc Descriptor for Depth-Stencil state.
  /// @return If successful, return handle of Depth-Stencil State resource.
  [[nodiscard]] static std::optional<D11HandleDepthStencilState>
  CreateDepthStencilState(const D11HandleDevice& hDevice, const D3D11_DEPTH_STENCIL_DESC& desc); 

  /// @brief Check Depth-Stencil State resource is valid and in container.
  /// @param handle Valid Depth-Stencil State handle.
  /// @return If find, return true. Otherwise, return false.
  [[nodiscard]] static bool HasDepthStencilState(const D11HandleDepthStencilState& handle) noexcept;

  /// @brief Get borrow type of Depth-Stencil State resource safely. \n
  /// This function does not check whether handle is valid 
  /// or not and Depth-Stencil State resource is exist or not. \n
  /// That can be checkable for using D11HandleDepthStencilState::IsValid() 
  /// and MD3D11Resources::HasDepthStencilState(). 
  /// @param handle Valid Depth-Stencil State handle.
  /// @return Return borrow type of actual D3D11 Depth-Stencil State resource.
  static IComBorrow<ID3D11RasterizerState> GetDepthStencilState(const D11HandleDepthStencilState& handle);

  /// @brief Remove Depth-Stencil State resource with handle.
  /// @param handle Valid Depth-Stencil State handle.
  /// @return If find, return true. If not find, return false.
  static bool RemoveDepthStencilState(const D11HandleDepthStencilState& handle);
  
  //!
  //! Texture2D
  //!

  /// @brief Create Texture2D of given valid device and optional buffer pointer.
  /// @param hDevice Valid device handle.
  /// @param desc Descriptor of D3D11 Texture2D.
  /// @param pInitBuffer Optional initial buffer ptr. This must be valid when Texture2D would be IMMUTABLE.
  /// @return If successful, retrun handle of Texture2D resource.
  [[nodiscard]] static std::optional<D11HandleTexture2D>
  CreateTexture2D(
    const D11HandleDevice& hDevice, const D3D11_TEXTURE2D_DESC& desc, 
    const void* pInitBuffer = nullptr);

  /// @brief Check Texture2D resource is valid and in container.
  /// @param handle Valid Texture2D handle.
  /// @return If find, return true. Otherwise, return false.
  [[nodiscard]] static bool HasTexture2D(const D11HandleTexture2D& handle);

  /// @brief Get borrow type of Texture2D resource safely.
  /// This function does not check whether handle is valid or not and Texture2D resource is exist or not.
  /// That can be checkable for using D11HandleTexture2D::IsValid() and MD3D11Resources::HasTexture2D().
  /// @param handle Valid Texture2D handle.
  /// @return Return borrow type of actual D3D11 Texture2D resource.
  static IComBorrow<ID3D11Texture2D> GetTexture2D(const D11HandleTexture2D& handle);

  /// @brief Remove Texture2D resource with handle.
  /// @param handle Valid Texture2D handle.
  /// @return If find, return true. If not find, return false.
  static bool RemoveTexture2D(const D11HandleTexture2D& handle);

private:
  template <typename TValue>
  using THashMap = std::unordered_map<::dy::math::DUuid, TValue>;

  using TThis = MD3D11Resources;

  /// @brief 
  static THashMap<DD3DResourceDevice> mDevices; 
  /// @brief
  static THashMap<IComOwner<IDXGISwapChain>> mSwapChains;
  /// @brief Render-Target-View Resource Container.
  static THashMap<IComOwner<ID3D11RenderTargetView>> mRTVs;
  /// @brief Depth-Stencil-View Resource container.
  static THashMap<IComOwner<ID3D11DepthStencilView>> mDSVs;
  /// @brief
  static THashMap<IComOwner<ID3D11Texture2D>> mTexture2Ds;
  /// @brief Rasterizer-State Resource Container.
  static THashMap<IComOwner<ID3D11RasterizerState>> mRasterStates;
  /// @brief Depth-Stencil-State Resource Container.
  static THashMap<IComOwner<ID3D11DepthStencilState>> mDepthStencilStates;
};