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

#include <cassert>
#include <memory>
#include <DGridYIterator.h>
#include <DGridYSubscript.h>

/// @class DDynamicGrid2D
/// @tparam TType
/// @tparam TAllocator
/// @brief Dynamic (Size-controllable) grid 2d container type that has continuous buffer.
template <typename TType, typename TAllocator = std::allocator<TType>>
class DDynamicGrid2D 
{
public:
  using value_type      = TType;
  using allocator_type  = TAllocator;
  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = value_type&;
  using const_reference = const value_type&;
  using alloc_traits    = std::allocator_traits<TAllocator>;
  using pointer         = typename alloc_traits::pointer;
  using const_pointer   = typename alloc_traits::const_pointer;
  using iterator        = DGridYIterator<value_type>;

  DDynamicGrid2D(size_type x, size_type y)
    : mGridX{x}, mGridY{y}
  {
    assert(x > 0 && y > 0);
    this->mOwnerPtr = this->mAlloc.allocate(x * y);
    
    auto* pItem = this->mOwnerPtr;
    for (size_type i = 0, count = x * y; i < count; ++i)
    {
      // Always call default constructor.
      alloc_traits::construct(this->mAlloc, pItem);
      ++pItem;
    }
  }

  template <typename... TArgs>
  DDynamicGrid2D(size_type x, size_type y, TArgs&&... args)
    : mGridX{x}, mGridY{y}
  {
    assert(x > 0 && y > 0);
    this->mOwnerPtr = this->mAlloc.allocate(x * y);
    
    auto* pItem = this->mOwnerPtr;
    for (size_type i = 0, count = x * y; i < count; ++i)
    {
      // Always call default constructor.
      alloc_traits::construct(this->mAlloc, pItem, std::forward<TArgs>(args)...);
      ++pItem;
    }  
  }

  DDynamicGrid2D(DDynamicGrid2D&& movedInstance) noexcept
    : mAlloc{ movedInstance.mAlloc },
      mOwnerPtr { movedInstance.mOwnerPtr },
      mGridX{ movedInstance.mGridX },
      mGridY{ movedInstance.mGridY }
  { 
    movedInstance.mOwnerPtr = nullptr;
  }

  virtual ~DDynamicGrid2D()
  {
    if (this->mOwnerPtr != nullptr)
    {
      auto* pItem = this->mOwnerPtr;
      const auto count = this->mGridX * this->mGridY;
      for (size_type i = 0; i < count; ++i)
      {
        // Call destructor.
        alloc_traits::destroy(this->mAlloc, pItem);
        ++pItem;
      }

      this->mAlloc.deallocate(this->mOwnerPtr, count);
      this->mOwnerPtr = nullptr;
    }
  }

  /// @brief 
  template <typename... TArgs>
  void Resize(size_type col, size_type row, TArgs&&... initArgs)
  {
    assert(col > 0 && row > 0);

    const int difCol = static_cast<int>(col) - static_cast<int>(this->mGridX);
    const int difRow = static_cast<int>(row) - static_cast<int>(this->mGridY);

    const int oldCol = static_cast<int>(this->mGridX) + (difCol < 0 ? difCol : 0);
    const int oldRow = static_cast<int>(this->mGridY) + (difRow < 0 ? difRow : 0);

    // Try to allocate new buffer and allocator.
    auto  newAlloc    = TAllocator{};
    auto* pNewBuffer  = newAlloc.allocate(col * row);

    auto* pNewItem  = pNewBuffer;
    auto* pOldItem  = this->mOwnerPtr;
    for (auto y = 0; y < oldRow; ++y)
    {
      // Call move constructor
      for (auto x = 0; x < oldCol; ++x)
      {
        alloc_traits::construct(newAlloc, pNewItem + x, std::move(*(pOldItem + x)));
      }

      // If oldCol < col (newCol), construct new values.... 
      for (auto x = oldCol; x < col; ++x)
      {
        // if sizeof...(initArgs) is not 0.
        if constexpr (sizeof...(initArgs) == 0)
        {
          alloc_traits::construct(newAlloc, pNewItem + x);
        }
        else
        {
          alloc_traits::construct(newAlloc, pNewItem + x, std::forward<TArgs>(initArgs)...);
        }
      }

      pNewItem += col;
      pOldItem += this->mGridX;
    }

    // If oldRow < row (newRow), construct new values...
    for (auto y = oldRow; y < row; ++y)
    {
      for (auto x = 0; x < col; ++x)
      {
        // if sizeof...(initArgs) is not 0.
        if constexpr (sizeof...(initArgs) == 0)
        {
          alloc_traits::construct(newAlloc, pNewItem + x);
        }
        else
        {
          alloc_traits::construct(newAlloc, pNewItem + x, std::forward<TArgs>(initArgs)...);
        }
      }

      pNewItem += col;
    }

    // Destroy and Deallocate old buffer.
    pOldItem = this->mOwnerPtr;
    for (auto y = 0; y < this->mGridY; ++y)
    {
      for (auto x = 0; x < this->mGridX; ++x)
      {
        alloc_traits::destroy(this->mAlloc, pOldItem);
        ++pOldItem;
      }
    }
    this->mAlloc.deallocate(this->mOwnerPtr, this->mGridX * this->mGridY);

    // Replace old variable with new value.
    this->mAlloc = std::move(newAlloc);
    this->mOwnerPtr = pNewBuffer;
    this->mGridX = col;
    this->mGridY = row;
  }

  /// @brief
  size_type GetRowSize() const noexcept
  {
    return this->mGridY;
  }

  /// @brief
  size_type GetColumnSize() const noexcept
  {
    return this->mGridX;
  }

  /// @brief 
  void Set(size_type x, size_type y, const_reference value)
  {
    assert(x < this->mGridX && y < this->mGridY);
    auto* pItem = mOwnerPtr + (y * mGridX) + x;
    *pItem = value;
  }

  /// @brief 
  reference Get(size_type x, size_type y)
  {
    assert(x < this->mGridX && y < this->mGridY);
    return *(mOwnerPtr + (y * mGridX) + x);
  }

  /// @brief 
  const_reference Get(size_type x, size_type y) const
  {
    assert(x < this->mGridX && y < this->mGridY);
    return *(mOwnerPtr + (y * mGridX) + x);
  }

  /// @brief
  pointer Data() noexcept
  {
    return this->mOwnerPtr;
  }

  /// @brief
  const_pointer Data() const noexcept
  {
    return this->mOwnerPtr;
  }

  DGridYSubscript<value_type> operator[](std::size_t b)
  {
    value_type* ptr = this->mOwnerPtr + (b * this->mGridX);
    return {ptr, this->mGridX};
  }

  //!
  //! Iterators
  //!

  iterator begin() noexcept
  {
    return {this->mOwnerPtr, this->mGridY, this->mGridX, 0};
  }

  iterator end() noexcept
  {
    return {this->mOwnerPtr, this->mGridY, this->mGridX, this->mGridY};
  }

private:
  TAllocator  mAlloc;
  value_type* mOwnerPtr = nullptr;
  size_type   mGridX = 0;
  size_type   mGridY = 0;
};