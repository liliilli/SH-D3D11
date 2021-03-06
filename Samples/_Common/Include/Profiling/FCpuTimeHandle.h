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

class FTimeContainer;

class FCpuTimeHandle final
{
public:
  FCpuTimeHandle(FTimeContainer& container);
  ~FCpuTimeHandle();

  FCpuTimeHandle(const FCpuTimeHandle&) = delete;
  FCpuTimeHandle& operator=(const FCpuTimeHandle&) = delete;

  FCpuTimeHandle(FCpuTimeHandle&&) noexcept;
  FCpuTimeHandle& operator=(FCpuTimeHandle&&) noexcept = delete;

private:
  bool mIsMoved = false;
  FTimeContainer& mContainer;

  using TStamp = decltype(std::chrono::steady_clock::now());
  TStamp mStart;
  TStamp mEnd;
};