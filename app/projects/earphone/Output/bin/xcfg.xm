depend(0x00090000);

config(SUB, "系统配置", "系统的相关配置");
config(LIST, "提示音语言选择", "选择系统的语言", LANG_ID, 4, "英文", "中文", "中英文(出厂默认英文)", "中英文(出厂默认中文)", 0);
config(LEVEL, 0x00);
config(CHECK, "SD/U盘播放功能", "是否需要SD/UDISK播放功能", FUNC_MUSIC_EN, 0);
config(CHECK, "AUX输入功能", "是否需要AUX功能", FUNC_AUX_EN, 0);
config(LEVEL, 0x03);
config(CHECK, "POWKEY 10s复位系统", "是否长按POWKEY 10s复位系统，用于防止系统死机", POWKEY_10S_RESET, 1);
config(LISTVAL, "自动保存参数时延", "设置参数后延时N秒保存", AUTO_SAVE_TIME, BIT, 3, 1, 8, ("5秒", 0), ("10秒", 1), ("15秒", 2), ("20秒", 3), ("25秒", 4), ("30秒", 5), ("35秒", 6), ("40秒", 7));
config(CHECK, "功放MUTE功能", "是否使能功放MUTE功能", SPK_MUTE_EN, 0);
config(LISTVAL, "功放MUTE控制IO选择", "功放MUTE控制GPIO选择", SPK_MUTE_IO_SEL, BIT, 5, 0, 21, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 26), ("PE0", 14), ("PE4", 18), ("PE5", 19), ("PE6", 20), ("PE7", 21), ("PF0", 22), ("PF1", 23), ("PF2", 24), ("PF3", 25), SPK_MUTE_EN);
config(CHECK, "高MUTE", "是否为高MUTE，否则为低MUTE", HIGH_MUTE, 1, SPK_MUTE_EN);
config(BYTE, "功放MUTE延时(单位5ms)", "功放MUTE的延时控制，防止解MUTE时间不够导致声音不全。", LOUDSPEAKER_UNMUTE_DELAY, 0, 255, 6, SPK_MUTE_EN);
config(LEVEL, 0x00);
config(LIST, "功放AB/D控制模式", "功放AB/D控制模式选择", AMPABD_TYPE, 2, "独立IO电平控制", "mute脉冲控制", 0);
config(LISTVAL, "功放AB/D控制IO选择", "功放AB/D控制GPIO选择", AMPABD_IO_SEL, BIT, 5, 0, 21, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 26), ("PE0", 14), ("PE4", 18), ("PE5", 19), ("PE6", 20), ("PE7", 21), ("PF0", 22), ("PF1", 23), ("PF2", 24), ("PF3", 25));
config(LISTVAL, "耳机检测IO选择", "选择耳机检测IO选择", EARPHONE_DET_IOSEL, BIT, 6, 0, 24, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 26), ("PE0", 14), ("PE4", 18), ("PE5", 19), ("PE6", 20), ("PE7", 21), ("PF0", 22), ("PF1", 23), ("PF2", 24), ("PF3", 25), ("复用SDCLK检测", 27), ("复用SDCMD检测", 28), ("复用PWRKEY检测", 29));
config(LEVEL, 0x03);
config(LISTVAL, "自动休眠时间", "设置自动休眠时间", SYS_SLEEP_TIME, SHORT, 30, 21, ("不休眠", 0), ("10秒钟后", 10), ("20秒钟后", 20), ("30秒钟后", 30), ("45秒钟后", 45), ("1分钟后", 60), ("2分钟后", 120), ("3分钟后", 180), ("4分钟后", 240), ("5分钟后", 300), ("6分钟后", 360), ("7分钟后", 420), ("8分钟后", 480), ("9分钟后", 540), ("10分钟后", 600), ("15分钟后", 900), ("20分钟后", 1200), ("25分钟后", 1500), ("30分钟后", 1800), ("45分钟后", 2700), ("1小时后", 3600));
config(LISTVAL, "自动关机时间", "设置自动关机时间", SYS_OFF_TIME, 	SHORT, 300, 18, ("不关机", 0), ("30秒钟后", 30), ("1分钟后", 60),  ("2分钟后", 120), ("3分钟后", 180), ("4分钟后", 240), ("5分钟后", 300), ("6分钟后", 360), ("7分钟后", 420), ("8分钟后", 480), ("9分钟后", 540), ("10分钟后", 600), ("15分钟后", 900), ("20分钟后", 1200), ("25分钟后", 1500), ("30分钟后", 1800), ("45分钟后", 2700), ("1小时后", 3600));
config(LIST, "电量满电电压", "选择满电下电池电压", FULL_VBAT, 10, "3.3V", "3.4V", "3.5V", "3.6V", "3.7V", "3.8V", "3.9V", "4.0V", "4.1V", "4.2V", 9);
config(CHECK, "低电提示", "当电压比较低的时候，会有提示音警告", LOWPOWER_WARNING_EN, 1);
config(LIST, "低电语音提示电压", "选择低电语音提示电压", LPWR_WARNING_VBAT, 10, "2.8V", "2.9V", "3.0V", "3.1V", "3.2V", "3.3V", "3.4V", "3.5V", "3.6V", "3.7V", 7, LOWPOWER_WARNING_EN);
config(LIST, "低电关机电压", "选择低电关机电压", LPWR_OFF_VBAT, 11, "不关机", "2.8V", "2.9V", "3.0V", "3.1V", "3.2V", "3.3V", "3.4V", "3.5V", "3.6V", "3.7V", 6, LOWPOWER_WARNING_EN);
config(BYTE, "低电语音播报周期(秒)", "设置低电语音播报周期(秒)", LPWR_WARNING_PERIOD, 1, 240, 30, LOWPOWER_WARNING_EN);
config(LIST, "音量级数", "选择系统音量级数", VOL_MAX, 4, "16级音量", "30级音量", "32级音量", "50级音量", 2);
config(BYTE, "开机默认音量", "开机默认音量级数", SYS_INIT_VOL, 0, 50, 15);
config(BYTE, "提示音播放最小音量", "设置提示音播放音量不可低于的级数", WARNING_VOLUME_MIN, 0, 50, 5);
config(BYTE, "提示音播放初始音量", "设置提示音播放音量级数", WARNING_VOLUME, 0, 50, 5);
config(LIST, "OSC基础电容", "同时配置26M晶振OSCI与OSCO负载电容", OSC_BOTH_CAP, 2, "0PF", "6PF", 0);
config(BYTE, "OSCI电容(0.25PF)", "独立配置26M晶振OSCI负载电容，单位0.25PF。OSCI电容大小：n * 0.25PF + 基础电容", OSCI_CAP, 0, 63, 23);
config(BYTE, "OSCO电容(0.25PF)", "独立配置26M晶振OSCO负载电容，单位0.25PF。OSCO电容大小：n * 0.25PF + 基础电容", OSCO_CAP, 0, 63, 23);
config(LISTVAL, "软开机长按时间选择", "长按PWRKEY多长时间后软开机", PWRON_PRESS_TIME,  BIT, 3, 3, 8, ("0.1秒", 0), ("0.5秒", 1), ("1秒", 2), ("1.5秒", 3), ("2秒", 4), ("2.5秒", 5), ("3秒", 6), ("3.5秒", 7));
config(LISTVAL, "软关机长按时间选择", "长按PWRKEY多长时间后软关机", PWROFF_PRESS_TIME, BIT, 3, 3, 8, ("1.5秒", 0), ("2秒", 1), ("2.5秒", 2), ("3秒", 3), ("3.5秒", 4), ("4秒", 5), ("4.5秒", 6), ("5秒", 7));
config(CHECK, "EQ调试（蓝牙串口）", "是否使用SPP调试EQ功能", EQ_DGB_SPP_EN, 0);
config(CHECK, "HUART调试(EQ/FCC)", "是否使用HUART调试功能", HUART_EN, 0);
config(LISTVAL, "HUART串口选择", "选择HUART的IO", HUART_SEL, BIT, 4, 0, 11, ("PA7", 0), ("PB2", 1), ("PB3", 2), ("PE7", 3), ("PA1", 4), ("PA6", 5),("PB1", 6),("PB4", 7),("PE6", 8),("PA0", 9),("VUSB", 10), HUART_EN);
config(CHECK,  "是否使能NTC",  "是否使能NTC",  NTC_EN,  0);
config(CHECK, "NTC使用内部10K上拉", "NTC是否使用内部10K上拉,使能程序中的宏USER_NTC后才有效", NTC_USER_INNER_PU,0, NTC_EN);
config(BYTE,  "NTC阈值",  "NTC阈值",  NTC_THD_VAL,  0, 255, 100,  NTC_EN);

config(SUB, "电源配置", "电源的相关配置");
config(CHECK, "BUCK MODE", "是否设置为BUCK MODE", BUCK_MODE_EN, 0);
config(CHECK, "关闭BUCK限流", "是否关闭BUCK MODE限流, 仅在BUCK MODE打开时有效", BUCK_LIMIT_DIS, 0);
config(CHECK, "VDDBT省电容", "是否VDDBT省电容，只能LDO模式才能省电容", VDDBT_CAPLESS_EN, 1, BUCK_MODE_EN);
config(LISTVAL, "VDDIO电压", "选择VDDIO电压, VDDAUD与VDDIO短接可能需要调VDDIO电压。", VDDIO_SEL, BIT, 4, 7, 12, ("None", 0), ("2.7V", 3), ("2.8V", 4), ("2.9V", 5), ("3.0V", 6), ("3.1V", 7), ("3.2V", 8), ("3.3V", 9), ("3.4V", 10), ("3.5V", 11), ("3.6V", 12), ("3.7V", 13));
config(LEVEL, 0x00);
config(LISTVAL, "VDDBT电压", "选择VDDBT电压, None表示使用trim值(1.2V)", VDDBT_SEL, BIT, 5, 0, 8, ("None", 0), ("1.1V", 8), ("1.15V", 9), ("1.2V", 10), ("1.25V", 11), ("1.3V", 12), ("1.35V", 13), ("1.4V", 14));
config(LEVEL, 0x03);

config(SUB, "DAC配置", "DAC的相关配置");
config(LISTVAL, "DAC声道选择", "选择DAC的输出方式", DAC_SEL, BIT, 4, 1, 6, ("单声道", 0), ("双声道", 1), ("VCMBUF单声道", 2), ("VCMBUF双声道", 3), ("差分单声道", 4), ("差分双声道", 5));
config(LISTVAL, "VDDDAC电压", "选择VDDDAC电压", DAC_LDOH_SEL, BIT, 4, 3, 8, ("2.5V", 0), ("2.7V", 1), ("2.9V", 2), ("3.0V", 3), ("3.1V", 4), ("3.2V", 5), ("3.1V_VSEL2", 6), ("3.2V_VSEL2", 7));
config(CHECK, "省VCM方案", "是否使用内部VCM，SOP16, SSOP24, SSOP28使用内部VCM", DAC_VCM_LESS_EN, 1);
config(CHECK, "省VDDDAC方案", "是否VDDDAC Bypass，内部VDDDAC与VDDIO短接。", DACVDD_BYPASS_EN, 0);
config(CHECK, "蓝牙连接时关闭DAC", "蓝牙连接过程关闭DAC，仅对差分/VCMVUF输出有效", DAC_OFF_FOR_CONN, 0);
config(LIST, "DAC最大音量", "配置系统最大模拟增益，自动调整模拟音量表。", DAC_MAX_GAIN, 15, "-9DB", "-8DB", "-7DB", "-6DB", "-5DB", "-4DB", "-3DB", "-2DB", "-1DB", "0DB", "+1DB", "+2DB", "+3DB", "+4DB", "+5DB", 9);
config(LIST, "通话最大音量", "配置通话时最大模拟增益，自动调整模拟音量表。", BT_CALL_MAX_GAIN, 15, "-9DB", "-8DB", "-7DB", "-6DB", "-5DB", "-4DB", "-3DB", "-2DB", "-1DB", "0DB", "+1DB", "+2DB", "+3DB", "+4DB", "+5DB", 8);
config(CHECK, "VUSB输出DACRP", "通过VUSB输出DACRP的声音", DACRP_VUSB_EN, 0);

config(SUB, "充电配置", "充电功能的相关配置");
config(CHECK, "充电使能", "是否打开充电功能", CHARGE_EN, 1);
config(CHECK, "涓流充电使能", "是否使能涓流充电", CHARGE_TRICK_EN, 1, CHARGE_EN);
config(CHECK, "插入DC复位系统", "是否插入DC充电复位系统，用于防止系统死机", CHARGE_DC_RESET, 1, CHARGE_EN);
config(CHECK, "插入DC禁止软开机", "PWRKEY软开机，DC Online时，禁止软开机", CHARGE_DC_NOT_PWRON, 1, CHARGE_EN);
config(LIST, "充电截止电流", "选择充电截止电流", CHARGE_STOP_CURR, 8, "2.5mA", "5mA", "10mA", "15mA", "20mA", "25mA", "30mA", "35mA", 3, CHARGE_EN);
config(LIST, "恒流充电电流", "恒流充电（电池电压大于2.9v）电流", CHARGE_CONSTANT_CURR, 16, "10mA", "20mA", "30mA", "40mA", "50mA", "60mA", "70mA", "80mA", "90mA", "100mA", "110mA", "120mA", "140mA", "160mA", "180mA", "200mA", 3, CHARGE_EN);
config(LIST, "涓流充电电流", "涓流充电（电池电压小于2.9v）电流", CHARGE_TRICKLE_CURR, 3, "10mA", "20mA", "30mA", 0, CHARGE_TRICK_EN);
config(LISTVAL, "充满电蓝灯亮", "设置充满电蓝灯亮时间", CHARGE_FULL_BLED, BIT, 3, 3, 8,  ("不亮蓝灯", 0), ("亮10秒", 1), ("亮20秒", 2), ("亮30秒", 3), ("1分钟", 4), ("2分钟", 5), ("3分钟", 6), ("常亮", 7), CHARGE_EN);
config(CHECK, "充满电自动关机", "电池充满自动关机功能，主要用于放入电池仓充电的选项", CH_FULL_AUTO_PWRDWN_EN, 1, CHARGE_EN);
config(CHECK, "充电仓功能", "是否使能充电仓功能", CHBOX_EN, 1, CHARGE_EN);
config(LISTVAL, "充电仓类型选择", "选择正确类型充电仓，确保充满关机和拿起开机功能正常", CH_BOX_TYPE_SEL, BIT, 2, 0, 4, ("兼容5V短暂掉0V后维持电压", 0), ("5V不掉电的充电仓", 1), ("5V掉电但有维持电压", 2), ("5V完全掉电无维持电压", 3), CHBOX_EN);
config(LISTVAL, "仓允许最低维持电压", "inbox信号电压，即充满维持电压的充电仓允许的最低维持电压", CH_INBOX_SEL, BIT, 1, 0, 2, ("1.1V", 0), ("1.7V", 1), CHBOX_EN);
config(CHECK, "从充电仓拿出自动开机", "耳机从充电仓拿出自动开机，注意充满后电池仓自动断电的不能开此选项", CH_OUT_AUTO_PWRON_EN, 1 CHBOX_EN);
config(LISTVAL, "出仓或入仓VUSB漏电配置", "加速耳机出仓时自动开机或入仓时唤醒充电仓", CH_LEAKAGE_SEL, BIT, 2, 2, 4, ("None", 0), ("漏电电流X1档", 1), ("漏电电流X2档", 2), ("漏电电流X3档", 3), CHARGE_EN);
config(CHECK, "入仓耳机关机", "有些维持电压的电池仓，耳机入仓后电池仓的5V不会自动起来进入充电, 这时耳机是否进入关机。", CHG_INBOX_PWRDWN_EN, 1, CHBOX_EN);
config(BIT, "短暂掉0V的仓稳定检测时间", "短暂掉0V的仓稳定检测时间: 500ms + n*100ms", CHBOX_OUT_DELAY,  4, 0, 15, 0, CHBOX_EN);

config(SUB, "蓝牙配置", "蓝牙的相关配置");
config(CHECK, "配置蓝牙名", "是否配置蓝牙名，使能后使用此处配置，否则为程序中宏定义", CONFIG_BT_NAME_EN, 0);
config(TEXT, "蓝牙名称", "手机上可以看到的蓝牙名称", BT_NAME, 32, "Bluetrum-TWS", CONFIG_BT_NAME_EN);
config(MAC, "蓝牙地址", "蓝牙的MAC地址", BT_ADDR, 6, 41:42:00:00:00:00, 41:42:FF:FF:FF:FF, 41:42:00:00:00:01);
config(LISTVAL, "蓝牙空闲几秒后休眠", "是否支持N秒空闲进入休眠低功耗状态", BT_SLEEP_TIME, BIT, 3, 6, 8, ("不支持", 0), ("10秒", 1), ("20秒", 2), ("30秒", 3), ("40秒", 4), ("50秒", 5), ("60秒", 6), ("70秒", 7));
config(LISTVAL, "长按几秒开机进配对", "是否支持长按N秒开机直接进入配对状态", BT_PWRKEY_NSEC_DISCOVER, BIT, 3, 0, 8, ("不支持", 0), ("2秒", 1), ("3秒", 2), ("4秒", 3), ("5秒", 4), ("6秒", 5), ("7秒", 6), ("8秒", 7));
config(CHECK, "配对提示音", "是否播放配对提示音", WARNING_BT_PAIR, 1);
config(CHECK, "连接两部手机功能", "是否支持连接两部手机功能（打开<TWS功能>后无效）", BT_2ACL_EN, 0);
config(CHECK, "音乐播放功能", "是否支持蓝牙音乐播放功能", BT_A2DP_EN, 1);
config(CHECK, "音乐音量同步", "是否支持A2DP音量与手机同步", BT_A2DP_VOL_CTRL_EN, 0, BT_A2DP_EN);
config(CHECK, "通话功能", "是否支持蓝牙通话的功能", BT_SCO_EN, 1);
config(CHECK, "私密接听功能", "是否使用手动私密接听（切换到手机端接听）", BT_HFP_PRIVATE_EN, 1, BT_SCO_EN);
config(CHECK, "来电报号功能", "是否支持来电报号功能", BT_HFP_RING_NUMBER_EN, 1, BT_SCO_EN);
config(CHECK, "串口功能", "是否支持蓝牙串口的功能", BT_SPP_EN, 0);
config(CHECK, "拍照功能", "是否支持蓝牙HID拍照的功能", BT_HID_EN, 0);
config(CHECK, "按键HID连接/断开功能", "是否支持蓝牙HID服务，按键手动连接/断开", BT_HID_MANU_EN, 0, BT_HID_EN);
config(CHECK, "HID默认不连接", "HID服务默认不连接，需要按键手动连接", BT_HID_DISCON_DEFAULT_EN, 0, BT_HID_EN);
config(LEVEL, 0x00);
config(CHECK, "HID独立自拍器模式", "是否支持独立自拍器模式功能", FUNC_BTHID_EN, 0);
config(TEXT, "HID蓝牙名称", "手机上可以看到的独立自拍器模式蓝牙名称", BTHID_NAME, 32, "BT-Photo", FUNC_BTHID_EN);
config(LEVEL, 0x03);
config(CHECK, "BLE控制功能", "是否支持BLE音乐控制的功能", BLE_EN, 0);
config(TEXT, "BLE名称", "手机上可以看到的BLE蓝牙名称", LE_NAME, 32, "LE-Remoter", CONFIG_BT_NAME_EN);
config(CHECK, "快速测试使能", "快速测试使能", QTEST_EN, 0);
config(LIST, "耳机拿起后状态", "设置耳机拿起后的状态,以测试盒设置为准",  QTEST_PICKUP_STATUS, 4, "不操作", "拿起关机", "拿起复位", "断开蓝牙连接后关机" 0,QTEST_EN);
config(U32, "快测配对ID选择", "测试盒可根据ID选择是否可进行快速配对", QTEST_TWS_PAIR_ID, 0, 0x7fffffff,  0x38383930,  QTEST_EN);

config(SUB, "TWS配置", "TWS的相关配置");
config(CHECK, "TWS功能", "是否支持TWS功能，打开后<连接两部手机功能>无效", BT_TWS_EN, 1);
config(CHECK, "TWS组队绑定功能", "打开TWS组队绑定后，需要手动删除组队信息才能重新组队", BT_TWS_PAIR_BONDING_EN, 0, BT_TWS_EN);
config(LISTVAL, "TWS组队方式选择", "选择TWS组队操作方式", BT_TWS_PAIR_MODE, BIT, 3, 0, 6, ("自动组队", 0), ("自定义组队<调用api>", 1), ("双击USER_DEF键组队", 2), ("单击USER_DEF键组队", 3), ("长按USER_DEF键组队", 4), ("双击MODE/POWER键组队", 5) BT_TWS_EN);
config(LISTVAL, "TWS声道分配选择", "选择TWS声道分配方式", BT_TWS_LR_MODE, BIT, 4, 1, 7, ("不分配，主副均双声道输出", 0), ("自动分配，主右声道副左声道", 1), ("自动分配，主左声道副右声道", 2), ("PWRKEY,有820K接地为左", 8), ("GPIOx有接地为左声道", 9), ("配置选择为左声道", 10), ("配置选择为右声道", 11), BT_TWS_EN);
config(LISTVAL, "TWS声道GPIOx", "GPIOx有接地为左声道", TWS_SEL_LEFT_GPIO_SEL, BIT, 5, 0, 21, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 26), ("PE0", 14), ("PE4", 18), ("PE5", 19), ("PE6", 20), ("PE7", 21), ("PF0", 22), ("PF1", 23), ("PF2", 24), ("PF3", 25), BT_TWS_EN);
config(CHECK, "TWS蓝牙名加L或R后缀", "是否使能TWS蓝牙名称加L或R后缀, 需要GPIOx固定左右耳", BT_TWS_NAME_SUFFIX_EN, 0, BT_TWS_EN);
config(CHECK, "TWS主从切换功能", "是否支持TWS主从自动切换功能", BT_TSWI_EN, 1, BT_TWS_EN);
config(CHECK, "TWS按键关机支持主从切换", "TWS按键手动关机是否自动切换主从, 否则同步关机", BT_TSWI_KPWR_EN, 1, BT_TSWI_EN);
config(CHECK, "TWS音乐模式支持主从切换", "TWS音乐模式,根据主副耳信号质量自动主从切换", BT_TSWI_MSC_EN, 0, BT_TSWI_EN);
config(CHECK, "TWS通话模式支持主从切换", "TWS通话模式,根据主副耳信号质量自动主从切换", BT_TSWI_SCO_EN, 0, BT_TSWI_EN);
config(CHECK, "TWS耳机入仓充电复位", "TWS耳机入仓主从切换后是否复位", BT_TSWI_CHARGE_RST_EN, 0, BT_TSWI_EN);
config(CHECK, "TWS耳机出仓播放开机音", "TWS耳机出仓是否播放开机音", BT_OUTBOX_VOICE_PWRON_EN, 0, BT_TWS_EN);

config(SUB, "RF参数", "蓝牙RF参数");
config(BIT, "预置RF参数功率", "使用预置的RF参数，每级可降低0.5dbm发射功率", BT_RF_PWRDEC, 6, 0, 24, 0);
config(CHECK, "自定义RF参数", "选用自定义RF参数后，预置RF参数无效", BT_RF_PARAM_EN, 0);
config(LEVEL, 0x00);
config(BYTE, "GL_PA_GAIN",    "PA_GAIN, 参考值3",         RF_PA_GAIN,    0,  3,  3, BT_RF_PARAM_EN);
config(LEVEL, 0x03);
config(BYTE, "GL_MIX_GAIN",   "MIX_GAIN, 参考值3~5",      RF_MIX_GAIN,   0,  7,  4, BT_RF_PARAM_EN);
config(BYTE, "GL_DIG_GAIN",   "DIG_GAIN, 参考值30~59",    RF_DIG_GAIN,   24,59, 59, BT_RF_PARAM_EN);
config(BYTE, "GL_PA_CAP",     "PA_CAP, 参考值12~14",      RF_PA_CAP,     0, 15, 14, BT_RF_PARAM_EN);
config(BYTE, "GL_MIX_CAP",    "MIX_CAP, 参考值7~9",       RF_MIX_CAP,    0, 15,  8, BT_RF_PARAM_EN);
config(LEVEL, 0x00);
config(BYTE, "GL_TX_DBM",     "Cable实测dbm值",           RF_TXDBM,      0, 12,  6, BT_RF_PARAM_EN);
config(BYTE, "GL_UDF0",       "UDF(保留使用)",            RF_UDF0,       0,  0,  0, BT_RF_PARAM_EN);
config(BYTE, "GL_UDF1",       "UDF(保留使用)",            RF_UDF1,       0,  0,  0, BT_RF_PARAM_EN);
config(LEVEL, 0x03);
config(LISTVAL, "降低回连功率", "是否降低回连手机或TWS的功率", BT_RF_PAGE_PWRDEC, BIT, 3, 1, 4, ("不降低", 0), ("降低3dbm", 1), ("降低6dbm", 2), ("降低9dbm", 3));
config(LISTVAL, "降低组队功率", "是否降低TWS搜索组队的功率", BLE_RF_PAGE_PWRDEC, BIT, 3, 0, 4, ("不降低", 0), ("降低3dbm", 1), ("降低6dbm", 2), ("降低9dbm", 3));
config(BYTE, "设置组队范围RSSI(-dbm)", "该值越小(-90dbm最小),组队范围约大", BLE_PAGE_RSSI_THR, 20, 90, 90);
config(CHECK, "通话RF参数", "是否单独配置通话RF参数", BT_RF_PARAM_SCO_EN, 0, BT_RF_PARAM_EN);
config(BYTE, "SCO_PA_GAIN",   "通话PA_GAIN, 参考值7",     RF_PA_GAIN_SCO,    0,  3,  3, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_MIX_GAIN",  "通话MIX_GAIN, 参考值3~5",  RF_MIX_GAIN_SCO,   0,  7,  4, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_DIG_GAIN",  "通话DIG_GAIN, 参考值30~59",RF_DIG_GAIN_SCO,   24,61, 59, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_PA_CAP",    "通话PA_CAP, 参考值7~9",    RF_PA_CAP_SCO,     0, 15,  8, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_MIX_CAP",   "通话MIX_CAP, 参考值7~9",   RF_MIX_CAP_SCO,    0, 15,  8, BT_RF_PARAM_SCO_EN);
config(LEVEL, 0x00);
config(BYTE, "SCO_TX_DBM",    "通话Cable实测dbm值",       RF_TXDBM_SCO,      0, 12,  6, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_UDF0",      "通话UDF(保留使用)",        RF_UDF0_SCO,       0,  0,  0, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_UDF1",      "通话UDF(保留使用)",        RF_UDF1_SCO,       0,  0,  0, BT_RF_PARAM_SCO_EN);
config(LEVEL, 0x03);


config(SUB, "MIC参数", "蓝牙和ANC的MIC参数");
config(CHECK, "MICL功能", "是否打开MICL功能", MICL_EN, 1);
config(LISTVAL, "MICL通路选择", "选择MICL通路", MICL_SEL, BIT, 3, 1, 3, ("MICNL_PF4", 0), ("MICPL_PF5", 1), ("MICL_VUSB", 2),MICL_EN);
config(LISTVAL, "MICL偏置电路配置", "MICL电路BIAS配置，省电容，省电阻配置", MICL_BIAS_METHOD, BIT, 3, 0, 7, ("单端MIC外部电阻电容", 0), ("单端MIC外部电阻电容PE7供电", 1), ("单端MIC内部电阻电容", 2), ("差分MIC", 3), ("差分MIC PE7供电", 4), ("单端MIC(10K)", 5), ("单端MIC(10K)PE7供电", 6), MICL_EN);
config(BIT, "MICL内部偏置电阻大小", "MICL内部偏置电阻配置", MICL_BIAS_RESISTOR, 4, 0, 15, 3, MICL_EN);
config(BYTE, "MICL模拟增益", "MICL模拟增益配置(3~42DB), Step 3DB", MICL_ANL_GAIN, 0, 13, 6, MICL_EN);
config(LISTVAL, "MICL通话配置", "MICL功能配置, 通话副MIC仅用于双MIC降噪功能", MICL_CFG, BIT, 2, 1, 3, ("None", 0), ("通话主MIC", 1), ("通话副MIC", 2), MICL_EN);

config(CHECK, "MICR功能", "是否打开MICR功能", MICR_EN, 0);
config(LISTVAL, "MICR通路选择", "选择MICR通路", MICR_SEL, BIT, 3, 5, 3, ("MICNR_PF2", 4), ("MICPR_PF3", 5), ("MICR_VUSB", 6),MICR_EN);
config(BIT, "MICR内部偏置电阻大小", "MICR内部偏置电阻配置", MICR_BIAS_RESISTOR, 4, 0, 15, 3, MICR_EN);
config(LISTVAL, "MICR偏置电路配置", "MICR电路BIAS配置，省电容，省电阻配置", MICR_BIAS_METHOD, BIT, 3, 0, 7, ("单端MIC外部电阻电容", 0), ("单端MIC外部电阻电容PE7供电", 1), ("单端MIC内部电阻电容", 2), ("差分MIC", 3), ("差分MIC PE7供电", 4), ("单端MIC(10K)", 5), ("单端MIC(10K)PE7供电", 6), MICR_EN);
config(BYTE, "MICR模拟增益", "MICR模拟增益配置(3~42DB), Step 3DB", MICR_ANL_GAIN, 0, 13, 6, MICR_EN);
config(LISTVAL, "MICR通话配置", "MICR功能配置, 通话副MIC仅用于双MIC降噪功能", MICR_CFG, BIT, 2, 0, 3,  ("None", 0), ("通话主MIC", 1), ("通话副MIC", 2), MICR_EN);
config(CHECK, "MICR反相", "双麦降噪两麦相位相反需要使能", MICR_PHASE_EN, 0);

config(BYTE, "通话MIC数字增益", "MIC数字增益配置(0~32DB), Step 0.5DB", BT_DIG_GAIN, 0, 63, 36, BT_SCO_EN);
config(BIT, "通话MIC后置数字增益", "调节算法后置MIC数字增益", MIC_POST_GAIN, 4, 0, 15, 2, BT_SCO_EN);

config(CHECK, "ANC功能", "是否打开ANC功能", ANC_EN, 0);
config(LISTVAL, "ANC模式", "选择ANC的模式", ANC_MODE, BIT, 1, 0, 2, ("FF/FB Mode", 0), ("Hybrid Mode", 1), ANC_EN);
config(CHECK, "MIC Gain产测配置", "MIC Gain是否取产测值，否则取资源文件值", ANC_GAIN_EN, 0, ANC_EN);

config(SUB, "通话参数", "蓝牙的通话参数", BT_SCO_EN);
config(NUM, "NR降噪环境噪声阈值", "环境噪声阈值(低于此阈值便认为是噪声)，打开声加算法后无效", BT_NOISE_THRESHOID, 0, 65535, 1536, BT_SCO_EN);
config(NUM, "SNDP单麦RNN降噪量", "声加算法降噪量（0~58dB,步进2dB）", BT_SNDP_LEVEL, 0, 29, 10, BT_SCO_EN);
config(CHECK, "SNDP单麦DNN算法", "单麦DNN降噪参数配置", BT_SNDP_DNN_EN, 0, BT_SCO_EN);
config(NUM, "DNN最大降噪量", "如果信号质量好的时候，感觉降噪量不够，可以适当调大最大降噪量（18~30dB,步进2dB,默认24dB）", BT_SNDP_DNN_MAX_LEVEL, 9, 15, 12, BT_SNDP_DNN_EN);
config(NUM, "DNN最小降噪量", "当信号质量差的时候，感觉有吃语音，可以适当调大最小降噪量（2~24dB,步进2dB,默认6dB）", BT_SNDP_DNN_MIN_LEVEL, 1, 12,  3, BT_SNDP_DNN_EN);
config(CHECK, "SNDP双麦功能", "是否支持双麦功能", BT_SNDP_DM_EN, 0, BT_SCO_EN);
config(BYTE, "双麦间距", "出音孔之间的直线距离（20~40mm）", BT_SNDP_DM_DISTANCE, 20, 40, 23, BT_SNDP_DM_EN);
config(BYTE, "拾音角度", "通话的拾音角度（与双麦之间的连线左右两边形成的夹角（10-89度 步进5度）），如果耳机佩戴位置变化大，建议加大角度", BT_SNDP_DM_DEGREE, 0, 16, 7, BT_SNDP_DM_EN);
config(BYTE, "最大保护角", "（10-89度 步进5度），默认89度；在调节拾音角度bf效果无法改善情况下可以调小此角度，但一定大于等于拾音角度", BT_SNDP_DM_MAX_DEGREE, 0, 16, 16, BT_SNDP_DM_EN);
config(BYTE, "通话降噪量", "通话角度的语音降噪量（0~58dB 步进2dB）", BT_SNDP_DM_LEVEL, 0, 29, 5, BT_SNDP_DM_EN);
config(BYTE, "风噪降噪量", "抑制风噪的降噪量，一般大于通话降噪量（0~58dB 步进2dB）", BT_SNDP_DM_WIND_LEVEL, 0, 29, 6, BT_SNDP_DM_EN);
config(BYTE, "低信噪比降噪量", "如果在一般信噪比下，降噪不足，就适当调大（0~0.4 步进0.05），默认2，2*0.05=0.1", BT_SNDP_DM_SNR_X0_LEVEL, 0, 8, 2, BT_SNDP_DM_EN);
config(BYTE, "低信噪比语音信号", "如果低信噪比下，语音断断续续，就适当调小（0.5~1 步进0.05），默认16，16*0.05=0.8", BT_SNDP_DM_SNR_X1_LEVEL, 10, 20, 16, BT_SNDP_DM_EN);
config(NUM, "响应限幅", "最大冲击响应限幅，声加工具会给一个参考值，也是最大值，越大收敛越快，超过参考值会导致间断失音，默认131072", BT_SNDP_DM_MAXIR, 0, 262144, 131072, BT_SNDP_DM_EN);
config(CHECK, "DMNS双麦降噪", "是否支持双麦降噪", BT_DMNS_EN, 0, BT_SCO_EN);
config(BYTE, "双麦间距", "出音孔之间的直线距离（20~40mm）", BT_DMNS_DISTANCE, 20, 40, 23, BT_DMNS_EN);
config(NUM, "拾音方向", "参考值为0，通话的拾音方向（双麦之间的连线顺时针旋转角度（步进1度，0~360度））", BT_DMNS_DEGREE, 0, 360, 0, BT_DMNS_EN);
config(BYTE, "通话降噪量", "参考值为0，值越大降噪越大", BT_DMNS_LEVEL, 0, 20, 0, BT_DMNS_EN);
config(BYTE, "喇叭声抑制量", "喇叭声抑制量，越大抑制效果越好，默认10", BT_TRUMPET_LEVEL, 0, 15, 10, BT_SCO_EN);
config(CHECK, "AEC功能", "是否支持AEC功能，AEC/ALC只能二选一", BT_AEC_EN, 1, BT_SCO_EN);
config(BYTE, "AEC回声消除级别", "回声消除级别(级别越高，回声衰减越明显，但通话效果越差)", BT_ECHO_LEVEL, 0, 15, 1, BT_AEC_EN);
config(BYTE, "AEC远端补偿值", "远端补偿值(0~255)", BT_FAR_OFFSET, 0, 255, 36, BT_AEC_EN);
config(CHECK, "AEC远端数据降噪", "使能则将远端数据经过远端降噪算法后再输入到AEC", BT_AEC_FAR_NR_EN, 0, BT_AEC_EN);
config(CHECK, "ALC功能", "是否支持ALC功能，AEC/ALC只能二选一", BT_ALC_EN, 0, BT_SCO_EN);
config(LEVEL, 0x00);
config(LIST, "ALC淡入延时", "ALC近端延时多少时间才淡入", BT_ALC_IN_DELAY, 6, "16ms", "64ms", "112ms", "160ms", "224ms", "288ms", 3, BT_ALC_EN);
config(LIST, "ALC淡入速度", "ALC近端淡入速度", BT_ALC_IN_STEP, 6, "4ms", "8ms", "16ms", "32ms", "64ms", "128ms", 4, BT_ALC_EN);
config(LIST, "ALC淡出延时", "ALC近端延时多少时间才淡出", BT_ALC_OUT_DELAY, 6, "16ms", "64ms", "112ms", "160ms", "224ms", "288ms", 0, BT_ALC_EN);
config(LIST, "ALC淡出速度", "ALC近端淡出速度", BT_ALC_OUT_STEP, 6, "4ms", "8ms", "16ms", "32ms", "64ms", "128ms", 0, BT_ALC_EN);
config(LEVEL, 0x03);

config(LEVEL, 0x00);
config(SUB, "音乐配置", "音乐播放模式的相关配置", FUNC_MUSIC_EN);
config(CHECK, "WAV解码", "是否支持WAV格式解码", MUSIC_WAV_SUPPORT, 1, FUNC_MUSIC_EN);
config(CHECK, "WMA解码", "是否支持WMA格式解码", MUSIC_WMA_SUPPORT, 0, FUNC_MUSIC_EN);
config(CHECK, "APE解码", "是否支持APE格式解码", MUSIC_APE_SUPPORT, 0, FUNC_MUSIC_EN);
config(CHECK, "FLAC解码", "是否支持FLAC格式解码", MUSIC_FLAC_SUPPORT, 0, FUNC_MUSIC_EN);
config(CHECK, "SDCARD播放功能", "是否支持SD/MMC卡播放功能", MUSIC_SDCARD_EN, 0, FUNC_MUSIC_EN);
config(CHECK, "UDISK播放功能", "是否支持UDISK播放功能", MUSIC_UDISK_EN, 0, FUNC_MUSIC_EN);
config(LISTVAL, "SDCARD检测IO选择", "选择SDCARD检测GPIO", SDDET_IOSEL, BIT, 6, 0, 24, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 26), ("PE0", 14), ("PE4", 18), ("PE5", 19), ("PE6", 20), ("PE7", 21), ("PF0", 22), ("PF1", 23), ("PF2", 24), ("PF3", 25), ("复用SDCLK检测", 27), ("复用SDCMD检测", 28), ("复用PWRKEY检测", 29), MUSIC_SDCARD_EN);
config(LISTVAL, "SDCARD1检测IO选择", "选择SDCARD1检测GPIO(双SD卡, 默认宏关闭的)", SD1DET_IOSEL, BIT, 6, 0, 24, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 26), ("PE0", 14), ("PE4", 18), ("PE5", 19), ("PE6", 20), ("PE7", 21), ("PF0", 22), ("PF1", 23), ("PF2", 24), ("PF3", 25), ("复用SDCLK检测", 27), ("复用SDCMD检测", 28), ("复用PWRKEY检测", 29), MUSIC_SDCARD_EN);

config(SUB, "AUX配置", "AUX模式的相关配置", FUNC_AUX_EN);
config(CHECK, "AUX通路进ADC", "AUX是否先通过ADC采样,否则直通DAC。进ADC可以调AUX EQ,及AUX录音等功能。", AUX_2_SDADC_EN, 1, FUNC_AUX_EN);
config(CHECK, "插入LINEIN关机", "是否使能插入LINEIN, 直接关机功能，大耳包功能", LINEIN_2_PWRDOWN_EN, 0, FUNC_AUX_EN);
config(CHECK, "模式切换进AUX模式", "有LINEIN检测时，LINEIN线没有插入，是否能通过模式转换进入AUX模式。", MODE_2_AUX_EN, 0, FUNC_AUX_EN);
config(LISTVAL, "AUX模拟增益", "AUX模拟增益选择", AUX_ANL_GAIN, BIT, 4, 2, 11, ("+6DB", 0), ("+3DB", 1), ("0DB", 2), ("-3DB", 3), ("-6DB", 4), ("-9DB", 5), ("-15DB", 6), ("-21DB", 7), ("-27DB", 8), ("-33DB", 9), ("-39DB", 10), FUNC_AUX_EN);
config(BIT, "AUX数字增益(0~32DB)", "SDADC数字增益, Step 0.5DB", AUX_DIG_GAIN, 5, 0, 31, 0, AUX_2_SDADC_EN);
config(LISTVAL, "AUXL通路选择", "AUX左声道通路选择", AUXL_SEL, BIT, 4, 0, 8, ("无AUXL输入", 0), ("AUXL_PA6", 1), ("AUXL_PB1", 2), ("AUXL_PE6", 3), ("AUXL_PF4", 4), ("AUXL_VCMBUF", 5), ("AUXL_MIC", 6), ("AUXL_VOUTRP", 7), FUNC_AUX_EN);
config(LISTVAL, "AUXR通路选择", "AUX右声道通路选择", AUXR_SEL, BIT, 4, 0, 8, ("无AUXR输入", 0), ("AUXR_PA7", 1), ("AUXR_PB2", 2), ("AUXR_PE7", 3), ("AUXR_PF5", 4), ("AUXR_VCMBUF", 5), ("AUXR_MIC", 6), ("AUXR_VOUTRN", 7), FUNC_AUX_EN);
config(LISTVAL, "LINEIN插入检测配置", "选择LINEIN检测GPIO，或Disable LINEIN检测", LINEIN_DET_IOSEL, BIT, 6, 0, 24, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 26), ("PE0", 14), ("PE4", 18), ("PE5", 19), ("PE6", 20), ("PE7", 21), ("PF0", 22), ("PF1", 23), ("PF2", 24), ("PF3", 25), ("复用SDCLK检测", 27), ("复用SDCMD检测", 28), ("复用PWRKEY检测", 29), FUNC_AUX_EN);
config(LEVEL, 0x03);

config(SUB, "按键配置", "选择按键及按键功能配置");
config(CHECK, "PWRKEY功能", "是否使用PWRKEY，使用软开关机时必须使能", USER_PWRKEY_EN, 0);
config(CHECK, "ADKEY1功能", "是否使用第一组ADKEY", USER_ADKEY_EN, 1);
config(CHECK, "ADKEY2功能", "是否使用第二组ADKEY", USER_ADKEY2_EN, 0);
config(CHECK, "IOKEY功能", "是否使用IOKEY", USER_IOKEY_EN, 0);
config(LEVEL, 0x00);
config(CHECK, "复用SDCLK的ADKEY", "是否使用复用SDCLK的ADKEY按键", USER_ADKEY_MUX_SDCLK_EN, 0);
config(LEVEL, 0x03);
config(LISTVAL, "ADKEY1通路选择", "第一组ADKEY io mapping select", ADKEY_CH,   BIT, 4, 9, 12, ("ADCCH_PA5", 0), ("ADCCH_PA6", 1), ("ADCCH_PA7", 2), ("ADCCH_PB1", 3), ("ADCCH_PB2", 4), ("ADCCH_PB3", 5), ("ADCCH_PB4", 6), ("ADCCH_PE5", 7), ("ADCCH_PE6", 8), ("ADCCH_PE7", 9), ("ADCCH_PF5", 10), ("ADCCH_PB0", 11), USER_ADKEY_EN);
config(LEVEL, 0x00);
config(LISTVAL, "ADKEY2通路选择", "第二组ADKEY io mapping select", ADKEY2_CH,  BIT, 4, 9, 12, ("ADCCH_PA5", 0), ("ADCCH_PA6", 1), ("ADCCH_PA7", 2), ("ADCCH_PB1", 3), ("ADCCH_PB2", 4), ("ADCCH_PB3", 5), ("ADCCH_PB4", 6), ("ADCCH_PE5", 7), ("ADCCH_PE6", 8), ("ADCCH_PE7", 9), ("ADCCH_PF5", 10), ("ADCCH_PB0", 11), USER_ADKEY2_EN);
config(LEVEL, 0x03);
config(CHECK, "按键多击功能", "是否使能按键多击（2/3/4/5击）功能", USER_KEY_MULTI_PRESS_EN, 1);
config(LISTVAL, "双击响应时间选择", "选择双击按键响应间隔时间", DOUBLE_KEY_TIME, BIT, 3, 1, 8, ("200ms", 0), ("300ms", 1), ("400ms", 2), ("500ms", 3), ("600ms", 4), ("700ms", 5), ("800ms", 6), ("900ms", 7), USER_KEY_MULTI_PRESS_EN);
config(LISTVAL, "USER_DEF键长按", "选择USER_DEF键长按的功能", USER_DEF_KL_SEL,    BIT, 4, 2, 15, ("无", 0), ("回拨", 1), ("SIRI", 2), ("PREV", 3), ("NEXT", 4), ("VOL+", 5), ("VOL-", 6), ("MODE", 7), ("HID拍照", 8), ("IOS HOME", 9), ("中英文切换", 10), ("PLAY/PAUSE", 11), ("CBT_TEST", 12), ("低延时", 13), ("降噪控制", 14));
config(LISTVAL, "USER_DEF键双击", "选择USER_DEF键双击的功能", USER_DEF_KD_SEL,    BIT, 4, 1, 15, ("无", 0), ("回拨", 1), ("SIRI", 2), ("PREV", 3), ("NEXT", 4), ("VOL+", 5), ("VOL-", 6), ("MODE", 7), ("HID拍照", 8), ("IOS HOME", 9), ("中英文切换", 10), ("PLAY/PAUSE", 11), ("CBT_TEST", 12), ("低延时", 13), ("降噪控制", 14));
config(LISTVAL, "USER_DEF键三击", "选择USER_DEF键三击的功能", USER_DEF_KT_SEL, 	  BIT, 4, 0, 15, ("无", 0), ("回拨", 1), ("SIRI", 2), ("PREV", 3), ("NEXT", 4), ("VOL+", 5), ("VOL-", 6), ("MODE", 7), ("HID拍照", 8), ("IOS HOME", 9), ("中英文切换", 10), ("PLAY/PAUSE", 11), ("CBT_TEST", 12), ("低延时", 13), ("降噪控制", 14));
config(LISTVAL, "USER_DEF键四击", "选择USER_DEF键四击的功能", USER_DEF_KFOUR_SEL, BIT, 4, 0, 15, ("无", 0), ("回拨", 1), ("SIRI", 2), ("PREV", 3), ("NEXT", 4), ("VOL+", 5), ("VOL-", 6), ("MODE", 7), ("HID拍照", 8), ("IOS HOME", 9), ("中英文切换", 10), ("PLAY/PAUSE", 11), ("CBT_TEST", 12), ("低延时", 13), ("降噪控制", 14));
config(LISTVAL, "USER_DEF键五击", "选择USER_DEF键五击的功能", USER_DEF_KFIVE_SEL, BIT, 4, 0, 15, ("无", 0), ("回拨", 1), ("SIRI", 2), ("PREV", 3), ("NEXT", 4), ("VOL+", 5), ("VOL-", 6), ("MODE", 7), ("HID拍照", 8), ("IOS HOME", 9), ("中英文切换", 10), ("PLAY/PAUSE", 11), ("CBT_TEST", 12), ("低延时", 13), ("降噪控制", 14));
config(CHECK, "USER_DEF键长按按键音", "选择USER_DEF键长按功能时，是否滴一下提示", USER_DEF_KL_TONE_EN, 1);
config(CHECK, "USER_DEF键双击切语言", "蓝牙未连接手机时，双击切换中英文语音", USER_DEF_KD_LANG_EN, 1);
config(CHECK, "USER_DEF键区分左右耳", "USER_DEF键区分左右耳的功能，配置中为主耳功能（例如主耳上一曲，则副耳为下一曲，音量加减同理）", USER_DEF_LR_EN, 0);

config(CHECK, "支持多击的按键定制", "是否配置支持多击的按键，不勾选使用SDK默认的多击按键配置列表", KEY_MULTI_CONFIG_EN, 0, USER_KEY_MULTI_PRESS_EN);
config(LISTVAL, "多击按键1选择", "支持多击第1个按键", DBLKEY_NUM0, BYTE, 20, 22, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), KEY_MULTI_CONFIG_EN);
config(LISTVAL, "多击按键2选择", "支持多击第2个按键", DBLKEY_NUM1, BYTE, 21, 22, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), KEY_MULTI_CONFIG_EN);
config(LISTVAL, "多击按键3选择", "支持多击第3个按键", DBLKEY_NUM2, BYTE, 0,  22, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), KEY_MULTI_CONFIG_EN);
config(LISTVAL, "多击按键4选择", "支持多击第4个按键", DBLKEY_NUM3, BYTE, 0,  22, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), KEY_MULTI_CONFIG_EN);
config(LISTVAL, "多击按键5选择", "支持多击第5个按键", DBLKEY_NUM4, BYTE 0,  22, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), KEY_MULTI_CONFIG_EN);

config(CHECK, "PWRKEY按键定制", "是否配置PWRKEY按键功能，不勾选使用SDK默认按键配置", PWRKEY_CONFIG_EN, 0, USER_PWRKEY_EN);
config(LISTVAL, "PWRKEY按键1功能", "PWRKEY第1个按键功能定制", PWRKEY_NUM0, BYTE, 20, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), PWRKEY_CONFIG_EN);
config(LISTVAL, "PWRKEY按键2功能", "PWRKEY第2个按键功能定制", PWRKEY_NUM1, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23),  PWRKEY_CONFIG_EN);
config(LISTVAL, "PWRKEY按键3功能", "PWRKEY第3个按键功能定制", PWRKEY_NUM2, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23),  PWRKEY_CONFIG_EN);
config(LISTVAL, "PWRKEY按键4功能", "PWRKEY第4个按键功能定制", PWRKEY_NUM3, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23),  PWRKEY_CONFIG_EN);
config(LISTVAL, "PWRKEY按键5功能", "PWRKEY第5个按键功能定制", PWRKEY_NUM4, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23),  PWRKEY_CONFIG_EN);

config(CHECK, "ADKEY1按键定制", "是否配置第一组ADKEY按键功能，不勾选使用SDK默认按键配置", ADKEY_CONFIG_EN, 0);
config(LISTVAL, "ADKEY1按键1功能", "ADKEY1第1个按键功能定制", ADKEY1_NUM0, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键2功能", "ADKEY1第2个按键功能定制", ADKEY1_NUM1, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键3功能", "ADKEY1第3个按键功能定制", ADKEY1_NUM2, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键4功能", "ADKEY1第4个按键功能定制", ADKEY1_NUM3, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键5功能", "ADKEY1第5个按键功能定制", ADKEY1_NUM4, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键6功能", "ADKEY1第6个按键功能定制", ADKEY1_NUM5, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键7功能", "ADKEY1第7个按键功能定制", ADKEY1_NUM6, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键8功能", "ADKEY1第8个按键功能定制", ADKEY1_NUM7, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键9功能", "ADKEY1第9个按键功能定制", ADKEY1_NUM8, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键10功能", "ADKEY1第10个按键功能定制", ADKEY1_NUM9, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键11功能", "ADKEY1第11个按键功能定制", ADKEY1_NUM10, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键12功能", "ADKEY1第12个按键功能定制", ADKEY1_NUM11, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ADKEY_CONFIG_EN);

config(CHECK, "IOKEY按键定制", "是否配置IOKEY，不勾选使用SDK默认按键配置", IOKEY_CONFIG_EN, 0, USER_IOKEY_EN);
config(LISTVAL, "IOKEY按键1的IO", "IOKEY第1个按键的GPIO口选择", IOKEY_IO0, BIT, 5, 0, 21, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 26), ("PE0", 14), ("PE4", 18), ("PE5", 19), ("PE6", 20), ("PE7", 21), ("PF0", 22), ("PF1", 23), ("PF2", 24), ("PF3", 25), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键2的IO", "IOKEY第2个按键的GPIO口选择", IOKEY_IO1, BIT, 5, 0, 21, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 26), ("PE0", 14), ("PE4", 18), ("PE5", 19), ("PE6", 20), ("PE7", 21), ("PF0", 22), ("PF1", 23), ("PF2", 24), ("PF3", 25), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键3的IO", "IOKEY第3个按键的GPIO口选择", IOKEY_IO2, BIT, 5, 0, 21, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 26), ("PE0", 14), ("PE4", 18), ("PE5", 19), ("PE6", 20), ("PE7", 21), ("PF0", 22), ("PF1", 23), ("PF2", 24), ("PF3", 25), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键4的IO", "IOKEY第4个按键的GPIO口选择", IOKEY_IO3, BIT, 5, 0, 21, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 26), ("PE0", 14), ("PE4", 18), ("PE5", 19), ("PE6", 20), ("PE7", 21), ("PF0", 22), ("PF1", 23), ("PF2", 24), ("PF3", 25), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键5的IO", "IOKEY第5个按键的GPIO口选择", IOKEY_IO4, BIT, 5, 0, 21, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 26), ("PE0", 14), ("PE4", 18), ("PE5", 19), ("PE6", 20), ("PE7", 21), ("PF0", 22), ("PF1", 23), ("PF2", 24), ("PF3", 25), IOKEY_CONFIG_EN);

config(LISTVAL, "IOKEY按键1功能", "IOKEY第1个按键功能定制", IOKEY_NUM0, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键2功能", "IOKEY第2个按键功能定制", IOKEY_NUM1, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键3功能", "IOKEY第3个按键功能定制", IOKEY_NUM2, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键4功能", "IOKEY第4个按键功能定制", IOKEY_NUM3, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键5功能", "IOKEY第5个按键功能定制", IOKEY_NUM4, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), IOKEY_CONFIG_EN);
config(LISTVAL, "两IO中间的按键功能", "IOKEY1与IOKEY2两IO中间的按键", IOKEY_MIDKEY_NUM, BYTE, 0, 24, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), IOKEY_CONFIG_EN);

config(SUB, "LED灯配置", "选择及配置LED状态指示灯");
config(CHECK, "系统指示灯(蓝灯)", "是否使用系统状态指示灯", LED_DISP_EN, 1);
config(CHECK, "电源状态灯(红灯)", "是否使用充电/电源指示灯", LED_PWR_EN, 1);
config(CHECK, "省电阻1个IO推两个灯", "省电阻1个IO推两个灯，不能兼容升级功能", PORT_2LED_RESLESS_EN, 1, LED_DISP_EN);
config(CHECK, "电池低电闪灯", "电池低电时，红灯或蓝灯是否闪烁？", RLED_LOWBAT_EN, 0, LED_DISP_EN);
config(LISTVAL, "蓝灯IO选择", "蓝灯的GPIO口选择", BLED_IO_SEL, BIT, 5, 25, 21, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 26), ("PE0", 14), ("PE4", 18), ("PE5", 19), ("PE6", 20), ("PE7", 21), ("PF0", 22), ("PF1", 23), ("PF2", 24), ("PF3", 25), LED_DISP_EN);
config(LISTVAL, "红灯IO选择", "红灯的GPIO口选择", RLED_IO_SEL, BIT, 5, 25, 21, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 26), ("PE0", 14), ("PE4", 18), ("PE5", 19), ("PE6", 20), ("PE7", 21), ("PF0", 22), ("PF1", 23), ("PF2", 24), ("PF3", 25), LED_PWR_EN);

config(CHECK, "开机状态配置LED", "是否配置开机LED指示灯状态", LED_PWRON_CONFIG_EN, 0, LED_DISP_EN);
config(LED, "开机", "配置开机状态下的闪灯状态", LED_POWERON, 0x02, 0x01, 10, 255, 		LED_PWRON_CONFIG_EN);

config(CHECK, "关机状态配置LED", "是否配置等待关机状态指示灯", LED_PWROFF_CONFIG_EN, 0, LED_DISP_EN);
config(LED, "关机", "配置关机状态下的闪灯状态", LED_POWEROFF, 0x40, 0x15, 4, 255, 		LED_PWROFF_CONFIG_EN);

config(CHECK, "蓝牙初始化状态配置LED", "是否配置初始化状态指示灯(蓝牙开机播提示音、等待5秒按键时)", LED_BTINIT_CONFIG_EN, 0);
config(LED, "初始化", "配置蓝牙初始化状态下的闪灯状态", LED_BTINIT, 0x00, 0xaa, 10, 0, 	LED_BTINIT_CONFIG_EN);

config(CHECK, "蓝牙回连状态配置LED", "是否配置蓝牙回连状态指示灯(蓝牙未连接)", LED_RECONNECT_CONFIG_EN, 0);
config(LED, "回连", "配置回连状态下的闪灯状态", LED_RECONNECT, 0x00, 0xaa, 6, 0, 		LED_RECONNECT_CONFIG_EN);

config(CHECK, "蓝牙配对状态配置LED", "是否配置蓝牙配对状态指示灯(蓝牙未连接)", LED_BTPAIR_CONFIG_EN, 0, LED_DISP_EN);
config(LED, "配对", "配置配对状态下的闪灯状态", LED_PAIRING, 0x60, 0x03, 3, 6, 			LED_BTPAIR_CONFIG_EN);

config(CHECK, "蓝牙已连接状态配置LED", "是否配置蓝牙已连接状态指示灯", LED_BTCONN_CONFIG_EN, 0, LED_DISP_EN);
config(LED, "已连接", "配置已连接状态下的闪灯状态", LED_CONNECTED, 0x00, 0x02, 2, 86, 	LED_BTCONN_CONFIG_EN);

config(CHECK, "蓝牙音乐状态配置LED", "是否配置蓝牙音乐状态指示灯", LED_BTMUSIC_CONFIG_EN, 0, LED_DISP_EN);
config(LED, "播放", "配置播放状态下的闪灯状态", LED_PLAYING, 0x00, 0x02, 2, 86, 		LED_BTMUSIC_CONFIG_EN);

config(CHECK, "蓝牙来电状态配置LED", "是否配置蓝牙来电状态指示灯", LED_BTRING_CONFIG_EN, 0, LED_DISP_EN);
config(LED, "来电", "配置来电状态下的闪灯状态", LED_RING, 0x00, 0x02, 2, 86, 			LED_BTRING_CONFIG_EN);

config(CHECK, "蓝牙通话状态配置LED", "是否配置蓝牙通话状态指示灯", LED_BTCALL_CONFIG_EN, 0, LED_DISP_EN);
config(LED, "通话", "配置通话状态下的闪灯状态", LED_CALLING, 0x00, 0x02, 2, 86, 		LED_BTCALL_CONFIG_EN);

config(CHECK, "低电状态配置LED", "是否配置低电状态指示灯", LED_LOWBAT_CONFIG_EN, 0, RLED_LOWBAT_EN);
config(LED, "低电", "配置低电状态下的闪灯状态", LED_LOWBAT, 0xaa, 0x00, 6, 0, 			LED_LOWBAT_CONFIG_EN);

config(LEVEL, 0x100);
makecfgfile(xcfg.bin);
makecfgdef(xcfg.h);
xcopy(xcfg.h, ../../xcfg.h);
