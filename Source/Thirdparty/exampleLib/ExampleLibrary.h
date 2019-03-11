#if defined _WIN32 || defined _WIN64
#define DLLIMPORT __declspec(dllimport)
#else
#define DLLIMPORT
#endif

#include <string>
class ExampleLibraryData;

#ifdef __cplusplus		//if C++ is used convert it to C to prevent C++'s name mangling of method names
extern "C"
{
#endif

DLLIMPORT void ExampleLibraryFunction(const ExampleLibraryData &msg);

DLLIMPORT bool ExampleLibraryTestDirectory(const std::string &name);

DLLIMPORT bool ExampleLibraryTestHdf(const std::string &filename);

#ifdef __cplusplus
}
#endif
