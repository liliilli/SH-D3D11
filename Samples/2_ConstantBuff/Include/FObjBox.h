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
#include <D3D11.h>

#include <Math/Type/Math/DVector3.h>
#include <Object/AObject.h>
#include <Resource/DD3D11Handle.h>
#include <ComWrapper/IComBorrow.h>
#include <XCBuffer.h>

/// @class FObjBox
/// @brief Box object
class FObjBox final : public AObject
{
public:
  FObjBox() = default;
  virtual ~FObjBox() = default;

  void Initialize(void* pData) override final;
  void Release(void* pData) override final;

  void Update(float delta) override final;
  void Render() override final;

private:
  ::dy::math::DVector3<::dy::math::TReal> mPosition   = {0, 0, -1};
  ::dy::math::DVector3<::dy::math::TReal> mDegRotate  = {0, -45, 0};
  ::dy::math::DVector3<::dy::math::TReal> mScale      = {1, 1, 1};

  D11HandleBuffer hVBuffer  = nullptr;
  D11HandleBuffer hIBuffer  = nullptr;
  D11HandleBuffer hCbObject = nullptr;
  DCbObject init;

  std::optional<IComBorrow<ID3D11DeviceContext>> mDc;
  std::optional<IComBorrow<ID3D11Buffer>> mVBuffer;
  std::optional<IComBorrow<ID3D11Buffer>> mIBuffer;
  std::optional<IComBorrow<ID3D11Buffer>> mCbObject;
};

class D11DefaultHandles;
class DObjBox final
{
public:
  D11DefaultHandles*  mpData = nullptr;
  D11HandleBuffer*    mpCbObject = nullptr;
};

