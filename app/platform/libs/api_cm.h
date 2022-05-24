#ifndef _API_CM_H
#define _API_CM_H

#define PAGE_DATA_SIZE      250

//每个Page 250 Byte
#define PAGE0(x)            (x)
#define PAGE1(x)            (0x100 + (x))
#define PAGE2(x)            (0x200 + (x))
#define PAGE3(x)            (0x300 + (x))
#define PAGE4(x)            (0x400 + (x))
#define PAGE5(x)            (0x500 + (x))
#define PAGE6(x)            (0x600 + (x))
#define PAGE7(x)            (0x700 + (x))
#define PAGE8(x)            (0x800 + (x))
#define PAGE9(x)            (0x900 + (x))


#define MAX_CM_PAGE         10       //根据需要定义大小
#define SYS_CM_PAGE_NUM     0
#define BT_CM_PAGE_NUM      1
#define BT_CM_PAGE1_NUM     2
#define EXT_CM_PAGE_NUM     3
#define BTHID_CM_PAGE_NUM   4

#define SYS_CM_PAGE(x)      PAGE0(x)
#define BT_CM_PAGE(x)       PAGE1(x)
#define BT_CM_PAGE1(x)      PAGE2(x)
#define EXT_CM_PAGE(x)      PAGE3(x)
#define BTHID_CM_PAGE(x)    PAGE4(x)

#if GFPS_EN
#ifdef MAX_CM_PAGE
#undef MAX_CM_PAGE
#define MAX_CM_PAGE         6       // 根据需要重新定义大小
#endif
#define GFPS_PAGE_NUM       5
#define GFPS_PAGE(x)        PAGE5(x)
#endif

void cm_init(uint max_page, u32 addr, uint len);

void cm_sync(void);                                 //将write写入cache的内容同步到spiflash中
void cm_read(void *buf, u32 addr, uint len);
u8 cm_read8(u32 addr);
u16 cm_read16(u32 addr);
u32 cm_read32(u32 addr);
void cm_write(void *buf, u32 addr, uint len);       //注意：write只是写入cache，需要调用cm_sync才会写入到spiflash中
void cm_write8(u32 addr, u8 val);
void cm_write16(u32 addr, u16 val);
void cm_write32(u32 addr, u32 val);
void cm_clear(u32 addr);                            //清除CM的一整个Page
bool xcfg_init(void *xcfg, uint len);

#endif
