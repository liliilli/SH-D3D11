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

#include <string>
#include <unordered_map>
#include <memory>

#include <FCpuTimeHandle.h>
#include <FTimeContainer.h>

/// @class MTimeChecker
/// @brief Static time checker class 
class MTimeChecker final
{
public:
  /// @brief Check CPU time.
  [[nodiscard]] static FCpuTimeHandle CheckCpuTime(const std::string& tagName);

  /// @brief Check D3D11 GPU time.


  /// @brief Get time container of tag name.
  /// If not exist, just throw error.
  static const FTimeContainer& Get(const std::string& tagName);

private:
  /// @brief Timer container.
  static std::unordered_map<std::string, std::unique_ptr<FTimeContainer>> mTimerContainer;
};