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

#include <FGuiProfiling.h>

#include <imgui.h>
#include <MTimeChecker.h>
#include <XPlatform.h>

FGuiProfiling::FGuiProfiling()
{
  this->mModel = std::make_unique<DModelProfiling>();
  this->SetVisibility(true);
}

void FGuiProfiling::Render()
{
  auto& model = static_cast<DModelProfiling&>(this->GetModel());
  ImGui::PushID(model.mUuid.c_str());

  // Create a window called "Hello, world!" and append into it.
  ImGui::Begin("Profiling Window");
  ImGui::SetWindowSize(ImVec2{300, 400});

  ImGui::Text("CPU Usage : ");
  ImGui::SameLine();
  ImGui::ProgressBar(platform->GetProfilingManager().GetCpuUsage() / 500.0f);

  ImGui::Text("CPU Frame : %.3f ms/frame", 
    MTimeChecker::Get("CpuFrame").GetRecent().count() * 1000.0);
  ImGui::Text("CPU Average : %.3f ms/50 frame",
    MTimeChecker::Get("CpuFrame").GetAverage().count() * 1000.0);

  const float ramMBUsage = platform->GetProfilingManager().GetRamUsage() / (1024.0 * 1024.0);
  ImGui::Text("RAM Usage : %.3f MB", ramMBUsage);

  ImGui::Separator();

  if (MTimeChecker::GetGpuD3D11("GpuFrame").HasFragment("Overall") == true)
  {
    ImGui::Text("GPU Frame %.3f ms/frame", 
      MTimeChecker::GetGpuD3D11("GpuFrame")["Overall"].GetRecent().count() * 1000.0);
  }

  ImGui::Text(
    "ImGui Rendering Average : %.3f ms/frame (%.1f FPS)", 
    1000.0f / ImGui::GetIO().Framerate, 
    ImGui::GetIO().Framerate);

  ImGui::End();
  ImGui::PopID();
}