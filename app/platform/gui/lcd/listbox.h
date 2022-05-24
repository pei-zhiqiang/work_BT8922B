#ifndef _LISTBOX_H
#define _LISTBOX_H

#define LB_NO_TITLE     0x01    //是否需要菜单标题栏
#define LB_RES          0x02    //文字模式还是资源模式
#define LB_NO_ICON      0x04    //是否不显示图标
#define LB_PAGE         0x08    //使用翻页菜单

enum {
    LB_STA_DISP     =   0,
    LB_STA_OK_KEY,
    LB_STA_DEL_KEY,
    LB_STA_EXIT     =   64,
    LB_STA_DEV_IN,
    LB_STA_DEV_OUT,
};

typedef struct {
    u8  icon_en     :   1,      //是否显示图标
        roll_en     :   1;      //滚动菜单还是翻页菜单
    u8  sta;
    u8  item_len;               //菜单项的字符串长度
    u8  page_nitem;             //每页显示条数
    u8  evt_dev;
    u16 disp_index;             //当前显示的菜单项
    u16 cindex;                 //当前菜单
    u16 total;
    u16 page_index;
    char *view;                 //列表内容
    char *(*get_list_view)(u16 index, u8 count);
} listbox_cb_t;
extern listbox_cb_t listbox_cb;

void listbox_next_item(void);
void listbox_prev_item(void);
u16 gui_listbox(u16 index, u16 total, u8 mode, u8 item_len, char *(*get_list_view)(u16 index, u8 count));
#endif // _LISTBOX_H
