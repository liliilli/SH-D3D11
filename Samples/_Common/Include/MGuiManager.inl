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

template <typename TType>
IGuiFrame* MGuiManager::CreateGui(const std::string& guiName)
{
  static_assert(
    std::is_base_of_v<IGuiFrame, TType>, 
    "TType is not derived from IGuiFrame.");

  if (mGuis.find(guiName) != mGuis.end())
  {
    return nullptr;
  }

  auto [it, isSucceeded] = mGuis.try_emplace(guiName, std::make_unique<TType>());
  assert(isSucceeded == true);

  return it->second.get();
}