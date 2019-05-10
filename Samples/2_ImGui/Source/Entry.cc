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

#include <examples/imgui_impl_win32.h>
#include <examples/imgui_impl_dx11.h>
#include <imgui.h>

#include <D3Dcompiler.h>
#include <D3D11.h>
#include <HelperMacro.h>
#include <XCommonDebug.h>
#include <ComWrapper/IComOwner.h>
#include <ComWrapper/IComBorrow.h>
#include <FD3D11Factory.h>
#include <Profiling/MTimeChecker.h>
#include <MGuiManager.h>

#include <StringUtil/XUtility.h>
#include <FGuiHelloWorld.h>
#include <FGuiBackground.h>
#include <FGuiMainMenu.h>

#include <FWindowsPlatform.h>
#include <PLowInputMousePos.h>
#include <XPlatform.h>
#include <Graphics/MD3D11Resources.h>

// Win32 message handler
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/// @brief
std::optional<dy::DWindowHandle>
CreateMainWindow(const std::string& titleName, unsigned width, unsigned height)
{
  auto value = MTimeChecker::CheckCpuTime("CreateMainWindow");

  dy::PWindowCreationDescriptor desc = {};
  desc.mIsWindowFullScreen = false;
  desc.mIsWindowResizable = true;
  desc.mIsWindowShouldFocus = true;
  desc.mIsWindowVisible = true;
  desc.mWindowName = "D3D11 2_Imgui";
  desc.mWindowWidth = 1280;
  desc.mWindowHeight = 720;
  
#ifdef _WIN32
  #ifdef CreateWindow
    #undef CreateWindow
  #endif
#endif

  auto optRes = platform->CreateWindow(desc);
  if (optRes.has_value() == false)
  {
    return std::nullopt;
  }
    
#ifdef _WIN32
  #ifndef CreateWindow
    #define CreateWindow CreateWindowW
  #endif
#endif

  auto& input = platform->GetInputManager();
  input.SetMousePosFeatureState(dy::base::ELowMousePosState::Normal);
  return optRes;
}

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
  auto optRes = CreateMainWindow("D3D11 2_ImGui", 1280, 720);
  assert(optRes.has_value() == true);

  const auto& checker = MTimeChecker::Get("CreateMainWindow");
  LOG("[%s] : %.4lf second.\n", "CreateMainWindow", checker.GetRecent().count());

  // Get width and height of main window client region.
  const auto width  = platform->GetWindowWidth(*optRes);
  const auto height = platform->GetWindowHeight(*optRes);
  HWND mainWindowHandle = static_cast<HWND>(platform->_GetHandleOf(*optRes));
  
  const auto optDefaults = FD3D11Factory::CreateDefaultFrameBuffer(*platform, *optRes);
  assert(optDefaults.has_value() == true);
  const auto& defaults = *optDefaults;

  {
    auto bDevice = MD3D11Resources::GetDevice(defaults.mDevice);
    auto d3dDc   = MD3D11Resources::GetDeviceContext(defaults.mDevice);

    // 1. Set RTV and DSV
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
  }

  // Make timestamp queries (disjoint and start-end queries)
  // https://docs.microsoft.com/ko-kr/windows/desktop/api/d3d11/nf-d3d11-id3d11device-createquery
  IComOwner<ID3D11Query> ownDisjointQuery       = nullptr;
  IComOwner<ID3D11Query> ownGpuStartFrameQuery  = nullptr; 
  IComOwner<ID3D11Query> ownGpuEndFrameQuery    = nullptr;
  {
    auto bDevice = MD3D11Resources::GetDevice(defaults.mDevice);
    auto d3dDc   = MD3D11Resources::GetDeviceContext(defaults.mDevice);

    ownDisjointQuery       = *FD3D11Factory::CreateTimestampQuery(bDevice.GetRef(), true);
    ownGpuStartFrameQuery  = *FD3D11Factory::CreateTimestampQuery(bDevice.GetRef(), false);
    ownGpuEndFrameQuery    = *FD3D11Factory::CreateTimestampQuery(bDevice.GetRef(), false);

    // Setup Dear ImGui context
    MGuiManager::Initialize(
      [&]()
      {
        ImGui_ImplWin32_Init(platform->_GetHandleOf(*optRes));
        ImGui_ImplDX11_Init(bDevice.GetPtr(), d3dDc.GetPtr());
        platform->SetPreProcessCallback(ImGui_ImplWin32_WndProcHandler);
      },
      [&]()
      {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
      }
    );
    MGuiManager::SetRenderCallbacks(
      [] {
        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
      },
      [] {
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
      }
    );
    MGuiManager::CreateGui<FGuiHelloWorld>("Hello");
    MGuiManager::CreateGui<FGuiMainMenu>("MainMenu");
  }
 
  // Loop
  {
    auto bDevice    = MD3D11Resources::GetDevice(defaults.mDevice);
    auto bSwapCHain = MD3D11Resources::GetSwapChain(defaults.mSwapChain);
    auto d3dDc      = MD3D11Resources::GetDeviceContext(defaults.mDevice);

    auto bRTV       = MD3D11Resources::GetRTV(defaults.mRTV);
    auto bDSV       = MD3D11Resources::GetDSV(defaults.mDSV);

    while (platform->CanShutdown() == false)
    {
      TIME_CHECK_CPU("CpuFrame");
      platform->PollEvents();
      MGuiManager::Update();

      auto* background = MGuiManager::GetGui("Background");
      static dy::math::DVector3<dy::math::TReal> bgCol = {0};
      if (background != nullptr)
      {
        auto& bgModel = static_cast<DModelBackground&>(background->GetModel());
        bgCol = bgModel.mBackgroundColor; 
      }

      TIME_CHECK_D3D11_STALL(gpuTime, "GpuFrame", *ownDisjointQuery.GetPtr(), d3dDc.GetRef());
      {
        TIME_CHECK_FRAGMENT(gpuTime, "Overall", *ownGpuStartFrameQuery.GetPtr(), *ownGpuEndFrameQuery.GetPtr());

        d3dDc->ClearRenderTargetView(bRTV.GetPtr(), std::array<FLOAT, 4>{bgCol.X, bgCol.Y, bgCol.Z, 1}.data());
        d3dDc->ClearDepthStencilView(bDSV.GetPtr(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        MGuiManager::Render();
        // Present the back buffer to the screen.
        HR(bSwapCHain->Present(0, 0));
      }
    }
  }

  MGuiManager::Shutdown();
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