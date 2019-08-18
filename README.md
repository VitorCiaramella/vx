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


GIT Submodules
https://git-scm.com/book/en/v2/Git-Tools-Submodules
cd extern
git rm boost
rm -rf ../.git/modules/extern/boost
git submodule add https://github.com/boostorg/boost.git
cd boost
git submodule init
git submodule update

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

export DYLD_LIBRARY_PATH=/usr/local/boost-1.68.0/lib:$DYLD_LIBRARY_PATH

check if lib++ is installed
printf "#include <ciso646>\nint main () {}" | clang -E -stdlib=libc++ -x c++ -dM - | grep _LIBCPP_VERSION

BOOST
https://github.com/boostorg/boost

./bootstrap.sh --with-toolset=clang --prefix=../../build/bin/boost --exec-prefix=../../build/bin/boost
./b2 clean
./b2 install --build-type=complete --build-dir=../../build/bin/boost/build toolset=clang cxxflags="-DBOOST_SYSTEM_NO_DEPRECATED -stdlib=libc++ -std=c++17" linkflags="-stdlib=libc++" --layout=tagged 


POTENTIAL INCLUDES and settings
                //"-DENABLE_PRECOMPILED_HEADERS=OFF",
                //"-I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/",
                //"-isystem",


Apple Command Line Tools
sudo rm -R /Library/Developer/CommandLineTools
xcode-select --install
sudo xcode-select --switch /Library/Developer/CommandLineTools/
open /Library/Developer/CommandLineTools/Packages/macOS_SDK_headers_for_macOS_10.14.pkg
export CPATH=/Library/Developer/CommandLineTools/usr/include/c++/v1
put on path if needed /Library/Developer/CommandLineTools/usr/bin/

brew install llvm


-DBOOST_SYSTEM_NO_DEPRECATED

in case git files don't work
    brew install boost
    /usr/local/Cellar/boost/1.70.0/lib/
    cp -R /usr/local/Cellar/boost/1.70.0/include/boost/ 


Terminal
    pwd    Get current path
    find / -name atomic.h 2>/dev/null
    clear
    cp -R
    rm -R


LLVM build
brew update
brew upgrade
brew install ocaml
brew install z3
brew install swig
brew install doxygen
brew install graphviz
sudo pip install --upgrade pip
sudo pip install epydoc

---not yet: sudo xcode-select -s /Library/Developer/CommandLineTools/

FOR APPLE
cd ./build/bin/llvm-project/
sudo xcode-select -r
xcode-select -p
rm -R *
cmake \
    -G Ninja \
    -DLLVM_ENABLE_PROJECTS="clang;lldb;lld;libcxx;libcxxabi" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=./ \
    -C ../../../extern/llvm-project/clang/cmake/caches/Apple-stage1.cmake \
    -DLLVM_CREATE_XCODE_TOOLCHAIN=On \
    ../../../extern/llvm-project/llvm
ninja stage2-distribution
ninja stage2-install-distribution
ninja cxx
ninja lld
ninja lldb

