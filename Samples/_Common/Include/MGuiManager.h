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
#include <cassert>
#include <IGuiFrame.h>
#include <IGuiModel.h>

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
   
  /// @brief Initialize manager.
  static void Initialize(
    const std::function<void(void)>& initFunc, 
    const std::function<void(void)>& shutFunc);

  /// @brief Update manager.
  static void Update();

  /// @brief Shutdown manager.
  static void Shutdown();

  /// @brief Create GUI with IGuiFrame derived type and guiName.
  /// If there is already exist with guiName in container, do nothing but return nullptr.
  /// Otherwise (If successful) return created gui instance pointer.
  template <typename TType, typename... TArgs>
  static TType* CreateGui(const std::string& guiName, TArgs&&... args);

  /// @brief Get GUI instance pointer with guiName.
  /// If item is not exist, just return nullptr. 
  static IGuiFrame* GetGui(const std::string& guiName);

  /// @brief Try to remove GUI instance with guiName.
  /// If item is not exist, just do nothing but return false.
  /// If successful, return true.
  static bool RemoveGui(const std::string& guiName);

  /// @brief Try to remove GUI instance with guiName.
  /// This function must be succeeded, or undefined error.
  static bool RemoveGui(IGuiFrame& guiInstance);

  /// @brief Render visible gui list.
  /// Pre-render and Post-render callback function will be called when exist.
  static void Render();

  /// @brief Create shared model with IGuiModel derived type and modelName.
  /// If there is already exist with modelName in container, do nothing but return nullptr.
  /// Otherwise (If successful) return created shared model instance pointer.
  template <typename TType, typename... TArgs>
  static TType* CreateSharedModel(const std::string& modelName, TArgs&&... args);

  /// @brief Check there is a model with modelName.
  /// This function should be called before call `GetSharedModel` function to avoid error.
  [[nodiscard]] static bool HasSharedModel(const std::string& modelName);

  /// @brief Get reference of shared model with modelName.
  /// If not found, do hard undefined behaviour.
  static IGuiModel& GetSharedModel(const std::string& modelName);

  /// @brief Try to remove shared model with modelName.
  /// If not found, just do nothing but return false. Otherwise, return true.
  static bool RemoveSharedModel(const std::string& modelName);

private:
  static std::function<void(void)> mInitCallback;
  static std::function<void(void)> mShutCallback;
  static void(*mPreRenderCallback)(void);
  static void(*mPostRenderCallback)(void);

  /// @brief
  static std::unordered_map<std::string, std::unique_ptr<IGuiFrame>> mGuis;
  /// @brief 
  static std::vector<std::unique_ptr<IGuiFrame>> mRemovedGuis;
  /// @brief
  static std::unordered_map<std::string, std::unique_ptr<IGuiModel>> mSharedModels;
};
#include <MGuiManager.inl>