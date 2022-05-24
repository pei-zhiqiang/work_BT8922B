#ifndef _DIALOGBOX_H
#define _DIALOGBOX_H

#define DB_TITLE        0x01    //是否需要菜单标题栏
#define DB_RES          0x02    //文字模式还是资源模式
#define DB_YESNO        0x80    //两个按键的模式
#define DB_OK           0x40    //单OK按键的形式

enum {
    DB_STA_DISP     =   0,
    DB_STA_OK_KEY,
    DB_STA_CANCEL_KEY,
    DB_STA_EXIT     =   64,
    DB_STA_DEV_IN,
    DB_STA_DEV_OUT,
};

typedef struct {
    u8  title_en     :   1,         //是否显示对话框标题
        yesno_en     :   1,         //是否显示YES与NO两个按键，否则只显示YES按键
        icon_en      :   1,         //是否显示对话框图标
        res_en       :   1;         //图片模式还是文字模式
    u8  disp_index;
    u8  index;
    u8  sta;
    u8  evt_dev;
    const char *text;                      //对话框内容
} dialogbox_cb_t;
extern dialogbox_cb_t dialogbox_cb;

int gui_dialogbox(const char *text, u8 mode);
#endif // _DIALOGBOX_H
