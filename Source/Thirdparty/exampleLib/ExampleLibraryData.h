#pragma once
#ifndef DLLEXPORT
    #define DLLEXPORT __declspec(dllexport)
#endif
#include <string>

class DLLEXPORT ExampleLibraryData {
    
    std::string _name;
    int _age;
    
public:
    
    ExampleLibraryData();
    
    void setNameAge(const std::string &nm, int ag);
    
    const std::string &name() const;
    const int &age() const;
    
};