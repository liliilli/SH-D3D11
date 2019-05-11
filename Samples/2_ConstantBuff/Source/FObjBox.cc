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

#include <FObjBox.h>
#include <Graphics/MD3D11Resources.h>
#include <Resource/D11DefaultHandles.h>
#include <Math/Utility/XGraphicsMath.h>
#include <MGuiManager.h>
#include <FGuiWindow.h>
#include <XBuffer.h>

namespace 
{

inline std::array<DVertex, 3> vertices =
{ 
  DVertex
  { {-1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f, 1.0f} },
  { {+1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 1.0f, 1.0f} },
  { {+0.0f, +1.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f} },
};

// Create Index Buffer.
inline std::array<unsigned, 3> indices =
{ // 1        2        3        4
  0, 1, 2, 
};

}

void FObjBox::Initialize(void* pData)
{
  const auto& param     = *static_cast<DObjBox*>(pData); 
  assert(param.mpData != nullptr);
  assert(param.mpCbObject != nullptr);

  const auto& defaults  = *param.mpData;
  this->hCbObject       = *param.mpCbObject;
  this->mDc.emplace(MD3D11Resources::GetDeviceContext(defaults.mDevice));
  this->mCbObject.emplace(MD3D11Resources::GetBuffer(this->hCbObject));

  {
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = 0;
    vbDesc.MiscFlags = 0;
    vbDesc.StructureByteStride = 0;

    this->hVBuffer = *MD3D11Resources::CreateBuffer(defaults.mDevice, vbDesc, vertices.data());
    assert(MD3D11Resources::HasBuffer(hVBuffer) == true);
    this->mVBuffer.emplace(MD3D11Resources::GetBuffer(this->hVBuffer));
  }

  {
    D3D11_BUFFER_DESC ibDesc;
    ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
    ibDesc.ByteWidth = sizeof(indices);
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;
    ibDesc.MiscFlags = 0;
    ibDesc.StructureByteStride = 0;

    this->hIBuffer = *MD3D11Resources::CreateBuffer(defaults.mDevice, ibDesc, indices.data());
    assert(MD3D11Resources::HasBuffer(hIBuffer) == true);
    this->mIBuffer.emplace(MD3D11Resources::GetBuffer(this->hIBuffer));
  }
}

void FObjBox::Release(void* pData)
{
  this->mVBuffer = std::nullopt;
  this->mIBuffer = std::nullopt;

  {
    const auto flag = MD3D11Resources::RemoveBuffer(this->hIBuffer);
    assert(flag == true);
  }
  {
    const auto flag = MD3D11Resources::RemoveBuffer(this->hVBuffer);
    assert(flag == true);
  }
}

void FObjBox::Update(float delta)
{
  if (MGuiManager::HasSharedModel("Window") == true)
  {
    auto& model = static_cast<DModelWindow&>(MGuiManager::GetSharedModel("Window"));
    this->mScale = model.mScale;
  }
}

void FObjBox::Render()
{
  assert(this->mCbObject.has_value() == true);
  assert(this->mDc.has_value() == true);

  // Update object matrix
  {
    using namespace ::dy::math;
    init.mModel = CreateModelMatrix<TReal>(
      EGraphics::DirectX, 
      this->mPosition, this->mDegRotate, this->mScale, 
      true);
  }
  (*this->mDc)->UpdateSubresource((*this->mCbObject).GetPtr(), 0, nullptr, &this->init, 0, 0);

  // Set Vertex, Index and draw.
  auto* pVBuffer = (*mVBuffer).GetPtr();
  UINT stride    = sizeof(DVertex); UINT offset = 0;

  (*this->mDc)->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
  (*this->mDc)->IASetIndexBuffer((*mIBuffer).GetPtr(), DXGI_FORMAT_R32_UINT, 0);
  (*this->mDc)->DrawIndexed(3, 0, 0);
}
