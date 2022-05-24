#include "include.h"
#include "func.h"
#include "func_hearaid.h"
#include "func_ble_aids.h"

#if FUNC_HEARAID_EN
void dac_aubufsize_set(u8 flag);
func_hearaid_t f_hearaid;

AT(.text.bsp.hearaid)
void func_hearaid_stop(void)
{
    dac_fade_out();
    dac_fade_wait();                    //等待淡出完成
    audio_path_exit(AUDIO_PATH_SPEAKER);
    bsp_hearaid_stop();
}

AT(.text.bsp.hearaid)
void func_hearaid_start(void)
{
    dac_fade_wait();                    //等待淡出完成
    bsp_hearaid_start();
    audio_path_init(AUDIO_PATH_SPEAKER);
    audio_path_start(AUDIO_PATH_SPEAKER);
    dac_aubufsize_set(2);
    dac_fade_in();
}

AT(.text.bsp.hearaid)
void func_hearaid_pause_play(void)
{
    if (f_hearaid.pause) {
        led_music_play();
        func_hearaid_start();
    } else {
        led_idle();
        func_hearaid_stop();
    }
    f_hearaid.pause ^= 1;
}

AT(.text.bsp.hearaid)
void func_hearaid_mp3_res_play(u32 addr, u32 len)
{
    if (len == 0) {
        return;
    }

#if MIC_REC_EN
    sfunc_record_pause();
#endif // MIC_REC_EN

    if (!f_hearaid.pause) {
        func_hearaid_stop();
        mp3_res_play(addr, len);
        func_hearaid_start();
    } else {
        mp3_res_play(addr, len);
    }

#if MIC_REC_EN
    sfunc_record_continue();
#endif // MIC_REC_EN
}

AT(.text.bsp.hearaid)
void func_hearaid_setvol_callback(u8 dir)
{
    if (f_hearaid.pause) {
        func_hearaid_pause_play();
    }
}

AT(.text.func.hearaid)
void func_hearaid_process(void)
{
    func_process();
}

static void func_hearaid_enter(void)
{
    memset(&f_hearaid, 0, sizeof(f_hearaid));
    func_cb.mp3_res_play = func_hearaid_mp3_res_play;
    func_cb.set_vol_callback = func_hearaid_setvol_callback;

    func_hearaid_enter_display();
    led_music_play();

#if HEARAID_EN
    bsp_hearaid_init();
#endif
    func_hearaid_start();
}

static void func_hearaid_exit(void)
{
  /* 立即检测参数随存 */
  Algorithm_Par_Save(true, 0);

  func_hearaid_exit_display();
  led_idle();
  func_hearaid_stop();
  func_cb.last = FUNC_HEARAID;

  /* 取消算法EQ */
  if(1 == HW_EQ_Enable_Flag)
  {
    music_set_eq_by_res(RES_BUF_EQ_ALNORMAL_EQ, RES_LEN_EQ_ALNORMAL_EQ);
  }
}

AT(.text.func.hearaid)
void func_hearaid(void)
{
    printf("%s\n", __func__);

    func_hearaid_enter();
    /* 初始化BLE协议栈 */
#if USE_ALGORITHM_FUNCTION
    func_ble_aids_init();
#endif
    while (func_cb.sta == FUNC_HEARAID) {
        /* BLE协议栈解析 */
#if USE_ALGORITHM_FUNCTION
        ble_data_decode();
#endif
        func_hearaid_process();
        func_hearaid_message(msg_dequeue());
        func_hearaid_display();
    }

    func_hearaid_exit();
}

#endif // FUNC_SPEAKER_EN
