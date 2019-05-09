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

#include <atomic>
#include <algorithm>
#include <mutex>
#include <cstdint>
#include <vector>

template <typename TType>
class IComBorrow;

template <typename TType>
class XComCounter final
{
public:
  /// @brief
  void syncPush(IComBorrow<TType>& borrow)
  {
    const auto counter = this->mCounter.load(std::memory_order_consume);
    this->mCounter.store(counter + 1, std::memory_order_release);

    {
      std::lock_guard<std::mutex> mtx(this->mMutexBorrows);
      this->mBorrows.emplace_back(&borrow);
    }
  }

  /// @brief
  void syncPop(IComBorrow<TType>& pop)
  {
    const auto counter = mCounter.load(std::memory_order_consume);
    this->mCounter.store(counter - 1, std::memory_order_release);

    {
      std::lock_guard<std::mutex> mtx(this->mMutexBorrows);
      auto it = 
        std::find_if(
          mBorrows.begin(), mBorrows.end(), 
          [pPop = &pop](const auto& pItem) { return pItem == pPop; }
      );
      assert(it != this->mBorrows.end());
      this->mBorrows.erase(it);
    }
  }

  /// @brief
  uint32_t syncGetCounter()
  {
    return this->mCounter.load(std::memory_order_consume);
  }

private:
  std::atomic<uint32_t> mCounter = 0;
  std::mutex mMutexBorrows;
  std::vector<IComBorrow<TType>*> mBorrows;
};
#include <Inline/XComCounter.inl>