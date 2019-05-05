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

std::unordered_map<std::string, std::unique_ptr<FTimeContainer>> 
MTimeChecker::mTimerContainer;

FCpuTimeHandle MTimeChecker::CheckCpuTime(const std::string& tagName)
{
  if (mTimerContainer.find(tagName) == mTimerContainer.end())
  {
    mTimerContainer[tagName] = std::make_unique<FTimeContainer>();
  }

  return std::move(FCpuTimeHandle{*mTimerContainer[tagName]});
}

const FTimeContainer& MTimeChecker::Get(const std::string& tagName)
{
  return *mTimerContainer.at(tagName);
}
