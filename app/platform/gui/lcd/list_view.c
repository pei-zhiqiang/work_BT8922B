#include "include.h"
#include "list_view.h"

#define FLV_PAGE0_NUM                8          //PAGE0缓存8项
#define FLV_PAGE_TOTAL              40          //最多缓存40项
#define FLV_PAGE_PREV               10          //缓存向前10项，向后30项
#define FLV_NAME_LEN                100
#define FILE_NAME_FLAG              (FLV_NAME_LEN-1)

typedef struct {
    u8  cdup_flag   :   1,      //是否支持返回上一级菜单
        begin_flag  :   1;      //是否第一次进入
    u8  resvd_cnt;              //保留项个数，最多保留一页
    u16 total;                  //当前目录文件与文件夹总数
    u16 dir_total;              //当前目录文件夹总数
    u16 index;
    u16 start;
    u16 end;
} flv_cb_t;
flv_cb_t flv_cb;

char flv_name_buf[FLV_PAGE_TOTAL][FLV_NAME_LEN] AT(.fnav_buf);
char flv_page0_buf[FLV_PAGE0_NUM][FLV_NAME_LEN] AT(.fnav_buf);

AT(.rodata.flv)
const char cdup_fname_gbk_zh[12] = {0xB7, 0xB5, 0xBB, 0xD8, 0xC9, 0xCF, 0xD2, 0xBB, 0xBC, 0xB6, 0x00, 0x00};       //"返回上一级"GBK编码
AT(.rodata.flv)
const char cdup_fname_gbk_en[9] = "UP LEVEL";
AT(.rodata.flv)
const char del_confirm_gbk_zh[] = {0xC8, 0xB7, 0xB6, 0xA8, 0xC9, 0xBE, 0xB3, 0xFD, 0xB8, 0xC3, 0xCE, 0xC4, 0xBC, 0xFE, 0xC2, 0xF0, 0xA3, 0xBF, 0x00, 0x00};   //"确定删除该文件吗？"
AT(.rodata.flv)
const char del_confirm_gbk_en[] = "Delete this file?";

char *list_view_get_adapter_item(u16 index, u8 count)
{
    flv_cb_t *s = &flv_cb;
    u16 start, num, rest;
    u16 item_total = s->total - s->resvd_cnt;
    u8 adapter_update = 0;
    char *buf;

    if (s->start <= s->end) {
        if ((index < s->start) || ((index + count - 1) > s->end)) {
            adapter_update = 1;
        }
    } else {
        if ((index < s->start) && ((index + count - 1) > s->end)) {
            adapter_update = 1;
        }
    }

    if (adapter_update) {
        buf = flv_name_buf[0];
        if (item_total <= FLV_PAGE_TOTAL) {
            s->start = start = 0;
            num = item_total;
        } else {
            s->start = (index > FLV_PAGE_PREV) ? (index - FLV_PAGE_PREV) : 0;
            start = s->start;
            num = FLV_PAGE_TOTAL;
            if ((start + num) > item_total) {
                rest = item_total - start;
                fs_nav_cache_fill(buf, start, rest);
                buf += rest * FLV_NAME_LEN;
                start = 0;
                num -= rest;
            }
        }
        s->end = start + num - 1;
        fs_nav_cache_fill(buf, start, num);
    }

    if (index < s->start) {
        index += item_total;
    }
    index -= s->start;
    return flv_name_buf[index];
}

char *list_view_get_item(u16 index, u8 count)
{
    if (index < 4) {
        return flv_page0_buf[index];
    }
    return list_view_get_adapter_item(index - flv_cb.resvd_cnt, count);
}

//每层目录需要更新结构体数据
void list_view_adapter_init(void)
{
    flv_cb_t *s = &flv_cb;

    s->start = s->end = 0xffff;           //清空缓存
    s->resvd_cnt = 0;
    if (s->cdup_flag && fs_get_dir_depth()) {
        s->resvd_cnt = 1;
    }
    s->total = fs_nav_dir_scan() + s->resvd_cnt;
    s->dir_total = fs_nav_get_dirtotal();
    if (s->total > s->resvd_cnt) {
        s->index += s->resvd_cnt;
    }
    if (s->begin_flag) {
        s->begin_flag = 0;
        s->index += s->dir_total;
    }
//    printf("%s: %d, %d, %d, %d\n", __func__, s->total, s->dir_total, s->index, s->resvd_cnt);
    if (s->resvd_cnt) {
        if (sys_cb.lang_id) {
            memcpy(flv_page0_buf[0], cdup_fname_gbk_zh, sizeof(cdup_fname_gbk_zh));
        } else {
            memcpy(flv_page0_buf[0], cdup_fname_gbk_en, sizeof(cdup_fname_gbk_en));
        }
        flv_page0_buf[0][FILE_NAME_FLAG] = FB_GBK | FB_FOLDERUP;
    }
    memcpy(flv_page0_buf[s->resvd_cnt], list_view_get_adapter_item(0, FLV_PAGE0_NUM), (FLV_PAGE0_NUM - s->resvd_cnt)*FLV_NAME_LEN);

}

u16 music_list_view_adapter(u16 index, u8 mode)
{
    flv_cb_t *s = &flv_cb;

    memset(&flv_cb, 0, sizeof(flv_cb));
    s->cdup_flag = (mode & FL_CDUP) ? 1 : 0;
    s->begin_flag = 1;
    s->index = fs_nav_init(index);
//    printf("s->index: %d, %d\n", s->index, index);

    while (1) {
        WDT_CLR();
        list_view_adapter_init();
        if (!s->total) {
            break;
        }
        index = gui_listbox(s->index, s->total, 0, FLV_NAME_LEN, list_view_get_item);
        s->index = index;
        if (listbox_cb.sta >= LB_STA_EXIT) {
            break;
        }

        //删除选中的文件
        if (listbox_cb.sta == LB_STA_DEL_KEY) {
            const char *text = del_confirm_gbk_en;
            if (sys_cb.lang_id) {
                text = del_confirm_gbk_zh;
            }
            if (index >= (s->resvd_cnt + s->dir_total)) {
                //确认选择文件是否删除
                if (gui_dialogbox(text, DB_YESNO|DB_TITLE) > 0) {
                    index -= s->resvd_cnt + s->dir_total;
                    if (fs_nav_delete(index)) {
                        if (s->index == (s->total - 1)) {
                            s->index--;
                        }
                    }
                }
                if (dialogbox_cb.sta >= DB_STA_EXIT) {
                    listbox_cb.sta = dialogbox_cb.sta;
                    listbox_cb.evt_dev = dialogbox_cb.evt_dev;
                    break;
                }
            }
            if (s->index > s->resvd_cnt) {
                s->index -= s->resvd_cnt;
            } else {
                s->index = 0;
            }
            continue;
        }

        //进入子目录或返回上一级目录
        if (index == 0xffff) {
            index = fs_nav_cd_updir();
            if (index != 0xffff) {
                s->index = index;
            } else {
                break;
            }
        } else if (index < s->resvd_cnt) {
            index = fs_nav_cd_updir();
            if (index != 0xffff) {
                s->index = index;
            } else {
                s->index = 0;
            }
        } else {
            index -= s->resvd_cnt;
            if (index < s->dir_total) {
                if (fs_nav_cd_subdir(index)) {
                    s->index = 0;
                }
            } else {
                index -= s->dir_total;
                break;
            }
        }
    }

    if (listbox_cb.sta >= LB_STA_EXIT) {
        index = 0;
    } else {
        index = fs_nav_get_file_number(index);
    }
    fs_nav_exit();

    return index;
}
