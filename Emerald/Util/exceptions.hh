#ifndef _EMERALD_EXCEPTIONS_H
#define _EMERALD_EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace Emerald {
    class BadID : public std::logic_error {
    public:
        BadID(const std::string& error) : std::logic_error(error) {}
    };

    class BadSystem : public std::logic_error {
    public:
        BadSystem(const std::string& error) : std::logic_error(error) {}
    };

    class BadComponent : public std::logic_error {
    public:
        BadComponent(const std::string& error) : std::logic_error(error) {}
    };

    class BadType : public std::logic_error {
    public:
        BadType(const std::string& error) : std::logic_error(error) {}
    };
};

#endif // _EMERALD_EXCEPTIONS_H
