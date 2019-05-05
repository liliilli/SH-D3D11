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