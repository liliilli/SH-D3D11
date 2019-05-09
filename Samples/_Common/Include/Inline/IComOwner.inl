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

#include <cassert>
#include <ComWrapper/IComBorrow.h>

template <typename TType>
IComOwner<TType>::IComOwner(std::nullptr_t) { }

template <typename TType>
IComOwner<TType>::IComOwner(TType* pCOMInstance)
  : mObj{new __IComOwner<TType>(pCOMInstance)}
{ };

template <typename TType>
IComOwner<TType>::IComOwner(IComOwner&& movedOwner) noexcept
  : mObj{movedOwner.mObj}
{
  movedOwner.mObj = nullptr;
}

template <typename TType>
IComOwner<TType>& IComOwner<TType>::operator=(IComOwner&& movedOwner) noexcept
{
  if (this == &movedOwner) { return *this; }
  
  // Release old one.
  if (this->mObj != nullptr)
  {
    TryReleaseSelf();
    delete this->mObj;
    this->mObj = nullptr;
  }

  this->mObj = movedOwner.mObj;
  movedOwner.mObj = nullptr;
  return *this;
}

template <typename TType>
IComOwner<TType>::~IComOwner()
{
  assert(this->mObj == nullptr 
      || this->mObj->mCounter.syncGetCounter() == 0);
  TryReleaseSelf();
}

template <typename TType>
bool IComOwner<TType>::IsValid() const noexcept
{
  return this->mObj != nullptr;
}

template <typename TType>
IComBorrow<TType> IComOwner<TType>::GetBorrow() noexcept
{
  assert(this->mObj != nullptr);
  return IComBorrow<TType>(*this->mObj);
}

template <typename TType>
TType* IComOwner<TType>::GetPtr() noexcept
{
  assert(this->mObj != nullptr);
  return this->mObj->mPtrOwner;
}

#if 0
template <typename TType>
TType** IComOwner<TType>::GetAddressOf() noexcept
{
  assert(this->mObj != nullptr);
  return &this->mPtrOwner;
}
#endif

template <typename TType>
TType& IComOwner<TType>::operator*()
{
  assert(this->mObj != nullptr);
  return *this->mObj->mPtrOwner;
}

template <typename TType>
TType* IComOwner<TType>::operator->()
{
  assert(this->mObj != nullptr);
  return this->mObj->mPtrOwner;
}

template <typename TType>
void IComOwner<TType>::Release()
{
  this->TryReleaseSelf();
}

template <typename TType>
void IComOwner<TType>::TryReleaseSelf()
{
  if (this->mObj == nullptr) { return; }

  if (this->mObj != nullptr)
  {
    this->mObj->mPtrOwner->Release();
    delete this->mObj;
    this->mObj = nullptr;
  }
}
