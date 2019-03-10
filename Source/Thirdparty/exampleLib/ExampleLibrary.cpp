#if defined _WIN32 || defined _WIN64
    #include <Windows.h>

    #define DLLEXPORT __declspec(dllexport)
#else
    #include <stdio.h>
#endif

#ifndef DLLEXPORT
    #define DLLEXPORT
#endif

#include "ExampleLibraryData.h"

#include <iostream>
#include <string>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

DLLEXPORT void ExampleLibraryFunction(const ExampleLibraryData &msg)
{
    std::string fullmsg = boost::str(boost::format("hellow world %1% %2% of how to use dll") % msg._name % msg._age );
#if defined _WIN32 || defined _WIN64
	MessageBox(NULL, TEXT(fullmsg.c_str()), NULL, MB_OK);
#else
    printf("Hello World");
#endif
}

DLLEXPORT bool ExampleLibraryTestDirectory(const std::string &name)
{
    boost::filesystem::path apath(name);
    if (boost::filesystem::is_directory(apath))
        return true;
    return false;
}
