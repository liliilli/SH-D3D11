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

#include <Profiling/FCpuTimeHandle.h>
#include <Profiling/FD3D11TimeHandle.h>
#include <Profiling/FTimeContainer.h>
#include <Profiling/FD3D11TimeContainer.h>
#include <Math/Common/XGlobalMacroes.h>

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

/// @def TIME_CHECK_CPU
/// @brief Fire CPU checking routine. Fire and forget.
#define TIME_CHECK_CPU(Name) \
  auto MATH_TOKEN_PASTE(_, __LINE__) = ::MTimeChecker::CheckCpuTime(Name)

/// @def TIME_CHECK_D3D11_STALL
/// @brief Fire GPU disjoint checking routine as D3D11.
#define TIME_CHECK_D3D11_STALL(Variable, Name, Disjoint, DeviceContext) \
  auto Variable = ::MTimeChecker::CheckGpuD3D11Time(Name, Disjoint, DeviceContext, false)

/// @def TIME_CHECK_FRAGMENT
/// @brief Fire GPU fragment gpu time checking routine by Disjoint Variable.
/// Fire and forget.
#define TIME_CHECK_FRAGMENT(Disjoint, Name, StartQuery, EndQuery) \
  auto MATH_TOKEN_PASTE(_, __LINE__) = Disjoint.CheckFragment( \
    Name, StartQuery, EndQuery)