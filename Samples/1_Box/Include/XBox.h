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

#include <XCommon.h>

inline std::array<DVertex, 8> vertices =
{ 
  DVertex
  { {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
  { {+1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f} },
  { {+1.0f, -1.0f, +1.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
  { {-1.0f, -1.0f, +1.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },

  { {-1.0f, +1.0f, -1.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
  { {+1.0f, +1.0f, -1.0f}, {1.0f, 1.0f, 0.0f, 1.0f} },
  { {+1.0f, +1.0f, +1.0f}, {1.0f, 0.0f, 1.0f, 1.0f} },
  { {-1.0f, +1.0f, +1.0f}, {0.0f, 1.0f, 1.0f, 1.0f} },
};

// Create Index Buffer.
inline std::array<unsigned, 36> indices =
{ // 1        2        3        4
  0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7,
  1, 2, 6, 1, 6, 5, 3, 0, 4, 3, 4, 7,
  0, 1, 5, 0, 5, 4, 2, 6, 7, 2, 7, 3,
};
