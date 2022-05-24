#ifndef _API_FS_H
#define _API_FS_H

#define SEEK_SET    0
#define SEEK_CUR    1

#define SCAN_SUB_FOLDER     0x01        //是否需要扫描子目录
#define SCAN_SPEED          0x10        //使用搜文件加速

/* Results of Disk Functions */
typedef enum {
    RES_OK = 0,     /* 0: Function succeeded */
    RES_ERROR,      /* 1: Disk error */
    RES_NOTRDY,     /* 2: Not ready */
    RES_PARERR      /* 3: Invalid parameter */
} DRESULT;

/* File function return code (FRESULT) */
typedef enum {
    FR_OK = 0,          /* 0 */
    FR_DISK_ERR,        /* 1 */
    FR_INT_ERR,			/* 2 */
    FR_NOT_READY,       /* 3 */
    FR_NO_FILE,         /* 4 */
    FR_NOT_OPENED,      /* 5 */
    FR_NOT_ENABLED,     /* 6 */
    FR_NO_FILESYSTEM,   /* 7 */
    FR_EXIST,			/* 8 */
    FR_DENIED,          /* 9 */
    FR_INVALID_NAME,    /* 10 */
    FR_NOT_ENOUGH_CORE, /* 11 */
} FRESULT;

/* File access mode and open method flags (2rd argument of f_open) */
#define	FA_READ				0x01
#define	FA_WRITE			0x02
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08

/* FAT sub type (FATFS.fs_type) */
#define FS_FAT12    1
#define FS_FAT16    2
#define FS_FAT32    3
#define FS_EXFAT    4

FRESULT fs_read (void* buff, UINT btr, UINT* br);           /* Read data from the open file */
FRESULT fs_lseek (DWORD ofs, u8 whence);                    /* Move file pointer of the open file */
FRESULT fs_write (void* buff, UINT btw);                    /* Write data to the open file */
DRESULT disk_readp (BYTE* buff, DWORD sector);              /* Read Partial Sector */
DRESULT disk_writep (BYTE* buff, DWORD sector);             /* Write Partial Sector  */
FRESULT fs_read_lrc(void* buff, UINT btr, UINT* br);        /* Read data from the lrc open file */
FRESULT fs_lseek_lrc(DWORD ofs, u8 whence);                 /* Move file pointer of the lrc open file */

bool fs_mount(void);
void fs_var_init(void);
void fsdisk_callback_init(u8 dev_num);
u16 fs_get_total_files(void);                               //扫描全盘文件，返回文件总数
u16 fs_getdir_files(void);                                  //扫描当前文件所在目录的文件总数
u16 fs_get_dir_fstart(void);                                //获取当前文件夹起始文件编号
u16 fs_get_dirs_count(void);                                //获取目录计数
bool fs_open_num(u16 file_number);                          //根据文件编号打开文件
u16 fs_open_dir_num(u16 dir_num);                           //根据文件夹编号打开目标文件夹，返回文件夹起始文件编号
void fs_get_short_fname(char *fnbuf, u8 uppercase);         //获取短文件名, uppercase = 0, 小写字母返回; uppercase = 1, 大写字母返回
void fs_get_filename(char *fnbuf, u16 buf_len);             //获取长文件名
void fs_get_fname_extension(char *ext);                     //获取文件名后缀
uint fs_get_dir_depth(void);                                //获取目录深度
u32 fs_get_file_ssect(void);                                //获取文件起始sector
u32 fs_get_file_size(void);                                 //获取文件SIZE
u8 fs_get_type(void);                                       //获取文件系统类型
u32 fs_get_file_count(void);                                //获取当前文件的编号
u32 fs_get_ftime(void);                                     //获取文件创建时间
bool fs_get_filename_for_number(u16 file_number, u8 lfn, char *fnbuf, u16 buf_len);     //根据文件编号获取文件名字

FRESULT fs_mkdir(const char *path);                                 //创建短文件名目录
FRESULT fs_mkdir_lfn(const char *path, const char *lfn);            //创建长文件名目录
FRESULT fs_open(const char *path, u8 mode);                         //按路径打开或创建文件（短文件名创建文件）
FRESULT fs_open_lfn(const char *path, const char *lfn, u8 mode);    //按路径打开或创建文件（长文件名创建文件）
FRESULT fs_close(void);                                             //Close File
FRESULT fs_sync(void);                                              //Synchronize the File
FRESULT fs_open_lrc(char *lfn);                                     //打开当前歌曲的歌词文件
bool fs_delete(u16 file_number);                                    //删除指定number文件

void fs_scan_set(u8 mode, u8 (*file_filter)(void), u8 (*dir_filter)(void));

///点阵屏文件导航相关的API函数
u16 fs_nav_init(u16 file_num);
void fs_nav_exit(void);
u16 fs_nav_get_dirtotal(void);
u16 fs_nav_dir_scan(void);
u16 fs_nav_cd_updir(void);
bool fs_nav_cd_subdir(u16 index);
u16 fs_nav_get_file_number(u16 index);
bool fs_nav_delete(u16 index);
void fs_nav_cache_fill(void *buf, u16 start, u16 num);

#endif // _API_FS_H
