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

#include <Profiling/FCpuTimeHandle.h>
#include <Profiling/FTimeContainer.h>

FCpuTimeHandle::FCpuTimeHandle(FTimeContainer& container)
  : mContainer{container},
    mStart{std::chrono::steady_clock::now()}
{ }

FCpuTimeHandle::~FCpuTimeHandle()
{
  if (this->mIsMoved == false)
  {
    this->mEnd = std::chrono::steady_clock::now();
    TTimeStamp duration = this->mEnd - this->mStart;

    this->mContainer.Insert(duration);
  }
}

FCpuTimeHandle::FCpuTimeHandle(FCpuTimeHandle&& handle) noexcept
  : mIsMoved{handle.mIsMoved},
    mContainer{handle.mContainer},
    mStart{handle.mStart}
{ }
