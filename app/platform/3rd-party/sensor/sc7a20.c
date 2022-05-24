#include "include.h"

#if SC7A20_EN
#include "sc7a20.h"

unsigned char interrupt_come = 0;
bool sc7a20_online = false;

void SL_MEMS_i2cWrite(u8 chip_id, u8 iic_addr, u8 iic_dat)
{
	bsp_i2c_start();
	bsp_i2c_tx_byte(chip_id);
	bsp_i2c_rx_ack();
	bsp_i2c_tx_byte(iic_addr);
	bsp_i2c_rx_ack();
	bsp_i2c_tx_byte(iic_dat);
	bsp_i2c_rx_ack();
	bsp_i2c_stop();
}
void SL_MEMS_i2cRead(u8 chip_id, u8 iic_addr,u8 n,u8 *iic_dat)
{
    bsp_i2c_start();
    bsp_i2c_tx_byte(chip_id);
    bsp_i2c_rx_ack();
    if (0xff != iic_addr) {
        bsp_i2c_tx_byte(iic_addr);
        bsp_i2c_rx_ack();
    }
    bsp_i2c_stop();
    delay_us(100);
    bsp_i2c_start();
    bsp_i2c_tx_byte(chip_id | 0x1);
    bsp_i2c_rx_ack();
    for (int i=0; i < n-1; i++) {
        *iic_dat++ = bsp_i2c_rx_byte();
        bsp_i2c_tx_ack();
    }
	*iic_dat++ = bsp_i2c_rx_byte();
	bsp_i2c_tx_nack();
    bsp_i2c_stop();
}

char SC7A20_Check()
{
	unsigned char reg_value=0;

	SL_MEMS_i2cRead(SC7A20_ADDR_W, SC7A20_WHO_AM_I, 1, &reg_value);
	//printf("reg_value=%x\n", reg_value);
	if (reg_value == 0x11)
	{
		return 0x01;
	}
	else
	{
		return 0x00;
	}
}

AT(.com_text)
const char test_str1[] = "SC7A20 interrupt coming\n";

AT(.com_text.isr)
void SC7A20_port_isr(void)
{
	WDT_CLR();
	if ((WKUPEDG & (BIT(21) )) && (!(GPIOB & BIT(5)))) {
		WKUPCPND = (BIT(21));  //CLEAR PENDING
		interrupt_come = 1;
		//printf(test_str1, WKUPEDG >> 16);
	}
}

void SC7A20_Config_int(void)     //sys_set_tmr_enable(1, 1);
{
#if 1
    GPIOBDE |= BIT(5);GPIOBDIR |= BIT(5);GPIOBFEN &= ~BIT(5);
    //GPIOBPU |= BIT(5);
    GPIOBPD |= BIT(5);
    //PORTINTEN |= BIT(12);
    //PORTINTEDG |= BIT(12);  //falling edge;
    WKUPCPND = 0xff << 16;
    //WKUPEDG |= BIT(5);     //falling edge
    WKUPEDG &= ~BIT(5);      //raising edge
    WKUPCON = BIT(5) | BIT(16);  //falling edge wake iput //wakeup int en
#else
	GPIOBDE |= BIT(5);GPIOBDIR |= BIT(5);GPIOBFEN &= ~BIT(5);
    GPIOBPU |= BIT(5);
    //GPIOBPD |= BIT(5);
    //PORTINTEN |= BIT(12);
    //PORTINTEDG |= BIT(12);  //falling edge;
    WKUPCPND = 0xff << 16;
    WKUPEDG |= BIT(5);     //falling edge
    //WKUPEDG &= ~BIT(5);      //raising edge
    WKUPCON = BIT(5) | BIT(16);  //falling edge wake iput //wakeup int en
#endif
    sys_irq_init(IRQ_PORT_VECTOR, 0, SC7A20_port_isr);
}

u8 SC7A20_Config(void)
{
	u8 Check_Flag=0;
	u8 cnt;
	//unsigned char reg_value=0;
	//timer3_init();
	//printf("SC7A20_Config\n");
	sc7a20_online = false;
	SC7A20_Config_int();
	for (cnt = 0x0; cnt < 0x03; cnt++)
	{
		Check_Flag = SC7A20_Check();
		if (!Check_Flag)
		{
			printf("will retry check\n");
		}
		else
		{
			break;
		}
	}

	if (Check_Flag == 1)
	{
		SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CTRL_REG1, 0x7f);
		SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CTRL_REG2, 0x0C);
		//SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CTRL_REG3, 0x80);// click int1
		//SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CTRL_REG6, 0x80);// click int2
		SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CTRL_REG4, 0x90);
		SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CTRL_REG5, 0x40);
		SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_FIFO_CTRL_REG, 0x80);
		SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CLICK_CFG, 0x15);
		SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CLICK_THS, 0x25);
		SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_TIME_LIMIT, 0x05);
		SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_TIME_LATENCY, 0x10);
		SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CTRL_REG6, 0x80);// click int2 low
		//SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CTRL_REG6, 0x82);// click int2 high

		sc7a20_online = true;
		return 1;
	}
	else
	{
		//printf("init sc7a20 fail\n");
		sc7a20_online = false;
		return 0;
	}
}
unsigned char sl_pp_num=0;

unsigned char SL_Get_CLICK_PP_CNT(unsigned char fun_flag)
{
	if (fun_flag == 0)
		sl_pp_num = 0;
	return sl_pp_num;
}
unsigned int SL_Click_Sqrt(unsigned int sqrt_data)
{
	unsigned int SL_SQRT_LOW, SL_SQRT_UP, SL_SQRT_MID;
	unsigned char sl_sqrt_num = 0;

	SL_SQRT_LOW = 0;
	SL_SQRT_UP = sqrt_data;
	SL_SQRT_MID = (SL_SQRT_UP+SL_SQRT_LOW)/2;
	while (sl_sqrt_num < 200)
	{
		if (SL_SQRT_MID*SL_SQRT_MID > sqrt_data)
		{
			SL_SQRT_UP = SL_SQRT_MID;
		}
		else
		{
			SL_SQRT_LOW = SL_SQRT_MID;
		}
		if (SL_SQRT_UP-SL_SQRT_LOW == 1)
		{
			if ((SL_SQRT_UP*SL_SQRT_UP - sqrt_data) > (sqrt_data - SL_SQRT_LOW*SL_SQRT_LOW))
			{
				return SL_SQRT_LOW;
			}
			else
			{
				return SL_SQRT_UP;
			}
		}
		SL_SQRT_MID = (SL_SQRT_UP+SL_SQRT_LOW)/2;
		sl_sqrt_num++;
	}
	return 0;
}

char SC7A20_Click_Read(int TH1,int TH2)
{
	unsigned char i = 0, j = 0, k = 0;
	unsigned char click_num = 0;
	unsigned char fifo_len;
	unsigned int  sc7a20_data = 0;
	unsigned int  fifo_data_xyz[32];
	unsigned char click_result = 0x00;
	unsigned int  click_sum = 0;
	unsigned char data1[6];
	signed char   data[6];

	SL_MEMS_i2cRead(SC7A20_ADDR_W, SC7A20_SRC_REG,1, &fifo_len);
	if ((fifo_len & 0x40) == 0x40)
		fifo_len = 32;
	else
		fifo_len = (fifo_len & 0x1f);

	for (i = 0; i < fifo_len; i++)
	{
		SL_MEMS_i2cRead(SC7A20_ADDR_W, 0xA8, 6, &data1[0]);
		data[1] = (signed char)data1[1];
		data[3] = (signed char)data1[3];
		data[5] = (signed char)data1[5];
		sc7a20_data = (data[1])*(data[1]) + (data[3])*(data[3]) + (data[5])*(data[5]);
		sc7a20_data = SL_Click_Sqrt(sc7a20_data);
		fifo_data_xyz[i] = sc7a20_data;
	}

	k = 0;
	for (i = 1;i < fifo_len - 1; i++)
	{
		if ((fifo_data_xyz[i+1] > TH1) && (fifo_data_xyz[i-1] < 30))
		{
			if (click_num == 0)
			{
				click_sum = 0;//first peak
				for (j = 0;j < i - 1; j++)
				{
					if (fifo_data_xyz[j] > fifo_data_xyz[j+1])
					{
 						click_sum += fifo_data_xyz[j] - fifo_data_xyz[j+1];
 					}
					else
					{
						click_sum += fifo_data_xyz[j+1] - fifo_data_xyz[j];
					}
				}
				if (click_sum > TH2)
				{
					sl_pp_num++;
					//printf("click_sum :%d > TH2  = %d sl_pp_num %d \n",click_sum, TH2, sl_pp_num);
					break;
				}
				k = i;
			}
			else
			{
				k = i;//sencond peak
			}
		}

		if (k != 0)
		{
			if ((fifo_data_xyz[i-1] - fifo_data_xyz[i+1]) > (TH1 - 10))
			{
				if ((i - k) < 5)
				{
					click_num = 1;
					break;
				}
			}
		}
	}
	if (click_num == 1)
	{
		click_result = 1;
	}
	else
	{
		click_result = 0;
	}
	//printf("click_result == %d\n",click_result);
	return click_result;
}

//Service function in Int Function
//unsigned char  sl_click_timer_en     =0;
unsigned char  sl_click_status       = 0;
unsigned short click_timer_cnt       = 0;
unsigned short click_timer_total_cnt = 0;
unsigned char  click_click_final_cnt = 0;
unsigned char  volatile  sl_click_timer_en = 0;


u8 SC7A20_Click_Alog(void)
{
	unsigned char click_status=0;

	//SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CTRL_REG6, 0x00);

	click_status = SC7A20_Click_Read(35, 55);

	if (click_status == 1)
	{
		if (sl_click_timer_en == 0)
		{
			//set click timer flag
			sl_click_timer_en    = 1;
			//clear click timer cnt value
			click_timer_cnt      = 0;
			click_timer_total_cnt = 0;
			click_click_final_cnt = 0;
		}
		sl_click_status = 1;
	}
	//printf("click_status : %d,sl_click_timer_en = %d\n",click_status,sl_click_timer_en);
	//SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CTRL_REG6, 0x82);
	return click_status;
}

#define click_pp_num    (unsigned short)10  //10-0.5s
#define click_max_num   (unsigned short)55//55 //5s
//This fuction is execute in 50ms timer when the timer is opened

u8 SC7A20_click_status(void)
{
	unsigned char click_e_cnt = 0;

	WDT_CLR();
	if (sl_click_timer_en == 1)
	{
	    click_timer_cnt++;

		if ((click_timer_cnt < click_pp_num) && sl_click_status == 1)
		{
			//printf("status:%d\n", sl_click_status);
			//printf("fin_num:%d\n", click_click_final_cnt);
			sl_click_status = 0;
			click_timer_total_cnt = click_timer_total_cnt + click_timer_cnt;
			click_timer_cnt = 0;
			click_click_final_cnt++;
		}

		click_e_cnt = SL_Get_CLICK_PP_CNT(1);
		//printf("click_e_cnt = %d,click_timer_cnt = %d,click_timer_total_cnt =%d\n",click_e_cnt,click_timer_cnt,click_timer_total_cnt);
		if ((((click_timer_cnt >= click_pp_num) || (click_timer_total_cnt >= click_max_num)) && (click_e_cnt < 1)) ||
		   ((click_timer_cnt >= click_pp_num) && (click_e_cnt > 0)))
		{
			//clear click timer flag
			sl_click_timer_en = 0;
			//clear click timer cnt value
			click_timer_cnt      = 0;
			click_timer_total_cnt = 0;
			if (click_e_cnt > 0)
			{
				//printf("click_e_cnt %d SC7A20_click_status return 0\n", click_e_cnt);
				click_e_cnt = SL_Get_CLICK_PP_CNT(0);
				return 0;
			}
			else
			{
				//printf("click_e_cnt %d SC7A20_click_status return final_cnt %d\n", click_e_cnt, click_click_final_cnt);
				return click_click_final_cnt;
			}
		}
	}
	return 0;
}

void sc7a20_status_scan(void)
{
  static u32 click_ticks = 0;
  u8 click_cnt = 0;
  u8 click_status = 0;

  if (sc7a20_online == true && interrupt_come) {
      interrupt_come = 0;
      click_status = SC7A20_Click_Alog();

      if (click_status == 1)
		  SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CTRL_REG6, 0x00);//SC7A20用来保护超时时间段的数据。
  }

  //WDT_CLR();
  if (tick_check_expire(click_ticks, 50) && sl_click_timer_en){

	    click_cnt = SC7A20_click_status();
		SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CTRL_REG6, 0x80);//SC7A20 low
		//SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CTRL_REG6, 0x82); //high

  		switch(click_cnt)
  		{
			case 1:
				msg_enqueue(KU_PLAY_USER_DEF);
				//printf("key:click\n");
				break;
			case 2:
				//printf("key:double click\n");
				msg_enqueue(KD_PLAY_PWR_USER_DEF);
				break;
			case 3:
				//printf("key:triple click\n");
				msg_enqueue(KTH_PLAY_PWR_USER_DEF);
				break;
			default:
				break;
		}
		click_ticks = tick_get();
  }
}

u8 sc7a20_enter_lowpow(void)
{
	//printf("%s\n", __func__);
	SL_MEMS_i2cWrite(SC7A20_ADDR_W, SC7A20_CTRL_REG1, 0x8f);
	return 0;
}
#endif
