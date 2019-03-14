#pragma once
#ifndef DLLEXPORT
    #define DLLEXPORT __declspec(dllexport)
#endif
#include <string>

class ABCD;

class DLLEXPORT ExampleLibraryData {
    
    ABCD *_data;
    
public:
    
    ExampleLibraryData();
    virtual ~ExampleLibraryData();
    
   	void setNameAge(const std::string &nm, int ag);
    
    const std::string &name() const;
    const int &age() const;
    
};