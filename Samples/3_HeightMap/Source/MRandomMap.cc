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

#include <MRandomMap.h>
#include <cmath>
#include <Math/Utility/XRandom.h>
#include <Math/Utility/XLinearMath.h>
#include <Expr/TZip.h>

void MRandomMap::TempMake()
{
  // Get random gradient value.
  DDynamicGrid2D<DVector2<TReal>> gradientMap = {9, 9};
  for (auto& row : gradientMap)
  {
    for (auto& item : row)
    {
      item = RandomVector2Length<TReal>(1.0f);
    }
  }

  // Get height value of center-point of each grid cell.
  // Make center-point map.
  DDynamicGrid2D<DVector2<TReal>> centerPointMap = {8, 8};
  for (std::size_t y = 0, ySize = centerPointMap.GetRowSize(); y < ySize; ++y)
  {
    for (std::size_t x = 0, xSize = centerPointMap.GetColumnSize(); x < xSize; ++x)
    {
      centerPointMap.Set(x, y, {x + 0.5f, y + 0.5f});
    }
  }

  // Get Height map from center-point map using linear interpolation.
  auto DotGridGradient = [&](const DVector2<TReal>& centerPoint, const DVector2<TI32>& index) -> TReal
  {
    const DVector2<TReal> delta = centerPoint - index;
    return Dot(gradientMap.Get(index.X, index.Y), delta);
  };

  // Calculate heights
  for (std::size_t y = 0, ySize = mHeightMap2.GetRowSize(); y < ySize; ++y)
  {
    for (std::size_t x = 0, xSize = mHeightMap2.GetColumnSize(); x < xSize; ++x)
    {
      // Get values 
      const auto& centerPoint = centerPointMap.Get(x, y);
      const DVector2<TI32> _0 = {static_cast<TI32>(centerPoint.X), static_cast<TI32>(centerPoint.Y)};
      const DVector2<TI32> _1 = _0 + DVector2<TI32>{1};
      const DVector2<TReal> weight = centerPoint - _0;

      // Get node's point.
      const TReal n00 = DotGridGradient(centerPoint, _0);
      const TReal n10 = DotGridGradient(centerPoint, {_1.X, _0.Y});
      const TReal x0 = Lerp(n00, n10, weight.X);

      const TReal n01 = DotGridGradient(centerPoint, {_0.X, _1.Y});
      const TReal n11 = DotGridGradient(centerPoint, _1);
      const TReal x1 = Lerp(n01, n11, weight.X);

      mHeightMap2.Set(x, y, Lerp(x0, x1, weight.Y));
    }
  }

  // Set buffer with height-map.
  for (std::size_t y = 0; y < mVertexBuffer2.GetRowSize(); ++y)
  {
    for (std::size_t x = 0; x < mVertexBuffer2.GetColumnSize(); ++x)
    {
      mVertexBuffer2.Set(
        x, y, 
        DVector3<TReal>{static_cast<TReal>(x), static_cast<TReal>(y), mHeightMap2.Get(x, y)}
      );
    }
  }

  // Set indice buffer index.
  const auto rowLen = 8;
  for (std::size_t y = 0; y < 7; ++y)
  {
    for (std::size_t x = 0; x < 7; ++x)
    {
      const unsigned i = (unsigned)(x + y * rowLen);
      mIndiceBuffer.emplace_back(i);
      mIndiceBuffer.emplace_back(i + rowLen);
      mIndiceBuffer.emplace_back(i + 1);

      mIndiceBuffer.emplace_back(i + 1);
      mIndiceBuffer.emplace_back(i + rowLen);
      mIndiceBuffer.emplace_back(i + rowLen + 1);
    }
  }
}

DDynamicGrid2D<float> MRandomMap::mHeightMap2 = {8, 8};
DDynamicGrid2D<DVector3<TReal>> MRandomMap::mVertexBuffer2 = {8, 8};
std::vector<unsigned> MRandomMap::mIndiceBuffer;
