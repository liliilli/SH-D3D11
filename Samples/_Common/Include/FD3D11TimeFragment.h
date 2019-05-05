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

class FD3D11TimeHandle;
struct ID3D11DeviceContext;
struct ID3D11Query;

class FD3D11TimeFragment final
{
public:
  FD3D11TimeFragment(
    FD3D11TimeHandle& parentHandle,
    ID3D11DeviceContext& deviceContext,
    ID3D11Query& startQuery,
    ID3D11Query& endQuery);
  ~FD3D11TimeFragment();
  
  FD3D11TimeFragment(const FD3D11TimeFragment&) = delete;
  FD3D11TimeFragment& operator=(const FD3D11TimeFragment&) = delete;

  FD3D11TimeFragment(FD3D11TimeFragment&&) noexcept;
  FD3D11TimeFragment& operator=(FD3D11TimeFragment&&) noexcept = delete;

private:
  bool mIsMoved = false;
  FD3D11TimeHandle& mContainer;
  ID3D11DeviceContext& mDC;
  ID3D11Query& mStartQuery;
  ID3D11Query& mEndQuery;
};