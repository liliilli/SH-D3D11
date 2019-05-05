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

#include <memory>
#include <unordered_map>
#include <Math/Type/Micellanous/DTristateBool.h>

#include <IGuiModel.h>

/// @interface IGuiFrame
/// @brief Interface of wrapping ImGui frames.
class IGuiFrame 
{
public:
  IGuiFrame() = default;
  virtual ~IGuiFrame() = 0;

  /// @brief Set visibility of GUI frame.
  void SetVisibility(bool isVisible) noexcept;

  /// @brief Check this frame and children are visible to screen.
  [[nodiscard]] bool IsVisible() const noexcept;

  /// @brief Check frame has model instance.
  [[nodiscard]] bool HasModel() const noexcept;

  /// @brief Get model reference.
  IGuiModel& GetModel();

  /// @brief Render GUI.
  virtual void Render() = 0;

protected:
  /// @brief Visibility variable.
  dy::math::DTristateBool mIsVisible = {true, false};
  /// @brief Model instance.
  std::unique_ptr<IGuiModel> mModel;
#if 0
  /// @brief
  std::unordered_map<std::string, std::unique_ptr<IGuiFrame>> mChildGuis;
#endif
};

inline IGuiFrame::~IGuiFrame() = default;