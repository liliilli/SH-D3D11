#pragma once
///
/// MIT License
/// Copyright (c) 2019 Jongmin Yun
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

template <typename TType>
class __IComOwner;

template <typename TType>
class XComCounter;

/// @class IComBorrow
/// @brief 
template <typename TType>
class IComBorrow final
{
public:
  explicit IComBorrow(__IComOwner<TType>& comOwner);

  IComBorrow(const IComBorrow& borrow)
    : mPtrCom{borrow.mPtrCom}
  {
    this->mPtrCom->mCounter.syncPush(*this);
  }

  IComBorrow& operator=(const IComBorrow& borrow)
  {
    if (this == &borrow) { return *this; }
    if (this->mPtrCom == borrow.mPtrCom) { return *this; }

    this->mPtrCom->mCounter.syncPop(*this);
    this->mPtrCom = borrow.mPtrCom;
    this->mPtrCom->mCounter.syncPush(*this);
    return *this;
  }

  IComBorrow(IComBorrow&& borrow) noexcept = delete;
  IComBorrow& operator=(IComBorrow&& borrow) noexcept = delete;

  ~IComBorrow();

  /// @brief Check this instance is valid (owns COM instance)
  [[nodiscard]] bool IsValid() const noexcept;

  TType& GetRef() 
  {
    return *this->mPtrCom->mPtrOwner;
  }
  
  TType* GetPtr()
  {
    return this->mPtrCom->mPtrOwner;
  }

  TType* operator->()
  {
    return this->mPtrCom->mPtrOwner;
  }

private:
  __IComOwner<TType>* mPtrCom = nullptr;

  friend class XComCounter<TType>;
};
#include <Inline/IComBorrow.inl>
