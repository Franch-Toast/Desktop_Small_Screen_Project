#include "stm32f10x.h"
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"	


ErrorStatus HSEStartUpStatus;

unsigned char HRES,VRES_byte1,VRES_byte2;

unsigned char temp_FULL[2888];
unsigned char temp_40_16[86];
unsigned char temp_40_93[86];
unsigned char temp_64_10[217];
unsigned char temp_64_41[217];
unsigned char temp_64_72[56];
unsigned char temp_64_80[217];
unsigned char temp_64_111[77];
unsigned char temp_64_122[140];

#define POS 1			//正显
#define NEG 2			//负显
#define CLEAR 3		//清除

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);

//EPD--OTP
void EPD_init(void);
void PIC_display (const unsigned char* picData);
void EPD_display_Clean(void);
void EPD_sleep(void);
void EPD_refresh(void);
void lcd_chkstatus(void);

//partial display
void EPD_init_partial(void);//EPD init partial
void EPD_Initial_value(unsigned char datas);//EPD init partial
void EPD_partial_alone(u16 x_start,u16 y_start,unsigned char *old_data,const unsigned char *new_data,unsigned int PART_COLUMN,unsigned int PART_LINE,unsigned char mode); //partial display 
void EPD_partial_mult(u16 x_start,u16 y_start,unsigned char *old_data1,const unsigned char *new_data1,unsigned int PART_COLUMN1,
																							unsigned char *old_data2,const unsigned char *new_data2,unsigned int PART_COLUMN2,
																							unsigned char *old_data3,const unsigned char *new_data3,unsigned int PART_COLUMN3,
																							unsigned char *old_data4,const unsigned char *new_data4,unsigned int PART_COLUMN4,
																							unsigned char *old_data5,const unsigned char *new_data5,unsigned int PART_COLUMN5,
																							unsigned char *old_data6,const unsigned char *new_data6,unsigned int PART_COLUMN6,
																							unsigned int PART_LINE);																							

void lut1(void);



unsigned char old_data[1000];
void driver_delay_us(unsigned int xus)
{
	for(;xus>1;xus--);
}

void driver_delay_xms(unsigned long xms)	
{	
    unsigned long i = 0 , j=0;

    for(j=0;j<xms;j++)
	{
        for(i=0; i<256; i++);
    }
}
void DELAY_S(unsigned int delaytime)     //  1s
{
	int i,j,k;
	for(i=0;i<delaytime;i++)
  {
		for(j=0;j<4000;j++)           
		{
			for(k=0;k<222;k++);
                
		}
	}
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
#define NVIC_VectTab_FLASH  misc.h
*******************************************************************************/

int	main(void)
{
	RCC_Configuration();
	//GPIO config
	GPIO_Configuration();
	
	
	while(1)
	{	
		EPD_Initial_value(0xff);									//Initial value of old data		
		
		//2 gray picture(OTP)
		EPD_init(); 															//EPD init
		PIC_display(gImage_1); 										//picture 1
    EPD_refresh();														//EPD_refresh		
		EPD_sleep();															//EPD_sleep
    DELAY_S(10);
		
		EPD_init(); 															//EPD init
		PIC_display(gImage_basemap); 							//picture basemap
    EPD_refresh();														//EPD_refresh		
		EPD_sleep();															//EPD_sleep
    DELAY_S(5);	

/*************************Partial refresh number*********************************/
		EPD_init_partial();												//EPD init partial       x , y  , gImage , column , line , mode
		EPD_partial_alone(40,16,temp_40_16,gImage_Celsius,43,16,NEG);    //40, 16 , 摄氏度 ,   43   ,  16  , 负显
		EPD_sleep(); 															//Enter deep sleep mode	

/****************************30.9°C*********************************************/
		EPD_init_partial();												//EPD init partial 			 x , y  , gImage , column , line  	
		EPD_partial_mult(64,10,temp_64_10,gImage_num3,31,								 //64, 10 , 数字3  ,   31   ,  56
													 temp_64_41,gImage_num0,31,								 //64, 41 , 数字0  ,   31   ,  56
													 temp_64_72,gImage_dot,8,									 //64. 72 , 小数点 ,   8    ,  56
													 temp_64_80,gImage_num9,31,								 //64, 80 , 数字9  ,   31   ,  56
													 temp_64_111,gImage_degree,11,						 //64, 111,   °    ,   11   ,  56
													 temp_64_122,gImage_C,20,									 //64, 122,   C    ,   20   ,  56
													 56);																			 //注意：line必须是8的整数倍
		EPD_sleep(); 																										 //Enter deep sleep mode	
		DELAY_S(10);		
																							
		EPD_init_partial();											 //EPD init partial        x , y  , gImage , column , line , mode
		EPD_partial_alone(40,16,temp_40_16,gImage_Celsius,43,16,CLEAR);  //40, 16 , 摄氏度 ,   43   ,  16  , 清除
		EPD_sleep();														 //Enter deep sleep mode
		
		EPD_init_partial();											//EPD init partial         x , y  , gImage , column , line , mode
		EPD_partial_alone(40,93,temp_40_93,gImage_Fahrenheit,43,16,NEG); //40, 93 , 华氏度 ,   43   ,  93  , 负显
		EPD_sleep(); 														//Enter deep sleep mode
		
/****************************87.6°F*********************************************/
		EPD_init_partial();											//EPD init partial         x , y  , gImage , column , line
		EPD_partial_mult(64,10,temp_64_10,gImage_num8,31,								 //64, 10 , 数字8  ,   31   ,  56
													 temp_64_41,gImage_num7,31,								 //64, 41 , 数字7  ,   31   ,  56
													 temp_64_72,gImage_dot,8,									 //64. 72 , 小数点 ,   8    ,  56
													 temp_64_80,gImage_num6,31,								 //64, 80 , 数字6  ,   31   ,  56
													 temp_64_111,gImage_degree,11,						 //64, 111,   °    ,   11   ,  56
													 temp_64_122,gImage_F,20,									 //64, 122,   F    ,   20   ,  56
													 56);																			 //注意：line必须是8的整数倍
		EPD_sleep(); 																										 //Enter deep sleep mode	
		DELAY_S(10);			

/*************************Partially refresh picture******************************/
		EPD_init(); 													 //EPD init
		EPD_display_Clean(); 									 //Clear screen
    EPD_refresh();												 //EPD_refresh		
		EPD_sleep();												   //EPD_sleep

		EPD_init_partial();										 //EPD init partial
		EPD_partial_alone(0,0,temp_FULL,gImage_1,152,152,POS); 					//picture 1，正显
		EPD_sleep(); 													 //Enter deep sleep mode
		DELAY_S(10);	

		EPD_init_partial();										 //EPD init partial
		EPD_partial_alone(0,0,temp_FULL,gImage_2,152,152,NEG); 					//picture 2，负显
		EPD_sleep(); 													 //Enter deep sleep mode
		DELAY_S(10);

		EPD_init_partial();										 //EPD init partial
		EPD_partial_alone(0,0,temp_FULL,gImage_3,152,152,POS); 					//picture 3，正显
		EPD_sleep(); 													 //Enter deep sleep mode
		DELAY_S(10);	

//Clear screen (OTP)
		EPD_init(); 													 //EPD init
		EPD_display_Clean(); 									 //Clear screen
    EPD_refresh();												 //EPD_refresh		
		EPD_sleep();													 //EPD_sleep

		while(1);	
	}
 }


 
 
 
 
 

/*************************EPD display init function******************************************************/
void EPD_init_partial(void)
{
		HRES=0x98;					  					//152
	  VRES_byte1=0x00;								//152
	  VRES_byte2=0x98;
		EPD_W21_Init();

		EPD_W21_WriteCMD(0x06);         //booster soft start
		EPD_W21_WriteDATA (0x17);				//A
		EPD_W21_WriteDATA (0x17);				//B
		EPD_W21_WriteDATA (0x17);				//C 
		
		EPD_W21_WriteCMD(0x04);
		lcd_chkstatus();
		
		EPD_W21_WriteCMD(0x00);					//panel setting
		EPD_W21_WriteDATA(0xb7);				//BWOTP 97
	  EPD_W21_WriteDATA(0x0d);     		//VCOM to 0V		
		
		EPD_W21_WriteCMD(0x30);					//PLL setting
		EPD_W21_WriteDATA (0x3c);      	//100hz 
		
		EPD_W21_WriteCMD(0x61);					//resolution setting
		EPD_W21_WriteDATA (HRES);        	 
		EPD_W21_WriteDATA (VRES_byte1);		
		EPD_W21_WriteDATA (VRES_byte2);

}

/////////////////////////////Enter deep sleep mode////////////////////////
void EPD_sleep(void) 								//Enter deep sleep mode
{
		EPD_W21_WriteCMD(0X50);
		EPD_W21_WriteDATA(0xf7);	
		EPD_W21_WriteCMD(0X02);  				//power off
	  lcd_chkstatus();
		EPD_W21_WriteCMD(0X07);  				//deep sleep
		EPD_W21_WriteDATA(0xA5);
}


///////////////////OTP/////////////////////////////////////////////////////////////////
void EPD_init(void)
{
		HRES=0x98;					  					//152
	  VRES_byte1=0x00;								//152
	  VRES_byte2=0x98;
	
		EPD_W21_Init();
	
		EPD_W21_WriteCMD(0x06);         //boost soft start
		EPD_W21_WriteDATA (0x17);				//A
		EPD_W21_WriteDATA (0x17);				//B
		EPD_W21_WriteDATA (0x17);				//C       

		EPD_W21_WriteCMD(0x04);  
		lcd_chkstatus();

		EPD_W21_WriteCMD(0x00);					//panel setting
		EPD_W21_WriteDATA(0x97);				//LUT from OTP，128x296
	  EPD_W21_WriteDATA(0x0d);     		//VCOM to 0V

		EPD_W21_WriteCMD(0x61);					//resolution setting
		EPD_W21_WriteDATA (HRES);        	 
		EPD_W21_WriteDATA (VRES_byte1);		
		EPD_W21_WriteDATA (VRES_byte2);

		EPD_W21_WriteCMD(0X50);					//VCOM AND DATA INTERVAL SETTING			
		EPD_W21_WriteDATA(0x97);				//WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
}
void PIC_display(const unsigned char* picData)
{
    unsigned int i;
		EPD_W21_WriteCMD(0x10);	       //Transfer old data
	  for(i=0;i<2888;i++)	     
	  EPD_W21_WriteDATA(0xff); 
	
		EPD_W21_WriteCMD(0x13);		     //Transfer new data
	  for(i=0;i<2888;i++)	     
	{
	  EPD_W21_WriteDATA(*picData);
	  picData++;
	}

}

/***************** full screen display picture*************************/
void EPD_display_Clean(void)
{
	unsigned int i;
		EPD_W21_WriteCMD(0x10);
		for(i=0;i<2888;i++)	     
		{
				EPD_W21_WriteDATA(0xff);  
		}  
		driver_delay_xms(2);	

		EPD_W21_WriteCMD(0x13);
		for(i=0;i<2888;i++)	     
		{
				EPD_W21_WriteDATA(0xff);  
		}  
		driver_delay_xms(2);		 
}

void EPD_refresh(void)
{
		EPD_W21_WriteCMD(0x12);					//DISPLAY REFRESH 	
		driver_delay_xms(100);	        //!!!The delay here is necessary, 200uS at least!!!     
		lcd_chkstatus();
}

//Detection busy
void lcd_chkstatus(void)
{
	unsigned char busy;
	do
	{
		EPD_W21_WriteCMD(0x71);
		busy = isEPD_W21_BUSY;
		busy =!(busy & 0x01);        
	}
	while(busy);   
	driver_delay_xms(200);                       
}






///////////////partial////////////////////////////////////////////////
//LUT download

void lut1(void)
{
	unsigned int count;
	EPD_W21_WriteCMD(0x20);
	for(count=0;count<44;count++)	     
		{EPD_W21_WriteDATA(lut_vcom1[count]);}

	EPD_W21_WriteCMD(0x21);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_ww1[count]);}   
	
	EPD_W21_WriteCMD(0x22);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_bw1[count]);} 

	EPD_W21_WriteCMD(0x23);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_wb1[count]);} 

	EPD_W21_WriteCMD(0x24);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_bb1[count]);}   
}


/*****************************Initial value of old data**********************************/
/*
temp_FULL：全屏刷新旧数据暂存，初始必须全刷0xFF（白屏）
temp_40_16：定义区域旧数据暂存，初始必须全刷0xFF（白屏）
... ...
... ...
temp_64_122：定义区域旧数据暂存，初始必须全刷0xFF（白屏）
有几个局刷区域，就要有几个旧数据暂存区，旧数据暂存区不能小于局刷区域
*/
void EPD_Initial_value(unsigned char datas)
{
		int i;
	
		for(i=0;i<2888;i++)	     
		{
			temp_FULL[i] = datas;  
			temp_40_16[i] = datas;
			temp_40_93[i] = datas;		
			temp_64_10[i] = datas;
			temp_64_41[i] = datas;	
			temp_64_72[i] = datas;
			temp_64_80[i] = datas;		
			temp_64_111[i] = datas;
			temp_64_122[i] = datas;				
		}  			
}
/***************************partial display function*************************************/

/*
mode=POS：正显；
mode=NEG：负显；
mode=CLEAR：清除定义区域显示
*/

void EPD_partial_alone(u16 x_start,u16 y_start,unsigned char *old_data,const unsigned char *new_data,unsigned int PART_COLUMN,unsigned int PART_LINE,unsigned char mode) //partial display 
{
	unsigned int i,count;  
	unsigned int x_end,y_startH,y_startL,y_endH,y_endL;
	x_start=x_start;//转换为字节
	x_end=x_start+PART_LINE-1; 
	
	y_startH=0;
	y_startL=y_start;

	y_endH=0;
	y_endL=y_start+PART_COLUMN-1;

	
count=PART_COLUMN*PART_LINE/8;
	
	  EPD_W21_WriteCMD(0x82);			//vcom_DC setting  	
    EPD_W21_WriteDATA (0x08);	
		EPD_W21_WriteCMD(0X50);
		EPD_W21_WriteDATA(0x47);		
		lut1();
	  EPD_W21_WriteCMD(0x91);		//This command makes the display enter partial mode
		EPD_W21_WriteCMD(0x90);		//resolution setting
		EPD_W21_WriteDATA (x_start);   //x-start     
		EPD_W21_WriteDATA (x_end);	 //x-end	

		EPD_W21_WriteDATA (y_startH);
		EPD_W21_WriteDATA (y_startL);   //y-start    
		
		EPD_W21_WriteDATA (y_endH);		
		EPD_W21_WriteDATA (y_endL);  //y-end
		EPD_W21_WriteDATA (0x00);	

		EPD_W21_WriteCMD(0x10);	       //writes Old data to SRAM for programming
		for(i=0;i<count;i++)	     
		{
			EPD_W21_WriteDATA(~old_data[i]);  
		}  

	
		EPD_W21_WriteCMD(0x13);				 //writes New data to SRAM.
		if(mode==POS) 								 //positive
		{
			for(i=0;i<count;i++)	     
			{
				EPD_W21_WriteDATA(~new_data[i]);  
				old_data[i]=new_data[i];    //刷新后的数据成为旧数据暂存起来，用做下次刷新的旧数据
			}
		}
		
		if(mode==NEG) 								 //negative
		{
			for(i=0;i<count;i++)	     
			{
				EPD_W21_WriteDATA(new_data[i]);  
				old_data[i]=~new_data[i];    //刷新后的数据成为旧数据暂存起来，用做下次刷新的旧数据
			}
		}		
		if(mode==CLEAR) 							 //clear
		{
			for(i=0;i<count;i++)	     
			{
				EPD_W21_WriteDATA(0x00);  
				old_data[i]=new_data[i];    //刷新后的数据成为旧数据暂存起来，用做下次刷新的旧数据				
			}		
		}		
    	
		EPD_W21_WriteCMD(0x12);		 //DISPLAY REFRESH 		             
		driver_delay_xms(10);     //!!!The delay here is necessary, 200uS at least!!!     
		lcd_chkstatus();
	  
}



void EPD_partial_mult(u16 x_start,u16 y_start,unsigned char *old_data1,const unsigned char *new_data1,unsigned int PART_COLUMN1,
																							unsigned char *old_data2,const unsigned char *new_data2,unsigned int PART_COLUMN2,
																							unsigned char *old_data3,const unsigned char *new_data3,unsigned int PART_COLUMN3,
																							unsigned char *old_data4,const unsigned char *new_data4,unsigned int PART_COLUMN4,
																							unsigned char *old_data5,const unsigned char *new_data5,unsigned int PART_COLUMN5,
																							unsigned char *old_data6,const unsigned char *new_data6,unsigned int PART_COLUMN6,
																							unsigned int PART_LINE)
{
	unsigned int i,count1,count2,count3,count4,count5,count6;  
	unsigned int x_end,y_startH,y_startL,y_endH,y_endL;
	x_start=x_start;//转换为字节
	x_end=x_start+PART_LINE-1; 
	
	y_startH=0;
	y_startL=y_start;

	y_endH=0;
	y_endL=y_start+PART_COLUMN1+PART_COLUMN2+PART_COLUMN3+PART_COLUMN4+PART_COLUMN5+PART_COLUMN6-1;

		count1=PART_COLUMN1*PART_LINE/8;		
		count2=PART_COLUMN2*PART_LINE/8;		
		count3=PART_COLUMN3*PART_LINE/8;		
		count4=PART_COLUMN4*PART_LINE/8;		
		count5=PART_COLUMN5*PART_LINE/8;	
		count6=PART_COLUMN6*PART_LINE/8;	
	
	  EPD_W21_WriteCMD(0x82);			//vcom_DC setting  	
    EPD_W21_WriteDATA (0x08);	
		EPD_W21_WriteCMD(0X50);
		EPD_W21_WriteDATA(0x47);		
		lut1();
	  EPD_W21_WriteCMD(0x91);		//This command makes the display enter partial mode
		EPD_W21_WriteCMD(0x90);		//resolution setting
		EPD_W21_WriteDATA (x_start);   //x-start     
		EPD_W21_WriteDATA (x_end);	 //x-end	

		EPD_W21_WriteDATA (y_startH);
		EPD_W21_WriteDATA (y_startL);   //y-start    
		
		EPD_W21_WriteDATA (y_endH);		
		EPD_W21_WriteDATA (y_endL);  //y-end
		EPD_W21_WriteDATA (0x00);	

		EPD_W21_WriteCMD(0x10);	       //writes Old data to SRAM for programming
		for(i=0;i<count1;i++)	     
		{
			EPD_W21_WriteDATA(~old_data1[i]);  
		}  
		for(i=0;i<count2;i++)	     
		{
			EPD_W21_WriteDATA(~old_data2[i]);  
		}  
		for(i=0;i<count3;i++)	     
		{
			EPD_W21_WriteDATA(~old_data3[i]);  
		}  
		for(i=0;i<count4;i++)	     
		{
			EPD_W21_WriteDATA(~old_data4[i]);  
		}  
		for(i=0;i<count5;i++)	     
		{
			EPD_W21_WriteDATA(~old_data5[i]);  
		}  
		for(i=0;i<count6;i++)	     
		{
			EPD_W21_WriteDATA(~old_data6[i]);  
		}  		
	
		EPD_W21_WriteCMD(0x13);				 //writes New data to SRAM.
		for(i=0;i<count1;i++)	     
		{
			EPD_W21_WriteDATA(~new_data1[i]);  
			old_data1[i]=new_data1[i];    //刷新后的数据成为旧数据暂存起来，用做下次刷新的旧数据
		}
		for(i=0;i<count2;i++)	     
		{
			EPD_W21_WriteDATA(~new_data2[i]);  
			old_data2[i]=new_data2[i];    //刷新后的数据成为旧数据暂存起来，用做下次刷新的旧数据
		}
		for(i=0;i<count3;i++)	     
		{
			EPD_W21_WriteDATA(~new_data3[i]);  
			old_data3[i]=new_data3[i];    //刷新后的数据成为旧数据暂存起来，用做下次刷新的旧数据
		}
		for(i=0;i<count4;i++)	     
		{
			EPD_W21_WriteDATA(~new_data4[i]);  
			old_data4[i]=new_data4[i];    //刷新后的数据成为旧数据暂存起来，用做下次刷新的旧数据
		}
		for(i=0;i<count5;i++)	     
		{
			EPD_W21_WriteDATA(~new_data5[i]);  
			old_data5[i]=new_data5[i];    //刷新后的数据成为旧数据暂存起来，用做下次刷新的旧数据
		}
		for(i=0;i<count6;i++)	     
		{
			EPD_W21_WriteDATA(~new_data6[i]);  
			old_data6[i]=new_data6[i];    //刷新后的数据成为旧数据暂存起来，用做下次刷新的旧数据
		}

    	
		EPD_W21_WriteCMD(0x12);		 //DISPLAY REFRESH 		             
		driver_delay_xms(10);     //!!!The delay here is necessary, 200uS at least!!!     
		lcd_chkstatus();
	  
}							


/***********************************************************
						end file
***********************************************************/















/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
 
  // Reset RCC clock configuration
  RCC_DeInit();
 
  // Enable external crystal
  RCC_HSEConfig(RCC_HSE_ON);
  
  // Waiting for the external crystal to stabilize
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if(HSEStartUpStatus == SUCCESS)
  {
    // Set the phase-locked loop frequency PLLCLK = 8MHz * 9 = 72 MHz
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
  }
  else {
    // Enable internal crystal
    RCC_HSICmd(ENABLE);
    // Waiting for the internal crystal to stabilize
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

    // Set the phase-locked loop frequency PLLCLK = 8MHz/2 * 16 = 64 MHz 
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_16);
  }

    // Enable flash prefetch cache
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

  //Set the code delay, FLASH_Latency_2 is two delay cycles
  FLASH_SetLatency(FLASH_Latency_2);
	
  //Set the system total clock
  RCC_HCLKConfig(RCC_SYSCLK_Div1); 

  //Set the high speed device total clock, RCC_HCLK_Div1 is the system clock divided by 1
  RCC_PCLK2Config(RCC_HCLK_Div1); 

  //Set the low speed device total clock, RCC_HCLK_Div2 is the system clock divided by 2
  RCC_PCLK1Config(RCC_HCLK_Div2);
  
  //Enable phase-locked loop multiplier
  RCC_PLLCmd(ENABLE);
  
  // Waiting for the frequency of the phase-locked loop to multiply after frequency stabilization
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
  
  // Select the phase-locked loop clock as the system clock
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  // Waiting for setup to complete
  while(RCC_GetSYSCLKSource() != 0x08);
    
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
            RCC_APB2Periph_USART1|RCC_APB2Periph_AFIO,
            ENABLE);

}

/*******************************************************************************
* Function name  : GPIO_Configuration
* Description         : Set the GPIO pin parameters used by the SPI serial port.
* Input        : None
* Output        : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE, ENABLE);
	  				     	
	
	 //CS-->PD8   SCK-->PD9  SDO--->PD10 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;		//Port configuration
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 		
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  	
	
	
	
	 // D/C--->PE15	   RES-->PE14
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;		//Port configuration
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 		
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				     		
	
	// BUSY--->PE13
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//Pull up input
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);				//Initialize GPIO
	
	 //LED
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;		//Port configuration
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 		
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{ 
  //NVIC_InitTypeDef NVIC_InitStructure;
  ;
}


#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif






