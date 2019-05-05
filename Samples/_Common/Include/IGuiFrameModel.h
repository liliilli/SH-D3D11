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
#include <IGuiFrame.h>
#include <IGuiModel.h>

template <typename TType>
class IGuiFrameModel : public IGuiFrame
{
public:
  virtual ~IGuiFrameModel() = default;

  /// @brief Check frame has model instance.
  [[nodiscard]] virtual bool HasModel() const noexcept override final
  {
    return this->mModel.index() != 0;
  }

  /// @brief 
  virtual IGuiModel& GetModel() override final
  {
    if (this->mModel.index() == 0) { throw 0; }

    if (this->mIsIndirect == false)
    {
      return *std::get<std::unique_ptr<IGuiModel>>(this->mModel);
    }
    else
    {
      return *std::get<IGuiModel*>(this->mModel);
    }
  }

protected:
  /// @brief Model instance.
  using TModel = std::variant<std::monostate, std::unique_ptr<IGuiModel>, IGuiModel*>;
  TModel mModel;
  /// @brief If false, mModel has std::unique_ptr<IGuiModel>.
  /// If true, mModel has IGuiModel* that is from outside world.
  bool mIsIndirect = false;
};