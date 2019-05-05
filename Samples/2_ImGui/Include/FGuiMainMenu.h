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

class DModelMainMenu final : public IGuiModel
{
public:
  bool mViewBackgroundColorPicker = false;

private:
  bool mPrevViewBackgroundColorPicker = false;

  friend class FGuiMainMenu;
};

class FGuiMainMenu final : public IGuiFrameModel<DModelMainMenu>
{
public:
  FGuiMainMenu();
  virtual ~FGuiMainMenu() = default;

private:
  void Render() override final;
  void UpdateBackgroundPicker(bool& present, bool& previous);
};

