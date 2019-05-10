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

#include <Resource/DD3D11Handle.h>
#include <Windows.h>

/// @class D11DefaultHandles
/// @brief Default handles that have handles of default device and swap-chain. \n
///
class D11DefaultHandles final
{
public:
  HWND                        mWHwnd;
  D11HandleDevice             mDevice = nullptr;
  D11SwapChainHandle          mSwapChain = nullptr;
  D11HandleRTV                mRTV = nullptr;
  D11HandleTexture2D          mDSTexture2D = nullptr;
  D11HandleDSV                mDSV = nullptr;
  D11HandleRasterState        mRasterState = nullptr;
  D11HandleDepthStencilState  mDepthStencilState = nullptr;
  D11HandleBlendState         mBlendState = nullptr;
};