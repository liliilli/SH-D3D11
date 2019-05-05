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

#include <MTimeChecker.h>

MTimeChecker::TCpuContainer   MTimeChecker::mTimerContainer;
MTimeChecker::TD3D11Container MTimeChecker::mD3D11TimeContainer;

FCpuTimeHandle MTimeChecker::CheckCpuTime(const std::string& tagName)
{
  if (mTimerContainer.find(tagName) == mTimerContainer.end())
  {
    mTimerContainer[tagName] = std::make_unique<FTimeContainer>();
  }

  return std::move(FCpuTimeHandle{*mTimerContainer[tagName]});
}

FD3D11TimeHandle MTimeChecker::CheckGpuD3D11Time(
  const std::string& tagName,
  ID3D11Query& disjointQuery,
  ID3D11DeviceContext& deviceContext,
  bool isUpdateDeferred)
{
  if (mTimerContainer.find(tagName) == mTimerContainer.end())
  {
    mD3D11TimeContainer[tagName] = std::make_unique<FD3D11TimeContainer>();
  }

  return std::move(
    FD3D11TimeHandle{*mD3D11TimeContainer[tagName], disjointQuery, deviceContext, isUpdateDeferred}
  );
}

const FTimeContainer& MTimeChecker::Get(const std::string& tagName)
{
  return *mTimerContainer.at(tagName);
}

const FD3D11TimeContainer& MTimeChecker::GetGpuD3D11(const std::string& tagName)
{
  return *mD3D11TimeContainer.at(tagName);
}