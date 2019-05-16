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

#include <FObjCamera.h>
#include <Graphics/MD3D11Resources.h>
#include <Resource/D11DefaultHandles.h>
#include <Math/Utility/XGraphicsMath.h>
#include <MGuiManager.h>
#include <FGuiWindow.h>

void FObjCamera::Initialize(void* pData)
{
  const auto& param = *static_cast<DObjCamera*>(pData); 
  assert(param.mpData != nullptr);
  assert(param.mpCbViewProj != nullptr);

  const auto& defaults = *param.mpData;
  this->hCbViewProj    = *param.mpCbViewProj;
  this->mDc.emplace(MD3D11Resources::GetDeviceContext(defaults.mDevice));
  this->mbViewProj.emplace(MD3D11Resources::GetBuffer(this->hCbViewProj));
}

void FObjCamera::Release(void* pData)
{

}

void FObjCamera::Update(float delta)
{
  auto& model = static_cast<DModelWindow&>(MGuiManager::GetSharedModel("Window"));
  const DVector4<TReal> initPos = {0, 0, model.mDistance, 1};
  const DQuaternion<TReal> initQuat = {{-model.mCamera, 0, 0}, true}; 
  const auto rotatedPos = initQuat.ToMatrix4() * initPos;
  this->mPosition = { rotatedPos.X, rotatedPos.Y, rotatedPos.Z };

  // Update view & projection matrix.
  this->mCbViewProj.mView = 
    LookAt2<TReal>(EGraphics::DirectX, this->mPosition, this->mLookAt, this->mUp);
  this->mCbViewProj.mProj = 
    ProjectionMatrix<TReal>(
      EGraphics::OpenGL, EProjection::Perspective, 
      120, 1280, 720, 0.f, 500.f).Transpose();

  assert(this->mbViewProj.has_value() == true);
  assert(this->mDc.has_value() == true);

  // Update constant buffer.
  (*this->mDc)->UpdateSubresource((*this->mbViewProj).GetPtr(), 0, nullptr, &this->mCbViewProj, 0, 0);
}

void FObjCamera::Render()
{
#if 0
  auto* pVBuffer = (*mVBuffer).GetPtr();
  UINT stride    = sizeof(DVertex); UINT offset = 0;

  (*this->mDc)->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
  (*this->mDc)->IASetIndexBuffer((*mIBuffer).GetPtr(), DXGI_FORMAT_R32_UINT, 0);
  (*this->mDc)->DrawIndexed(3, 0, 0);
#endif
}
