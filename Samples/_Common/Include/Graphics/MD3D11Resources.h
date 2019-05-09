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
  /// @brief
  /// @param
  /// @return
  [[nodiscard]] static std::optional<D11HandleDevice> 
  CreateD3D11DefaultDevice(dy::APlatformBase& platform);

  /// @brief
  /// @param
  /// @return
  [[nodiscard]] static bool HasDevice(const D11HandleDevice& handle);

  /// @brief
  /// @param
  /// @return
  static IComBorrow<ID3D11Device> GetDevice(const D11HandleDevice& handle);

  /// @brief
  /// @param
  /// @return
  static bool RemoveDevice(const D11HandleDevice& handle);

private:
  template <typename TValue>
  using THashMap = std::unordered_map<::dy::math::DUuid, TValue>;

  using TThis = MD3D11Resources;

  /// @brief 
  static THashMap<DD3DResourceDevice> mDevices; 
};