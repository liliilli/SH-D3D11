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

#include <array>
#include <IGuiFrameModel.h>
#include <IGuiModel.h>

class DModelWindow final : public IGuiModel
{
public:
  bool mDrawWireframe = false;

  float mCamera = 45.0f;
  float mDistance = 10.0f;

  std::array<int, 2> mTerrainGrid = {8, 8};
  std::array<int, 2> mTerrainFragment = {2, 2};
};

class FGuiWindow final : public IGuiFrameModel<DModelWindow>
{
public:
  FGuiWindow(DModelWindow& mModel);
  virtual ~FGuiWindow() = default;

  void Render() override final;
};