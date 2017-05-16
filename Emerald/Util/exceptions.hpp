#ifndef _EMERALD_EXCEPTIONS_H
#define _EMERALD_EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace Emerald {
    class bad_id : public std::logic_error {
    public:
        bad_id(const std::string& error) : std::logic_error(error) {}
    };

    class bad_system : public std::logic_error {
    public:
        bad_system(const std::string& error) : std::logic_error(error) {}
    };

    class bad_component : public std::logic_error {
    public:
        bad_component(const std::string& error) : std::logic_error(error) {}
    };
};

#endif // _EMERALD_EXCEPTIONS_H
