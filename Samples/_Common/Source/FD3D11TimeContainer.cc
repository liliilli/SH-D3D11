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

#include <FD3D11TimeContainer.h>

const FTimeContainer& 
FD3D11TimeContainer::operator[](const std::string& fragmentName) const noexcept
{
  return this->mFragments.at(fragmentName);
}

bool FD3D11TimeContainer::HasFragment(const std::string& fragmentName) const noexcept
{
  return this->mFragments.find(fragmentName) != this->mFragments.end();
}

void FD3D11TimeContainer::Insert(const FD3D11TimeHandle::TTimeStamps& elapsedTimes)
{
  for (auto& [fragmentName, timeStamp] : elapsedTimes)
  {
    if (this->mFragments.find(fragmentName) == this->mFragments.end())
    {
      this->mFragments[fragmentName] = FTimeContainer{};
    }

    this->mFragments[fragmentName].Insert(TTimeStamp{timeStamp / 1000.0});
  }
}
