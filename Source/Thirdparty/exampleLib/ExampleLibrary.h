#if defined _WIN32 || defined _WIN64
#define DLLIMPORT __declspec(dllimport)
#else
#define DLLIMPORT
#endif

#include <string>
class ExampleLibraryData;

DLLIMPORT void ExampleLibraryFunction(const ExampleLibraryData &msg);

DLLIMPORT bool ExampleLibraryTestDirectory(const std::string &name);