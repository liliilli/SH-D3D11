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

using namespace ::dy::math;

/// @class FObjTerrain
/// @brief Terrain object
class FObjTerrain final : public AObject
{
public:
  FObjTerrain() = default;
  virtual ~FObjTerrain() = default;

  void Initialize(void* pData) override final;
  void Release(void* pData) override final;

  void Update(float delta) override final;
  void Render() override final;

private:
  DVector3<TReal> mPosition   = {-4, -2, -4};
  DVector3<TReal> mDegRotate  = {90, 0, 0};
  DVector3<TReal> mScale      = {1, 1, 1};

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
class DObjTerrain final
{
public:
  D11DefaultHandles*  mpData = nullptr;
  D11HandleBuffer*    mpCbObject = nullptr;
};

