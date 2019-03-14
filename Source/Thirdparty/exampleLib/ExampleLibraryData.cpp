#include "ExampleLibraryData.h"
#include "ABCD.h"

ExampleLibraryData::ExampleLibraryData()
{ _data = new ABCD; }

ExampleLibraryData::~ExampleLibraryData()
{ delete _data; }

void ExampleLibraryData::setNameAge(const std::string &nm, int ag)
{
    _data->setNameAge(nm, ag);
}

const std::string &ExampleLibraryData::name() const
{ return _data->name(); }

const int &ExampleLibraryData::age() const
{ return _data->age(); }
