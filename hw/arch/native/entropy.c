#include <time.h>
#include <stdint.h>


uint32_t get_entropy()
{
    srand(time(NULL));
    return rand();
}