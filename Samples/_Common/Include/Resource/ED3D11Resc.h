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

/// @enum ED3D11Resc
/// @brief Resource types of D3D11.
enum class ED3D11Resc 
{
  Device,           // ID3D11Device & ID3D11DeviceContext.
  SwapChain,        // ID3D11SwapChain
  RTV,              // ID3D11RenderTargetView
  DSV,              // ID3D11DepthStencilView
  RasterizerState,  // ID3D11RasterizerState 
  DepthStencilState,// ID3D11DepthStencilState
  BlendState,       // ID3D11BlendState
  VertexShader,     // ID3D11VertexShader
  PixelShader,      // ID3D11PixelShader
  InputLayout,      // ID3D11InputLayout
  Buffer,           // ID3D11Buffer
  Texture2D,        // ID3D11Texture2D
  Blob,             // ID3D11Blob
  Query,            // ID3D11Query
};
