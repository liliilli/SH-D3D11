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

#include <Profiling/FD3D11TimeHandle.h>
#include <Profiling/FTimeContainer.h>

/// @brief Time stamp type.
using TTimeStamp = std::chrono::duration<double>;

/// @class FD3D11TimeContainer
/// @brief Time stamp container
class FD3D11TimeContainer final
{
public:
  /// @brief
  const FTimeContainer& operator[](const std::string& fragmentName) const noexcept;

  /// @brief
  [[nodiscard]] bool HasFragment(const std::string& fragmentName) const noexcept;

  /// @brief Insert new elapsed time stamp into list.
  void Insert(const FD3D11TimeHandle::TTimeStamps& elapsedTimes);

private:
  std::unordered_map<std::string, FTimeContainer> mFragments;
};

