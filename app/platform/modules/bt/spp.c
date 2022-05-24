#include "include.h"
#include "api.h"

#if BT_SPP_EN
#define SPP_TX_BUF_INX      8
#define SPP_TX_BUF_LEN      256     //max=512
#define SPP_POOL_SIZE       (SPP_TX_BUF_LEN + sizeof(struct txbuf_tag)) * SPP_TX_BUF_INX


AT(.ble_buf.stack.spp)
uint8_t spp_tx_pool[SPP_POOL_SIZE];

void spp_txpkt_init(void)
{
    txpkt_init(&spp_tx, spp_tx_pool, SPP_TX_BUF_INX, SPP_TX_BUF_LEN);
    spp_tx.send_kick = spp_send_kick;
}

u16 get_spp_mtu_size(void)
{
    return SPP_TX_BUF_LEN;
}
#endif

