#pragma once
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

#include <IGuiFrameModel.h>
#include <IGuiModel.h>
#include <Math/Type/Micellanous/DUuid.h>

class DModelProfiling;
class FGuiProfiling final : public IGuiFrameModel<DModelProfiling>
{
public:
  FGuiProfiling();
  virtual ~FGuiProfiling() = default;

  void Render() override final;
};

class DModelProfiling final : public IGuiModel
{
private:
  std::string mUuid = ::dy::math::DUuid{true}.ToString();
  friend class FGuiProfiling;
};

