#include "include.h"
#include "bsp_led.h"

#if LED_DISP_EN

typedef struct {
    u8 rled_sta;    //红灯 亮-灭控制
    u8 bled_sta;    //蓝灯 亮-灭控制
    u8 unit;        //50ms单位 (范围：1~255)
    u8 period;      //间隔周期 (范围：0~255)
    struct {
        u16 offset  : 15;
        u16 init    : 1;
    };
    u16 circle;

    bool lowbat;
} led_cb_t;

led_cb_t led_sync AT(.buf.led);
led_cb_t led_cb AT(.buf.led);
led_cb_t led_lb AT(.buf.led);       //low battery
void bt_set_sync_info(u8 *buf);

void m_led2_port_init(gpio_t *g, bool ext_res);
void m_port_2led_scan(gpio_t *g);
void led_cfg_port_init(gpio_t *g);
void led_cfg_set_onoff(gpio_t *g, bool onoff);


AT(.com_text.led_disp)
void led_scan(u32 tick)
{
    u16 offset;
    led_cb_t *s;

    if (!is_led_scan_enable()) {
        return;
    }

    s = (led_cb.lowbat)? &led_lb : &led_cb;
    if(s->unit == 0 || s->circle == 0) {
		if (RLED_LOWBAT_FOLLOW_EN) {
			if(led_cb.lowbat){
				led_cb.lowbat = false;
			}
		}
        return;
    }

    bool init = (bool)(tick & BIT(31));
    if(init != s->init) {           //wrap around
        u16 old_offset = s->offset;
        old_offset += (0xffffffff / 50) % 50;
        s->offset = old_offset % s->circle;
        s->init = init;
    }

    tick = tick/50;
    offset = (tick + s->circle - s->offset) % s->circle;
    if(offset < (s->unit * 8)) {
        if(offset % s->unit == 0) {
            u8 bcnt = offset/s->unit;
            if (s->bled_sta & BIT(bcnt)) {
                bled_set_on();
            } else {
                bled_set_off();
            }
            if (s->rled_sta & BIT(bcnt)) {
                rled_set_on();
            } else {
                rled_set_off();
            }
        }
    } else {
        //只亮一次
        if(s->circle == 0x7fff) {
            s->unit = 0;
        }
    }
}

//获取LED当前设置的状态
AT(.com_text.led_disp)
u8 get_led_sta(u32 type)
{
    if (type) {
        return led_cb.rled_sta;
    } else {
        return led_cb.bled_sta;
    }
}

AT(.com_text.led_disp)
void led_cfg_set_on(gpio_t *g)
{
    led_cfg_set_onoff(g, 1);
}

AT(.com_text.led_disp)
void led_cfg_set_off(gpio_t *g)
{
    led_cfg_set_onoff(g, 0);
}

#if USER_ADKEY_MUX_LED
AT(.text.led_disp)
void led_mux_cfg_port_init(gpio_t *g)
{
    if ((g == NULL) || (g->sfr == NULL)) {
        return;
    }
    g->type = 2;    //ADKEY_MUX
    led_cfg_port_init(g);
}
#endif // USER_ADKEY_MUX_LED

#if LED_PWR_EN
//一个IO口推两个LED灯
void led2_port_init(gpio_t *g)
{
    m_led2_port_init(g, !xcfg_cb.port_2led_resless_en);
}

//每ms扫描一次
AT(.com_text.led_disp)
void port_2led_scan(void)
{
    if ((!sys_cb.port2led_en)) {
        return;
    }
    m_port_2led_scan(&bled_gpio);
}

//1个IO推两个灯，充电状态下不进sleep，否则充电灯异常。
AT(.sleep_text.led)
bool port_2led_is_sleep_en(void)
{
    if ((sys_cb.port2led_en) && (CHARGE_DC_IN())) {
        return false;
    }
    return true;
}
#endif // LED_PWR_EN

AT(.com_text.led_disp)
void bled_set_off(void)
{
    if (!sys_cb.charge_bled_flag) {       //充满状态优先控制
        LED_SET_OFF();
    }
}

AT(.com_text.led_disp)
void bled_set_on(void)
{
   if (!sys_cb.charge_bled_flag) {        //充满状态优先控制
        LED_SET_ON();
    }
}

AT(.com_text.led_disp)
void rled_set_off(void)
{
    if (!sys_cb.charge_sta) {             //充电状态优先控制
        LED_PWR_SET_OFF();
    }
}

AT(.com_text.led_disp)
void rled_set_on(void)
{
   if (!sys_cb.charge_sta) {               //充电状态优先控制
        LED_PWR_SET_ON();
    }
}

AT(.text.led_disp)
void led_init(void)
{
    LED_INIT();
    LED_PWR_INIT();
    memset(&led_sync, 0, sizeof(led_sync));
    memset(&led_cb, 0, sizeof(led_cb));
    memset(&led_lb, 0, sizeof(led_lb));
}

//蓝灯灭
AT(.text.led_disp)
void led_off(void)
{
    led_cb.bled_sta = 0x00;
    bled_set_off();
}

//蓝灯亮
AT(.text.led_disp)
void led_on(void)
{
    led_cb.bled_sta = 0xff;
    bled_set_on();
}

AT(.text.led_disp)
void rled_on(void)
{
    led_cb.rled_sta = 0xff;
    rled_set_on();
}

AT(.text.led_disp)
void rled_off(void)
{
    led_cb.rled_sta = 0x00;
    rled_set_off();
}

//拔出充电红灯灭
AT(.charge_text.det)
void charge_led_off(void)
{
    LED_PWR_SET_OFF();
}

//充电红灯亮
AT(.charge_text.det)
void charge_led_on(void)
{
    led_cb.bled_sta = 0x00;     //充电关蓝灯亮红灯
    LED_SET_OFF();
    LED_PWR_SET_ON();
}

//充满蓝灯常亮
AT(.charge_text.det)
void charge_bled_on(void)
{
    LED_SET_ON();
}


ALIGNED(512)
void led_set_sta_p(const led_cfg_t *cfg, led_cb_t *s)
{
    led_cfg_t sta;
    memcpy(&sta, cfg, sizeof(sta));

    GLOBAL_INT_DISABLE();
    u32 tick = timer_get_tick();

    s->rled_sta = sta.redpat;
    s->bled_sta = sta.bluepat;
    s->unit     = sta.unit;
    s->period   = sta.cycle;

    if (s->period == 0xff) {
        s->circle = 0x7fff;
    } else {
        s->circle = s->unit * 8 + s->period;
    }

    s->init = (bool)(tick & BIT(31));
    s->offset = (tick/50 + 1) % s->circle;
    GLOBAL_INT_RESTORE();
}

void led_set_sta_lowbat(const led_cfg_t *cfg)
{
    rled_set_off();
    bled_set_off();
    led_set_sta_p(cfg, &led_lb);
}

void led_set_sta_normal(const led_cfg_t *cfg)
{
    if(!led_cb.lowbat) {
        rled_set_off();
        bled_set_off();
    }
    led_set_sta_p(cfg, &led_cb);
}

AT(.com_text.led_disp)
void led_set_sync_sta(uint8_t *buf)
{
    GLOBAL_INT_DISABLE();
    memcpy(&led_cb, buf, 8);
    GLOBAL_INT_RESTORE();
}

ALIGNED(64)
void led_get_sync_info(uint8_t *buf)
{
    GLOBAL_INT_DISABLE();
    memcpy(buf, &led_sync, 8);
    GLOBAL_INT_RESTORE();
}

void led_cal_sync_info(const led_cfg_t *cfg)
{
    led_set_sta_p(cfg, &led_sync);
}

#if (LED_LOWBAT_EN || RLED_LOWBAT_FOLLOW_EN)
AT(.rodata.led.cfg)
const led_cfg_t led_cfg_lowbat          = {0xaa, 0x00, 0x06, 0x00};     //红灯300ms周期闪烁
AT(.rodata.led.cfg)
const led_cfg_t led_cfg_lowbat_follow   = {0x0a, 0x00, 0x02, 0xff};     //红灯间隔100ms闪2次

AT(.text.led_disp)
void led_lowbat(void)
{
    if (!led_cb.lowbat) {
        if (xcfg_cb.led_lowbat_config_en) {
            led_set_sta_lowbat((const led_cfg_t *)&xcfg_cb.led_lowbat);
        } else {
            led_set_sta_lowbat(&led_cfg_lowbat);
        }
        led_cb.lowbat = true;
    }
}

AT(.text.led_disp) ALIGNED(128)
void led_lowbat_recover(void)
{
    if(led_cb.lowbat){
        GLOBAL_INT_DISABLE();
        rled_set_off();
        bled_set_off();
        led_cb.lowbat = false;
        GLOBAL_INT_RESTORE();
    }
}

void led_lowbat_follow_warning(void)
{
    if (!led_cb.lowbat) {
        led_set_sta_lowbat(&led_cfg_lowbat_follow);
        led_cb.lowbat = true;
    }
}
#endif // LED_LOWBAT_EN

#endif
