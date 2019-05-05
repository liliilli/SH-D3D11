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

#include <FGuiHelloWorld.h>

#include <imgui.h>
#include <MTimeChecker.h>

FGuiHelloWorld::FGuiHelloWorld()
{
  this->mModel = std::make_unique<DModelHelloWorld>();
  this->SetVisibility(true);
}

void FGuiHelloWorld::Render()
{
  auto& model = static_cast<DModelHelloWorld&>(*this->mModel);

  // Create a window called "Hello, world!" and append into it.
  ImGui::Begin("Hello, world!"); 
  // Display some text (you can use a format strings too)
  ImGui::Text("This is some useful text.");  

  // Edit 1 float using a slider from 0.0f to 1.0f
  ImGui::SliderFloat("float", &model.f, 0.0f, 1.0f); 
  if (ImGui::Button("Button")) 
  {
    model.counter++;
  }
  ImGui::SameLine();
  ImGui::Text("counter = %d", model.counter);

  ImGui::Text(
    "Application average of ImGui %.3f ms/frame (%.1f FPS)", 
    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::Text("CPU Average frame %.3f ms/frame",
    MTimeChecker::Get("CpuFrame").GetAverage().count() * 1000.0);

  if (MTimeChecker::GetGpuD3D11("GpuFrame").HasFragment("Overall") == true)
  {
    ImGui::Text("GPU Frame %.3f ms/frame", 
      MTimeChecker::GetGpuD3D11("GpuFrame")["Overall"].GetRecent().count() * 1000.0);
  }
  ImGui::End();
}