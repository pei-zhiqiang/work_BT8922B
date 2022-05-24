#ifndef _FUNC_HEARAID_H
#define _FUNC_HEARAID_H

typedef struct {
    u8  pause       :   1,
        rec_en      :   1,
        draw_update :   1;
}func_hearaid_t;
extern func_hearaid_t f_hearaid;

void func_hearaid_message(u16 msg);
void func_hearaid_stop(void);
void func_hearaid_start(void);
void func_hearaid_pause_play(void);
void func_hearaid_mp3_res_play(u32 addr, u32 len);

#if (GUI_SELECT != GUI_NO)
void func_hearaid_display(void);
void func_hearaid_enter_display(void);
void func_hearaid_exit_display(void);
#else
#define func_hearaid_display()
#define func_hearaid_enter_display()
#define func_hearaid_exit_display()
#endif

#endif // _FUNC_HEARAID_H
