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
    static const ::dy::math::DUuid noneUuid = ::dy::math::DUuid{};
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
/// @brief Handle type for internal ID3D11DepthStencilView resource.
using D11HandleDSV = DD3D11Handle<ED3D11Resc::DSV>;
/// @brief Handle type for internal ID3D11RasterizerState resource.
using D11HandleRasterState = DD3D11Handle<ED3D11Resc::RasterizerState>;
/// @brief Handle type for internal ID3D11DepthStencilState resource.
using D11HandleDepthStencilState = DD3D11Handle<ED3D11Resc::DepthStencilState>;
/// @brief Handle type for internal ID3D11BlendState resource.
using D11HandleBlendState = DD3D11Handle<ED3D11Resc::BlendState>;
/// @brief Handle type for internal ID3D11Buffer resource.
using D11HandleBuffer = DD3D11Handle<ED3D11Resc::Buffer>;
/// @brief Handle type for internal ID3D11VertexShader resource.
using D11HandleVS = DD3D11Handle<ED3D11Resc::VertexShader>;
/// @brief Handle type for internal ID3D11PixelShader resource.
using D11HandlePS = DD3D11Handle<ED3D11Resc::PixelShader>;
/// @brief Handle type for internal ID3D11InputLayout resource.
using D11HandleInputLayout = DD3D11Handle<ED3D11Resc::InputLayout>;
/// @brief Handle type for internal ID3D11Texture2D resource.
using D11HandleTexture2D = DD3D11Handle<ED3D11Resc::Texture2D>;
/// @brief Handle type for internal ID3DBlob (ID3D10Blob) resource.
using D11HandleBlob = DD3D11Handle<ED3D11Resc::Blob>;
/// @brief Handle type for internal ID3D11Query resource.
using D11HandleQuery = DD3D11Handle<ED3D11Resc::Query>;