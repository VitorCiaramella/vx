#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/process.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>

#include "../../extern/tiny-process-library/process.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <fstream>

namespace fs = boost::filesystem;
namespace bp = boost::process;
namespace ba = boost::asio;
namespace bs = boost::system;

#define AssertExists(path) \
    if (!fs::exists(path)) \
    { \
        printf("Error: File/Directory does not exist: %s\n", path.c_str()); \
        return -1; \
    }

#define StdPipeFunction std::function<void(const bs::error_code & ec, std::size_t n)>

struct Process;

typedef struct BPProcess 
{
    Process* rpProcess;

    bp::child bpChildProcess;
    ba::io_service ios;

    StdPipeFunction onStdOutFunction;
    char stdOutBufferVector[128 << 10];
    ba::mutable_buffers_1 stdOutBuffer;
    bp::async_pipe stdOutPipe;

    StdPipeFunction onStdErrorFunction;
    char stdErrorBufferVector[128 << 10];
    ba::mutable_buffers_1 stdErrorBuffer;
    bp::async_pipe stdErrorPipe;

    BPProcess(Process * process);
    ~BPProcess();
} BPProcess;

typedef struct Process 
{
private:
    std::unique_ptr<TinyProcessLib::Process> upProcess;
public:
    std::string description;
    fs::path exePath;
    std::vector<std::string> args;
    int exitCode;
    std::stringstream stdOutStream;
    std::stringstream stdErrorStream;
    fs::path stdOutPath;
    fs::path stdErrorPath;
    
    void runAsync()
    {
        std::string cmdWithArgs;
        cmdWithArgs += exePath.string();
        for (auto && arg : args)
        {
            cmdWithArgs += " " + arg;
        }
        upProcess = std::make_unique<TinyProcessLib::Process>(cmdWithArgs, "", 
            [&](const char *bytes, size_t n) 
            {
                stdOutStream.write(bytes, n);
            }, 
            [&](const char *bytes, size_t n) 
            {
                stdErrorStream.write(bytes, n);
            });
    }

    bool isCompleted()
    {
        if (upProcess == nullptr)
        {
            return true;
        }
        if (upProcess->try_get_exit_status(exitCode))
        {
            upProcess = nullptr;
            if (!stdOutPath.empty())
            {
                auto file = std::ofstream(stdOutPath.c_str());
                if (file.is_open())
                {
                    file << stdOutStream.rdbuf();
                    file.close();
                    stdOutStream.clear();
                }
            }
            if (!stdErrorPath.empty())
            {
                auto file = std::ofstream(stdErrorPath.c_str());
                if (file.is_open())
                {
                    file << stdErrorStream.rdbuf();
                    file.close();
                    stdErrorStream.clear();
                }
            }
            return true;
        }
        return false;
    }
} Process;

typedef struct ProcessManager
{
    uint maxCurrentProcesses;
    std::queue<std::shared_ptr<Process>> processQueue;
    std::list<std::shared_ptr<Process>> currentProcesses;
    std::list<std::shared_ptr<Process>> completedProcesses;
    std::list<std::shared_ptr<Process>> completedProcessesWithSuccess;
    std::list<std::shared_ptr<Process>> completedProcessesWithError;
} ProcessManager;

void listAllFiles(const fs::path & path, std::list<fs::path> & filesFound, const std::list<std::string> & extensionFilters, bool isRecursive)
{
    if (!fs::exists(path))
    {
        return;
    }
    if (fs::is_regular_file(path))
    {
        filesFound.push_back(path);
    }        
    else if (fs::is_directory(path))
    {
        for (fs::directory_entry & entry : fs::directory_iterator(path))
        {
            auto entryPath = entry.path();
            if (fs::is_regular_file(entryPath))
            {
                if (extensionFilters.size() == 0)
                {
                        filesFound.push_back(entryPath);
                }
                else
                {
                    auto extension = entryPath.extension().string();
                    for (auto && extensionFilter : extensionFilters)
                    {
                        if (boost::iequals(extension, extensionFilter))
                        {
                            filesFound.push_back(entryPath);
                            break;
                        }
                    }
                }
            }
            else if (fs::is_directory(path) && isRecursive)
            {
                listAllFiles(entryPath, filesFound, extensionFilters, isRecursive);
            }
        }
    }
}

void printProcessOutput(const std::list<std::shared_ptr<Process>> & processes)
{
    std::string line;
    for (auto && process : processes)
    {        
        printf("  %s\n", process->description.c_str());
        if (process->exitCode != 0)
        {
            printf("    Exit code: %i\n", process->exitCode);
            while (process->stdErrorStream && std::getline(process->stdErrorStream, line) && !line.empty())
            {
                printf("    %s\n", line.c_str());
            }
            while (process->stdOutStream && std::getline(process->stdOutStream, line) && !line.empty())
            {
                printf("    %s\n", line.c_str());
            }
        }
    }
}

BPProcess::BPProcess(Process * process)
    : stdOutBuffer(ba::buffer(stdOutBufferVector))
    , stdErrorBuffer(ba::buffer(stdErrorBufferVector))
    , stdOutPipe(bp::async_pipe(ios))
    , stdErrorPipe(bp::async_pipe(ios))
{
    rpProcess = process;
    onStdOutFunction = [&](const bs::error_code & ec, size_t n)
    {
        rpProcess->stdOutStream.write(stdOutBufferVector, n);
        if (!ec)
        {
            ba::async_read(stdOutPipe, stdOutBuffer, onStdOutFunction);
        }
    };

    onStdErrorFunction = [&](const bs::error_code & ec, size_t n)
    {
        rpProcess->stdErrorStream.write(stdErrorBufferVector, n);
        if (!ec)
        {
            ba::async_read(stdErrorPipe, stdErrorBuffer, onStdErrorFunction);
        }
    };

    bpChildProcess = bp::child(bp::exe(rpProcess->exePath),bp::args(rpProcess->args),bp::std_out > stdOutPipe,bp::std_err > stdErrorPipe);

    ba::async_read(stdOutPipe, stdOutBuffer, onStdOutFunction);
    ba::async_read(stdErrorPipe, stdErrorBuffer, onStdErrorFunction);

    ios.run();
}

BPProcess::~BPProcess()
{
    if (bpChildProcess.valid())
    {
        bpChildProcess.wait();            
        bpChildProcess.terminate();
    }
    if (!ios.stopped())
    {
        ios.stop();
    }
    stdOutPipe.close();
    stdErrorPipe.close();
}

void processJobs(const std::shared_ptr<ProcessManager> & processManager)
{
    auto maxCurrentProcesses = std::clamp(processManager->maxCurrentProcesses, (uint)1, (uint)16);

    while (processManager->processQueue.size()>0)
    {
        while ((processManager->processQueue.size()>0)
                && (processManager->currentProcesses.size() < maxCurrentProcesses))
        {
            auto process = processManager->processQueue.front();
            processManager->currentProcesses.push_back(process);                
            processManager->processQueue.pop();
            printf("Executing: %s\n", process->description.c_str());
            process->runAsync();
        }
        auto anyProcessDone = false;
        while (!anyProcessDone)
        {
            auto currentProcessesTmp = processManager->currentProcesses;
            for (auto && currentProcessTmp : currentProcessesTmp)
            {
                if (currentProcessTmp->isCompleted())
                {
                    printf("Done: %s\n", currentProcessTmp->description.c_str());
                    processManager->completedProcesses.push_back(currentProcessTmp);
                    processManager->currentProcesses.remove(currentProcessTmp);
                    anyProcessDone = true;
                    if (currentProcessTmp->exitCode==0)
                    {
                        processManager->completedProcessesWithSuccess.push_back(currentProcessTmp);
                    }
                    else
                    {
                        processManager->completedProcessesWithError.push_back(currentProcessTmp);
                    }
                }
            }        
            if (!anyProcessDone)
            {
                printf("Awaiting...\n");
                boost::this_thread::sleep_for(boost::chrono::milliseconds(250));
            }
        }
    }
}

void getFiles(const std::list<std::string> & searchPaths, const std::string & rootPath, const std::list<std::string> & extensions, std::list<fs::path> & filesFound)
{
     for (auto && originalPath : searchPaths)
    {
        //printf("%s:\n", originalPath.c_str());
        auto absPath = fs::absolute(originalPath, rootPath);
        if (fs::is_regular_file(absPath))
        {
            filesFound.push_back(fs::canonical(absPath));
            continue;
        }
        auto filename = absPath.filename();
        auto directory = absPath.parent_path();
        if (!fs::exists(directory))
        {
            printf("Error: Directory does not exist: %s\n", directory.c_str());
            printf("Error: Invalid path: %s\n", originalPath.c_str());
            continue;
        }
        directory = fs::canonical(directory);
        auto extension = filename.extension().string();
        filename.replace_extension("");
        auto isRecursiveWildcard = boost::iequals(filename.string(), "**");
        auto isWildcard = boost::iequals(filename.string(), "*");
        if (!isRecursiveWildcard && !isWildcard)
        {
            printf("Error: Invalid path: %s\n", originalPath.c_str());
            continue;
        }
        auto extensionFilter = extensions;
        if (extension.size()>0)
        {
            extensionFilter.clear();
            extensionFilter.push_back(extension);
        }
        listAllFiles(directory, filesFound, extensionFilter, isRecursiveWildcard);
        /*
        for (auto && file : filesFound)
        {
            printf("%s\n", file.c_str());
        }
        filesFound.clear();
        */
    }    
}

void dedupFiles(std::list<fs::path> & files)
{
    auto previousList = files;
    files.clear();
    for (auto && file : previousList)
    {
        auto contains = false;
        for (auto && existingFile : files)
        {
            if (fs::equivalent(file, existingFile))
            {
                contains = true;
                break;
            }
        }
        if (!contains)
        {
            files.push_back(file);
        }
    }
}

typedef struct CppCompileTask
{
    std::string cppCompilerPath;
    std::list<std::string> inputPaths;
    std::list<std::string> inputExtensions;
    std::string outputPath;
    std::list<std::string> options;
    std::list<std::string> includePaths;
    bool alwaysRecompile;
} CppCompileTask;

void createHppPreCompileProcessRequest(const std::shared_ptr<ProcessManager> & processManager, const CppCompileTask & cppCompileTask, const fs::path & absRootPath)
{
    auto filesFound = std::list<fs::path>();
    auto hppExtensions = std::list<std::string>();
    hppExtensions.push_back(".hpp");
    for (auto && includePath : cppCompileTask.includePaths)
    {
        auto searchPaths = std::list<std::string>();
        auto absSearchPath = fs::absolute(includePath, absRootPath);
        if (!fs::exists(absSearchPath) || !fs::is_directory(absSearchPath))
        {
            printf("Error: Directory does not exist: %s\n", absSearchPath.c_str());
            continue;
        }        
        auto searchPath = fs::absolute(fs::path("**"),fs::canonical(absSearchPath));        
        searchPaths.push_back(searchPath.string());
        getFiles(searchPaths, absRootPath.string(), hppExtensions, filesFound);
    }
    //dedupFiles(filesFound);
    printf("\nHeaders to pre-compile:\n");
    for (auto && file : filesFound)
    {
        printf("  -%s\n", file.c_str());
    }

    auto compilerPath = bp::search_path(fs::path(cppCompileTask.cppCompilerPath));
    if (!fs::exists(compilerPath) || !fs::is_regular_file(compilerPath))
    {
        printf("\Error: Compiler not found.\n");
        printf("  Compiler path: %s\n", cppCompileTask.cppCompilerPath.c_str());
    }
    printf("Compiler found: %s\n", compilerPath.c_str());

    auto outputRootPath = fs::absolute(fs::path(cppCompileTask.outputPath), absRootPath).append("/headers/");
    if (!fs::exists(outputRootPath))
    {
        fs::create_directory(outputRootPath);
    }
    outputRootPath = fs::canonical(outputRootPath);

    auto commonArgs = std::vector<std::string>();
    commonArgs.push_back("-x");
    commonArgs.push_back("c++-header");
    for (auto && option : cppCompileTask.options)
    {
        commonArgs.push_back(option);
    }
    for (auto && includePath : cppCompileTask.includePaths)
    {
        auto absIncludePath = fs::absolute(fs::path(includePath), absRootPath);
        if (!fs::exists(absIncludePath))
        {
            printf("Error: Directory does not exist: %s\n", absIncludePath.c_str());
            printf("Error: Invalid path: %s\n", includePath.c_str());
            continue;
        }
        absIncludePath = fs::canonical(absIncludePath);
        commonArgs.push_back("-I"+absIncludePath.string());
    }

    for (auto && file : filesFound)
    {
        auto outputFolderPath = fs::absolute(fs::relative(file, absRootPath).parent_path(), outputRootPath);
        if (!fs::exists(outputFolderPath))
        {
            fs::create_directories(outputFolderPath);
        }
        outputFolderPath = fs::canonical(outputFolderPath);

        auto filePath = fs::path(file);
        auto hppFilePath = fs::absolute(filePath.filename(), outputFolderPath);
        auto pchFilePath = fs::absolute(fs::change_extension(filePath.filename(),filePath.extension().string() + ".pch"), outputFolderPath);
        auto stdOutFilePath = fs::absolute(fs::change_extension(filePath.filename(),filePath.extension().string() + ".out.log"), outputFolderPath);
        auto stdErrorFilePath = fs::absolute(fs::change_extension(filePath.filename(),filePath.extension().string() + ".err.log"), outputFolderPath);

        if (cppCompileTask.alwaysRecompile 
            || !fs::exists(hppFilePath)
            || (fs::last_write_time(file) > fs::last_write_time(hppFilePath)))
        {
            fs::copy_file(file, hppFilePath);
        }

        if (!cppCompileTask.alwaysRecompile && fs::exists(pchFilePath))
        {
            if (fs::last_write_time(pchFilePath) > fs::last_write_time(hppFilePath))
            {
                printf("Skipping %s\n", file.c_str());
                continue;
            }
        }
        if (!cppCompileTask.alwaysRecompile && fs::exists(stdOutFilePath))
        {
            if (fs::last_write_time(stdOutFilePath) > fs::last_write_time(hppFilePath))
            {
                printf("Skipping %s\n", file.c_str());
                continue;
            }
        }

        auto args = commonArgs;            
        args.push_back("-o");
        args.push_back(pchFilePath.string());
        args.push_back(hppFilePath.string());

        auto compilerProcess = std::make_shared<Process>();
        compilerProcess->description = "Header pre-compile " + file.filename().string();
        compilerProcess->exePath = compilerPath;
        compilerProcess->args = args;
        compilerProcess->stdOutPath = stdOutFilePath;
        compilerProcess->stdErrorPath = stdErrorFilePath;
        processManager->processQueue.push(compilerProcess);
    }
}

void createCppCompileProcessRequest(const std::shared_ptr<ProcessManager> & processManager, const CppCompileTask & cppCompileTask, const fs::path & absRootPath)
{
    auto filesFound = std::list<fs::path>();
    getFiles(cppCompileTask.inputPaths, absRootPath.string(), cppCompileTask.inputExtensions, filesFound);
    dedupFiles(filesFound);
    printf("\nFiles to compile:\n");
    for (auto && file : filesFound)
    {
        printf("  -%s\n", file.c_str());
    }

    auto compilerPath = bp::search_path(fs::path(cppCompileTask.cppCompilerPath));
    if (!fs::exists(compilerPath) || !fs::is_regular_file(compilerPath))
    {
        printf("\Error: Compiler not found.\n");
        printf("  Compiler path: %s\n", cppCompileTask.cppCompilerPath.c_str());
    }
    printf("Compiler found: %s\n", compilerPath.c_str());

    auto outputRootPath = fs::absolute(fs::path(cppCompileTask.outputPath), absRootPath);
    if (!fs::exists(outputRootPath))
    {
        fs::create_directory(outputRootPath);
    }
    outputRootPath = fs::canonical(outputRootPath);

    auto commonArgs = std::vector<std::string>();
    commonArgs.push_back("-c");
    commonArgs.push_back("-x");
    commonArgs.push_back("c++");
    commonArgs.push_back("-emit-obj");
    for (auto && option : cppCompileTask.options)
    {
        commonArgs.push_back(option);
    }
    for (auto && includePath : cppCompileTask.includePaths)
    {
        auto absIncludePath = fs::absolute(fs::path(includePath), absRootPath);
        if (!fs::exists(absIncludePath))
        {
            printf("Error: Directory does not exist: %s\n", absIncludePath.c_str());
            printf("Error: Invalid path: %s\n", includePath.c_str());
            continue;
        }
        absIncludePath = fs::canonical(absIncludePath);
        commonArgs.push_back("-I"+absIncludePath.string());
    }

    for (auto && file : filesFound)
    {
        if (boost::iequals(fs::extension(file),  ".cpp"))
        {
            auto outputFolderPath = fs::absolute(fs::relative(file, absRootPath).parent_path(), outputRootPath);
            if (!fs::exists(outputFolderPath))
            {
                fs::create_directories(outputFolderPath);
            }
            outputFolderPath = fs::canonical(outputFolderPath);
            auto objFilePath = fs::change_extension(fs::path(file).filename(),".o");
            objFilePath = fs::absolute(objFilePath, outputFolderPath);

            if (!cppCompileTask.alwaysRecompile && fs::exists(objFilePath))
            {
                if (fs::last_write_time(objFilePath) > fs::last_write_time(file))
                {
                    printf("Skipping %s\n", file.c_str());
                    continue;
                }
            }

            auto args = commonArgs;            
            args.push_back("-o");
            args.push_back(objFilePath.string());
            args.push_back(file.string());

            auto compilerProcess = std::make_shared<Process>();
            compilerProcess->description = "C++ compile " + file.filename().string();
            compilerProcess->exePath = compilerPath;
            compilerProcess->args = args;
            processManager->processQueue.push(compilerProcess);
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

    CppCompileTask cppCompileTask;
    cppCompileTask.alwaysRecompile = false;
    cppCompileTask.cppCompilerPath = "clang++";
    cppCompileTask.inputPaths.push_back("./src/**.cpp");
    //cppCompileTask.inputExtensions.push_back(".cpp");
    cppCompileTask.includePaths.push_back("./include/");
    cppCompileTask.includePaths.push_back("./extern/boost/boost/");
    cppCompileTask.includePaths.push_back("./extern/glfw/include/");
    cppCompileTask.includePaths.push_back("./extern/vulkansdk/macos/macOS/include/");
    cppCompileTask.options.push_back("-std=c++17");
    cppCompileTask.options.push_back("-stdlib=libc++");
    cppCompileTask.options.push_back("-D_DEBUG");
    cppCompileTask.options.push_back("-DVK_USE_PLATFORM_MACOS_MVK");
    cppCompileTask.options.push_back("-DVK_USE_PLATFORM_METAL_EXT");
    cppCompileTask.options.push_back("-v");
    cppCompileTask.options.push_back("-g");
    cppCompileTask.outputPath = "./build/bin/";

    auto processManager = std::make_shared<ProcessManager>();
    processManager->maxCurrentProcesses = 8;

    createHppPreCompileProcessRequest(processManager, cppCompileTask, absRootPath);
    processJobs(processManager);

    //createCppCompileProcessRequest(processManager, cppCompileTask, absRootPath);
    //processJobs(processManager);


    /*
    -framework Cocoa 
    -framework OpenGL 
    -framework IOKit 
    -framework CoreVideo 
    -L/Users/vitorciaramella/Documents/GitHub/vx/build/bin/glfw/src 
    -lglfw3 
    $VULKAN_SDK/lib/libvulkan.1.dylib 

    -o /Users/vitorciaramella/Documents/GitHub/vx/build/bin/vxGraphics/vxGraphics
    */

    //clang -x c-header test.h -o test.h.pch



    int exitCode = 0;
    if (processManager->completedProcessesWithSuccess.size() > 0)
    {
        printf("\nSuccesses:\n");
        printProcessOutput(processManager->completedProcessesWithSuccess);
    }
    if (processManager->completedProcessesWithError.size() > 0)
    {
        printf("\nErrors:\n");
        printProcessOutput(processManager->completedProcessesWithError);
        exitCode = 1;
    }
    printf("\n");

    return exitCode;
}

