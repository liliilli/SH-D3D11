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

#include <optional>
#include <DWindowHandle.h>

struct ID3D11Device;
struct ID3D11DeviceContext;

/// @brief 
/// @param titleName
/// @param width
/// @param height
/// @return
std::optional<dy::DWindowHandle>
CreateMainWindow(const std::string& titleName, unsigned width, unsigned height);

/// @brief
void SetupGuiSettings(dy::DWindowHandle& wHandle, ID3D11Device& device, ID3D11DeviceContext& dc);