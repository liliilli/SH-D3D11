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

#include <Math/Type/Micellanous/DUuid.h>
#include <Resource/ED3D11Resc.h>

/// @class DD3D11Handle
/// @tparam EValue
/// @brief
template <ED3D11Resc EValue>
class DD3D11Handle final
{
public:
  DD3D11Handle(std::nullptr_t) {};
  DD3D11Handle(const ::dy::math::DUuid& validUuid)
    : mUuid{validUuid}
  { } 

  [[nodiscard]] bool IsValid() const noexcept
  {
    static const decltype(mUuid) noneUuid = {false};
    return this->mUuid != noneUuid;
  }

  const auto& GetUuid() const noexcept
  {
    return this->mUuid;
  }

private:
  ED3D11Resc mValue = EValue;
  ::dy::math::DUuid mUuid = ::dy::math::DUuid{false};
};

/// @brief 
using D11HandleDevice = DD3D11Handle<ED3D11Resc::Device>;
/// @brief
using D11SwapChainHandle = DD3D11Handle<ED3D11Resc::SwapChain>;
/// @brief
using D11HandleRTV = DD3D11Handle<ED3D11Resc::RTV>;
/// @brief Handle type for internal ID3D11Texture2D resource.
using D11HandleTexture2D = DD3D11Handle<ED3D11Resc::Texture2D>;
/// @brief Handle type for internal ID3D11DepthStencilView resource.
using D11HandleDSV = DD3D11Handle<ED3D11Resc::DSV>;