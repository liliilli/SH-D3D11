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

#include <chrono>
#include <vector>

/// @brief Time stamp type.
using TTimeStamp = std::chrono::duration<double>;

/// @class FTimeContainer
/// @brief Time stamp container
class FTimeContainer final
{
public:
  /// @brief Get Lenght of elapsed second timer items.
  std::size_t Length() const noexcept;

  /// @brief Get recent time stamp. 
  /// If not exist, just return -1.
  TTimeStamp GetRecent() const noexcept;

  /// @brief Get average time stamp of list.
  /// If not exist, just return -1.
  TTimeStamp GetAverage() const noexcept;

  /// @brief Get time stamp of index. 
  /// this function does not check potential error.
  TTimeStamp operator[](std::size_t index) const noexcept;

  /// @brief Insert new elapsed time stamp into list.
  void Insert(const TTimeStamp& elapsedTime);

private:
  /// @brief
  static constexpr std::size_t kMax = 50;
  /// @brief List
  std::vector<TTimeStamp> mElapsedSeconds;
  /// @brief Present index of list cursor.
  std::size_t mPresentIndex = 0;
};

