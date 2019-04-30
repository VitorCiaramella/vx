#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <vector>
#include <iostream>

using std::cout;
namespace fs = boost::filesystem;

#define AssertExists(path) \
    if (!fs::exists(path)) \
    { \
        printf("Error: File/Directory does not exist: %s\n", path.c_str()); \
        return -1; \
    }

void listAllFiles(const fs::path & path, std::vector<std::string> & filesFound)
{
    if (fs::is_regular_file(path))
    {
        filesFound.push_back(path.string());
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


    auto filesFound = std::vector<std::string>();

    try { 
    for (auto && sourcePath : sourcePaths)
    {
        for (fs::directory_entry & x : fs::directory_iterator(currentPath))
        {
            cout << "    " << x.path() << '\n'; 
        }


        auto fsSourcePath = fs::absolute(sourcePath, absRootPath);
        if (!fs::exists(fsSourcePath))
        {
            printf("Error: File/Directory does not exist: %s\n", fsSourcePath.c_str());
            continue;
        }
        fsSourcePath = fs::canonical(fsSourcePath);
        listAllFiles(fsSourcePath, filesFound);
    }
    }
    catch(boost::filesystem::filesystem_error &ex){std::cout << ex.what() << "\n";}

    for (auto && fileFound : filesFound)
    {
        printf("%s\n", fileFound.c_str());
    }


    return 0;
}

