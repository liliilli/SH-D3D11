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

#include <functional>
#include <string>
#include <IGuiFrame.h>

/// @class MGuiManager
/// @brief wrapped ImGui OOP management type.
class MGuiManager final
{
public:
  /// @brief Set render callbacks
  static void SetRenderCallbacks(
    void(*preRenderCallback)(void),
    void(*postRenderCallback)(void)
  );

  /// @brief 
  static void Initialize(
    const std::function<void(void)>& initFunc, 
    const std::function<void(void)>& shutFunc);

  /// @brief
  static void Shutdown();

  /// @brief Create GUI with IGuiFrame derived type and guiName.
  /// If there is already exist with guiName in container, do nothing but return nullptr.
  /// Otherwise (If successful) return created gui instance pointer.
  template <typename TType, typename... TArgs>
  static IGuiFrame* CreateGui(const std::string& guiName, TArgs&&... args);

  /// @brief Get GUI instance pointer with guiName.
  /// If item is not exist, just return nullptr. 
  static IGuiFrame* GetGui(const std::string& guiName);

  /// @brief Try to remove GUI instance with guiName.
  /// If item is not exist, just do nothing but return false.
  /// If successful, return true.
  static bool RemoveGui(const std::string& guiName);

  /// @brief Render visible gui list.
  /// Pre-render and Post-render callback function will be called when exist.
  static void Render();

private:
  inline static std::function<void(void)> mInitCallback = nullptr;
  inline static std::function<void(void)> mShutCallback = nullptr;
  inline static void(*mPreRenderCallback)(void)   = nullptr;
  inline static void(*mPostRenderCallback)(void)  = nullptr;

  /// @brief
  inline static std::unordered_map<std::string, std::unique_ptr<IGuiFrame>> mGuis;
};
#include <MGuiManager.inl>