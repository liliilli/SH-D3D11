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
IComBorrow<TType>::IComBorrow(__IComOwner<TType>& comOwner)
  : mPtrCom{&comOwner}
{
  this->mPtrCom->mCounter.syncPush(*this);
};

template <typename TType>
IComBorrow<TType>::~IComBorrow()
{
  this->mPtrCom->mCounter.syncPop(*this);
}

template <typename TType>
bool IComBorrow<TType>::IsValid() const noexcept
{
  return this->mPtrCom != nullptr;
}
