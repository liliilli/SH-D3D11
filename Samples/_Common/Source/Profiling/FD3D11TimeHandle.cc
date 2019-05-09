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

#include <Profiling/FD3D11TimeHandle.h>

#include <cassert>
#include <D3D11.h>
#include <Profiling/FD3D11TimeContainer.h>

FD3D11TimeHandle::FD3D11TimeHandle(
  FD3D11TimeContainer& container,
  ID3D11Query& disjointQuery,
  ID3D11DeviceContext& deviceContext,
  bool isUpdateDeferred)
  : mIsUpdateDeferred{isUpdateDeferred},
    mContainer{container},
    mDC{deviceContext},
    mDisjointQuery{disjointQuery}
{
  this->mDC.Begin(&this->mDisjointQuery);
}

FD3D11TimeHandle::~FD3D11TimeHandle()
{
  if (this->mIsMoved == false)
  {
    // If update should be deferred, 
    if (this->mIsUpdateDeferred == true)
    {
      assert(false);
      return;
    }

    // If not, end disjoint query and stall thread to get time stamps.
    this->mDC.End(&this->mDisjointQuery);
    const auto results = this->CalculateTimestamps(this->mDC, this->mDisjointQuery, this->mTimeFragments);

    // Insert results into container.
    this->mContainer.Insert(results);
  }
}

FD3D11TimeHandle::FD3D11TimeHandle(FD3D11TimeHandle&& handle) noexcept
  : mIsMoved{handle.mIsMoved},
    mIsUpdateDeferred{handle.mIsUpdateDeferred},
    mContainer{handle.mContainer},
    mDC{handle.mDC},
    mDisjointQuery{handle.mDisjointQuery}
{ 
  handle.mIsMoved = true;
}

FD3D11TimeFragment FD3D11TimeHandle::CheckFragment(
  const std::string& fragmentName,
  ID3D11Query& startQuery,
  ID3D11Query& endQuery)
{
  if (this->mTimeFragments.find(fragmentName) == this->mTimeFragments.end())
  {
    this->mTimeFragments.try_emplace(fragmentName, &startQuery, &endQuery);
  }

  return std::move(
    FD3D11TimeFragment{*this, this->mDC, startQuery, endQuery}
  );
}

std::unordered_map<std::string, double>
FD3D11TimeHandle::CalculateTimestamps(
  ID3D11DeviceContext& dc, 
  ID3D11Query& disjoint, 
  TTimeFragments& fragments)
{
  std::unordered_map<std::string, double> results;

  // Stall until disjointQuery is available.
  while (dc.GetData(&disjoint, nullptr, 0, 0) == S_FALSE)
    ;

  // Check whether timestamps were disjoint during the last frame
  D3D11_QUERY_DATA_TIMESTAMP_DISJOINT tsDisjoint;
  dc.GetData(&disjoint, &tsDisjoint, sizeof(tsDisjoint), 0);
  if (tsDisjoint.Disjoint)
  {
    return results;
  }

  for (auto& [fragmentName, pair] : fragments)
  {
    auto& [start, end] = pair;

    // Get All the timestamps.
    UINT64 tsBeginFrame, tsEndFrame;
    dc.GetData(start, &tsBeginFrame, sizeof(UINT64), 0);
    dc.GetData(end, &tsEndFrame, sizeof(UINT64), 0);

    // Convert to real time (ms).
    const auto msGpuFrame = 
        double(tsEndFrame - tsBeginFrame) / double(tsDisjoint.Frequency) 
      * 1'000.0;
    results.try_emplace(fragmentName, msGpuFrame);
  }

  return results;
}
