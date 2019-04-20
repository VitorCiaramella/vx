# vx

Build steps:

1) Have CLang available
https://clang.llvm.org/get_started.html

2) 
cd /Library/Developer/CommandLineTools/Packages
open macOS_SDK_headers_for_macOS_10.14.pkg 
sudo xcode-select -s /Library/Developer/CommandLineTools
rm -rf "$(getconf DARWIN_USER_CACHE_DIR)/org.llvm.clang/ModuleCache"

3) Update "compilerPath" in .vscode/c_cpp_properties.json
https://code.visualstudio.com/docs/cpp/config-clang-mac

