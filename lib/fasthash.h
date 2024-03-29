// https://burtleburtle.net/bob/hash/integer.html
#include <cstdint>
const inline uint32_t fasthash( uint32_t a) {
    a = (a ^ 61) ^ (a >> 16);
    a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);
    return a;
}