#ifndef  _BSP_LED_H_
#define  _BSP_LED_H_

#define LED_SCAN_TIME       50  //单位ms

typedef struct {
    u8 redpat;
    u8 bluepat;
    u8 unit;
    u8 cycle;
} led_cfg_t;

#if LED_DISP_EN
    extern volatile int port2led_sta;
    void led_set_sta_normal(const led_cfg_t *cfg);
    void led_cal_sync_info(const led_cfg_t *cfg);
    void led_set_sync_sta(uint8_t *buf);
    void led_set_keep_sta(void);
    #if BT_TWS_EN
        #define led_set_sta(cfg)       bsp_tws_set_led(cfg)
    #else
        #define led_set_sta(cfg)       led_set_sta_normal(cfg)
    #endif

    u8 get_led_sta(u32 type);

    void led_cfg_port_init(gpio_t *g);
    void led_cfg_set_on(gpio_t *g);
    void led_cfg_set_off(gpio_t *g);

    void led_mux_cfg_port_init(gpio_t *g);
    #define led_mux_cfg_set_on(g)       led_cfg_set_on(g)
    #define led_mux_cfg_set_off(g)      led_cfg_set_off(g)

    void led_lowbat(void);
    void led_lowbat_follow_warning(void);
    void led_lowbat_recover(void);

    void led2_port_init(gpio_t *g);
    void bled2_set_on(gpio_t *g);
    void bled2_set_off(gpio_t *g);
    void rled2_set_on(gpio_t *g);
    void rled2_set_off(gpio_t *g);

#if LED_PWR_EN
    void port_2led_scan(void);
    bool port_2led_is_sleep_en(void);
#else
    #define port_2led_scan()
    #define port_2led_is_sleep_en()         1
#endif // LED_PWR_EN

    void bled_set_off(void);
    void bled_set_on(void);

    void led_init(void);
    void led_on(void);
    void rled_on(void);
    void led_off(void);
    void rled_off(void);
    void led_idle(void);
    void led_scan(u32 tick);
    void led_bt_init(void);
    void led_bt_idle(void);
    void led_bt_connected(void);
    void led_bt_connectting(void);
    void led_bt_reconnect(void);
    void led_bt_play(void);
    bool led_bt_sleep(void);
    void led_bt_ring(void);
    void led_bt_call(void);
    void led_bt_scan(void);
    void led_power_up(void);
    void led_power_down(void);
    void charge_led_off(void);
    void charge_led_on(void);
    void charge_bled_on(void);
    void rled_set_off(void);
    void rled_set_tog(void);
    void rled_set_on(void);

    void led_music_busy(void);
    void led_music_play(void);
    void led_aux_play(void);
    void led_fm_play(void);
    void led_fm_scan(void);
    void led_record(void);
#else
    #define led_cal_sync_info(cfg)
    #define led_set_sync_sta(buf)
    #define led_set_sta(cfg)
    #define led_cfg_set_sta(x)
    #define led_cfg_port_init(x)
    #define led_cfg_set_on(x)
    #define led_cfg_set_off(x)
    #define led_mux_cfg_port_init(x)
    #define led_mux_cfg_set_on(x)
    #define led_mux_cfg_set_off(x)
    #define get_led_sta(x)                  0

    #define led_set_keep_sta()
    #define bled_set_off()
    #define bled_set_on()

    #define led_init()
    #define led_on()
    #define rled_on()
    #define led_off()
    #define rled_off()
    #define led_idle()
    #define led_scan(n)
    #define led_bt_init()
    #define led_bt_idle()
    #define led_bt_connected()
    #define led_bt_reconnect()
    #define led_bt_connectting()
    #define led_bt_play()
    #define led_bt_sleep()                  0
    #define led_bt_ring()
    #define led_bt_call()
    #define led_bt_scan()
    #define led_power_up()
    #define led_power_down()
    #define charge_led_off()
    #define charge_led_on()
    #define charge_bled_on()
    #define led_lowbat()
    #define led_lowbat_follow_warning()
    #define led_lowbat_recover()

    #define led2_port_init(x)
    #define bled2_set_on(x)
    #define bled2_set_off(x)
    #define rled2_set_on(x)
    #define rled2_set_off(x)
    #define port_2led_scan()
    #define port_2led_is_sleep_en()         1

    #define led_music_busy()
    #define led_music_play()
    #define led_aux_play()
    #define led_fm_play()
    #define led_fm_scan()
    #define led_record()
#endif

#endif //_BSP_LED_H_
