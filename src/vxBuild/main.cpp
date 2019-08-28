#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/process.hpp>

#include "../../extern/tiny-process-library/process.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

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
private:
    std::unique_ptr<TinyProcessLib::Process> upProcess;
public:
    std::string description;
    fs::path exePath;
    std::vector<std::string> args;
    int exitCode;
    std::stringstream stdOutStream;
    fs::path stdOutPath;
    std::ofstream * stdOutFile;
    
    std::string getCmdWithArgs()
    {
        auto cmdWithArgs = exePath.string();
        for (auto && arg : args)
        {
            cmdWithArgs += " " + arg;
        }
        return cmdWithArgs;
    }

    void runAsync()
    {
        auto cmdWithArgs = getCmdWithArgs();
        auto logFunction = [&](const char *bytes, size_t n) 
        {
            stdOutStream.write(bytes, n);
            if (stdOutFile != nullptr)
            {
                stdOutFile->write(bytes, n);
            }
        };
        logFunction(cmdWithArgs.c_str(), cmdWithArgs.size());
        auto newLine = std::string("\n");
        logFunction(newLine.c_str(), newLine.size());
        upProcess = std::make_unique<TinyProcessLib::Process>(cmdWithArgs, "", logFunction, logFunction);
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
                    file << getCmdWithArgs() << "\n";
                    file << "Exit code: " << exitCode << "\n\n";
                    file << stdOutStream.rdbuf();
                    file.close();
                    stdOutStream.clear();
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

void printCompletedProcesses(const std::list<std::shared_ptr<Process>> & processes, bool & containsFailures)
{
    std::string line;
    bool headerPrinted = false;
    for (auto && process : processes)
    {        
        if (process->exitCode == 0)
        {
            if (!headerPrinted)
            {
                printf("\nSuccesses:\n");
                headerPrinted = true;
            }
            printf("  %s\n", process->description.c_str());
        }
    }
    headerPrinted = false;
    for (auto && process : processes)
    {        
        if (process->exitCode != 0)
        {
            containsFailures = true;
            if (!headerPrinted)
            {
                printf("\n\nFailures:\n");
                headerPrinted = true;
            }
            printf("  %s\n", process->description.c_str());
            printf("    Exit code: %i\n", process->exitCode);
            if (!process->stdOutPath.empty())
            {
                auto logFile = std::ifstream(process->stdOutPath.c_str());
                if (logFile.is_open())
                {
                    while ( getline (logFile,line) )
                    {
                        printf("    %s\n", line.c_str());
                    }
                    logFile.close();
                }
            }
            else
            {
                while (process->stdOutStream && std::getline(process->stdOutStream, line) && !line.empty())
                {
                    printf("    %s\n", line.c_str());
                }
            }            
            printf("\n\n");
        }
    }
}

void processJobs(const std::shared_ptr<ProcessManager> & processManager)
{
    auto maxCurrentProcesses = std::clamp(processManager->maxCurrentProcesses, (uint)1, (uint)16);

    while (processManager->processQueue.size()>0 || processManager->currentProcesses.size()>0)
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
        auto awaitingMessagePrinted = false;
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
                }
            }        
            if (!anyProcessDone)
            {   
                if (!awaitingMessagePrinted)
                {
                    printf("Awaiting...\n");
                }
                awaitingMessagePrinted = true;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }
}

void getFiles(const std::list<std::string> & searchPaths, const std::list<std::string> & extensions, const std::string & rootPath, std::list<fs::path> & filesFound)
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
        auto filename = fs::path("**");
        auto directory = absPath;
        if (!fs::is_directory(absPath))
        {
            filename = absPath.filename();        
            directory = absPath.parent_path();
            if (!fs::exists(directory))
            {
                printf("Error: Directory does not exist: %s\n", directory.c_str());
                printf("Error: Invalid path: %s\n", originalPath.c_str());
                continue;
            }
            directory = fs::canonical(directory);
        }
        auto extension = filename.extension().string();
        filename.replace_extension("");
        auto isRecursiveWildcard = boost::iequals(filename.string(), "**") || boost::iequals(filename.string(), ".");
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

void getFiles(const std::list<std::string> & searchPaths, const std::list<std::string> & extensions, const fs::path & rootPath, std::list<fs::path> & filesFound)
{
    getFiles(searchPaths, extensions, rootPath.string(), filesFound);
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
    std::list<std::string> sourcePaths;
    std::string outputPath;
    std::list<std::string> options;
    std::list<std::string> includePaths;
    std::list<std::string> preCompiledIncludePaths;
    std::list<std::string> frameworks;
    std::list<std::string> libraryPaths;
    std::list<std::string> libraries;
    bool forceCppRecompilation;
    bool preCompileHeaders;
    bool forceHeaderCopy;
    bool forceHeaderRecompilation;
} CppCompileTask;

typedef struct GlslCompileTask
{
    std::string glslCompilerPath;
    std::list<std::string> sourcePaths;
    std::string outputPath;
    bool forceRecompilation;
} GlslCompileTask;

bool assertDirectoryExists(const fs::path & path)
{
    if (!fs::exists(path) || !fs::is_directory(path))
    {
        printf("Error: Directory does not exist: %s\n", path.c_str());
        return false;
    }   
    return true;
}


void printPathList(const std::string & header, const std::list<fs::path> & pathList)
{
    return;
    printf("\n%s\n", header.c_str());
    for (auto && path : pathList)
    {
        printf("  -%s\n", path.c_str());
    }
}

fs::path createSubDirectory(const fs::path & path, const std::string & subFolder)
{
    auto subDirectoryPath = fs::path(path).append(subFolder);
    if (!fs::exists(subDirectoryPath))
    {
        fs::create_directories(subDirectoryPath);
    }
    subDirectoryPath = fs::canonical(subDirectoryPath);
    return subDirectoryPath;
}

fs::path createSubDirectory(const std::string & path, const std::string & subFolder)
{
    return createSubDirectory(fs::path(path), subFolder);
}

fs::path createDirectory(const fs::path & path)
{
    if (!fs::exists(path))
    {
        fs::create_directories(path);
    }
    return fs::canonical(path);
}

fs::path absolutePath(const std::string & path, const fs::path & absRootPath)
{
    return fs::absolute(fs::path(path), absRootPath);
}

fs::path createCorrespondingPath(const fs::path & originalPath, const fs::path & rootPath, const fs::path & newRootPath)
{
    auto relativeIncludePath = fs::relative(fs::absolute(originalPath, rootPath),rootPath);
    auto correspondingPath = fs::absolute(relativeIncludePath, newRootPath);
    auto correspondingFolderPath = correspondingPath;
    if (correspondingFolderPath.has_extension())
    {
        correspondingFolderPath = correspondingFolderPath.parent_path();
    }
    createDirectory(correspondingFolderPath);
    return correspondingPath;
}

void copyHeaderFilesForPreCompilation(const std::shared_ptr<ProcessManager> & processManager, CppCompileTask & cppCompileTask, const fs::path & absRootPath, std::ofstream * logFile)
{
    auto filesFound = std::list<fs::path>();
    auto extensions = std::list<std::string>();
    extensions.push_back(".h");
    extensions.push_back(".hpp");

    //TODO
    auto reducedIncludePaths = std::list<std::string>();
    reducedIncludePaths.push_back(cppCompileTask.includePaths.front());

    getFiles(reducedIncludePaths, extensions, absRootPath, filesFound);
    //dedupFiles(filesFound);
    printPathList("Headers to copy an pre-compile:", filesFound);

    auto outputRootPath = absolutePath(cppCompileTask.outputPath, absRootPath);
    auto headersOutputRootPath = createSubDirectory(outputRootPath, "/temp/headers/");

    for (auto && includePath : cppCompileTask.includePaths)
    {
        auto absoluteOutputPath = createCorrespondingPath(includePath, absRootPath, headersOutputRootPath);
        cppCompileTask.preCompiledIncludePaths.push_back(absoluteOutputPath.string());
    }

    for (auto && file : filesFound)
    {
        auto relativeFilePath = fs::relative(file, absRootPath);
        auto relativeFolderPath = relativeFilePath.parent_path();
        auto absoluteOutputPath = fs::absolute(relativeFolderPath, headersOutputRootPath);
        absoluteOutputPath = createDirectory(absoluteOutputPath);
        
        auto absoluteOutputFilePath = fs::absolute(file.filename(), absoluteOutputPath);

        if (cppCompileTask.forceHeaderCopy 
            || !fs::exists(absoluteOutputFilePath)
            || (fs::last_write_time(file) > fs::last_write_time(absoluteOutputFilePath)))
        {
            //fs::copy_file(file, headerFilePath, fs::copy_option::overwrite_if_exists);
            auto args = std::vector<std::string>();
            args.push_back("-p");
            args.push_back(file.string());
            args.push_back(absoluteOutputFilePath.string());

            auto command = std::make_shared<Process>();
            command->description = "Copy header for pre-compile " + file.filename().string();
            command->exePath = fs::path("cp");
            command->args = args;
            command->stdOutFile = logFile;
            processManager->processQueue.push(command);
        }
    }
}

bool getCompilerPath(const std::string & compilerPath, fs::path & path)
{
    path = bp::search_path(fs::path(compilerPath));
    if (!fs::exists(path) || !fs::is_regular_file(path))
    {
        printf("\Error: Compiler not found.\n");
        printf("  Compiler path: %s\n", compilerPath.c_str());
        return false;
    }
    printf("Compiler found: %s at %s \n", compilerPath.c_str(), path.c_str());
    return true;
}

bool getCanonicalPath(fs::path & path)
{
    if (!fs::exists(path) || !fs::is_directory(path))
    {
        printf("Error: Directory does not exist: %s\n", path.c_str());
        return false;
    }
    path = fs::canonical(path);
    return true;
}

void createClangCompileCommand(const std::list<fs::path> & filesFound,const std::shared_ptr<ProcessManager> & processManager, const CppCompileTask & cppCompileTask, const fs::path & absRootPath, std::ofstream & compilerComandsJson)
{
    auto outputRootPath = absolutePath(cppCompileTask.outputPath, absRootPath);
    auto headersOutputRootPath = createSubDirectory(outputRootPath, "/temp/headers/");
    auto objectsOutputRootPath = createSubDirectory(outputRootPath, "/temp/objs/");

    fs::path compilerPath;
    if (!getCompilerPath(cppCompileTask.cppCompilerPath, compilerPath))
    {
        return;
    }

    auto commonArgs = std::vector<std::string>();
    for (auto && option : cppCompileTask.options)
    {
        commonArgs.push_back(option);
    }
    for (auto && includePath : cppCompileTask.preCompiledIncludePaths)
    {
        auto absIncludePath = fs::absolute(fs::path(includePath), absRootPath);
        if (!getCanonicalPath(absIncludePath))
        {
            continue;
        }
        commonArgs.push_back("-I"+absIncludePath.string());
    }
    for (auto && includePath : cppCompileTask.includePaths)
    {
        auto absIncludePath = fs::absolute(fs::path(includePath), absRootPath);
        if (!getCanonicalPath(absIncludePath))
        {
            continue;
        }
        commonArgs.push_back("-I"+absIncludePath.string());
    }

    for (auto && file : filesFound)
    {
        auto outputFilePath = file;

        auto compilerProcess = std::make_shared<Process>();
        compilerProcess->exePath = compilerPath;
        compilerProcess->args = commonArgs;            
        compilerProcess->args.push_back(file.string());
        compilerProcess->args.push_back("-std=c++17");
        compilerProcess->args.push_back("-stdlib=libc++");

        auto fileExtension = file.extension().string();
        std::string outputExtension;
        auto forceRecompilation = false;
        auto skipIfLogIsPresent = true;
        if (boost::iequals(fileExtension, ".c") || boost::iequals(fileExtension, ".cpp"))
        {
            outputFilePath = createCorrespondingPath(outputFilePath, absRootPath, objectsOutputRootPath);
            skipIfLogIsPresent = false;
            compilerProcess->description = "C++ compile " + file.filename().string();
            forceRecompilation = cppCompileTask.forceCppRecompilation;
            outputExtension = ".o";
            compilerProcess->args.push_back("-x");
            compilerProcess->args.push_back("c++");
            compilerProcess->args.push_back("-c");
            compilerProcess->args.push_back("-emit-obj");
        }
        else if (boost::iequals(fileExtension, ".h") || boost::iequals(fileExtension, ".hpp"))
        {
            outputFilePath = createCorrespondingPath(outputFilePath, absRootPath, headersOutputRootPath);

            compilerProcess->description = "Header pre-compile " + file.filename().string();
            forceRecompilation = cppCompileTask.forceHeaderRecompilation;
            outputExtension = ".pch";
            compilerProcess->args.push_back("-x");
            compilerProcess->args.push_back("c++-header");
            //args.push_back("c-header");//for pure c headers
            //args.push_back("-std=c17");//for pure c headers
        }
        else
        {
            printf("Unsupported extension %s for %s\n", fileExtension.c_str(), file.c_str());
            continue;
        }
        
        auto stdOutFilePath = fs::change_extension(outputFilePath, fileExtension + ".build.log");
        outputFilePath = fs::change_extension(outputFilePath, fileExtension + outputExtension);

        compilerProcess->args.push_back("-o");
        compilerProcess->args.push_back(outputFilePath.string());
        compilerProcess->stdOutPath = stdOutFilePath;

        compilerComandsJson << "  {\n";
        compilerComandsJson << "    \"command\":\"" << compilerProcess->getCmdWithArgs() << "\",\n";
        compilerComandsJson << "    \"directory\":\"" << absRootPath.string() << "\",\n";
        compilerComandsJson << "    \"file\":\"" << file.string() << "\",\n";
        compilerComandsJson << "    \"output\":\"" << outputFilePath.string() << "\"\n";
        compilerComandsJson << "  },\n";

        if (!forceRecompilation)
        {
            if ((fs::exists(outputFilePath) && fs::last_write_time(outputFilePath) > fs::last_write_time(file))
                || (skipIfLogIsPresent && fs::exists(stdOutFilePath) && fs::last_write_time(stdOutFilePath) > fs::last_write_time(file)))
            {
                //printf("Skipping %s\n", file.c_str());
                continue;
            }
        }
        processManager->processQueue.push(compilerProcess);
    }
}

void createGlslCompileCommand(const std::list<fs::path> & filesFound,const std::shared_ptr<ProcessManager> & processManager, const GlslCompileTask & glslCompileTask, const fs::path & absRootPath)
{
    auto outputRootPath = absolutePath(glslCompileTask.outputPath, absRootPath);
    auto spvOutputRootPath = createSubDirectory(outputRootPath, "/temp/spvs/");

    auto compilerPath = absolutePath(glslCompileTask.glslCompilerPath, absRootPath);
    if (!fs::exists(compilerPath) || !fs::is_regular_file(compilerPath))
    {
        printf("Glsl compiler not found at %s\n", compilerPath.c_str());
        return;
    }
    compilerPath = fs::canonical(compilerPath);

    auto commonArgs = std::vector<std::string>();

    for (auto && file : filesFound)
    {
        auto outputFilePath = file;

        auto compilerProcess = std::make_shared<Process>();
        compilerProcess->exePath = compilerPath;
        compilerProcess->args = commonArgs;

        auto fileExtension = file.extension().string();
        auto fileSubExtension = fs::change_extension(file, "").extension().string();
        std::string outputExtension;
        auto forceRecompilation = false;
        auto skipIfLogIsPresent = false;
        if (boost::iequals(fileExtension, ".glsl"))
        {
            outputExtension = ".spv";
            outputFilePath = createCorrespondingPath(outputFilePath, absRootPath, spvOutputRootPath);
            compilerProcess->description = "Shader compile " + file.filename().string();
            if (fileSubExtension.size() > 1)
            {
                fileSubExtension.erase(0,1);
                compilerProcess->args.push_back("-fshader-stage=" + fileSubExtension);
            }
        }
        else
        {
            printf("Unsupported extension %s for %s\n", fileExtension.c_str(), file.c_str());
            continue;
        }
        
        auto stdOutFilePath = fs::change_extension(outputFilePath, fileExtension + ".build.log");
        outputFilePath = fs::change_extension(outputFilePath, fileExtension + outputExtension);

        compilerProcess->args.push_back(file.string());
        compilerProcess->args.push_back("-o");
        compilerProcess->args.push_back(outputFilePath.string());
        compilerProcess->stdOutPath = stdOutFilePath;

        if (!forceRecompilation)
        {
            if ((fs::exists(outputFilePath) && fs::last_write_time(outputFilePath) > fs::last_write_time(file))
                || (skipIfLogIsPresent && fs::exists(stdOutFilePath) && fs::last_write_time(stdOutFilePath) > fs::last_write_time(file)))
            {
                //printf("Skipping %s\n", file.c_str());
                continue;
            }
        }
        processManager->processQueue.push(compilerProcess);
    }
}

void createHppPreCompileProcessRequest(const std::shared_ptr<ProcessManager> & processManager, const CppCompileTask & cppCompileTask, const fs::path & absRootPath, std::ofstream & compilerComandsJson)
{
    auto filesFound = std::list<fs::path>();
    auto extensions = std::list<std::string>();
    extensions.push_back(".h");
    extensions.push_back(".hpp");
    getFiles(cppCompileTask.preCompiledIncludePaths, extensions, absRootPath, filesFound);
    //dedupFiles(filesFound);
    printPathList("Headers to pre-compile:", filesFound);

    createClangCompileCommand(filesFound, processManager, cppCompileTask, absRootPath, compilerComandsJson);
}

void createCppCompileProcessRequest(const std::shared_ptr<ProcessManager> & processManager, const CppCompileTask & cppCompileTask, const fs::path & absRootPath, std::ofstream & compilerComandsJson)
{
    auto filesFound = std::list<fs::path>();
    auto extensions = std::list<std::string>();
    extensions.push_back(".c");
    extensions.push_back(".cpp");
    getFiles(cppCompileTask.sourcePaths, extensions, absRootPath, filesFound);
    //dedupFiles(filesFound);
    printPathList("C++ to compile:", filesFound);

    createClangCompileCommand(filesFound, processManager, cppCompileTask, absRootPath, compilerComandsJson);
}

void createGlslCompileProcessRequest(const std::shared_ptr<ProcessManager> & processManager, const GlslCompileTask & glslCompileTask, const fs::path & absRootPath)
{
    auto filesFound = std::list<fs::path>();
    auto extensions = std::list<std::string>();
    extensions.push_back(".glsl");
    getFiles(glslCompileTask.sourcePaths, extensions, absRootPath, filesFound);
    //dedupFiles(filesFound);
    printPathList("Shader to compile:", filesFound);

    createGlslCompileCommand(filesFound, processManager, glslCompileTask, absRootPath);
}

void createLinkerProcessRequest(const std::shared_ptr<ProcessManager> & processManager, const CppCompileTask & cppCompileTask, const fs::path & absRootPath, bool library)
{
    auto filesFound = std::list<fs::path>();
    auto extensions = std::list<std::string>();
    extensions.push_back(".c");
    extensions.push_back(".cpp");
    getFiles(cppCompileTask.sourcePaths, extensions, absRootPath, filesFound);
    //dedupFiles(filesFound);
    printPathList("C++ to link:", filesFound);

    auto outputRootPath = absolutePath(cppCompileTask.outputPath, absRootPath);
    auto objectsOutputRootPath = createSubDirectory(outputRootPath, "/temp/objs/");
    auto binOutputRootPath = createSubDirectory(outputRootPath, "/bin/vxGraphics/");

    fs::path linkerPath;
    if (!getCompilerPath("ld", linkerPath))
    {
        return;
    }

    auto commonArgs = std::vector<std::string>();
    if (library)
    {
        commonArgs.push_back("-dylib"); 
    }
    commonArgs.push_back("-t"); //trace
    commonArgs.push_back("-demangle"); 
    //commonArgs.push_back("-lto_library"); 
    //commonArgs.push_back("/Library/Developer/CommandLineTools/usr/lib/libLTO.dylib "); 
    commonArgs.push_back("-no_deduplicate"); //for debug only
    commonArgs.push_back("-dynamic"); 
    commonArgs.push_back("-arch"); 
    commonArgs.push_back("x86_64"); 
    commonArgs.push_back("-macosx_version_min"); 
    commonArgs.push_back("10.14.0 "); 
    commonArgs.push_back("-lc++ "); 
    commonArgs.push_back("-lSystem"); 

    for (auto && framework : cppCompileTask.frameworks)
    {
        commonArgs.push_back("-framework");
        commonArgs.push_back(framework);
    }
    for (auto && libraryPath : cppCompileTask.libraryPaths)
    {
        commonArgs.push_back("-L" + fs::absolute(fs::path(libraryPath), absRootPath).string());
    }
    for (auto && library : cppCompileTask.libraries)
    {
        commonArgs.push_back("-l" + library);
    }
    for (auto && file : filesFound)
    {
        auto objectFile = createCorrespondingPath(fs::change_extension(file,file.extension().string() + ".o"), absRootPath, objectsOutputRootPath);
        commonArgs.push_back(objectFile.string());
    }

    std::string outputFilename = "VxGraphics";
    if (library)
    {
        outputFilename = "lib" + outputFilename + ".dylib";
    }
    auto stdOutFilePath = fs::absolute(fs::path(outputFilename + ".build.log"), binOutputRootPath);
    auto outputFilePath = fs::absolute(fs::path(outputFilename), binOutputRootPath);

    auto compilerProcess = std::make_shared<Process>();
    compilerProcess->description = "Linker";
    compilerProcess->exePath = linkerPath;
    compilerProcess->args = commonArgs;
    compilerProcess->args.push_back("-o");
    compilerProcess->args.push_back(outputFilePath.string());
    compilerProcess->stdOutPath = stdOutFilePath;
    processManager->processQueue.push(compilerProcess);
}

void createExeLinkerProcessRequest(const std::shared_ptr<ProcessManager> & processManager, const CppCompileTask & cppCompileTask, const fs::path & absRootPath)
{
    createLinkerProcessRequest(processManager, cppCompileTask, absRootPath, false);
}

void createLibLinkerProcessRequest(const std::shared_ptr<ProcessManager> & processManager, const CppCompileTask & cppCompileTask, const fs::path & absRootPath)
{
    createLinkerProcessRequest(processManager, cppCompileTask, absRootPath, true);
}


int main(int argc, char *argv[])
{
    auto debugMode = false;
    for (int i = 0; i < argc; i++)
    {
        if (strcasecmp(argv[i],"DEBUG")==0)
        {
            debugMode = true;
        }
    }
    
    auto currentPath = fs::current_path();

    auto rootPath = fs::path("../../../");
    auto absRootPath = fs::absolute(rootPath, currentPath);
    AssertExists(absRootPath);
    absRootPath = fs::canonical(absRootPath);

    CppCompileTask cppCompileTask;
    cppCompileTask.forceCppRecompilation = true;
    cppCompileTask.preCompileHeaders = false;
    cppCompileTask.cppCompilerPath = "clang++";
    //cppCompileTask.sourcePaths.push_back("./src/**.cpp");
    cppCompileTask.sourcePaths.push_back("./src/vxGraphics/**.cpp");
    cppCompileTask.sourcePaths.push_back("./src/vxApplication/**.cpp");
    cppCompileTask.sourcePaths.push_back("./src/vxCommon/**.cpp");
    //cppCompileTask.inputExtensions.push_back(".cpp");
    cppCompileTask.includePaths.push_back("./include/");
    cppCompileTask.includePaths.push_back("./extern/boost/boost/");
    cppCompileTask.includePaths.push_back("./extern/vulkansdk/macos/macOS/include/");
    cppCompileTask.options.push_back("-D_DEBUG");
    cppCompileTask.options.push_back("-DVK_USE_PLATFORM_MACOS_MVK");
    cppCompileTask.options.push_back("-DVK_USE_PLATFORM_METAL_EXT");
    cppCompileTask.options.push_back("-v");
    cppCompileTask.options.push_back("-g");
    cppCompileTask.outputPath = "./build/";
    cppCompileTask.frameworks.push_back("Cocoa");
    cppCompileTask.frameworks.push_back("OpenGL");
    cppCompileTask.frameworks.push_back("IOKit");
    cppCompileTask.frameworks.push_back("CoreVideo");
    cppCompileTask.libraryPaths.push_back("./extern/vulkansdk/macos/macOS/lib");
    cppCompileTask.libraries.push_back("vulkan");

    auto processManager = std::make_shared<ProcessManager>();
    processManager->maxCurrentProcesses = 8;

    int exitCode = 0;
    bool containsFailures;

    auto outputRootPath = absolutePath(cppCompileTask.outputPath, absRootPath);
    auto headersOutputRootPath = createSubDirectory(outputRootPath, "/temp/headers/");
    auto copyHeaderLogPath = fs::absolute("copyHeaders.build.log", headersOutputRootPath);


    GlslCompileTask glslCompileTask;
    glslCompileTask.forceRecompilation = false;
    glslCompileTask.glslCompilerPath = fs::absolute("./extern/vulkansdk/macos/macOS/bin/glslc", absRootPath).string();
    glslCompileTask.outputPath = "./build/";
    glslCompileTask.sourcePaths.push_back("./src/vxGraphics/**");
    createGlslCompileProcessRequest(processManager, glslCompileTask, absRootPath);
    processJobs(processManager);
    printCompletedProcesses(processManager->completedProcesses, containsFailures);
    processManager->completedProcesses.clear();

    if (cppCompileTask.preCompileHeaders)
    {
        auto copyHeaderLog = std::ofstream(copyHeaderLogPath.c_str());
        if (copyHeaderLog.is_open())
        {
            copyHeaderFilesForPreCompilation(processManager, cppCompileTask, absRootPath, &copyHeaderLog);
            processJobs(processManager);
            copyHeaderLog.close();
        }
        printCompletedProcesses(processManager->completedProcesses, containsFailures);
        processManager->completedProcesses.clear();
    }

    auto compileCommandsJsonPath = fs::absolute(".vscode/compile_commands.json", absRootPath);
    auto compilerComandsJson = std::ofstream(compileCommandsJsonPath.c_str());
    if (compilerComandsJson.is_open())
    {
        compilerComandsJson << "[\n";

        if (cppCompileTask.preCompileHeaders)
        {
            createHppPreCompileProcessRequest(processManager, cppCompileTask, absRootPath, compilerComandsJson);
            processJobs(processManager);
            printCompletedProcesses(processManager->completedProcesses, containsFailures);
            processManager->completedProcesses.clear();
        }

        containsFailures = false;
        createCppCompileProcessRequest(processManager, cppCompileTask, absRootPath, compilerComandsJson);
        processJobs(processManager);
        printCompletedProcesses(processManager->completedProcesses, containsFailures);
        processManager->completedProcesses.clear();

        compilerComandsJson << "  {}\n";
        compilerComandsJson << "]\n";
        compilerComandsJson.close();
    }

    // if (!containsFailures)
    // {
    //     createExeLinkerProcessRequest(processManager, cppCompileTask, absRootPath);
    //     processJobs(processManager);
    //     printCompletedProcesses(processManager->completedProcesses, containsFailures);
    //     processManager->completedProcesses.clear();
    // }

    if (!containsFailures)
    {
        createLibLinkerProcessRequest(processManager, cppCompileTask, absRootPath);
        processJobs(processManager);
        printCompletedProcesses(processManager->completedProcesses, containsFailures);
        processManager->completedProcesses.clear();
    }

    /*
    TODO: 
        cp libvulkan.1.1.114.dylib libvulkan.dylib
        cp libvulkan.1.1.114.dylib libvulkan.1.dylib
        The link was not working for XCode Framework reference


    "/Library/Developer/CommandLineTools/usr/bin/dsymutil" -o /Users/vitorciaramella/Documents/GitHub/vx/build/bin/vxBuild/vxBuild.dSYM /Users/vitorciaramella/Documents/GitHub/vx/build/bin/vxBuild/vxBuild
    
    -o /Users/vitorciaramella/Documents/GitHub/vx/build/bin/vxGraphics/vxGraphics

    linker
    to export dynamic library
        -E
    reference libraries
        -L searchDir
        -l namespec
            search a directory for a library called libnamespec.so before searching for one called libnamespec.a
        -l :filename
            search the library path for a file called filename, otherwise it will search the library path for a file called libnamespec.a

     /Library/Developer/CommandLineTools/usr/lib/clang/10.0.1/lib/darwin/libclang_rt.osx.a
    */

    if (debugMode)
    {
        std::cout << "Press any key to close...";
        std::getchar();
    }
    return containsFailures ? 1 : 0;
}

