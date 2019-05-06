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

#include <FGuiAbout.h>
#include <imgui.h>

FGuiAbout::FGuiAbout()
{
  this->mModel = std::make_unique<DModelAbout>();
  this->SetVisibility(true);
}

void FGuiAbout::Render()
{
  auto& model = static_cast<DModelAbout&>(this->GetModel());

  // Create a window called "Hello, world!" and append into it.
  ImGui::Begin("About", &model.mOpended, ImGuiWindowFlags_AlwaysAutoResize);

  // Display some text (you can use a format strings too)
  ImGui::Text("SH-D3D11 DirectX11 Sandbox project");  
  ImGui::Separator();

  ImGui::Text("Made by Neu(Jongmin Yun)).");
  ImGui::Text(
    "SH-D3D11 is licensed under the MIT License\n"
    "except for boost library.");
  ImGui::End();

  // Post-process
  if (model.mOpended == false)
  {
    this->DestroySelf();
  }
}
