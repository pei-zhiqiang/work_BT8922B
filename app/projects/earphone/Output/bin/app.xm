#include "config.h"
depend(0x01010200);
setflash(1, FLASH_SIZE, FLASH_ERASE_4K, FLASH_DUAL_READ, FLASH_QUAD_READ);
setspace(0x6000);
setkey(FUNCKEY_TWS);
addkey(FUNCKEY_ANC);
addkey(FUNCKEY_ANC_HYBRID);
addkey(FUNCKEY_SNDP_NS);
addkey(FUNCKEY_SNDP_DMIC);
addkey(FUNCKEY_LOW_LATENCY);
make(dcf_buf, header.bin, app.bin, res.bin, xcfg.bin, updater.bin);
save(dcf_buf, app.dcf);