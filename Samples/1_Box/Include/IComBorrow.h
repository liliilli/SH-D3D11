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
class IComOwner;

template <typename TType>
class IComBorrow final
{
public:
  explicit IComBorrow(IComOwner<TType>& comOwner);

  TType& Get() 
  {
    return *this->mPtrCom;
  }

  operator TType*()
  {
    return this->mPtrCom;
  }

  TType* operator->()
  {
    return this->mPtrCom;
  }

private:
  TType* mPtrCom = nullptr;
};
#include <IComBorrow.inl>