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
#include <FD3D11TimeHandle.h>
#include <FTimeContainer.h>
#include <FD3D11TimeContainer.h>

struct ID3D11Query;
struct ID3D11DeviceContext;

/// @class MTimeChecker
/// @brief Static time checker class 
class MTimeChecker final
{
public:
  using TCpuContainer = std::unordered_map<std::string, std::unique_ptr<FTimeContainer>>;
  using TD3D11Container = std::unordered_map<std::string, std::unique_ptr<FD3D11TimeContainer>>;

  /// @brief Check CPU time.
  [[nodiscard]] static FCpuTimeHandle CheckCpuTime(const std::string& tagName);

  /// @brief Check D3D11 GPU time.
  [[nodiscard]] static FD3D11TimeHandle CheckGpuD3D11Time(
    const std::string& tagName,
    ID3D11Query& disjointQuery,
    ID3D11DeviceContext& deviceContext,
    bool isUpdateDeferred);

  /// @brief Get CPU time container of tag name.
  /// If not exist, just throw error.
  static const FTimeContainer& Get(const std::string& tagName);

  /// @brief Get D3D11 time container of tag name.
  /// If not exist, just throw error.
  static const FD3D11TimeContainer& GetGpuD3D11(const std::string& tagName);

private:
  /// @brief CPU Timer container.
  static TCpuContainer mTimerContainer;

  /// @brief GPU D3D11 Timer container.
  static TD3D11Container mD3D11TimeContainer;
};