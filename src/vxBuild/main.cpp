#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/process.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>

#include <string>
#include <vector>
#include <iostream>

using std::cout;
namespace fs = boost::filesystem;
namespace bp = boost::process;

#define AssertExists(path) \
    if (!fs::exists(path)) \
    { \
        printf("Error: File/Directory does not exist: %s\n", path.c_str()); \
        return -1; \
    }

typedef struct Process 
{
    bp::child childProcess;
} Process;

void listAllFiles(const fs::path & path, std::vector<fs::path> & filesFound)
{
    if (fs::is_regular_file(path))
    {
        filesFound.push_back(path);
    }        
    else if (fs::is_directory(path))
    {
        for (fs::directory_entry & entry : fs::directory_iterator(path))
        {
            auto entryPath = entry.path();
            listAllFiles(entryPath, filesFound);
        }
    }
}

int main()
{
    auto currentPath = fs::current_path();

    auto rootPath = fs::path("../../../");
    auto absRootPath = fs::absolute(rootPath, currentPath);
    AssertExists(absRootPath);
    absRootPath = fs::canonical(absRootPath);

    auto sourcePaths = std::vector<std::string>();
    sourcePaths.push_back("./src/");
    sourcePaths.push_back("./srca/");
    sourcePaths.push_back("./src/vxGraphics/main.cpp");
    auto includePaths = std::vector<std::string>();
    includePaths.push_back("./include/");
    includePaths.push_back("./extern/boost/boost/");
    includePaths.push_back("./extern/glfw/include/");
    includePaths.push_back("./extern/vulkansdk/macos/macOS/include/");


    auto filesFound = std::vector<fs::path>();
    for (auto && sourcePath : sourcePaths)
    {
        auto fsSourcePath = fs::absolute(sourcePath, absRootPath);
        if (!fs::exists(fsSourcePath))
        {
            printf("Error: File/Directory does not exist: %s\n", fsSourcePath.c_str());
            continue;
        }
        fsSourcePath = fs::canonical(fsSourcePath);
        listAllFiles(fsSourcePath, filesFound);
    }    

    auto cppFiles = std::vector<fs::path>();
    for (auto && file : filesFound)
    {
        if (boost::iequals(fs::extension(file),  ".cpp"))
        {
            cppFiles.push_back(file);
        }
    }

    /*
    -std=c++17
    -stdlib=libc++ 
    -DVK_USE_PLATFORM_MACOS_MVK 
    -DVK_USE_PLATFORM_METAL_EXT 
    -D_DEBUG 
    -v 
    -g 
    -framework Cocoa 
    -framework OpenGL 
    -framework IOKit 
    -framework CoreVideo 
    -L/Users/vitorciaramella/Documents/GitHub/vx/build/bin/glfw/src 
    -lglfw3 
    $VULKAN_SDK/lib/libvulkan.1.dylib 

    -I/Users/vitorciaramella/Documents/GitHub/vx/include 
    -I/Users/vitorciaramella/Documents/GitHub/vx/extern/boost/boost 
    -I/Users/vitorciaramella/Documents/GitHub/vx/extern/glfw/include 
    -I/Users/vitorciaramella/Documents/GitHub/vx/extern/vulkansdk/macos/macOS/include 
    /Users/vitorciaramella/Documents/GitHub/vx/src/vxCommon/debug.cpp 
    /Users/vitorciaramella/Documents/GitHub/vx/src/vxGraphics/pipeline.cpp 
    /Users/vitorciaramella/Documents/GitHub/vx/src/vxGraphics/main.cpp 
    /Users/vitorciaramella/Documents/GitHub/vx/src/vxGraphics/instance.cpp 
    /Users/vitorciaramella/Documents/GitHub/vx/src/vxGraphics/window.cpp 
    /Users/vitorciaramella/Documents/GitHub/vx/src/vxGraphics/debug.cpp 
    /Users/vitorciaramella/Documents/GitHub/vx/src/vxGraphics/synchronization.cpp 
    /Users/vitorciaramella/Documents/GitHub/vx/src/vxGraphics/device.cpp 
    /Users/vitorciaramella/Documents/GitHub/vx/src/vxGraphics/macos/window.cpp 
    /Users/vitorciaramella/Documents/GitHub/vx/src/vxGraphics/win32/window.cpp 
    -o /Users/vitorciaramella/Documents/GitHub/vx/build/bin/vxGraphics/vxGraphics

    clang
    -cc1 
    -triple x86_64-apple-macosx10.14.0 
    -Wdeprecated-objc-isa-usage 
    -Werror=deprecated-objc-isa-usage 
    -emit-obj 
    -mrelax-all 
    -disable-free 
    -disable-llvm-verifier 
    -discard-value-names 
    -main-file-name pipeline.cpp 
    -mrelocation-model pic -pic-level 2 
    -mthread-model posix 
    -mdisable-fp-elim 
    -fno-strict-return 
    -masm-verbose 
    -munwind-tables 
    -target-cpu penryn 
    -dwarf-column-info 
    -debug-info-kind=standalone 
    -dwarf-version=4 
    -debugger-tuning=lldb 
    -target-linker-version 450.3 
    -v 
    -resource-dir /Library/Developer/CommandLineTools/usr/lib/clang/10.0.1 
    -D VK_USE_PLATFORM_MACOS_MVK 
    -D VK_USE_PLATFORM_METAL_EXT 
    -D _DEBUG 
    -I /Users/vitorciaramella/Documents/GitHub/vx/include 
    -I /Users/vitorciaramella/Documents/GitHub/vx/extern/boost/boost 
    -I /Users/vitorciaramella/Documents/GitHub/vx/extern/glfw/include 
    -I /Users/vitorciaramella/Documents/GitHub/vx/extern/vulkansdk/macos/macOS/include 
    -stdlib=libc++ 
    -Wno-atomic-implicit-seq-cst 
    -Wno-framework-include-private-from-public 
    -Wno-atimport-in-framework-header 
    -Wno-quoted-include-in-framework-header 
    -std=c++17 
    -fdeprecated-macro 
    -fdebug-compilation-dir /Library/Developer/CommandLineTools/usr/bin 
    -ferror-limit 19 
    -fmessage-length 177 
    -stack-protector 1 
    -fblocks 
    -fencode-extended-block-signature 
    -fregister-global-dtors-with-atexit 
    -fobjc-runtime=macosx-10.14.0 
    -fcxx-exceptions 
    -fexceptions 
    -fmax-type-align=16 
    -fdiagnostics-show-option 
    -fcolor-diagnostics 
    -o /var/folders/__/9hvxlz753rx0xphmy3xl32nc0000gn/T/pipeline-732818.o 
    -x c++ /Users/vitorciaramella/Documents/GitHub/vx/src/vxGraphics/pipeline.cpp
    */

    //clang -x c-header test.h -o test.h.pch
    //clang -x c-header --relocatable-pch -isysroot /path/to/build /path/to/build/mylib.h mylib.h.pch

    auto compilerProcesses = std::vector<std::shared_ptr<Process>>();

    const char * objOutputRoot = "/Users/vitorciaramella/Documents/GitHub/vx/build/bin/vxGraphics/";
    const char * clangpp = "clang++";
    auto exe = bp::search_path(fs::path(clangpp)).c_str();

    auto args = std::vector<std::string>();
    args.push_back("-std=c++17");
    args.push_back("-stdlib=libc++");
    args.push_back("-D_DEBUG");
    args.push_back("-v");
    args.push_back("-x");
    args.push_back("c++");
    args.push_back("-g");
    args.push_back("-c");
    args.push_back("-emit-obj");
    args.push_back("-I/Users/vitorciaramella/Documents/GitHub/vx/include");
    args.push_back("-I/Users/vitorciaramella/Documents/GitHub/vx/extern/glfw/include");
    args.push_back("-I/Users/vitorciaramella/Documents/GitHub/vx/extern/vulkansdk/macos/macOS/include");
    args.push_back("-I/Users/vitorciaramella/Documents/GitHub/vx/build/bin/boost/include");

    for (auto && cppFile : cppFiles)
    {
        auto objFilePath = fs::change_extension(fs::path(cppFile).filename(),".o");
        objFilePath = fs::absolute(objFilePath, objOutputRoot);

        if (fs::exists(objFilePath))
        {
            if (fs::last_write_time(objFilePath) > fs::last_write_time(cppFile))
            {
                printf("\nSkipping %s\n", cppFile.c_str());
                continue;
            }
        }

        auto args2 = args;
        args2.push_back("-o");
        args2.push_back(objFilePath.string());
        args2.push_back(cppFile.string());

        printf("\n");
        printf("Compiling %s\n", cppFile.c_str());
        printf("%s ", exe);
        for (auto && arg : args2)
        {
            printf("%s ", arg.c_str());
        }
        printf("\n");
        printf("\n");

        auto compilerProcess = std::make_shared<Process>();
        //,bp::std_out > stdout)
        compilerProcess->childProcess = bp::child(bp::exe(exe),bp::args(args2),bp::std_out > bp::null,bp::std_err > bp::null);
        compilerProcesses.push_back(compilerProcess);
    }

    auto running = true;
    while (running)
    {
        running = false;
        for (auto && compilerProcess : compilerProcesses)
        {        
            if (compilerProcess->childProcess.running())
            {
                running = true;
                break;
            }
        }
        if (running)
        {
            boost::this_thread::sleep_for(boost::chrono::seconds(1));
            printf("Still running...\n");
        }
    }
    for (auto && compilerProcess : compilerProcesses)
    {        
        compilerProcess->childProcess.wait();
        int result = compilerProcess->childProcess.exit_code();
        printf("Exit code: %i\n", result);
    }

    /*
    bp::ipstream pipe_stream;
    bp::child c("gcc --version", bp::std_out > pipe_stream);

    std::string line;

    while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
    {
        std::cerr << line << std::endl;
    }
    c.wait();
    */
    return 0;
}

