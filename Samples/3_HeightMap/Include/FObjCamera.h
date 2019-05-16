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
#include <Resource/DD3D11Handle.h>
#include <ComWrapper/IComBorrow.h>
#include <Object/AObject.h>
#include <XCBuffer.h>

/// @class FObjCamera
/// @brief Camera object
class FObjCamera final : public AObject
{
public:
  FObjCamera() = default;
  virtual ~FObjCamera() = default;

  void Initialize(void* pData) override final;
  void Release(void* pData) override final;

  void Update(float delta) override final;
  void Render() override final;

private:
  DVector3<TReal> mPosition = {0, 0, 10};
  DVector3<TReal> mUp       = {0, 1, 0};
  DVector3<TReal> mLookAt   = {0, 0, 0};

  DCbViewProj mCbViewProj;
  D11HandleBuffer hCbViewProj = nullptr;

  std::optional<IComBorrow<ID3D11DeviceContext>> mDc;
  std::optional<IComBorrow<ID3D11Buffer>> mbViewProj;
};

class D11DefaultHandles;
class DObjCamera final
{
public:
  D11DefaultHandles* mpData       = nullptr;
  D11HandleBuffer*   mpCbViewProj = nullptr;
};

