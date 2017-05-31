#ifndef _EMERALD_TYPES_H
#define _EMERALD_TYPES_H

#include <cstdint>

namespace Emerald {

    typedef uint32_t emerald_long;
    typedef uint16_t emerald_id;
    static constexpr emerald_id invalid_id = 0xFFFF;
    static constexpr emerald_long comp_type_mask = 0xFFFF0000;
    static constexpr emerald_long comp_id_mask = 0x0000FFFF;

};

#endif // _EMERALD_TYPES_H
