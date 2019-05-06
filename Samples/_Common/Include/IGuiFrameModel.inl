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

#include <MGuiManager.h>

template <typename TType>
bool IGuiFrameModel<TType>::HasModel() const noexcept
{
  return this->mModel.index() != 0;
}

template <typename TType>
IGuiModel& IGuiFrameModel<TType>::GetModel()
{
  if (this->mModel.index() == 0) 
  { 
    throw std::runtime_error("Model has not been initilaized."); 
  }

  if (this->mIsIndirect == false)
  {
    return *std::get<std::unique_ptr<IGuiModel>>(this->mModel);
  }
  else
  {
    return *std::get<IGuiModel*>(this->mModel);
  }
}

template <typename TType>
void IGuiFrameModel<TType>::DestroySelf() 
{
  MGuiManager::RemoveGui(*this);
}

inline bool IGuiFrameModel<void>::HasModel() const noexcept 
{ 
  return false; 
}

inline IGuiModel& IGuiFrameModel<void>::GetModel() 
{
  static DGuiNoneModel model = {};
  return model;
}

inline void IGuiFrameModel<void>::DestroySelf() 
{
  MGuiManager::RemoveGui(*this);
}