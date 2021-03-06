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

#include <FObjTerrain.h>
#include <Graphics/MD3D11Resources.h>
#include <Resource/D11DefaultHandles.h>
#include <Math/Utility/XGraphicsMath.h>
#include <MGuiManager.h>
#include <MRandomMap.h>
#include <FGuiWindow.h>

void FObjTerrain::Initialize(void* pData)
{
  const auto& param     = *static_cast<DObjTerrain*>(pData); 
  assert(param.mpData != nullptr);
  assert(param.mpCbObject != nullptr);

  const auto& defaults  = *param.mpData;
  this->hCbObject       = *param.mpCbObject;
  this->mDc.emplace(MD3D11Resources::GetDeviceContext(defaults.mDevice));
  this->mCbObject.emplace(MD3D11Resources::GetBuffer(this->hCbObject));
  this->hDevice = defaults.mDevice;

  if (MGuiManager::HasSharedModel("Window") == true)
  {
    const auto& model = static_cast<DModelWindow&>(MGuiManager::GetSharedModel("Window"));
    bool isChanged = false;

    if (this->mTerrainGrid != model.mTerrainGrid)
    {
      this->mTerrainGrid = model.mTerrainGrid;
      isChanged = true;
    }

    if (this->mTerrainFragment != model.mTerrainFragment)
    {
      this->mTerrainFragment = model.mTerrainFragment;
      isChanged = true;
    }

    if (isChanged == true)
    {
      MRandomMap::MakeMap(this->mTerrainGrid, this->mTerrainFragment[0]);
      {
        const auto& buffer = MRandomMap::TempGetVertexBuffer();

        D3D11_BUFFER_DESC vbDesc = {};
        vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
        vbDesc.ByteWidth = UINT(sizeof(DVector3<TReal>) * buffer.GetRowSize() * buffer.GetColumnSize());
        vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbDesc.CPUAccessFlags = 0;
        vbDesc.MiscFlags = 0;
        vbDesc.StructureByteStride = 0;

        if (this->hVBuffer.IsValid() == true)
        {
          MD3D11Resources::RemoveBuffer(this->hVBuffer);
        }
        this->hVBuffer = *MD3D11Resources::CreateBuffer(defaults.mDevice, vbDesc, buffer.Data());
        assert(MD3D11Resources::HasBuffer(hVBuffer) == true);
        this->mVBuffer.emplace(MD3D11Resources::GetBuffer(this->hVBuffer));
      }

      {
        const auto& buffer = MRandomMap::TempGetIndiceBuffer();

        D3D11_BUFFER_DESC ibDesc;
        ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
        ibDesc.ByteWidth = UINT(buffer.size() * sizeof(TU32));
        ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibDesc.CPUAccessFlags = 0;
        ibDesc.MiscFlags = 0;
        ibDesc.StructureByteStride = 0;

        if (this->hIBuffer.IsValid() == true)
        {
          MD3D11Resources::RemoveBuffer(this->hIBuffer);
        }
        this->hIBuffer = *MD3D11Resources::CreateBuffer(defaults.mDevice, ibDesc, buffer.data());
        assert(MD3D11Resources::HasBuffer(hIBuffer) == true);
        this->mIBuffer.emplace(MD3D11Resources::GetBuffer(this->hIBuffer));
      }
    }
  }
}

void FObjTerrain::Release(void* pData)
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

void FObjTerrain::Update(float delta)
{
  if (MGuiManager::HasSharedModel("Window") == true)
  {
    auto& model = static_cast<DModelWindow&>(MGuiManager::GetSharedModel("Window"));
    bool isChanged = false;

    if (this->mTerrainGrid != model.mTerrainGrid)
    {
      this->mTerrainGrid = model.mTerrainGrid;
      isChanged = true;
    }

    if (this->mTerrainFragment != model.mTerrainFragment)
    {
      this->mTerrainFragment = model.mTerrainFragment;
      isChanged = true;
    }

    if (isChanged == true)
    {
      MRandomMap::MakeMap(this->mTerrainGrid, this->mTerrainFragment[0]);
      {
        const auto& buffer = MRandomMap::TempGetVertexBuffer();

        D3D11_BUFFER_DESC vbDesc = {};
        vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
        vbDesc.ByteWidth = UINT(sizeof(DVector3<TReal>) * buffer.GetRowSize() * buffer.GetColumnSize());
        vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbDesc.CPUAccessFlags = 0;
        vbDesc.MiscFlags = 0;
        vbDesc.StructureByteStride = 0;

        if (this->hVBuffer.IsValid() == true)
        {
          this->mVBuffer = std::nullopt;
          MD3D11Resources::RemoveBuffer(this->hVBuffer);
        }
        this->hVBuffer = *MD3D11Resources::CreateBuffer(this->hDevice, vbDesc, buffer.Data());
        assert(MD3D11Resources::HasBuffer(hVBuffer) == true);
        this->mVBuffer.emplace(MD3D11Resources::GetBuffer(this->hVBuffer));
      }

      {
        const auto& buffer = MRandomMap::TempGetIndiceBuffer();

        D3D11_BUFFER_DESC ibDesc;
        ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
        ibDesc.ByteWidth = UINT(buffer.size() * sizeof(TU32));
        ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibDesc.CPUAccessFlags = 0;
        ibDesc.MiscFlags = 0;
        ibDesc.StructureByteStride = 0;

        if (this->hIBuffer.IsValid() == true)
        {
          this->mIBuffer = std::nullopt;
          MD3D11Resources::RemoveBuffer(this->hIBuffer);
        }
        this->hIBuffer = *MD3D11Resources::CreateBuffer(this->hDevice, ibDesc, buffer.data());
        assert(MD3D11Resources::HasBuffer(hIBuffer) == true);
        this->mIBuffer.emplace(MD3D11Resources::GetBuffer(this->hIBuffer));
      }
    }

    this->mPosition.X = -model.mTerrainGrid[0] * 0.5f;
    this->mPosition.Z = -model.mTerrainGrid[1] * 0.5f;
  }

  mDegRotate.Y += delta * 30;
}

void FObjTerrain::Render()
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
  UINT stride    = sizeof(DVector3<TReal>); UINT offset = 0;

  (*this->mDc)->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
  (*this->mDc)->IASetIndexBuffer((*mIBuffer).GetPtr(), DXGI_FORMAT_R32_UINT, 0);
  (*this->mDc)->DrawIndexed(UINT(MRandomMap::TempGetIndiceBuffer().size()), 0, 0);
}
