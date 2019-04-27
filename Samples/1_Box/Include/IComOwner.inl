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

#include <IComBorrow.h>

template <typename TType>
IComOwner<TType>::IComOwner(TType* pCOMInstance)
  : mPtrOwner{pCOMInstance} 
{ };

template <typename TType>
IComOwner<TType>::~IComOwner()
{
  TryReleaseSelf();
}

template <typename TType>
IComOwner<TType>::IComOwner(IComOwner&& movedOwner) noexcept
  : mPtrOwner{movedOwner.mPtrOwner}
{
  movedOwner.mPtrOwner = nullptr;
}

template <typename TType>
IComOwner<TType>& IComOwner<TType>::operator=(IComOwner&& movedOwner) noexcept
{
  if (this == &movedOwner) { return *this; }

  TryReleaseSelf();
  this->mPtrOwner = movedOwner.mPtrOwner;
  movedOwner.mPtrOwner = nullptr;

  return *this;
}

template <typename TType>
bool IComOwner<TType>::IsValid() const noexcept
{
  return this->mPtrOwner != nullptr;
}

template <typename TType>
IComBorrow<TType> IComOwner<TType>::GetBorrow() noexcept
{
  return IComBorrow<TType>(*this);
}

template <typename TType>
IComBorrow<TType> IComOwner<TType>::operator->()
{
  return IComBorrow<TType>(*this);
}

template <typename TType>
void IComOwner<TType>::Release()
{
  this->TryReleaseSelf();
}

template <typename TType>
void IComOwner<TType>::TryReleaseSelf()
{
  if (this->mPtrOwner != nullptr)
  {
    this->mPtrOwner->Release();
    this->mPtrOwner = nullptr;
  }
}
