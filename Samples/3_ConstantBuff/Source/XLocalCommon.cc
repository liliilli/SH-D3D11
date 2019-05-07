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

#include <XLocalCommon.h>

#include <examples/imgui_impl_win32.h>
#include <examples/imgui_impl_dx11.h>
#include <imgui.h>

#include <DWindowHandle.h>
#include <MTimeChecker.h>
#include <MGuiManager.h>
#include <XPlatform.h>

#include <Windows.h>

// Win32 message handler
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::optional<dy::DWindowHandle>
CreateMainWindow(const std::string& titleName, unsigned width, unsigned height)
{
  auto value = MTimeChecker::CheckCpuTime("CreateMainWindow");

  dy::PWindowCreationDescriptor desc = {};
  desc.mIsWindowFullScreen = false;
  desc.mIsWindowResizable = true;
  desc.mIsWindowShouldFocus = true;
  desc.mIsWindowVisible = true;
  desc.mWindowName = "D3D11 3_ConstantBuff";
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

void SetupGuiSettings(dy::DWindowHandle& wHandle, ID3D11Device& device, ID3D11DeviceContext& dc)
{
  MGuiManager::Initialize(
    [&]()
    {
      ImGui_ImplWin32_Init(platform->_GetHandleOf(wHandle));
      ImGui_ImplDX11_Init(&device, &dc);
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
}