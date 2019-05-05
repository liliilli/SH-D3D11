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

#include <IComOwner.h>

namespace dy
{
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

  /// @brief Try to create timestamp query.
  /// If failed, just return nullopt.
  static std::optional<IComOwner<ID3D11Query>> CreateTimestampQuery(
    ID3D11Device& device,
    bool isDisjoint);
};

