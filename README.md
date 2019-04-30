# vx

Build steps:

PRE-REQS
    BREW
        /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
        brew install cmake python python3 git
        brew install doxygen

GLFW
    https://www.glfw.org/docs/latest/compile_guide.html#compile_generate
    
    1) 
    cmake -S extern/glfw/. -B build/bin/glfw/ -DBUILD_SHARED_LIBS=OFF 
        -DGLFW_VULKAN_STATIC=ON
    cd build/bin/glfw/
    make 

VULKAN
    Vulkan-Headers
        cmake -S extern/Vulkan-Headers/. -B build/bin/Vulkan-Headers/ -DCMAKE_INSTALL_PREFIX=build/bin/Vulkan-Headers/install
        cmake --build build/bin/Vulkan-Headers/ --target install

    Vulkan-Loader
        cmake -S extern/Vulkan-Loader/. -B build/bin/Vulkan-Loader/ -DVULKAN_HEADERS_INSTALL_DIR=../../build/bin/Vulkan-Headers/install/ -DCMAKE_BUILD_TYPE=Debug 
        cmake --build build/bin/Vulkan-Loader/ 

    -> Failed to build

    Vulkan-ValidationLayers

    Vulkan-Tools

install_name_tool -change libvulkan.1.dylib @loader_path/libvulkan.1.dylib vxGraphics
install_name_tool -add_rpath "@loader_path" vxGraphics

install_name_tool -change '@rpath/libvulkan.1.dylib' '@executable_path/libvulkan.1.dylib' ${workspaceFolder}/build/bin/vxGraphics/vxGraphics


Vulkan SDK
https://vulkan.lunarg.com/sdk/home

    /etc/profile
    export VULKAN_SDK=/Users/vitorciaramella/Documents/GitHub/vx/extern/vulkansdk/macos/macOS
    export PATH=$VULKAN_SDK/bin:$PATH
    export DYLD_LIBRARY_PATH=$VULKAN_SDK/lib:$DYLD_LIBRARY_PATH
    export VK_ICD_FILENAMES=$VULKAN_SDK/etc/vulkan/icd.d/MoltenVK_icd.json


CLANG
    1) Have CLang available
    https://clang.llvm.org/get_started.html

2) 
cd /Library/Developer/CommandLineTools/Packages
open macOS_SDK_headers_for_macOS_10.14.pkg 
sudo xcode-select -s /Library/Developer/CommandLineTools
rm -rf "$(getconf DARWIN_USER_CACHE_DIR)/org.llvm.clang/ModuleCache"

3) Update "compilerPath" in .vscode/c_cpp_properties.json
https://code.visualstudio.com/docs/cpp/config-clang-mac




build
http://www.bitsnbites.eu/faster-c-builds/
If you use CMake as your main build tool (you probably should), simply specify a Ninja-based generator (e.g. cmake -G Ninja path/to/source).

brew install ccache

export PATH=/usr/lib/ccache:$PATH



build creation
cmake -S ./src -B ./build -G Ninja 


GIT after initial pull
git submodule init
?git submodule pull recursive

BOOST
./bootstrap.sh --with-toolset=clang --prefix=../../build/bin/boost --exec-prefix=../../build/bin/boost
./b2 clean
./b2 install --build-type=complete --build-dir=../../build/bin/boost/build toolset=clang cxxflags="-DBOOST_SYSTEM_NO_DEPRECATED -stdlib=libc++ -std=c++17" linkflags="-stdlib=libc++" --layout=tagged 


copy from /Library/Developer/CommandLineTools/usr/bin/clang
to /usr/bin/

-DBOOST_SYSTEM_NO_DEPRECATED
