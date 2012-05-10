#pragma once

/**
 * Class to be overloaded by any class willing for a string name
 */

#include <string>

class NamedClass {
public:
    NamedClass(std::string name):
        name(name)
    {}

    virtual ~NamedClass() {}

    std::string getName() const {return name;}
    void setName(std::string n) {name = n;}

protected:
    std::string name;
};
