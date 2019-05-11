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

#include <XBuffer.h>

inline const std::array<DVertex, 3> sBoxVertices =
{ 
  DVertex
  { {-1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f, 1.0f} },
  { {+1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 1.0f, 1.0f} },
  { {+0.0f, +1.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f} },
};

// Create Index Buffer.
inline const std::array<unsigned, 3> sBoxIndices =
{ // 1        2        3        4
  0, 1, 2, 
};
