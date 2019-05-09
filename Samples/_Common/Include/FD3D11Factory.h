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
#include <string>
#include <D3D11.h>

#include <ComWrapper/IComOwner.h>

namespace dy
{
struct DWindowHandle;
class APlatformBase;
} /// ::dy namespace

/// @class FD3D11Factory
/// @brief Factory class for creating micellaneous thing of D3D11.
class FD3D11Factory final
{
public:
  /// @brief Try to create D3D11 Device.
  static std::optional<std::pair<IComOwner<ID3D11Device>, IComOwner<ID3D11DeviceContext>>>
  CreateD3D11Device(dy::APlatformBase& platform);

  /// @brief Try compile shader from file.
  static HRESULT CompileShaderFromFile(
    dy::APlatformBase& platform,
    const std::string& szFilePath, 
    LPCSTR szEntryPoint, 
    LPCSTR szShaderModel, 
    ID3DBlob** ppBlobOut);

  /// @brief Get default swap-chain descriptor with pOutputWindowHandle.
  /// Format will be R8G8B8A8_UNORM.
  static DXGI_SWAP_CHAIN_DESC GetDefaultSwapChainDesc(
    unsigned width,
    unsigned height,
    HWND pOutputWindowHandle);

  /// @brief Get default depth stencil texture2D descriptor with (width, height).
  /// FORMAT will be D24_UNORM_S8_UINT.
  static D3D11_TEXTURE2D_DESC GetDefaultDepthStencilDesc(unsigned width, unsigned height);

  /// @brief Get default rasterizer state descriptor.
  static D3D11_RASTERIZER_DESC GetDefaultRasterStateDesc();

  /// @brief Get default depth-stencil state descriptor.
  static D3D11_DEPTH_STENCIL_DESC GetDefaultDepthStencilStateDesc();
  
  /// @brief Try create D3D11 default swap-chain.
  static HRESULT CreateD3D11SwapChain(
    ID3D11Device& device, 
    HWND pWindowHandle,
    DXGI_SWAP_CHAIN_DESC& descriptor,
    IComOwner<IDXGISwapChain>& ownSwapChainRef);

  /// @brief Try to create timestamp query.
  /// If failed, just return nullopt.
  static std::optional<IComOwner<ID3D11Query>> CreateTimestampQuery(
    ID3D11Device& device,
    bool isDisjoint);

  /// @brief Try to create a pair of timestamp fragment (start, end).
  /// If failed, just return nullopt.
  static std::optional<std::pair<IComOwner<ID3D11Query>, IComOwner<ID3D11Query>>>
  CreateTimestampQueryPair(ID3D11Device& device);
};

