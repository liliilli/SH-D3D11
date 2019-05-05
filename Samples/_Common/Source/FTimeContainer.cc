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

#include <FTimeContainer.h>

std::size_t FTimeContainer::Length() const noexcept
{
  return this->mElapsedSeconds.size();
}

TTimeStamp FTimeContainer::GetRecent() const noexcept
{
  if (this->Length() == 0)
  {
    return TTimeStamp(-1.0);
  }

  if (this->Length() == kMax)
  {
    if (this->mPresentIndex == 0) { return this->mElapsedSeconds[kMax - 1]; }
    else                          { return this->mElapsedSeconds[this->mPresentIndex - 1]; }
  }
  else
  {
    return this->mElapsedSeconds.back();
  }
}

TTimeStamp FTimeContainer::GetAverage() const noexcept
{
  if (this->Length() == 0)
  {
    return TTimeStamp(-1.0);
  }

  auto sum = TTimeStamp(0);
  for (const auto& elapsedSecond : this->mElapsedSeconds)
  {
    sum += elapsedSecond;
  }

  return sum / this->Length();
}

TTimeStamp FTimeContainer::operator[](std::size_t index) const noexcept
{
  return this->mElapsedSeconds[index];
}

void FTimeContainer::Insert(const TTimeStamp& elapsedTime)
{
  if (this->Length() == kMax)
  {
    this->mElapsedSeconds[this->mPresentIndex] = elapsedTime;
  }
  else
  {
    this->mElapsedSeconds.emplace_back(elapsedTime);
  }

  this->mPresentIndex = (this->mPresentIndex + 1) % kMax;
}
