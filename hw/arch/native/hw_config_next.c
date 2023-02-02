#include "SECTIONS.h"

/* 
Does nothing in Native mode.
Could be used for more advanced initialization of HW. DMA or Analog pins for getting entropy.
*/
void hw_config_next() BOOTLOADER_MEMORY_SECTION;

void hw_config_next()
{

}