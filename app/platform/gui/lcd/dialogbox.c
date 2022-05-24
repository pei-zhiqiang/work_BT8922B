#include "include.h"
#include "dialogbox.h"

#if GUI_LCD_EN
dialogbox_cb_t dialogbox_cb AT(.buf.dialogbox);

void dialogbox_process(void)
{
    func_process();

    //从其它子控件显示回到主界面
    if (gui_box_process() && (!box_cb.disp_sta)) {
        dialogbox_cb.disp_index = 0xff;
    }

    //退出模式
    if (func_cb.sta == FUNC_NULL) {
        dialogbox_cb.sta = DB_STA_EXIT;
    }
}

int gui_dialogbox(const char *text, u8 mode)
{
    dialogbox_cb_t *s = &dialogbox_cb;

    s->disp_index = 0xff;
    s->index = 0;
    s->title_en = (mode & DB_TITLE) ? 1 : 0;
    s->yesno_en = (mode & DB_YESNO) ? 1 : 0;
    s->res_en   = (mode & DB_RES)   ? 1 : 0;
    s->icon_en  = 0;
    s->sta      = DB_STA_DISP;
    s->text     = text;
    if (s->yesno_en) {
        s->index = 1;               //只有一个确认按键
    }
    while (s->sta == DB_STA_DISP) {
        dialogbox_process();
        dialogbox_message(msg_dequeue());
        dialogbox_display();
    }

    if (s->sta == DB_STA_OK_KEY) {
        return 1;
    } else if (s->sta == DB_STA_CANCEL_KEY) {
        return 0;
    }
    return -1;
}
#endif // GUI_LCD_EN
