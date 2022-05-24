#ifndef _FUNC_AUX_H
#define _FUNC_AUX_H

#define AUX2ADC_MASK            BIT(7)          //AUX经SDADC采样后再到DAC
#define AUX2PA_ADC_MASK         BIT(6)          //AUX模拟直通同时打开SDADC用于能量采样或录音

typedef struct {
    u8  pause       :   1,
        rec_en      :   1,
        disp_update :   1;
    u8  aux2adc;
}func_aux_t;
extern func_aux_t f_aux;

void func_aux_pause_play(void);
void func_aux_mp3_res_play(u32 addr, u32 len);
void func_aux_message(u16 msg);
void func_aux_stop(void);
void func_aux_start(void);

#if (GUI_SELECT != GUI_NO)
void func_aux_display(void);
void func_aux_exit_display(void);
void func_aux_enter_display(void);
#else
#define func_aux_display()
#define func_aux_exit_display()
#define func_aux_enter_display()
#endif


#endif // _FUNC_AUX_H
