# 
# MIT License
# Copyright (c) 2018-2019 Jongmin Yun
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

cmake_minimum_required (VERSION 3.8)
project(Common CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQAUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE true)

set(WIN32_KIT_INCLUDE "C:\\Program Files (x86)\\Windows Kits\\10\\Include")
set(WIN32_KIT_NUMBER 10.0.17763.0)
set(D3D11SDK "${WIN32_KIT_INCLUDE}\\${WIN32_KIT_NUMBER}\\um")
set(COMMON_INCLUDE "${CMAKE_SOURCE_DIR}\\Samples\\_Common\\Include")

include_directories($ENV{UniversalCRTSdkDir}/Include/ucrt)
include_directories($ENV{UniversalCRTSdkDir}/Include/10.0.1.0/ucrt)

set(IMGUI_SOURCE
	"${CMAKE_SOURCE_DIR}/Library/imgui/imgui.cpp"
	"${CMAKE_SOURCE_DIR}/Library/imgui/imgui_draw.cpp"
	"${CMAKE_SOURCE_DIR}/Library/imgui/imgui_widgets.cpp"
	"${CMAKE_SOURCE_DIR}/Library/imgui/examples/imgui_impl_win32.cpp"
	"${CMAKE_SOURCE_DIR}/Library/imgui/examples/imgui_impl_dx11.cpp"
)

# Set Include path.
include_directories("./Include")

add_library(Common STATIC 
	${SOURCE}
	${IMGUI_SOURCE}
)
add_subdirectory(Source)
target_include_directories(Common
PUBLIC
	${COMMON_INCLUDE}
	${CMAKE_CURRENT_SOURCE_DIR}/Include
	${CMAKE_SOURCE_DIR}/Library/imgui
	"${D3D11SDK}/Include"
	
PRIVATE
	${CMAKE_SOURCE_DIR}/DyUtils/DyExpression/Include
	${CMAKE_SOURCE_DIR}/DyUtils/DyStringUtil/Include
	${CMAKE_SOURCE_DIR}/DyUtils/DyMath/Include
	${CMAKE_SOURCE_DIR}/Platform/InputsBase/Include
	${CMAKE_SOURCE_DIR}/Platform/NativePlatformBase/Include	
	${CMAKE_SOURCE_DIR}/Platform/PlatformWin32/Include	
)
set_target_properties(Common PROPERTIES 
	LINKER_LANGUAGE CXX
)
add_definitions(-D_CRT_SECURE_NO_WARNINGS -DUNICODE)

# Add dependencies.
target_link_libraries(Common 
	DyStringUtil
	DyMath
	InputsBase
	PlatformWin32
)
target_link_libraries(Common 
	"${D3D11SDK}/../../../Lib/10.0.17763.0/um/x64/d3d11.lib"
	"${D3D11SDK}/../../../Lib/10.0.17763.0/um/x64/dxgi.lib"
	"${D3D11SDK}/../../../Lib/10.0.17763.0/um/x64/D3Dcompiler.lib"
)

# Install Settings
set_target_properties(Common
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}\\lib\\${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}\\lib\\${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}\\bin\\${CMAKE_BUILD_TYPE}"
)
