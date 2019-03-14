#include "ABCD.h"

ABCD::ABCD() : _name("unknown"),
_age(0)
{}

ABCD::~ABCD()
{}

void ABCD::setNameAge(const std::string &nm, int ag)
{
    _name = nm;
    _age = ag;
}

const std::string &ABCD::name() const
{ return _name; }

const int &ABCD::age() const
{ return _age; }
