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
#include <vector>
#include <Math/Type/Math/DVector3.h>
#include <DDynamicGrid2D.h>

using namespace ::dy::math;

/// @class MRandomMap
/// @brief Random map orginizer.
class MRandomMap final
{
public:
  static void MakeMap(const std::array<int, 2>& grid, std::size_t fragment);

  static const auto& TempGetVertexBuffer()
  {
    return mVertexBuffer2;
  }

  static const auto& TempGetIndiceBuffer()
  {
    return mIndiceBuffer;
  }

private:
  static DDynamicGrid2D<float> mHeightMap2;
  static DDynamicGrid2D<DVector3<TReal>> mVertexBuffer2;
  static std::vector<unsigned> mIndiceBuffer;
};