#include "include.h"
#include "listbox.h"

#if GUI_LCD_EN
listbox_cb_t listbox_cb AT(.buf.listbox);

void listbox_process(void)
{
    func_process();

    //从其它子控件显示回到主界面
    if (gui_box_process() && (!box_cb.disp_sta)) {
        listbox_cb.disp_index = 0xffff;
    }

    //退出模式
    if (func_cb.sta == FUNC_NULL) {
        listbox_cb.sta = LB_STA_EXIT;
    }
}

void listbox_prev_item(void)
{
    u8 page_chg = 0;
    listbox_cb_t *s = &listbox_cb;

    if (s->cindex <= s->page_index) {
        if (s->roll_en) {
            s->page_index--;
        } else {
            s->page_index -= s->page_nitem;
        }
        page_chg = 1;
    }
    if (s->cindex == 0) {
        s->cindex = s->total;
        if (s->roll_en) {
            s->page_index = (s->total > s->page_nitem) ? s->total - s->page_nitem : 0;
        } else {
            s->page_index = ((s->total - 1) / s->page_nitem) * s->page_nitem;
        }
        page_chg = 1;
    }
    s->cindex--;
    if (page_chg) {
        s->view = (*s->get_list_view)(s->page_index, s->page_nitem);
    }
}

void listbox_next_item(void)
{
    u16 num;
    u8 page_chg = 0;
    listbox_cb_t *s = &listbox_cb;

    s->cindex++;
    if (s->cindex - s->page_index >= s->page_nitem) {
        if (s->roll_en) {
            s->page_index++;
        } else {
            s->page_index += s->page_nitem;
        }
        page_chg = 1;
    }
    if (s->cindex == s->total) {
        s->cindex = 0;
        s->page_index = 0;
        page_chg = 1;
    }
    if (page_chg) {
        num = s->total - s->page_index;
        if (s->total - s->page_index > s->page_nitem) {
            num = s->page_nitem;
        }
        s->view = (*s->get_list_view)(s->page_index, num);
    }
}

u16 gui_listbox(u16 index, u16 total, u8 mode, u8 item_len, char *(*get_list_view)(u16 index, u8 count))
{
    listbox_cb_t *s = &listbox_cb;

    s->cindex = (index > total) ? 0: index;
    s->total = total;
    s->icon_en = (mode & LB_NO_ICON) ? 0 : 1;
    s->roll_en = (mode & LB_PAGE) ? 0 : 1;
    s->item_len = item_len;
    s->sta = LB_STA_DISP;
    s->page_nitem = 4;
    s->disp_index = 0xffff;
    if (s->roll_en) {
        if (s->cindex == 0) {
            s->page_index = 0;
        } else if ((s->cindex - 1 + s->page_nitem) > s->total) {
            s->page_index = (s->total > s->page_nitem) ? s->total - s->page_nitem : 0;
        } else {
            s->page_index = s->cindex - 1;
        }
    } else {
        s->page_index = (s->cindex / s->page_nitem) * s->page_nitem;
    }
//    printf("%s: %d, %d, %d, %d\n", __func__, s->cindex, s->page_index, s->total, s->page_nitem);
    s->get_list_view = get_list_view;
    s->view = (*s->get_list_view)(s->page_index, s->page_nitem);
    memset(&nmv_cb, 0, sizeof(nmv_cb));
    while (s->sta == LB_STA_DISP) {
        listbox_process();
        listbox_message(msg_dequeue());
        listbox_display();
    }

    if ((s->sta == LB_STA_OK_KEY) || (s->sta == LB_STA_DEL_KEY)) {
        return s->cindex;
    }
    return 0xffff;
}
#endif

