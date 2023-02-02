#include "SECTIONS.h"

/* 
Does nothing in Native mode.
Used for invoking firmware updates. Depending on the hardware can trigger pins by buttons or check the USB connection for 
switching to update mode.
*/
void fw_update() BOOTLOADER_MEMORY_SECTION;

void fw_update()
{

}