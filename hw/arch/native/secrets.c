#include <stdint.h>
#include "SECTIONS.h"

ROM_MEMORY_SECTION uint8_t bootloader_check_sum[32] = {"AVDS-AVDS-AVDS-AVDS-AVDS-AVDS-A"};
ROM_MEMORY_SECTION uint8_t master_key[32] = {"AVDS-AVDS-AVDS-AVDS-AVDS-AVDS-A"};
FW_MEMORY_SECTION uint8_t fw_check_sum[32] = {"AVDS-AVDS-AVDS-AVDS-AVDS-AVDS-A"};