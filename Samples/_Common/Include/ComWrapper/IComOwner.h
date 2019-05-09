#pragma once
///
/// MIT License
/// Copyright (c) 2019 Jongmin Yun
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include <type_traits>

template <typename TType>
class IComBorrow;

template <typename TType>
class IComOwner final
{
public:
  IComOwner(std::nullptr_t);
  explicit IComOwner(TType* pCOMInstance); 
  ~IComOwner();

  IComOwner(const IComOwner&) = delete;
  IComOwner& operator=(const IComOwner&) = delete;
  IComOwner(IComOwner&& movedOwner) noexcept;
  IComOwner& operator=(IComOwner&& movedOwner) noexcept;

  /// @brief Check this instance is valid (owns COM instance)
  [[nodiscard]] bool IsValid() const noexcept;

  TType** operator&()
  {
    return &this->mPtrOwner;
  }

  /// @brief Get layered address of COM instance pointer.
  /// This function should be used carefully.
  TType** GetAddressOf() noexcept;

  /// @brief Get Borrow type of COM Owner.
  IComBorrow<TType> GetBorrow() noexcept;

  /// @brief Get pointer of COM instance pointer.
  /// This function should be used carefully.
  TType* GetPtr() noexcept;

  /// @brief Get reference of COM instance ptr.
  /// This does not check validity.
  TType& operator*();

  /// @brief Get pointer of COM instance.
  /// This does not check validity.
  TType* operator->();

  /// @brief Try release COM instance if COM is bound to wrapping type.
  void Release();

private:
  /// @brief Release (Decrease reference count of COM instance) 
  /// when COM instance is exist.
  void TryReleaseSelf();

  /// @brief Actual COM instance pointer.
  TType* mPtrOwner = nullptr;
};

#include <Inline/IComOwner.inl>
