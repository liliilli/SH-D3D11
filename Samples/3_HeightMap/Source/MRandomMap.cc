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

namespace
{

std::vector<std::pair<DVector2<TI32>, DVector2<TReal>>>
CalculateOffset(std::size_t fragmentNum);

/// @brief Create Gradient 2D grid map with given grid count.
DDynamicGrid2D<DVector2<TReal>> 
Create2DGradientMap(const std::array<int, 2>& grid)
{
  DDynamicGrid2D<DVector2<TReal>> gradientMap = 
    { std::size_t(grid[0] + 1), std::size_t(grid[1] + 1) };

  for (auto& row : gradientMap)
  {
    for (auto& item : row)
    {
      item = RandomVector2Length<TReal>(1.0f);
    }
  }  

  return std::move(gradientMap);
}

/// @brief
DDynamicGrid2D<DVector2<TReal>>
Create2DCenterPointMap(
  const DDynamicGrid2D<DVector2<TReal>>& gradient, 
  const std::array<int, 2>& grid, std::size_t fragment)
{
  DDynamicGrid2D<DVector2<TReal>> centerPointMap = 
  { 
    std::size_t(grid[0] * fragment), 
    std::size_t(grid[1] * fragment) 
  };

  const auto col = gradient.GetColumnSize() - 1;
  const auto row = gradient.GetRowSize() - 1;
  for (std::size_t y = 0; y < row; ++y)
  {
    const auto iY = fragment * y;
    for (std::size_t x = 0; x < col; ++x)
    {
      const auto iX = fragment * x;
      for (const auto& [pos, offset] : CalculateOffset(fragment))
      {
        centerPointMap[iY + pos.Y][iX + pos.X] = DVector2<TI32>{TI32(x), TI32(y)} + offset;
      }
    }
  }

  return std::move(centerPointMap);
}

/// @brief Calculate offset of each vertex that has height on each grid.
/// Result vector has a pair of relative index and offset item.
std::vector<std::pair<DVector2<TI32>, DVector2<TReal>>>
CalculateOffset(std::size_t fragmentNum)
{
  const TReal edgeOffset    = 1 / static_cast<TReal>(fragmentNum * 2);
  const TReal centerOffset  = 1 / static_cast<TReal>(fragmentNum);

  std::vector<std::pair<DVector2<TI32>, DVector2<TReal>>> result = {};
  TReal yPos = 0.0f;
  for (decltype(fragmentNum) y = 0; y < fragmentNum; ++y)
  {
    if (y == 0) { yPos += edgeOffset; } else { yPos += centerOffset; }
    TReal xPos = 0.0f;
    for (decltype(fragmentNum) x = 0; x < fragmentNum; ++x)
    {
      if (x == 0) { xPos += edgeOffset; } else { xPos += centerOffset; }
      result.emplace_back(
        DVector2<TI32>{(TI32)x, (TI32)y}, 
        DVector2<TReal>{xPos, yPos}
      );
    }
  }

  return result;
}

}

void MRandomMap::MakeMap(const std::array<int, 2>& grid, std::size_t fragment)
{
  // Get random gradient value.
  auto gradientMap = Create2DGradientMap(grid);

  // Get height value of center-point of each grid cell. Make center-point map.
  DDynamicGrid2D<DVector2<TReal>> centerPointMap = Create2DCenterPointMap(gradientMap, grid, fragment);
  
  // Get Height map from center-point map using linear interpolation.
  auto DotGridGradient = [&](const DVector2<TReal>& centerPoint, const DVector2<TI32>& index) -> TReal
  {
    const DVector2<TReal> delta = centerPoint - index;
    return Dot(gradientMap.Get(index.X, index.Y), delta);
  };

  // Calculate heights
  mHeightMap2.Resize(centerPointMap.GetColumnSize(), centerPointMap.GetRowSize());
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
  mVertexBuffer2.Resize(mHeightMap2.GetColumnSize(), mHeightMap2.GetRowSize());
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
  const auto rowLen = static_cast<TU32>(mVertexBuffer2.GetColumnSize());
  for (std::size_t y = 0; y < mVertexBuffer2.GetRowSize() - 1; ++y)
  {
    for (std::size_t x = 0; x < mVertexBuffer2.GetColumnSize() - 1; ++x)
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

DDynamicGrid2D<float> MRandomMap::mHeightMap2 = {8 * 2, 8 * 2};
DDynamicGrid2D<DVector3<TReal>> MRandomMap::mVertexBuffer2 = {8 * 2, 8 * 2};
std::vector<unsigned> MRandomMap::mIndiceBuffer;
