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

#include <Math/Type/Math/DMatrix4.h>

using namespace ::dy::math;

/// @struct DCbScale
/// @brief  Constant buffer 
struct DCbScale final
{
  alignas(16) float mScale;
};

/// @struct DCbObject
/// @brief Constant Buffer
struct DCbObject final
{
  DMatrix4<TReal> mModel;
};

/// @struct DCbViewProj
/// @brief Constant Buffer
struct DCbViewProj final
{
  DMatrix4<TReal> mView;
  DMatrix4<TReal> mProj;
};

static_assert(sizeof(DCbScale) % 16 == 0);
static_assert(sizeof(DCbViewProj) % 16 == 0);