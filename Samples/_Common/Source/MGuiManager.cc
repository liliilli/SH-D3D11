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

#include <MGuiManager.h>
#include <imgui.h>

void MGuiManager::SetRenderCallbacks(
  void(*preRenderCallback)(void),
  void(*postRenderCallback)(void))
{
  mPreRenderCallback  = preRenderCallback;
  mPostRenderCallback = postRenderCallback; 
}

void MGuiManager::Initialize(
  const std::function<void(void)>& initFunc, 
  const std::function<void(void)>& shutFunc)
{
  // Create imgui context.
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::GetIO();
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

  // Setup Dear ImGui style
  //ImGui::StyleColorsDark();
  ImGui::StyleColorsClassic();

  // Set callback and call init function.
  mInitCallback = initFunc;
  mShutCallback = shutFunc;

  mInitCallback();
  mInitCallback = nullptr;
}

void MGuiManager::Shutdown()
{
  mShutCallback();
  mShutCallback       = nullptr;
  mPreRenderCallback  = nullptr;
  mPostRenderCallback = nullptr;
  mGuis.clear();
}

IGuiFrame* MGuiManager::GetGui(const std::string& guiName)
{
  auto it = mGuis.find(guiName);
  if (it == mGuis.end())
  {
    return nullptr;
  }

  return it->second.get();
}

bool MGuiManager::RemoveGui(const std::string& guiName)
{
  if (mGuis.find(guiName) == mGuis.end())
  {
    return false;
  }

  mGuis.erase(guiName);
  return true;  
}

void MGuiManager::Render()
{
  if (mPreRenderCallback != nullptr)
  {
    mPreRenderCallback();
  }
  
  for (auto& [guiName, guiFrame] : mGuis)
  {
    if (guiFrame == nullptr) { continue; }
    if (guiFrame->IsVisible() == false) { continue; }

    guiFrame->Render();
  }

  // Render
  ImGui::Render();
  if (mPostRenderCallback != nullptr)
  {
    mPostRenderCallback();
  }
}
