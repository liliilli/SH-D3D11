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

#include <FGuiWindow.h>

#include <imgui.h>
#include <Profiling/MTimeChecker.h>
#include <XPlatform.h>

FGuiWindow::FGuiWindow(DModelWindow& mModel)
{
  this->mModel = &mModel;
  this->mIsIndirect = true;
  this->SetVisibility(true);
}

void FGuiWindow::Render()
{
  auto& model = static_cast<DModelWindow&>(this->GetModel());

  // Create a window called "Hello, world!" and append into it.
  ImGui::Begin("Sample 3 : HeightMap", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

#if 0
  //!
  //! Sample Description.
  //!
  ImGui::Text("Sample Description.");

  ImGui::Separator();
  //!
  //! Profiling
  //!

  ImGui::Text("CPU Usage : ");
  ImGui::SameLine();
  ImGui::ProgressBar(platform->GetProfilingManager().GetCpuUsage() / 500.0f);

  auto& cpuFrame = MTimeChecker::Get("CpuFrame");
  ImGui::Text("CPU Frame : %.3f ms/frame",      cpuFrame.GetRecent().count() * 1000.0);
  ImGui::Text("CPU Average : %.3f ms/50 frame", cpuFrame.GetAverage().count() * 1000.0);

  const float ramMBUsage = platform->GetProfilingManager().GetRamUsage() / (1024.0 * 1024.0);
  ImGui::Text("RAM Usage : %.3f MB", ramMBUsage);

  const auto& gpuFrame = MTimeChecker::GetGpuD3D11("GpuFrame");
  if (gpuFrame.HasFragment("Overall") == true)
  {
    ImGui::Text("GPU Average : %.3f ms/50 frame", 
      gpuFrame["Overall"].GetAverage().count() * 1000.0f);
  }
  if (gpuFrame.HasFragment("Draw") == true)
  {
    ImGui::Text("GPU Draw : %.3f ms/50 frame",
      gpuFrame["Draw"].GetAverage().count() * 1000.0f);
  }

  ImGui::Text(
    "ImGui Rendering Average : %.3f ms/frame (%.1f FPS)", 
    1000.0f / ImGui::GetIO().Framerate, 
    ImGui::GetIO().Framerate);

  ImGui::Separator();
  ImGui::Spacing();
  //!
  //! Value properties.
  //!

  ImGui::SliderFloat("Scale", &model.mScale, 0.0f, 2.0f);
#endif
  ImGui::End();
}
