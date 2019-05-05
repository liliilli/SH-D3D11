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

#include <FD3D11Factory.h>

#include <cassert>
#include <filesystem>
#include <d3dcompiler.h>

#include <StringUtil/XUtility.h>
#include <APlatformBase.h>
#include <HelperMacro.h>

std::optional<std::pair<IComOwner<ID3D11Device>, IComOwner<ID3D11DeviceContext>>> 
FD3D11Factory::CreateD3D11Device(dy::APlatformBase& platform)
{
  IComOwner<ID3D11Device>         mD3DDevice            = nullptr;
  IComOwner<ID3D11DeviceContext>  mD3DImmediateContext  = nullptr;
  D3D_FEATURE_LEVEL               featureLevel;
  
  // Crate D3D11 Device & Context
  UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  // Make device. (physical? logical?)
  HRESULT hr = D3D11CreateDevice(
    nullptr,              // Default Adapter (Primary)
    D3D_DRIVER_TYPE_HARDWARE, // Use hardware driver (Most optimal) 
    nullptr,              // No Software device because we use TYPE_HARDWARE (D3D11).
    createDeviceFlags,    // Set flags (DEBUG, SINGLETHREAD etc...)
    nullptr,              // If no flag is exist, just pick up the highest version of SDK.
    0,                    // Above argument brings the array of D3D_FEATURE, so we set it to 0 as nullptr. 
    D3D11_SDK_VERSION,    // Always specify this.
    &mD3DDevice,          // Output
    &featureLevel,        // Output
    &mD3DImmediateContext // Output
  );
  
  // Error checking.
  if (FAILED(hr))
  {
    platform.GetDebugManager().OnAssertionFailed(
      "D3D11CreateDevice Failed.\n", __FUNCTION__, __FILE__, __LINE__
    );
    return std::nullopt;
  }
  if (featureLevel != D3D_FEATURE_LEVEL_11_0)
  {
    platform.GetDebugManager().OnAssertionFailed(
      "Direct3D Feature Level 11 unsupported.\n", __FUNCTION__, __FILE__, __LINE__
    );
    return std::nullopt;
  }

  return std::pair{std::move(mD3DDevice), std::move(mD3DImmediateContext)};
}

HRESULT 
FD3D11Factory::CompileShaderFromFile(
  dy::APlatformBase& platform,
  const std::string& szFilePath, 
  LPCSTR szEntryPoint, 
  LPCSTR szShaderModel, 
  ID3DBlob** ppBlobOut)
{
  DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
  // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
  // Setting this flag improves the shader debugging experience, but still allows 
  // the shaders to be optimized and to run exactly the way they will run in 
  // the release configuration of this program.
  dwShaderFlags |= D3DCOMPILE_DEBUG;
  // Disable optimizations to further improve shader debugging
  dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

  // Convert file path to wchar_t string.
  const auto wSzFilePath = dy::ConvertStringToWString(szFilePath);

  std::filesystem::path fsPath = szFilePath;
  if (std::filesystem::exists(fsPath) == false)
  {
    const auto absolutePath = std::filesystem::absolute(fsPath);
    platform.GetDebugManager().OnAssertionFailed(
      absolutePath.string().c_str(),
      __FUNCTION__, __FILE__, __LINE__
    );
    return E_FAIL;
  }

  // Create file descriptor.
  FILE* fdFile = fopen(szFilePath.c_str(), "r");
  if (fdFile == nullptr) 
  { 
    platform.GetDebugManager().OnAssertionFailed(
      "Failed to read shader file.", __FUNCTION__, __FILE__, __LINE__
    );
    return E_FAIL; 
  }

  // Read file buffer into fdBuffer.
  fseek(fdFile, 0, SEEK_END);
  const auto fdLength = ftell(fdFile);
  fseek(fdFile, 0, SEEK_SET);

  std::vector<char> fdBuffer(fdLength, 0);
  fread(fdBuffer.data(), sizeof(char), fdLength, fdFile);
  assert(feof(fdFile) != 0);

  // Close file descriptor.
  fclose(fdFile);

  // D3DCompileFromFile.
  // https://docs.microsoft.com/en-us/windows/desktop/api/d3dcompiler/nf-d3dcompiler-d3dcompilefromfile
  // https://docs.microsoft.com/ko-kr/windows/desktop/api/d3dcompiler/nf-d3dcompiler-d3dcompile2
  ID3DBlob* pErrorBlob = nullptr;
  HRESULT hr = D3DCompile2(
    fdBuffer.data(), fdBuffer.size(),
    nullptr,
    nullptr, nullptr,
    szEntryPoint, szShaderModel,
    dwShaderFlags, 0,
    0,
    nullptr, 0,
    ppBlobOut, &pErrorBlob
  );

  // Check error.
  if (FAILED(hr))
  {
    if (pErrorBlob != nullptr)
    {
      OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
    }
  }

  ReleaseCOM(pErrorBlob);
  return hr;
}