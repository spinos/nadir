#include "ExampleLibraryData.h"

ExampleLibraryData::ExampleLibraryData() : _name("unknown"),
_age(0)
{}

void ExampleLibraryData::setNameAge(const std::string &nm, int ag)
{
    _name = nm;
    _age = ag;
}

const std::string &ExampleLibraryData::name() const
{ return _name; }

const int &ExampleLibraryData::age() const
{ return _age; }
