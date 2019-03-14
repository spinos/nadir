#pragma once

#include <string>

class ABCD {

	std::string _name;
    int _age;

public:
	ABCD();
	virtual ~ABCD();

	void setNameAge(const std::string &nm, int ag);
    
    const std::string &name() const;
    const int &age() const;

};
