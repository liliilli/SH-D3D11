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

#include <array>
#include <Math/Type/Math/DVector3.h>
#include <Math/Type/Math/DVector4.h>

// * Build Geometry Buffers.
using dy::math::DVector3;
using dy::math::DVector4;
using dy::math::TReal;

// Create Vertex Buffer.
struct DVertex final
{
  DVector3<TReal> mPos;
  DVector4<TReal> mCol;
};

using TColor = std::array<float, 4>;
inline const std::array<TColor, 4> colors =
{
  TColor
  {0, 0, 1, 1}, {1, 0, 1, 1}, {1, 1, 0, 1}, {1, 0, 0, 1},
};
