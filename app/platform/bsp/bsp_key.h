#ifndef _BSP_KEY_H
#define _BSP_KEY_H

#define KEY_SCAN_TIMES          6           //按键防抖的扫描次数
#define KEY_UP_TIMES            6           //抬键防抖的扫描次数
#define KEY_LONG_TIMES          150         //长按键的次数
#define KEY_HOLD_TIMES          35          //连按的频率次数
#define KEY_LONG_HOLD_TIMES     (KEY_LONG_TIMES + KEY_HOLD_TIMES)

#define KEY_SHORT               0x000
#define KEY_SHORT_UP            0x800
#define KEY_LONG                0xA00
#define KEY_LONG_UP             0xC00
#define KEY_HOLD                0xE00
#define KEY_LHOLD               0x1E00
#define KEY_DOUBLE              0x1800      //2击
#define KEY_THREE               0x1900      //3击
#define KEY_FOUR                0x1A00      //4击
#define KEY_FIVE                0x1B00      //5击

#define KEY_TYPE_MASK           0x1f00

//Event Message, 范围：0x700 ~ 0x7ff
#define MSG_SYS_1S              0x7ff
#define MSG_SYS_500MS           0x7fe
#define EVT_SD_INSERT           0x7fd
#define EVT_SD_REMOVE           0x7fc
#define EVT_SD1_INSERT          0x7fb
#define EVT_SD1_REMOVE          0x7fa
#define EVT_UDISK_INSERT        0x7f9
#define EVT_UDISK_REMOVE        0x7f8
#define EVT_PC_INSERT           0x7f7
#define EVT_PC_REMOVE           0x7f6
#define EVT_LINEIN_INSERT       0x7f5
#define EVT_LINEIN_REMOVE       0x7f4
#define EVT_HFP_SET_VOL         0x7f3
#define EVT_A2DP_SET_VOL        0x7f2
#define EVT_A2DP_MUSIC_PLAY     0x7f1
#define EVT_A2DP_MUSIC_STOP     0x7f0
#define EVT_BT_SCAN_START       0x7ef
#define EVT_ONLINE_SET_EQ       0x7ee
#define EVT_TWS_SET_VOL         0x7ed
#define EVT_KEY_2_UNMUTE        0x7ec
#define EVT_TWS_HID_SHUTTER     0x7eb
#define EVT_BT_SET_LANG_ID      0x7ea
#define EVT_BT_SET_EQ           0x7e9
#define EVT_MIC_INSERT          0x7e8
#define EVT_MIC_REMOVE          0x7e7
#define EVT_ECHO_LEVEL          0x7e6
#define EVT_MIC_VOL             0x7e5
#define EVT_MUSIC_VOL           0x7e4
#define EVT_CHARGE_INBOX        0x7e3
#define EVT_ONLINE_SET_ANC      0x7e2
#define EVT_ONLINE_SET_ENC      0x7e1
#define EVT_ONLINE_SET_SMIC     0x7e0
#define EVT_CHARGE_DCIN         0x7df

#define EVT_BLE_ADV0_BAT        0x7de
#define EVT_AUDIO_BY_PASS       0x7dd
#define EVT_AUDIO_ENABLE        0x7dc

#define EVT_MSG_DUEROS_INBOX_SW 0x7db
#define EVT_MSG_DUEROS_USER_SW  0x7da
#define EVT_MSG_SYS_30S         0x7d9
#define EVT_MSG_DMA_TWS_SYNC    0x7d8       //TWS连接上，用于主机发同步数据给附机
#define EVT_TWS_LED_SYNC        0x7d7
#define EVT_LED_BT_IDLE         0x7d6

#define EVT_BT_TWS_EAR_STA      0x7d5
#define EVT_BT_MENU_VA_EN       0x7d4
#define EVT_BT_MENU_VA_DIS      0x7d3
#define EVT_NR_STA_CHANGE       0x7d2
#define EVT_QTEST_PICKUP_PWROFF 0x7d1
#define EVT_BT_UPDATE_STA       0x7d0

#define EVT_A2DP_LLTY_EN        0x7cf
#define EVT_A2DP_LLTY_DIS       0x7ce
#define EVT_A2DP_VOL_SAVE       0x7cd

#define EVT_AUTO_PWFOFF_EN      0x7cc
#define EVT_AUTO_PWFOFF_DIS     0x7cb


//普通按键定义，可以支持短按，长按，双击等。范围：0 ~ 0xdf
#define NO_KEY                  0x00
#define KEY_1                   0x01
#define KEY_2                   0x02
#define KEY_3                   0x03
#define KEY_4                   0x04
#define KEY_5                   0x05
#define KEY_6                   0x06
#define KEY_7                   0x07
#define KEY_8                   0x08
#define KEY_9                   0x09
#define KEY_10                  0x0a
#define KEY_11                  0x0b
#define KEY_12                  0x0c
#define KEY_13                  0x0d
#define KEY_14                  0x0e
#define KEY_15                  0x0f
#define KEY_16                  0x10
#define KEY_17                  0x11
#define KEY_18                  0x12
#define KEY_19                  0x13
#define KEY_20                  0x14
#define KEY_21                  0x15
#define KEY_22                  0x16
#define KEY_23                  0x17
#define KEY_24                  0x18
#define KEY_25                  0x19
#define KEY_26                  0x1a
#define KEY_27                  0x1b
#define KEY_28                  0x1c
#define KEY_29                  0x1d
#define KEY_30                  0x1e
#define KEY_31                  0x1f

//POWER KEY相关0xe0打头，便于开关机按键判断 (0xe0 ~ 0xef)
#define K_PWR_MASK              0xF0
#define K_PWR                   0xe0
#define KL_PWR                 (KEY_LONG    | K_PWR)
#define KLU_PWR                (KEY_LONG_UP | K_PWR)
#define KLH_PWR                (KEY_LHOLD   | K_PWR)
#define KEY_PWR0                0xe0
#define KEY_PWR1                0xe1
#define KEY_PWR2                0xe2
#define KEY_PWR3                0xe3
#define KEY_PWR4                0xe4
#define KEY_PWR5                0xe5
#define KEY_PWR6                0xe6
#define KEY_PWR7                0xe7
#define KEY_PWR8                0xe8
#define KEY_PWR9                0xe9
#define KEY_PWR10               0xea
#define KEY_PWR11               0xeb
#define KEY_PWR12               0xec
#define KEY_PWR13               0xed
#define KEY_PWR14               0xee
#define KEY_PWR15               0xef


//数字按键等，只支持短按。范围: 0xf0 ~ 0xff
#define KEY_NUM_0               0xf0
#define KEY_NUM_1               0xf1
#define KEY_NUM_2               0xf2
#define KEY_NUM_3               0xf3
#define KEY_NUM_4               0xf4
#define KEY_NUM_5               0xf5
#define KEY_NUM_6               0xf6
#define KEY_NUM_7               0xf7
#define KEY_NUM_8               0xf8
#define KEY_NUM_9               0xf9
#define KEY_NUM_P100            0xfa        //100+数字键

#define KEY_PLAY                0x20
#define KEY_PLAY_MODE           0x21      //短按：P/P，      长按:  MODE
#define KEY_PLAY_HSF            0x22
#define KEY_PLAY_MODE_HSF       0x23
#define KEY_PLAY_USER_DEF       0x24       //短按: PLAY,      长按：拒接     双击：下一曲   三击：上一曲

#define KEY_PREV                0x40
#define KEY_VOL_DOWN            0x41
#define KEY_VOL_DOWN_PREV       0x42      //短按：VOL-,      长按：上一曲
#define KEY_PREV_VOL_DOWN       0x43      //短按：上一曲，   长按： VOL-
#define KEY_PREV_VOL_UP         0x44      //短按：上一曲，   长按： VOL+
#define KEY_VOL_UP_PREV         0x45      //短按：VOL+,      长按：上一曲
#define KEY_VOL_UP_DOWN         0x46      //短按：VOL+,      长按：VOL-

#define KEY_NEXT                0x60
#define KEY_VOL_UP              0x61
#define KEY_VOL_UP_NEXT         0x62      //短按：VOL+,      长按：下一曲
#define KEY_NEXT_VOL_UP         0x63      //短按：下一曲，   长按： VOL+
#define KEY_NEXT_VOL_DOWN       0x64      //短按：下一曲，   长按： VOL-
#define KEY_VOL_DOWN_NEXT       0x65      //短按：VOL-,      长按：下一曲

#define KEY_MODE                KEY_1
#define KEY_HSF                 KEY_2
#define KEY_EQ                  KEY_3
#define KEY_REPEAT              KEY_4
#define KEY_REC                 KEY_5
#define KEY_STOP                KEY_6
#define KEY_MUTE                KEY_7
#define KEY_CH_DOWN             KEY_8
#define KEY_CH_UP               KEY_9
#define KEY_SCAN                KEY_10
#define KEY_KARAOK              KEY_11
#define KEY_IR_POWER            KEY_12      //IR Remote的开关机键
#define KEY_SIRI                KEY_13
#define KEY_HOME                KEY_14      //短按：ios home, 长按：siri
#define KEY_USB_SD              KEY_15
#define KEY_ANC                 KEY_16
#define KEY_VOICE_RM            KEY_17

#define KEY_POWER               KEY_PWR0
#define KEY_MODE_PWR            KEY_PWR1    //短按：MODE,      长按： POWER
#define KEY_PLAY_PWR_USER_DEF   KEY_PWR2   //短按PLAY,        长按: POWER,下一曲, 双击:回拔,           三击: 上一曲
#define KEY_PLAY_HSF_PWR        KEY_PWR3    //短按：P/P，      长按： POWER        双击：回拔电话

#define K_PLAY                  (KEY_PLAY | KEY_SHORT)
#define KU_PLAY                 (KEY_PLAY | KEY_SHORT_UP)
#define KL_PLAY                 (KEY_PLAY | KEY_LONG)
#define KLU_PLAY                (KEY_PLAY | KEY_LONG_UP)
#define KH_PLAY                 (KEY_PLAY | KEY_HOLD)
#define KD_PLAY                 (KEY_PLAY | KEY_DOUBLE)
#define KTH_PLAY                (KEY_PLAY | KEY_THREE)
#define KFO_PLAY                (KEY_PLAY | KEY_FOUR)
#define KFI_PLAY                (KEY_PLAY | KEY_FIVE)

#define K_PLAY_HSF              (KEY_PLAY_HSF | KEY_SHORT)
#define KU_PLAY_HSF             (KEY_PLAY_HSF | KEY_SHORT_UP)
#define KL_PLAY_HSF             (KEY_PLAY_HSF | KEY_LONG)
#define KLU_PLAY_HSF            (KEY_PLAY_HSF | KEY_LONG_UP)
#define KH_PLAY_HSF             (KEY_PLAY_HSF | KEY_HOLD)
#define KD_PLAY_HSF             (KEY_PLAY_HSF | KEY_DOUBLE)
#define KTH_PLAY_HSF            (KEY_PLAY_HSF | KEY_THREE)
#define KFO_PLAY_HSF            (KEY_PLAY_HSF | KEY_FOUR)
#define KFI_PLAY_HSF            (KEY_PLAY_HSF | KEY_FIVE)

#define K_PREV                  (KEY_PREV | KEY_SHORT)
#define KU_PREV                 (KEY_PREV | KEY_SHORT_UP)
#define KL_PREV                 (KEY_PREV | KEY_LONG)
#define KLU_PREV                (KEY_PREV | KEY_LONG_UP)
#define KH_PREV                 (KEY_PREV | KEY_HOLD)
#define KD_PREV                 (KEY_PREV | KEY_DOUBLE)
#define KTH_PREV                (KEY_PREV | KEY_THREE)

#define K_NEXT                  (KEY_NEXT | KEY_SHORT)
#define KU_NEXT                 (KEY_NEXT | KEY_SHORT_UP)
#define KL_NEXT                 (KEY_NEXT | KEY_LONG)
#define KLU_NEXT                (KEY_NEXT | KEY_LONG_UP)
#define KH_NEXT                 (KEY_NEXT | KEY_HOLD)
#define KD_NEXT                 (KEY_NEXT | KEY_DOUBLE)
#define KTH_NEXT                (KEY_NEXT | KEY_THREE)

#define K_MODE                  (KEY_MODE | KEY_SHORT)
#define KU_MODE                 (KEY_MODE | KEY_SHORT_UP)
#define KL_MODE                 (KEY_MODE | KEY_LONG)
#define KLU_MODE                (KEY_MODE | KEY_LONG_UP)
#define KH_MODE                 (KEY_MODE | KEY_HOLD)
#define KD_MODE                 (KEY_MODE | KEY_DOUBLE)
#define KTH_MODE                (KEY_MODE | KEY_THREE)

#define K_HSF                   (KEY_HSF | KEY_SHORT)
#define KU_HSF                  (KEY_HSF | KEY_SHORT_UP)
#define KL_HSF                  (KEY_HSF | KEY_LONG)
#define KLU_HSF                 (KEY_HSF | KEY_LONG_UP)
#define KH_HSF                  (KEY_HSF | KEY_HOLD)
#define KD_HSF                  (KEY_HSF | KEY_DOUBLE)
#define KTH_HSF                 (KEY_HSF | KEY_THREE)
#define KFO_HSF                 (KEY_HSF | KEY_FOUR)
#define KFI_HSF                 (KEY_HSF | KEY_FIVE)

#define K_VOL_DOWN              (KEY_VOL_DOWN | KEY_SHORT)
#define KU_VOL_DOWN             (KEY_VOL_DOWN | KEY_SHORT_UP)
#define KL_VOL_DOWN             (KEY_VOL_DOWN | KEY_LONG)
#define KLU_VOL_DOWN            (KEY_VOL_DOWN | KEY_LONG_UP)
#define KH_VOL_DOWN             (KEY_VOL_DOWN | KEY_HOLD)
#define KD_VOL_DOWN             (KEY_VOL_DOWN | KEY_DOUBLE)
#define KTH_VOL_DOWN            (KEY_VOL_DOWN | KEY_THREE)

#define K_VOL_UP                (KEY_VOL_UP | KEY_SHORT)
#define KU_VOL_UP               (KEY_VOL_UP | KEY_SHORT_UP)
#define KL_VOL_UP               (KEY_VOL_UP | KEY_LONG)
#define KLU_VOL_UP              (KEY_VOL_UP | KEY_LONG_UP)
#define KH_VOL_UP               (KEY_VOL_UP | KEY_HOLD)
#define KD_VOL_UP               (KEY_VOL_UP | KEY_DOUBLE)
#define KTH_VOL_UP              (KEY_VOL_UP | KEY_THREE)

#define K_VOL_UP_DOWN           (KEY_VOL_UP_DOWN | KEY_SHORT)
#define KU_VOL_UP_DOWN          (KEY_VOL_UP_DOWN | KEY_SHORT_UP)
#define KL_VOL_UP_DOWN          (KEY_VOL_UP_DOWN | KEY_LONG)
#define KLU_VOL_UP_DOWN         (KEY_VOL_UP_DOWN | KEY_LONG_UP)
#define KH_VOL_UP_DOWN          (KEY_VOL_UP_DOWN | KEY_HOLD)
#define KD_VOL_UP_DOWN          (KEY_VOL_UP_DOWN | KEY_DOUBLE)
#define KTH_VOL_UP_DOWN         (KEY_VOL_UP_DOWN | KEY_THREE)

#define K_EQ                    (KEY_EQ | KEY_SHORT)
#define KU_EQ                   (KEY_EQ | KEY_SHORT_UP)
#define KL_EQ                   (KEY_EQ | KEY_LONG)
#define KLU_EQ                  (KEY_EQ | KEY_LONG_UP)
#define KH_EQ                   (KEY_EQ | KEY_HOLD)
#define KD_EQ                   (KEY_EQ | KEY_DOUBLE)
#define KTH_EQ                  (KEY_EQ | KEY_THREE)

#define K_REPEAT                (KEY_REPEAT | KEY_SHORT)
#define KU_REPEAT               (KEY_REPEAT | KEY_SHORT_UP)
#define KL_REPEAT               (KEY_REPEAT | KEY_LONG)
#define KLU_REPEAT              (KEY_REPEAT | KEY_LONG_UP)
#define KH_REPEAT               (KEY_REPEAT | KEY_HOLD)
#define KD_REPEAT               (KEY_REPEAT | KEY_DOUBLE)
#define KTH_REPEAT              (KEY_REPEAT | KEY_THREE)

#define K_SCAN                  (KEY_SCAN | KEY_SHORT)
#define KU_SCAN                 (KEY_SCAN | KEY_SHORT_UP)
#define KL_SCAN                 (KEY_SCAN | KEY_LONG)
#define KLU_SCAN                (KEY_SCAN | KEY_LONG_UP)
#define KH_SCAN                 (KEY_SCAN | KEY_HOLD)
#define KD_SCAN                 (KEY_SCAN | KEY_DOUBLE)
#define KTH_SCAN                (KEY_SCAN | KEY_THREE)

#define K_REC                   (KEY_REC | KEY_SHORT)
#define KU_REC                  (KEY_REC | KEY_SHORT_UP)
#define KL_REC                  (KEY_REC | KEY_LONG)
#define KLU_REC                 (KEY_REC | KEY_LONG_UP)
#define KH_REC                  (KEY_REC | KEY_HOLD)
#define KD_REC                  (KEY_REC | KEY_DOUBLE)
#define KTH_REC                 (KEY_REC | KEY_THREE)

#define K_POWER                 (KEY_POWER | KEY_SHORT)
#define KU_POWER                (KEY_POWER | KEY_SHORT_UP)
#define KL_POWER                (KEY_POWER | KEY_LONG)
#define KLU_POWER               (KEY_POWER | KEY_LONG_UP)
#define KH_POWER                (KEY_POWER | KEY_HOLD)
#define KD_POWER                (KEY_POWER | KEY_DOUBLE)
#define KTH_POWER               (KEY_POWER | KEY_THREE)
#define KLH_POWER               (KEY_POWER | KEY_LHOLD)

#define K_STOP                  (KEY_STOP | KEY_SHORT)
#define KU_STOP                 (KEY_STOP | KEY_SHORT_UP)
#define KL_STOP                 (KEY_STOP | KEY_LONG)
#define KLU_STOP                (KEY_STOP | KEY_LONG_UP)
#define KH_STOP                 (KEY_STOP | KEY_HOLD)
#define KD_STOP                 (KEY_STOP | KEY_DOUBLE)
#define KTH_STOP                (KEY_STOP | KEY_THREE)

#define K_MUTE                  (KEY_MUTE | KEY_SHORT)
#define KU_MUTE                 (KEY_MUTE | KEY_SHORT_UP)
#define KL_MUTE                 (KEY_MUTE | KEY_LONG)
#define KLU_MUTE                (KEY_MUTE | KEY_LONG_UP)
#define KH_MUTE                 (KEY_MUTE | KEY_HOLD)
#define KD_MUTE                 (KEY_MUTE | KEY_DOUBLE)
#define KTH_MUTE                (KEY_MUTE | KEY_THREE)

#define K_VOL_DOWN_PREV         (KEY_VOL_DOWN_PREV | KEY_SHORT)
#define KU_VOL_DOWN_PREV        (KEY_VOL_DOWN_PREV | KEY_SHORT_UP)
#define KL_VOL_DOWN_PREV        (KEY_VOL_DOWN_PREV | KEY_LONG)
#define KLU_VOL_DOWN_PREV       (KEY_VOL_DOWN_PREV | KEY_LONG_UP)
#define KH_VOL_DOWN_PREV        (KEY_VOL_DOWN_PREV | KEY_HOLD)
#define KD_VOL_DOWN_PREV        (KEY_VOL_DOWN_PREV | KEY_DOUBLE)

#define K_VOL_UP_NEXT           (KEY_VOL_UP_NEXT | KEY_SHORT)
#define KU_VOL_UP_NEXT          (KEY_VOL_UP_NEXT | KEY_SHORT_UP)
#define KL_VOL_UP_NEXT          (KEY_VOL_UP_NEXT | KEY_LONG)
#define KLU_VOL_UP_NEXT         (KEY_VOL_UP_NEXT | KEY_LONG_UP)
#define KH_VOL_UP_NEXT          (KEY_VOL_UP_NEXT | KEY_HOLD)
#define KD_VOL_UP_NEXT          (KEY_VOL_UP_NEXT | KEY_DOUBLE)

#define K_VOL_DOWN_NEXT         (KEY_VOL_DOWN_NEXT | KEY_SHORT)
#define KU_VOL_DOWN_NEXT        (KEY_VOL_DOWN_NEXT | KEY_SHORT_UP)
#define KL_VOL_DOWN_NEXT        (KEY_VOL_DOWN_NEXT | KEY_LONG)
#define KLU_VOL_DOWN_NEXT       (KEY_VOL_DOWN_NEXT | KEY_LONG_UP)
#define KH_VOL_DOWN_NEXT        (KEY_VOL_DOWN_NEXT | KEY_HOLD)
#define KD_VOL_DOWN_NEXT        (KEY_VOL_DOWN_NEXT | KEY_DOUBLE)

#define K_VOL_UP_PREV           (KEY_VOL_UP_PREV | KEY_SHORT)
#define KU_VOL_UP_PREV          (KEY_VOL_UP_PREV | KEY_SHORT_UP)
#define KL_VOL_UP_PREV          (KEY_VOL_UP_PREV | KEY_LONG)
#define KLU_VOL_UP_PREV         (KEY_VOL_UP_PREV | KEY_LONG_UP)
#define KH_VOL_UP_PREV          (KEY_VOL_UP_PREV | KEY_HOLD)
#define KD_VOL_UP_PREV          (KEY_VOL_UP_PREV | KEY_DOUBLE)

#define K_PREV_VOL_DOWN         (KEY_PREV_VOL_DOWN | KEY_SHORT)
#define KU_PREV_VOL_DOWN        (KEY_PREV_VOL_DOWN | KEY_SHORT_UP)
#define KL_PREV_VOL_DOWN        (KEY_PREV_VOL_DOWN | KEY_LONG)
#define KLU_PREV_VOL_DOWN       (KEY_PREV_VOL_DOWN | KEY_LONG_UP)
#define KH_PREV_VOL_DOWN        (KEY_PREV_VOL_DOWN | KEY_HOLD)
#define KD_PREV_VOL_DOWN        (KEY_PREV_VOL_DOWN | KEY_DOUBLE)
#define KTH_PREV_VOL_DOWN       (KEY_PREV_VOL_DOWN | KEY_THREE)

#define K_NEXT_VOL_UP           (KEY_NEXT_VOL_UP | KEY_SHORT)
#define KU_NEXT_VOL_UP          (KEY_NEXT_VOL_UP | KEY_SHORT_UP)
#define KL_NEXT_VOL_UP          (KEY_NEXT_VOL_UP | KEY_LONG)
#define KLU_NEXT_VOL_UP         (KEY_NEXT_VOL_UP | KEY_LONG_UP)
#define KH_NEXT_VOL_UP          (KEY_NEXT_VOL_UP | KEY_HOLD)
#define KD_NEXT_VOL_UP          (KEY_NEXT_VOL_UP | KEY_DOUBLE)
#define KTH_NEXT_VOL_UP         (KEY_NEXT_VOL_UP | KEY_THREE)

#define K_NEXT_VOL_DOWN         (KEY_NEXT_VOL_DOWN | KEY_SHORT)
#define KU_NEXT_VOL_DOWN        (KEY_NEXT_VOL_DOWN | KEY_SHORT_UP)
#define KL_NEXT_VOL_DOWN        (KEY_NEXT_VOL_DOWN | KEY_LONG)
#define KLU_NEXT_VOL_DOWN       (KEY_NEXT_VOL_DOWN | KEY_LONG_UP)
#define KH_NEXT_VOL_DOWN        (KEY_NEXT_VOL_DOWN | KEY_HOLD)
#define KD_NEXT_VOL_DOWN        (KEY_NEXT_VOL_DOWN | KEY_DOUBLE)
#define KTH_NEXT_VOL_DOWN       (KEY_NEXT_VOL_DOWN | KEY_THREE)

#define K_PREV_VOL_UP           (KEY_PREV_VOL_UP | KEY_SHORT)
#define KU_PREV_VOL_UP          (KEY_PREV_VOL_UP | KEY_SHORT_UP)
#define KL_PREV_VOL_UP          (KEY_PREV_VOL_UP | KEY_LONG)
#define KLU_PREV_VOL_UP         (KEY_PREV_VOL_UP | KEY_LONG_UP)
#define KH_PREV_VOL_UP          (KEY_PREV_VOL_UP | KEY_HOLD)
#define KD_PREV_VOL_UP          (KEY_PREV_VOL_UP | KEY_DOUBLE)
#define KTH_PREV_VOL_UP         (KEY_PREV_VOL_UP | KEY_THREE)

#define K_PLAY_PWR              (KEY_PLAY_PWR | KEY_SHORT)
#define KU_PLAY_PWR             (KEY_PLAY_PWR | KEY_SHORT_UP)
#define KL_PLAY_PWR             (KEY_PLAY_PWR | KEY_LONG)
#define KLU_PLAY_PWR            (KEY_PLAY_PWR | KEY_LONG_UP)
#define KH_PLAY_PWR             (KEY_PLAY_PWR | KEY_HOLD)
#define KD_PLAY_PWR             (KEY_PLAY_PWR | KEY_DOUBLE)
#define KTH_PLAY_PWR            (KEY_PLAY_PWR | KEY_THREE)
#define KFO_PLAY_PWR            (KEY_PLAY_PWR | KEY_FOUR)
#define KFI_PLAY_PWR            (KEY_PLAY_PWR | KEY_FIVE)
#define KLH_PLAY_PWR            (KEY_PLAY_PWR | KEY_LHOLD)

#define K_PLAY_HSF_PWR          (KEY_PLAY_HSF_PWR | KEY_SHORT)
#define KU_PLAY_HSF_PWR         (KEY_PLAY_HSF_PWR | KEY_SHORT_UP)
#define KL_PLAY_HSF_PWR         (KEY_PLAY_HSF_PWR | KEY_LONG)
#define KLU_PLAY_HSF_PWR        (KEY_PLAY_HSF_PWR | KEY_LONG_UP)
#define KH_PLAY_HSF_PWR         (KEY_PLAY_HSF_PWR | KEY_HOLD)
#define KD_PLAY_HSF_PWR         (KEY_PLAY_HSF_PWR | KEY_DOUBLE)
#define KTH_PLAY_HSF_PWR        (KEY_PLAY_HSF_PWR | KEY_THREE)
#define KFO_PLAY_HSF_PWR        (KEY_PLAY_HSF_PWR | KEY_FOUR)
#define KFI_PLAY_HSF_PWR        (KEY_PLAY_HSF_PWR | KEY_FIVE)
#define KLH_PLAY_HSF_PWR        (KEY_PLAY_HSF_PWR | KEY_LHOLD)

#define K_HSF_PWR               (KEY_HSF_PWR | KEY_SHORT)
#define KU_HSF_PWR              (KEY_HSF_PWR | KEY_SHORT_UP)
#define KL_HSF_PWR              (KEY_HSF_PWR | KEY_LONG)
#define KLU_HSF_PWR             (KEY_HSF_PWR | KEY_LONG_UP)
#define KH_HSF_PWR              (KEY_HSF_PWR | KEY_HOLD)
#define KD_HSF_PWR              (KEY_HSF_PWR | KEY_DOUBLE)
#define KTH_HSF_PWR             (KEY_HSF_PWR | KEY_THREE)
#define KFO_HSF_PWR             (KEY_HSF_PWR | KEY_FOUR)
#define KFI_HSF_PWR             (KEY_HSF_PWR | KEY_FIVE)
#define KLH_HSF_PWR             (KEY_HSF_PWR | KEY_LHOLD)

#define K_PLAY_MODE             (KEY_PLAY_MODE | KEY_SHORT)
#define KU_PLAY_MODE            (KEY_PLAY_MODE | KEY_SHORT_UP)
#define KL_PLAY_MODE            (KEY_PLAY_MODE | KEY_LONG)
#define KLU_PLAY_MODE           (KEY_PLAY_MODE | KEY_LONG_UP)
#define KH_PLAY_MODE            (KEY_PLAY_MODE | KEY_HOLD)
#define KD_PLAY_MODE            (KEY_PLAY_MODE | KEY_DOUBLE)
#define KTH_PLAY_MODE           (KEY_PLAY_MODE | KEY_THREE)

#define K_PLAY_MODE_HSF         (KEY_PLAY_MODE_HSF | KEY_SHORT)
#define KU_PLAY_MODE_HSF        (KEY_PLAY_MODE_HSF | KEY_SHORT_UP)
#define KL_PLAY_MODE_HSF        (KEY_PLAY_MODE_HSF | KEY_LONG)
#define KLU_PLAY_MODE_HSF       (KEY_PLAY_MODE_HSF | KEY_LONG_UP)
#define KH_PLAY_MODE_HSF        (KEY_PLAY_MODE_HSF | KEY_HOLD)
#define KD_PLAY_MODE_HSF        (KEY_PLAY_MODE_HSF | KEY_DOUBLE)
#define KTH_PLAY_MODE_HSF       (KEY_PLAY_MODE_HSF | KEY_THREE)
#define KFO_PLAY_MODE_HSF       (KEY_PLAY_MODE_HSF | KEY_FOUR)
#define KFI_PLAY_MODE_HSF       (KEY_PLAY_MODE_HSF | KEY_FIVE)

#define K_MODE_PWR              (KEY_MODE_PWR | KEY_SHORT)
#define KU_MODE_PWR             (KEY_MODE_PWR | KEY_SHORT_UP)
#define KL_MODE_PWR             (KEY_MODE_PWR | KEY_LONG)
#define KLU_MODE_PWR            (KEY_MODE_PWR | KEY_LONG_UP)
#define KH_MODE_PWR             (KEY_MODE_PWR | KEY_HOLD)
#define KD_MODE_PWR             (KEY_MODE_PWR | KEY_DOUBLE)
#define KTH_MODE_PWR            (KEY_MODE_PWR | KEY_THREE)
#define KLH_MODE_PWR            (KEY_MODE_PWR | KEY_LHOLD)

#define K_PLAY_MODE_PWR         (KEY_PLAY_MODE_PWR | KEY_SHORT)
#define KU_PLAY_MODE_PWR        (KEY_PLAY_MODE_PWR | KEY_SHORT_UP)
#define KL_PLAY_MODE_PWR        (KEY_PLAY_MODE_PWR | KEY_LONG)
#define KLU_PLAY_MODE_PWR       (KEY_PLAY_MODE_PWR | KEY_LONG_UP)
#define KH_PLAY_MODE_PWR        (KEY_PLAY_MODE_PWR | KEY_HOLD)
#define KD_PLAY_MODE_PWR        (KEY_PLAY_MODE_PWR | KEY_DOUBLE)
#define KTH_PLAY_MODE_PWR       (KEY_PLAY_MODE_PWR | KEY_THREE)
#define KLH_PLAY_MODE_PWR       (KEY_PLAY_MODE_PWR | KEY_LHOLD)

#define K_SIRI                  (KEY_SIRI | KEY_SHORT)
#define KU_SIRI                 (KEY_SIRI | KEY_SHORT_UP)
#define KL_SIRI                 (KEY_SIRI | KEY_LONG)
#define KLU_SIRI                (KEY_SIRI | KEY_LONG_UP)
#define KH_SIRI                 (KEY_SIRI | KEY_HOLD)
#define KD_SIRI                 (KEY_SIRI | KEY_DOUBLE)
#define KTH_SIRI                (KEY_SIRI | KEY_THREE)

#define K_HOME                  (KEY_HOME | KEY_SHORT)
#define KU_HOME                 (KEY_HOME | KEY_SHORT_UP)
#define KL_HOME                 (KEY_HOME | KEY_LONG)
#define KLU_HOME                (KEY_HOME | KEY_LONG_UP)
#define KH_HOME                 (KEY_HOME | KEY_HOLD)
#define KD_HOME                 (KEY_HOME | KEY_DOUBLE)
#define KTH_HOME                (KEY_HOME | KEY_THREE)

#define K_PLAY_USER_DEF         (KEY_PLAY_USER_DEF | KEY_SHORT)
#define KU_PLAY_USER_DEF        (KEY_PLAY_USER_DEF | KEY_SHORT_UP)
#define KL_PLAY_USER_DEF        (KEY_PLAY_USER_DEF | KEY_LONG)
#define KLU_PLAY_USER_DEF       (KEY_PLAY_USER_DEF | KEY_LONG_UP)
#define KH_PLAY_USER_DEF        (KEY_PLAY_USER_DEF | KEY_HOLD)
#define KD_PLAY_USER_DEF        (KEY_PLAY_USER_DEF | KEY_DOUBLE)
#define KTH_PLAY_USER_DEF       (KEY_PLAY_USER_DEF | KEY_THREE)
#define KFO_PLAY_USER_DEF       (KEY_PLAY_USER_DEF | KEY_FOUR)
#define KFI_PLAY_USER_DEF       (KEY_PLAY_USER_DEF | KEY_FIVE)

#define K_PLAY_PWR_USER_DEF     (KEY_PLAY_PWR_USER_DEF | KEY_SHORT)
#define KU_PLAY_PWR_USER_DEF    (KEY_PLAY_PWR_USER_DEF | KEY_SHORT_UP)
#define KL_PLAY_PWR_USER_DEF    (KEY_PLAY_PWR_USER_DEF | KEY_LONG)
#define KLU_PLAY_PWR_USER_DEF   (KEY_PLAY_PWR_USER_DEF | KEY_LONG_UP)
#define KH_PLAY_PWR_USER_DEF    (KEY_PLAY_PWR_USER_DEF | KEY_HOLD)
#define KD_PLAY_PWR_USER_DEF    (KEY_PLAY_PWR_USER_DEF | KEY_DOUBLE)
#define KTH_PLAY_PWR_USER_DEF   (KEY_PLAY_PWR_USER_DEF | KEY_THREE)
#define KFO_PLAY_PWR_USER_DEF   (KEY_PLAY_PWR_USER_DEF | KEY_FOUR)
#define KFI_PLAY_PWR_USER_DEF   (KEY_PLAY_PWR_USER_DEF | KEY_FIVE)
#define KLH_PLAY_PWR_USER_DEF   (KEY_PLAY_PWR_USER_DEF | KEY_LHOLD)

#define K_ANC                   (KEY_ANC | KEY_SHORT)
#define KU_ANC                  (KEY_ANC | KEY_SHORT_UP)
#define KL_ANC                  (KEY_ANC | KEY_LONG)
#define KLU_ANC                 (KEY_ANC | KEY_LONG_UP)
#define KH_ANC                  (KEY_ANC | KEY_HOLD)
#define KD_ANC                  (KEY_ANC | KEY_DOUBLE)
#define KTH_ANC                 (KEY_ANC | KEY_THREE)

#define KU_IR_POWER             (KEY_IR_POWER | KEY_SHORT_UP)

#define KU_VOICE_RM             (KEY_VOICE_RM | KEY_SHORT_UP)
#define KL_VOICE_RM             (KEY_VOICE_RM | KEY_LONG)

#define K_USB_SD                (KEY_USB_SD | KEY_SHORT)
#define KU_USB_SD               (KEY_USB_SD | KEY_SHORT_UP)

//按键消抖参数
typedef struct {
    u16 scan_cnt;
    u16 up_cnt;
    u16 long_cnt;
    u16 hold_cnt;
} key_shake_tbl_t;

typedef struct {
    u8 adc_val;
    u8 usage_id;
} adkey_tbl_t;

typedef struct {
    psfr_t sfr;             //SADCDAT SFR
    u16 channel;

#if USER_ADKEY
    u8 key_val;
#endif // USER_ADKEY

#if USER_ADKEY2
    u8 key2_val;
#endif // USER_ADKEY2

#if USER_NTC
    u8 ntc_val;
#endif

#if USER_ADKEY_MUX_SDCLK
    u8 sdclk_val;
    u8 sdclk_convert : 1,                //ADC转换标志
       sdclk_valid   : 1;                //ADC转换数据有效
#endif // USER_ADKEY_MUX_SDCLK

    u16 vrtc_val;
    u16 wko_val;

#if VBAT_DETECT_EN
    u16 bg;
    u16 vbat2;
    u32 vbat_val;
    u32 vbat_total;
#endif // VBAT_DETECT_EN

    u8 mic_dc_val;
} adc_cb_t;
extern adc_cb_t adc_cb;

extern const adkey_tbl_t adkey_table[];
extern const adkey_tbl_t adkey2_table[];
extern const adkey_tbl_t pwrkey_table[6];
extern const u8 key_config_table[];

void io_key_init(void);
u8 get_iokey(void);
void key_var_init(void);
void key_init(void);
u8 bsp_key_scan(void);
void power_on_check(void);

u8 *get_adkey_configure(u8 num);
u8 *get_pwrkey_configure(u8 num);
u8 check_key_double_configure(u16 key_val);

void bsp_saradc_init(u16 adc_ch);
int is_lowpower_vbat_warning(void);
void pwr_down_flag_clr(void);
void pwr_down_flag_set(void);
bool pwrkey_get_status(void);
u8 get_double_key_time(void);
void adc_mux_sdclk_w4_convert(void);

u8 bsp_key_scan_do(void);
u8 bsp_key_pwr_scan(void);
#endif // _BSP_KEY_H
