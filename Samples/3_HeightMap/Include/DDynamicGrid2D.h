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

#include <memory>

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
  using pointer         = std::allocator_traits<TAllocator>::pointer;
  using const_pointer   = std::allocator_traits<Allocator>::const_pointer;

  explicit DDynamicGrid2D(size_type x, size_type y);
  explicit DDynamicGrid2D(size_type x, size_type y, const TType& value);
  
  /// @brief 
  void SetRowSize(size_type rowSize);
  /// @brief
  void SetColumnSize(size_type colSize);

  /// @brief
  size_type GetRowSize() const noexcept;
  /// @brief
  size_type GetColumnSize() const noexcept;

private:
  

};