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

#include <memory>
#include <type_traits>
#include <ComWrapper/XComCounter.h>

template <typename TType>
class IComBorrow;

template <typename TType>
class __IComOwner;

/// @class IComOwner
/// @brief
template <typename TType>
class [[nodiscard]] IComOwner final
{
public:
  using Type = TType;
  using TPtrType = TType*;

  IComOwner(std::nullptr_t);
  explicit IComOwner(TType* pCOMInstance); 
  ~IComOwner();

  IComOwner(const IComOwner&) = delete;
  IComOwner& operator=(const IComOwner&) = delete;
  IComOwner(IComOwner&& movedOwner) noexcept;
  IComOwner& operator=(IComOwner&& movedOwner) noexcept;

  /// @brief Check this instance is valid (owns COM instance)
  [[nodiscard]] bool IsValid() const noexcept;

#if 0
  /// @brief Get layered address of COM instance pointer.
  /// This function should be used carefully.
  TType** GetAddressOf() noexcept;
#endif

  /// @brief Get Borrow type of COM Owner.
  IComBorrow<TType> GetBorrow() noexcept;

  /// @brief Get pointer of COM instance pointer.
  /// This function should be used carefully.
  TType* GetPtr() noexcept;

  /// @brief Try release COM instance if COM is bound to wrapping type.
  void Release();

  /// @brief Get reference of COM instance ptr.
  /// This does not check validity.
  TType& operator*();

  /// @brief Get pointer of COM instance.
  /// This does not check validity.
  TType* operator->();

private:
  /// @brief Release (Decrease reference count of COM instance) 
  /// when COM instance is exist.
  void TryReleaseSelf();

  //TType* mPtrOwner = nullptr;
  __IComOwner<TType>* mObj = nullptr;
  //std::unique_ptr<XComCounter<TType>> mCounter = nullptr;

  static_assert(
    std::is_base_of_v<IUnknown, TType>,
    "TType must be derived from IUnknown.");
};

template <typename TType>
class __IComOwner final
{
public:
  __IComOwner(TType* pComInstance) : mPtrOwner{pComInstance} {};

  __IComOwner(const __IComOwner&) = delete;
  __IComOwner& operator=(const __IComOwner&) = delete;
  __IComOwner(__IComOwner&&) noexcept = delete;
  __IComOwner& operator=(__IComOwner&&) noexcept = delete;

  /// @brief Actual COM instance pointer.
  TType* mPtrOwner = nullptr;
  /// @brief Counter. life-scope is __IComOwner's cosntruction and destruction.
  XComCounter<TType> mCounter;
};
#include <Inline/IComOwner.inl>
