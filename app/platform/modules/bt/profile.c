#include "include.h"
#include "api.h"

extern const uint8_t   sdp_spp_service_record[];
extern const uint8_t   sdp_spp_dueros_service_record[];
extern const uint8_t   sdp_spp_proc_test_service_record[];
extern const uint8_t   sdp_spp_dueros_ota_service_record[];
extern const uint8_t   sdp_hfp_service_record[];
extern const uint8_t   sdp_hsp_service_record[];
extern const uint8_t   sdp_avdtp_service_record[];
extern const uint8_t   sdp_avctp_controller_service_record[];
extern const uint8_t   sdp_avctp_target_category1_service_record[];
extern const uint8_t   sdp_avctp_target_category2_service_record[];
extern const uint8_t   sdp_hid_keyboard_service_record[];
extern const uint8_t   sdp_hid_consumer_service_record[];
extern const uint8_t   sdp_hid_tscreen_service_record[];
extern const uint8_t   sdp_hid_tscreen_ext_service_record[];
extern const uint8_t   sdp_devic_id_service_record[];

typedef struct {
    void    *item;
    uint32_t service_record_handle;
    uint8_t *service_record;
} service_record_item_t;

#if DUEROS_SUPPORT_EN
service_record_item_t spp_sdp_record_item = {
    .service_record_handle  = 0x50010001,
    .service_record         = (uint8_t *)sdp_spp_dueros_service_record,
};

service_record_item_t spp_proc_test_sdp_record_item = {
    .service_record_handle  = 0x50010002,
    .service_record         = (uint8_t *)sdp_spp_proc_test_service_record,
};

service_record_item_t spp_dueros_ota_sdp_record_item = {
    .service_record_handle  = 0x50010003,
    .service_record         = (uint8_t *)sdp_spp_dueros_ota_service_record,
};
#elif BT_SPP_EN
service_record_item_t spp_sdp_record_item = {
    .service_record_handle  = 0x00010001,
    .service_record         = (uint8_t *)sdp_spp_service_record,
};
#endif


#if BT_HFP_EN
service_record_item_t hfp_sdp_record_item = {
    .service_record_handle  = 0x00010002,
    .service_record         = (uint8_t *)sdp_hfp_service_record,
};
#endif

#if BT_HSP_EN
service_record_item_t hsp_sdp_record_item = {
    .service_record_handle  = 0x00010008,
    .service_record         = (uint8_t *)sdp_hsp_service_record,
};
#endif

#if BT_A2DP_EN
service_record_item_t avdtp_sdp_record_item = {
    .service_record_handle  = 0x00010003,
    .service_record         = (uint8_t *)sdp_avdtp_service_record,
};

service_record_item_t avctp_ct_sdp_record_item = {
    .service_record_handle  = 0x00010004,
    .service_record         = (uint8_t *)sdp_avctp_controller_service_record,
};

service_record_item_t avctp_tg_sdp_record_item = {
    .service_record_handle  = 0x00010005,
    .service_record         = (uint8_t *)sdp_avctp_target_category2_service_record,
};
#endif // BT_A2DP_EN


#if BT_HID_EN
service_record_item_t hid_sdp_record_item = {
    .service_record_handle  = 0x00010006,
#if (BT_HID_TYPE == 0)
    .service_record         = (uint8_t *)sdp_hid_consumer_service_record,
#elif (BT_HID_TYPE == 1)
    .service_record         = (uint8_t *)sdp_hid_keyboard_service_record,
#elif (BT_HID_TYPE == 2)
    .service_record         = (uint8_t *)sdp_hid_tscreen_service_record,
#else
    .service_record         = ...
#endif
};

service_record_item_t device_id_sdp_record_item = {
    .service_record_handle  = 0x00010007,
    .service_record         = (uint8_t *)sdp_devic_id_service_record,
};
#endif // BT_HID_EN

void bt_init_lib(void)
{
    uint32_t profile = cfg_bt_support_profile;
#if BT_SPP_EN
    if (profile & PROF_SPP) {
#if DUEROS_SUPPORT_EN
		spp_support_mul_server(1);
#endif
        spp_init();
        sdp_add_service(&spp_sdp_record_item);
#if DUEROS_SUPPORT_EN
		sdp_add_service(&spp_proc_test_sdp_record_item);
		sdp_add_service(&spp_dueros_ota_sdp_record_item);
#endif
    }
#endif

#if BT_HFP_EN
    if (profile & PROF_HFP) {
        hfp_hf_init();
        sdp_add_service(&hfp_sdp_record_item);
    }
#endif

	hsp_hs_init_var();
#if BT_HSP_EN
    if (profile & PROF_HSP) {
        hsp_hs_init();
        sdp_add_service(&hsp_sdp_record_item);
    }
#endif

#if BT_A2DP_EN
    if (profile & PROF_A2DP) {
        a2dp_init();
        avctp_tg_sdp_record_item.service_record         = (uint8_t *)sdp_avctp_target_category2_service_record;
        sdp_add_service(&avdtp_sdp_record_item);
        sdp_add_service(&avctp_ct_sdp_record_item);
        sdp_add_service(&avctp_tg_sdp_record_item);
    }
#endif

#if BT_HID_EN
    if (profile & PROF_HID) {
        hid_device_init();
        sdp_add_service(&hid_sdp_record_item);
        sdp_add_service(&device_id_sdp_record_item);
    }
#endif

#if BT_PBAP_EN
    // init PBAP
    if (profile & PROF_PBAP) {
        // init PBAP Client
        pbap_client_init();
    }
#endif
}
