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

using namespace ::dy::math;

/// @class MRandomMap
/// @brief Random map orginizer.
class MRandomMap final
{
public:
  static void TempMake();

  static const auto& TempGetVertexBuffer()
  {
    return mVertexBuffer;
  }

  static const auto& TempGetIndiceBuffer()
  {
    return mIndiceBuffer;
  }

private:
  static std::array<std::array<float, 8>, 8> mHeightMap;
  static std::array<std::array<DVector3<TReal>, 8>, 8> mVertexBuffer;
  static std::vector<unsigned> mIndiceBuffer;
};