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

#include <cassert>
#include <cstdio>
#include <array>
#include <iostream>
#include <optional>
#include <vector>
#include <filesystem>

#include <D3Dcompiler.h>
#include <D3D11.h>
#include <HelperMacro.h>
#include <XCommonDebug.h>
#include <ComWrapper/IComOwner.h>
#include <ComWrapper/IComBorrow.h>
#include <FD3D11Factory.h>
#include <Profiling/MTimeChecker.h>
#include <MGuiManager.h>
#include <XTriangle.h>
#include <XCBuffer.h>

#include <StringUtil/XUtility.h>
#include <FGuiWindow.h>

#include <FWindowsPlatform.h>
#include <PLowInputMousePos.h>
#include <XPlatform.h>
#include <XLocalCommon.h>
#include <Graphics/MD3D11Resources.h>

int WINAPI WinMain(
  [[maybe_unused]] HINSTANCE hInstance, 
  [[maybe_unused]] HINSTANCE hPrevInstance, 
  [[maybe_unused]] LPSTR lpCmdLine, 
  [[maybe_unused]] int nCmdShow)
{
  WIN32_TRY_TURN_ON_DEBUG();

  // Create base system.
  platform = std::make_unique<dy::FWindowsPlatform>();
  platform->InitPlatform();
  platform->CreateConsoleWindow();
  auto optRes = CreateMainWindow("D3D11 2_ConstantBuff", 1280, 720);
  assert(optRes.has_value() == true);

  // Get width and height of main window client region.
  const auto width  = platform->GetWindowWidth(*optRes);
  const auto height = platform->GetWindowHeight(*optRes);
  HWND mainWindowHandle = static_cast<HWND>(platform->_GetHandleOf(*optRes));

  //!
  //! D3D11 Setting up
  //!

  const auto optDefaults = FD3D11Factory::CreateDefaultFrameBuffer(*platform, *optRes);
  assert(optDefaults.has_value() == true);
  const auto& defaults = *optDefaults;

  // Create vertex & indice buffers and constant buffers
  D11HandleBuffer handleVertexBuffer = nullptr;
  {
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = 0;
    vbDesc.MiscFlags = 0;
    vbDesc.StructureByteStride = 0;

    handleVertexBuffer = *MD3D11Resources::CreateBuffer(defaults.mDevice, vbDesc, vertices.data());
    assert(MD3D11Resources::HasBuffer(handleVertexBuffer) == true);
  }

  D11HandleBuffer handleIndiceBuffer = nullptr;
  {
    D3D11_BUFFER_DESC ibDesc;
    ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
    ibDesc.ByteWidth = sizeof(indices);
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;
    ibDesc.MiscFlags = 0;
    ibDesc.StructureByteStride = 0;

    handleIndiceBuffer = *MD3D11Resources::CreateBuffer(defaults.mDevice, ibDesc, indices.data());
    assert(MD3D11Resources::HasBuffer(handleIndiceBuffer) == true);
  }

  D11HandleBuffer handleConstantBuffer = nullptr;
  {
    D3D11_BUFFER_DESC desc;
    desc.Usage      = D3D11_USAGE_DEFAULT;
    desc.ByteWidth  = sizeof(DCbScale);
    desc.BindFlags  = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    DCbScale initScale; initScale.mScale = 0.5f;

    handleConstantBuffer = *MD3D11Resources::CreateBuffer(defaults.mDevice, desc, &initScale);
    assert(MD3D11Resources::HasBuffer(handleConstantBuffer) == true);
  }

  //!
  //! Create shaders and input layer.
  //!

  D11HandleVS handleVS = nullptr;
  D11HandleInputLayout handleIL = nullptr;
  {
    const auto optVsBlob = FD3D11Factory::CompileShaderFromFile2(
      *platform, "../../Resource/Shader.fx", "VS", "vs_5_0");
    assert(optVsBlob.has_value() == true);

    const auto optVs = MD3D11Resources::CreateVertexShader(defaults.mDevice, *optVsBlob);
    assert(optVs.has_value() == true);

    // Create Vertex shader input layout.
    // https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ns-d3d11-d3d11_input_element_desc
    std::array<D3D11_INPUT_ELEMENT_DESC, 2> vertexDesc =
    {
      decltype(vertexDesc)::value_type
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA , 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    const auto optIL = MD3D11Resources::CreateInputLayout(
      defaults.mDevice, *optVsBlob, vertexDesc.data(), vertexDesc.size());
    assert(optIL.has_value() == true);

    handleVS = *optVs;
    handleIL = *optIL;
    MD3D11Resources::RemoveBlob(*optVsBlob);
  }

  D11HandlePS handlePS = nullptr;
  {
    const auto optPSBlob = FD3D11Factory::CompileShaderFromFile2(
      *platform, "../../Resource/Shader.fx", "PS", "ps_5_0");
    assert(optPSBlob.has_value() == true);

    const auto optPS = MD3D11Resources::CreatePixelShader(defaults.mDevice, *optPSBlob);
    assert(optPS.has_value() == true);
    handlePS = *optPS;

    MD3D11Resources::RemoveBlob(*optPSBlob);
  }

  //!
  //! Create Queries
  //!

  // Make timestamp queries (disjoint and start-end queries)
  // https://docs.microsoft.com/ko-kr/windows/desktop/api/d3d11/nf-d3d11-id3d11device-createquery
  auto handleDisjoint = *MD3D11Resources::CreateQuerySimple(defaults.mDevice, E11SimpleQueryType::TimeStampDisjoint);
  auto [handleFrameStart, handleFrameEnd] = *FD3D11Factory::CreateTimestampQueryPair2(defaults.mDevice);
  auto [handleCbStart, handleCbEnd] = *FD3D11Factory::CreateTimestampQueryPair2(defaults.mDevice);
  auto [handleDrawStart, handleDrawEnd] = *FD3D11Factory::CreateTimestampQueryPair2(defaults.mDevice);

  //!
  //! Set initial settings.
  //!

  // 1. Set RTV and DSV
  {
    auto bDevice = MD3D11Resources::GetDevice(defaults.mDevice);
    auto d3dDc   = MD3D11Resources::GetDeviceContext(defaults.mDevice);
    {
      auto bRTV     = MD3D11Resources::GetRTV(defaults.mRTV);
      auto bDSV     = MD3D11Resources::GetDSV(defaults.mDSV);
      auto* pRTV    = bRTV.GetPtr();

      d3dDc->OMSetRenderTargets(1, &pRTV, bDSV.GetPtr());
    }

    // 2. Set Viewport
    {
      D3D11_VIEWPORT vp;
      vp.TopLeftX = 0.0f; vp.TopLeftY = 0.0f;
      vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
      vp.Width    = static_cast<float>(width);
      vp.Height   = static_cast<float>(height);
      d3dDc->RSSetViewports(1, &vp);
    }

    // 3. Set Render, Rect, Deptn-Stencil, Blend states
    {
      // A. Set raster state (RS)
      auto bRS = MD3D11Resources::GetRasterState(defaults.mRasterState);
      d3dDc->RSSetState(bRS.GetPtr());

      // B. Set rect state.
      std::array<D3D11_RECT, 1> ownRectState = { D3D11_RECT {0, 0, 1280, 720} };
      d3dDc->RSSetScissorRects(1, ownRectState.data());

      // C. Set depth stencil state
      auto bDSS = MD3D11Resources::GetDepthStencilState(defaults.mDepthStencilState);
      d3dDc->OMSetDepthStencilState(bDSS.GetPtr(), 0x00);

      // D. Set blend state
      auto bBS = MD3D11Resources::GetBlendState(defaults.mBlendState);
      const FLOAT blendFactor[4] = {0, 0, 0, 0}; 
      d3dDc->OMSetBlendState(bBS.GetPtr(), blendFactor, 0xFFFFFFFF);
    }
     
    // 4. Set topologies.
    {
      auto vBuffer  = MD3D11Resources::GetBuffer(handleVertexBuffer);
      auto iBuffer  = MD3D11Resources::GetBuffer(handleIndiceBuffer);
      auto cBuffer  = MD3D11Resources::GetBuffer(handleConstantBuffer);
      auto* pVBuffer= vBuffer.GetPtr();
      auto* pCBuffer= cBuffer.GetPtr();

      d3dDc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // 5. Set layout into logical device context.
      auto bIL = MD3D11Resources::GetInputLayout(handleIL);
      d3dDc->IASetInputLayout(bIL.GetPtr());

      // 6. Set Vertex & Index & Constant Buffers
      UINT stride = sizeof(DVertex);
      UINT offset = 0;
      d3dDc->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
      d3dDc->IASetIndexBuffer(iBuffer.GetPtr(), DXGI_FORMAT_R32_UINT, 0);
      d3dDc->VSSetConstantBuffers(0, 1, &pCBuffer);
    }
  }
 
  //!
  //! Set up ImGui settings.
  //!

  // Setup Dear ImGui context
  {
    auto bDevice = MD3D11Resources::GetDevice(defaults.mDevice);
    auto d3dDc   = MD3D11Resources::GetDeviceContext(defaults.mDevice);
    SetupGuiSettings(*optRes, bDevice.GetRef(), d3dDc.GetRef());
  }

  auto& windowModel = *MGuiManager::CreateSharedModel<DModelWindow>("Window");
  MGuiManager::CreateGui<FGuiWindow>("Window", std::ref(windowModel));

  {
    auto bDevice      = MD3D11Resources::GetDevice(defaults.mDevice);
    auto d3dDc        = MD3D11Resources::GetDeviceContext(defaults.mDevice);
    auto bRTV         = MD3D11Resources::GetRTV(defaults.mRTV);
    auto bDSV         = MD3D11Resources::GetDSV(defaults.mDSV);
    auto bVS          = MD3D11Resources::GetVertexShader(handleVS);
    auto bPS          = MD3D11Resources::GetPixelShader(handlePS);

    auto bDisjoint    = MD3D11Resources::GetQuery(handleDisjoint);
    auto bFrameStart  = MD3D11Resources::GetQuery(handleFrameStart);
    auto bFrameEnd    = MD3D11Resources::GetQuery(handleFrameEnd);
    auto bCbStart     = MD3D11Resources::GetQuery(handleCbStart);
    auto bCbEnd       = MD3D11Resources::GetQuery(handleCbEnd);
    auto bDrawStart   = MD3D11Resources::GetQuery(handleDrawStart);
    auto bDrawEnd     = MD3D11Resources::GetQuery(handleDrawEnd);

    auto bSwapCHain   = MD3D11Resources::GetSwapChain(defaults.mSwapChain);
    auto cBuffer      = MD3D11Resources::GetBuffer(handleConstantBuffer);

    // Loop
    while (platform->CanShutdown() == false)
    {
      // Update Routine
      TIME_CHECK_CPU("CpuFrame");
      //auto cpuTime = MTimeChecker::CheckCpuTime("CpuFrame");
      platform->PollEvents();
      MGuiManager::Update();

      // Render Routine
      TIME_CHECK_D3D11_STALL(gpuTime, "GpuFrame", bDisjoint.GetRef(), d3dDc.GetRef());
      {
        TIME_CHECK_FRAGMENT(gpuTime, "Overall", bFrameStart.GetRef(), bFrameEnd.GetRef());

        d3dDc->ClearRenderTargetView(bRTV.GetPtr(), std::array<FLOAT, 4>{0, 0, 0, 1}.data());
        d3dDc->ClearDepthStencilView(bDSV.GetPtr(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        // https://bell0bytes.eu/shader-data/
        {
          TIME_CHECK_FRAGMENT(gpuTime, "CBuffer", bCbStart.GetRef(), bCbEnd.GetRef());

          DCbScale scale;
          scale.mScale = windowModel.mScale;
          d3dDc->UpdateSubresource(cBuffer.GetPtr(), 0, nullptr, &scale, 0, 0);
        }

        d3dDc->VSSetShader(bVS.GetPtr(), nullptr, 0);
        d3dDc->PSSetShader(bPS.GetPtr(), nullptr, 0);

        {
          TIME_CHECK_FRAGMENT(gpuTime, "Draw", bDrawStart.GetRef(), bDrawEnd.GetRef());
          d3dDc->DrawIndexed(3, 0, 0);
        }

        MGuiManager::Render();

        // Present the back buffer to the screen.
        HR(bSwapCHain->Present(1, 0));
      }
    }
  }

  MGuiManager::Shutdown();
  
  // Remove all resources.
  {
    MD3D11Resources::RemoveQuery(handleFrameStart);
    MD3D11Resources::RemoveQuery(handleFrameEnd);
    MD3D11Resources::RemoveQuery(handleCbStart);
    MD3D11Resources::RemoveQuery(handleCbEnd);
    MD3D11Resources::RemoveQuery(handleDrawStart);
    MD3D11Resources::RemoveQuery(handleDrawEnd);
    MD3D11Resources::RemoveQuery(handleDisjoint);
  }
  {
    const auto flag = MD3D11Resources::RemovePixelShader(handlePS);
    assert(flag == true);
  }
  {
    const auto flag = MD3D11Resources::RemoveInputLayout(handleIL);
    assert(flag == true);
  }
  {
    const auto flag = MD3D11Resources::RemoveVertexShader(handleVS);
    assert(flag == true);
  }
  {
    const auto flag = MD3D11Resources::RemoveBuffer(handleConstantBuffer);
    assert(flag == true);
  }
  {
    const auto flag = MD3D11Resources::RemoveBuffer(handleIndiceBuffer);
    assert(flag == true);
  }
  {
    const auto flag = MD3D11Resources::RemoveBuffer(handleVertexBuffer);
    assert(flag == true);
  }
  {
    const auto flag = MD3D11Resources::RemoveDefaultFrameBufferResouce(*optDefaults);
    assert(flag == true);
  }

  platform->RemoveAllWindow();
  platform->RemoveConsoleWindow();
  platform->ReleasePlatform();
  WIN32_TRY_TURN_OFF_DEBUG();
	return 0;
}