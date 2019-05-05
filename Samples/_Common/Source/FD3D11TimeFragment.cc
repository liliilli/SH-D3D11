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

#include <FD3D11TimeFragment.h>
#include <D3D11.h>

FD3D11TimeFragment::FD3D11TimeFragment(
  FD3D11TimeHandle& parentHandle,
  ID3D11DeviceContext& deviceContext,
  ID3D11Query& startQuery,
  ID3D11Query& endQuery)
  : mContainer{parentHandle},
    mDC{deviceContext},
    mStartQuery{startQuery},
    mEndQuery{endQuery}
{
  this->mDC.End(&this->mStartQuery);
}

FD3D11TimeFragment::~FD3D11TimeFragment()
{
  if (this->mIsMoved == false)
  {
    this->mDC.End(&this->mEndQuery);
  }
}

FD3D11TimeFragment::FD3D11TimeFragment(FD3D11TimeFragment&& handle) noexcept
  : mIsMoved{handle.mIsMoved},
    mContainer{handle.mContainer},
    mDC{handle.mDC},
    mStartQuery{handle.mStartQuery},
    mEndQuery{handle.mEndQuery}
{
  handle.mIsMoved = true;
} 