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

#include <variant>
#include <stdexcept>
#include <IGuiFrame.h>
#include <IGuiModel.h>
#include <DGuiNoneModel.h>

template <typename TType>
class IGuiFrameModel : public IGuiFrame
{
public:
  virtual ~IGuiFrameModel() = default;

  /// @brief Check frame has model instance.
  [[nodiscard]] bool HasModel() const noexcept override final;

  /// @brief Get model reference.
  /// If not exist or model has not been initialized, just throw error.
  IGuiModel& GetModel() override final;

  /// @brief Destroy GUI instance.
  void DestroySelf() override final;

protected:
  /// @brief Model instance.
  using TModel = std::variant<std::monostate, std::unique_ptr<IGuiModel>, IGuiModel*>;
  TModel mModel;
  /// @brief If false, mModel has std::unique_ptr<IGuiModel>.
  /// If true, mModel has IGuiModel* that is from outside world.
  bool mIsIndirect = false;
};

template <>
class IGuiFrameModel<void> : public IGuiFrame
{
public:
  virtual ~IGuiFrameModel() = default;

  /// @brief Check frame has model instance.
  [[nodiscard]] bool HasModel() const noexcept override final; 

  /// @brief 
  IGuiModel& GetModel() override final;

  /// @brief Destroy GUI instance.
  void DestroySelf() override final;

};
#include <IGuiFrameModel.inl>