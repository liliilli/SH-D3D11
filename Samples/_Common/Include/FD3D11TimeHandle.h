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
#include <string>
#include <unordered_map>
#include <FD3D11TimeFragment.h>

class FD3D11TimeContainer;
struct ID3D11Query;
struct ID3D11DeviceContext;

class FD3D11TimeHandle final
{
public:
  using TFragmentPair = std::pair<ID3D11Query*, ID3D11Query*>;
  using TTimeFragments = std::unordered_map<std::string, TFragmentPair>;
  using TTimeStamps = std::unordered_map<std::string, double>;

  FD3D11TimeHandle(
    FD3D11TimeContainer& container,
    ID3D11Query& disjointQuery,
    ID3D11DeviceContext& deviceContext,
    bool isUpdateDeferred);
  ~FD3D11TimeHandle();

  FD3D11TimeHandle(const FD3D11TimeHandle&) = delete;
  FD3D11TimeHandle& operator=(const FD3D11TimeHandle&) = delete;

  FD3D11TimeHandle(FD3D11TimeHandle&&) noexcept;
  FD3D11TimeHandle& operator=(FD3D11TimeHandle&&) noexcept = delete;

  /// @brief
  [[nodiscard]] FD3D11TimeFragment CheckFragment(
    const std::string& fragmentName,
    ID3D11Query& startQuery,
    ID3D11Query& endQuery);

private:
  /// @brief This function does not check every fragment query had been done.
  std::unordered_map<std::string, double>
  CalculateTimestamps(ID3D11DeviceContext& dc, ID3D11Query& disjoint, TTimeFragments& fragments);
  
  bool mIsMoved = false;
  bool mIsUpdateDeferred = false;
  FD3D11TimeContainer& mContainer;
  ID3D11DeviceContext& mDC;
  ID3D11Query& mDisjointQuery;

  TTimeFragments mTimeFragments;
};