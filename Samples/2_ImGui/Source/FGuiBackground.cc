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

#include <FGuiBackground.h>

#include <imgui.h>
#include <Profiling/MTimeChecker.h>

FGuiBackground::FGuiBackground(DModelBackground& model)
{
  this->mModel = &model;
  this->mIsIndirect = true;
  this->SetVisibility(true);
}

void FGuiBackground::Render()
{
  auto& model = static_cast<DModelBackground&>(this->GetModel());

  // Create a window called "Hello, world!" and append into it.
  ImGui::PushID("AA");
  ImGui::Begin("Hello, World!"); 
  // Display some text (you can use a format strings too)
  ImGui::Text("This is some useful text.");  

  ImGui::ColorPicker3(
    "Background Picker", 
    model.mBackgroundColor.Data());
  // Edit 1 float using a slider from 0.0f to 1.0f
  ImGui::SameLine();

  ImGui::End();
  ImGui::PopID();
}