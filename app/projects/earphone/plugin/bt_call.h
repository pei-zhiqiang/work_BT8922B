#ifndef __BT_CALL
#define __BT_CALL


void bt_sco_aec_init(u8 *sysclk, aec_cb_t *aec, alc_cb_t *alc);
void bt_sco_far_nr_init(u8 *sysclk, nr_cb_t *nr);

void bt_sco_sndp_dm_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_bcns_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_dmns_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_ains2_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_sndp_sm_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_near_nr_dft_init(u8 *sysclk, nr_cb_t *nr);

void bt_sco_dump_init(u8 *sysclk, bt_voice_cfg_t *p);

#if BT_SNDP_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_sndp_sm_init(a, b)
	#define bt_sco_ns_process(a)            bt_sndp_process(a)
#elif BT_SCO_AINS2_EN
    #define bt_sco_ns_process(a)            bt_ains2_plus_process(a)
    #define bt_sco_near_nr_init(a, b)       bt_sco_ains2_init(a, b)
#elif BT_SCO_BCNS_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_bcns_init(a, b)
    #define bt_sco_ns_process(a)            bt_bcns_plus_process(a)
#elif BT_SCO_DMNS_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_dmns_init(a, b)
    #define bt_sco_ns_process(a)            bt_dmns_plus_process(a)
#elif BT_SNDP_DMIC_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_sndp_dm_init(a, b)
    #define bt_sco_ns_process(a)            bt_sndp_dm_process(a)
#else
    #define bt_sco_near_nr_init(a, b)       bt_sco_near_nr_dft_init(a, b)
    #define bt_sco_ns_process(a)
#endif

#endif
