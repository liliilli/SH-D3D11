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

#include <FGuiMainMenu.h>

#include <imgui.h>
#include <MGuiManager.h>
#include <FGuiBackground.h>
#include <FGuiProfiling.h>
#include <FGuiAbout.h>
#include <XPlatform.h>

FGuiMainMenu::FGuiMainMenu()
{
  this->mModel = std::make_unique<DModelMainMenu>();
  this->SetVisibility(true);
}

void FGuiMainMenu::Render()
{
  // Get model
  auto& model = static_cast<DModelMainMenu&>(this->GetModel());

  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Exit")) 
      {
        // Clicked menu-item
        platform->TryShutdown();
      };
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View"))
    {
      if (ImGui::MenuItem("Background Color Picker", "", &model.mViewBackgroundColorPicker))
      {
        // Clicked menu-item
        this->UpdateBackgroundPicker(
          model.mViewBackgroundColorPicker, 
          model.mPrevViewBackgroundColorPicker
        );
      }

      if (ImGui::MenuItem("Profiling Window", "", &model.mProfilingWindow))
      {
        // Clicked menu-item.
        this->UpdateProfilingWindow(
          model.mProfilingWindow,
          model.mPrevProfilingWindow
        );
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help"))
    {
      if (ImGui::MenuItem("About"))
      {
        // Clicked menu-item.
        if (MGuiManager::GetGui("About") == nullptr)
        {
          MGuiManager::CreateGui<FGuiAbout>("About");
        }
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

void FGuiMainMenu::UpdateBackgroundPicker(bool& present, bool& previous)
{
  static DModelBackground modelBackground = {};

  if (previous != present)
  {
    if (present == true)
    {
      MGuiManager::CreateGui<FGuiBackground>("Background", std::ref(modelBackground));
    }
    else
    {
      MGuiManager::RemoveGui("Background");
    }

    previous = present;
  }
}

void FGuiMainMenu::UpdateProfilingWindow(bool& present, bool& previous)
{
  if (previous != present)
  {
    if (present == true)
    {
      MGuiManager::CreateGui<FGuiProfiling>("Profiling");
    }
    else
    {
      MGuiManager::RemoveGui("Profiling");
    }

    previous = present;
  }
}

