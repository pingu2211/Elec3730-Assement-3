//     $Date: 2018-05-22 06:24:02 +1000 (Tue, 22 May 2018) $
// $Revision: 1330 $
//   $Author: Peter $
//
// STEPIEN: Modified to be interrupt driven and feed result into a message queue.
//          Added interrupt version of function and added interrupt driven nested state machine.

/*********************************************************************************************************
*
* File                : touch_panel.c
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "touch_panel.h"
#include "openx07v_c_lcd.h"
#include "spi.h"

// STEPIEN: Include the assignment header file just to make it easier
#include "../../src/Ass-03.h"

/* Private variables ---------------------------------------------------------*/
static Matrix matrix ; // STEPIEN: Made static (calibration matrix)
// static Coordinate  display ; // STEPIEN: No longer needed
// STEPIEN: Made static (used for calibration)
static Coordinate ScreenSample[3];

static Coordinate DisplaySample[3] = {
                                {30, 45},
                                {220, 45},
                                {160,210}
                              };

// STEPIEN: Added two touch panel functions to make the interface more
//          consistent with the LCD BSP.

uint8_t BSP_TP_Init(void)
{
  // Initialise the interface and calibrate
  TP_Init(); // This is an empty function since done by STM32CubeMX
  TouchPanel_Calibrate();
//  matrix.An = -466620;
//  matrix.Bn = -6610;
//  matrix.Cn = 156312970;
//  matrix.Dn = 19800;
//  matrix.En = 315480;
//  matrix.Fn = -1194221640;
//  matrix.Divider = -4691496;

  return 0;
}

uint8_t BSP_TP_GetDisplayPoint(Coordinate *pDisplay)
{
  volatile Coordinate *pScreen;

  static uint16_t r0=0;
  static uint16_t r11=0;
  static uint16_t r12=0;
  static uint16_t xstat[8]={0};
  static uint16_t ystat[8]={0};
  // static uint16_t xystat[8][8]={0};
  // uint16_t xx,yy;
#define XC (320/2)
#define YC (240/2)

  pScreen = Read_Ads7846();

  if (pScreen == NULL)
  {
	  r11++;
    return 1; // Panel not touched
  }
  if (getDisplayPoint(pDisplay, pScreen, &matrix ) == DISABLE)
  {
	  r12++;
    return 1; // Error in LCD
  }
  // xstat[pDisplay->x & 0x0007]++;
  // ystat[pDisplay->y & 0x0007]++;
  xstat[pScreen->x & 0x0007]++;
  ystat[pScreen->y & 0x0007]++;
  // xx = pScreen->x; // pDisplay->x;
  // yy = pScreen->y; // pDisplay->y;
  // if (xx >= XC-4 && xx < XC+4 && yy >= YC-4 && yy < YC+4)
  // {
  //  xystat[xx - (XC-4)][yy - (YC-4)]++;
  // }
  r0++;
  return 0;
}

/* Private define ------------------------------------------------------------*/
#define THRESHOLD 2
#define TP_hspi hspi2
/*******************************************************************************
* Function Name  : TP_Init
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void TP_Init(void) 
{ 

//  GPIO_InitTypeDef GPIO_InitStructure;
//  
//  RCC_AHB1PeriphClockCmd(Open_SPI_SCK_GPIO_CLK | Open_SPI_MISO_GPIO_CLK | Open_SPI_MOSI_GPIO_CLK,ENABLE);

//  RCC_AHB1PeriphClockCmd(Open_TP_CS_CLK | Open_TP_IRQ_CLK,ENABLE);
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI,ENABLE);
//  Open_SPI_CLK_INIT(Open_RCC_SPI,ENABLE);

//  GPIO_PinAFConfig(Open_SPI_SCK_GPIO_PORT,  Open_SPI_SCK_SOURCE,  Open_GPIO_AF_SPI);
//  GPIO_PinAFConfig(Open_SPI_MISO_GPIO_PORT, Open_SPI_MISO_SOURCE, Open_GPIO_AF_SPI);
//  GPIO_PinAFConfig(Open_SPI_MOSI_GPIO_PORT, Open_SPI_MOSI_SOURCE, Open_GPIO_AF_SPI);

//  GPIO_InitStructure.GPIO_Pin = Open_SPI_SCK_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;  
//  GPIO_Init(Open_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

//  GPIO_InitStructure.GPIO_Pin = Open_SPI_MISO_PIN;
//  GPIO_Init(Open_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

//  GPIO_InitStructure.GPIO_Pin = Open_SPI_MOSI_PIN;
//  GPIO_Init(Open_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);


//  /*7 inch TP_CS  */
//  GPIO_InitStructure.GPIO_Pin = Open_TP_CS_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(Open_TP_CS_PORT, &GPIO_InitStructure);

//   /*4.3 inch TP_CS  */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(GPIOI, &GPIO_InitStructure);
//  
//    /*TP_IRQ */
//    GPIO_InitStructure.GPIO_Pin = Open_TP_IRQ_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(Open_TP_IRQ_PORT, &GPIO_InitStructure);  

//  GPIO_SetBits(GPIOI,GPIO_Pin_3);
//    TP_CS(1); 
//    ADS7843_SPI_Init(); 
} 





/*******************************************************************************
* Function Name  : DelayUS
* Description    : 
* Input          : - cnt:
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void DelayUS(uint32_t cnt)
{
  uint32_t i;
  i = cnt * 4;
  while(i--);
}


/*******************************************************************************
* Function Name  : WR_CMD
* Description    : 
* Input          : - cmd: 
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
static void WR_CMD (uint8_t cmd)  
{ 
  HAL_SPI_Transmit(&TP_hspi,&cmd,1,1000);
}

// STEPIEN: Interrupt version
static uint8_t WR_CMD_IT (uint8_t cmd)
{
  HAL_SPI_Transmit_IT(&TP_hspi,&cmd,1);
  return 0;
} 

/*******************************************************************************
* Function Name  : RD_AD
* Description    : 
* Input          : None
* Output         : None
* Return         : 
* Attention      : None
*******************************************************************************/
static int RD_AD(void)  
{ 
  uint8_t buf[2];
  int value;
  HAL_SPI_Receive(&TP_hspi,buf,2,1000);
  value = (uint16_t)((buf[0] << 8) + buf[1]) >> 4; // STEPIEN: Was 3 It is 12 bit ADC
  return value;
} 

// STEPIEN: Interrupt version broken into two states
//            1: Initiate the transfer
//            2: Read the result

static uint8_t RD_AD_IT(int *value)
{
  static uint8_t buf[2];
  static uint8_t state=0;

  switch (state)
  {
	case 0:
	  HAL_SPI_Receive_IT(&TP_hspi,buf,2);
	  state = 1;
	  break;
	case 1:
	  *value = (uint16_t)((buf[0] << 8) + buf[1]) >> 3;
	  state = 0;
	  break;
	}
  return state;
}

/*******************************************************************************
* Function Name  : Read_X
* Description    : Read ADS7843 ADC X 
* Input          : None
* Output         : None
* Return         : 
* Attention      : None
*******************************************************************************/
int Read_X(void)  
{  
  int i; 
  TP_CS(0); 
  DelayUS(1); 
  WR_CMD(CHX); 
  DelayUS(1); 
  i=RD_AD(); 
  TP_CS(1); 
  return i;    
} 

// STEPIEN: Interrupt version broken into two states

uint8_t Read_X_IT(int *i)
{
  static uint8_t state=0;

  switch (state)
  {
	case 0:
	case 1:
	  TP_CS(0);
	  DelayUS(1);
	  if (WR_CMD_IT(CHX) == 0)
	  {
		state = 2;
	  }
	  else
	  {
	    state = 1;
	  }
	  break;
	case 2:
	  DelayUS(1);
      if (RD_AD_IT(i) == 0)
      {
    	TP_CS(1);
        state = 0;
      }
	  break;
	}
  return state;

}

/*******************************************************************************
* Function Name  : Read_Y
* Description    : Read ADS7843 ADC Y
* Input          : None
* Output         : None
* Return         : 
* Attention      : None
*******************************************************************************/
int Read_Y(void)  
{  
  int i; 
  TP_CS(0); 
  DelayUS(1); 
  WR_CMD(CHY); 
  DelayUS(1); 
  i=RD_AD(); 
  TP_CS(1); 
  return i;     
} 

// STEPIEN: Interrupt version broken into two states

uint8_t Read_Y_IT(int *i)
{
  static uint8_t state=0;

  switch (state)
  {
	case 0:
	case 1:
	  TP_CS(0);
	  DelayUS(1);
	  if (WR_CMD_IT(CHY) == 0)
	  {
		state = 2;
	  }
	  else
	  {
	    state = 1;
	  }
	  break;
	case 2:
	  DelayUS(1);
      if (RD_AD_IT(i) == 0)
      {
    	TP_CS(1);
        state = 0;
      }
	  break;
	}
  return state;

}


/*******************************************************************************
* Function Name  : TP_GetAdXY
* Description    : Read ADS7843
* Input          : None
* Output         : None
* Return         : 
* Attention      : None
*******************************************************************************/
void TP_GetAdXY(int *x,int *y)  
{ 
  int adx,ady; 
  adx=Read_X(); 
  DelayUS(1); 
  ady=Read_Y(); 
  *x=adx; 
  *y=ady; 
} 

// STEPIEN: Interrupt version broken into two states

uint8_t TP_GetAdXY_IT_OLD(int *x,int *y)
{
  static uint8_t state=0;

  switch (state)
  {
	case 0:
	case 1:
	  if (Read_X_IT(x) == 0)
	  {
	    state = 2;
	  }
	  else
	  {
	    state = 1;
	  }
	  break;
	case 2:
	  DelayUS(1);
	  if (Read_Y_IT(y) == 0)
      {
        state = 0;
      }
	  break;
  }
  return state;

}

// STEPIEN: Interrupt version broken into a number of states
//          New version to do manually

void LD3Tog(uint8_t n)
{
  uint8_t i;
  for (i=0;i<n+1;i++)
    HAL_GPIO_TogglePin(GPIOD, LD3_Pin); // Toggle LED3
}

void check_error(HAL_StatusTypeDef err)
{
	if (err != HAL_OK)
	{
		while (1)
		{
			LD3Tog(0);
		}
	}
}
void TP_GetAdXY_IT(int *x, int *y)
{
  static uint8_t buf[2];
  static uint8_t outbuf[2] = {0,0};

  static uint8_t ChannelX = CHX;
  static uint8_t ChannelY = CHY;

  volatile HAL_StatusTypeDef hal_error;

	  // Write X command
	  TP_CS(0);
	  DelayUS(1);
	  hal_error = HAL_SPI_Transmit_IT(&TP_hspi,&ChannelX,1);
	  // hal_error = HAL_SPI_Transmit(&TP_hspi,&ChannelX,1,1000);
      check_error(hal_error);
      // HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_SET);
      osSemaphoreWait(myBinarySem03Handle, osWaitForever);
      // HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_RESET);

	  // Read X command
	  DelayUS(1);
      hal_error = HAL_SPI_TransmitReceive_IT(&TP_hspi,outbuf,buf,2);
      // hal_error = HAL_SPI_Receive(&TP_hspi,buf,2,1000); // STEPIEN: Need to transmit 0
	  // hal_error = HAL_SPI_TransmitReceive(&TP_hspi,outbuf,buf,2,1000);
      check_error(hal_error);
      // HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_SET);
      osSemaphoreWait(myBinarySem03Handle, osWaitForever);
      // HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_RESET);

	  // Read X value
	  TP_CS(1);
	  *x = (int)((uint16_t)((buf[0] << 8) + buf[1]) >> 4); // STEPIEN: Was 3

	  // Write Y command
	  TP_CS(0);
	  DelayUS(1);
	  hal_error = HAL_SPI_Transmit_IT(&TP_hspi,&ChannelY,1);
	  // hal_error = HAL_SPI_Transmit(&TP_hspi,&ChannelY,1,1000);
      check_error(hal_error);
      // HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_SET);
      osSemaphoreWait(myBinarySem03Handle, osWaitForever);
      // HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_RESET);

	  // Read Y value
	  DelayUS(1);
	  hal_error = HAL_SPI_TransmitReceive_IT(&TP_hspi,outbuf,buf,2);
	  // hal_error = HAL_SPI_Receive(&TP_hspi,buf,2,1000); // STEPIEN: Need to transmit 0
	  // hal_error = HAL_SPI_TransmitReceive(&TP_hspi,outbuf,buf,2,1000);
      check_error(hal_error);
      // HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_SET);
      osSemaphoreWait(myBinarySem03Handle, osWaitForever);
      // HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_RESET);

	  // Read Y value
	  *y = (int)((uint16_t)((buf[0] << 8) + buf[1]) >> 4); // STEPIEN: Was 3

	  TP_CS(1);

}

// STEPIEN: Callback function for LCDTP_IRQ (PC5) LCDTP_IRQ_EXTI_IRQn

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//      Coordinate screen; // Will not be set on the first call
//
//      if (GPIO_Pin == 32)
//      {
//	    HAL_NVIC_DisableIRQ(LCDTP_IRQ_EXTI_IRQn);
//	    HAL_GPIO_TogglePin(GPIOD, LD4_Pin); // Toggle LED4
//        TP_GetAdXY_IT();
//      }
//}

// STEPIEN: Callback function for SPI2

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
      Coordinate screen;

      if (hspi == &TP_hspi)
      {
    		osSemaphoreRelease(myBinarySem03Handle);
      }
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
      Coordinate screen;

      if (hspi == &TP_hspi)
      {
  		osSemaphoreRelease(myBinarySem03Handle);
      }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
      Coordinate screen;

      if (hspi == &TP_hspi)
      {
  		osSemaphoreRelease(myBinarySem03Handle);
      }
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	volatile uint32_t hal_error;

	hal_error=HAL_SPI_GetError(hspi);
	if (hal_error != HAL_SPI_ERROR_NONE)
	  while (1);
}

/*******************************************************************************
* Function Name  : TP_DrawPoint
* Description    : 
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void TP_DrawPoint(uint16_t Xpos,uint16_t Ypos)
{
//   LCD_SetPoint(Xpos,Ypos,Blue);     /* Center point */
//   LCD_SetPoint(Xpos+1,Ypos,Blue);
//   LCD_SetPoint(Xpos,Ypos+1,Blue);
//   LCD_SetPoint(Xpos+1,Ypos+1,Blue);  
}  

/*******************************************************************************
* Function Name  : DrawCross
* Description    : 
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void DrawCross(uint16_t Xpos,uint16_t Ypos)
{
  BSP_LCD_DrawHLine(Xpos-13, Ypos, 10);
  BSP_LCD_DrawHLine(Xpos+4, Ypos, 10);
  BSP_LCD_DrawVLine(Xpos, Ypos-13, 10);
  BSP_LCD_DrawVLine(Xpos, Ypos+4, 10);  

}  
  
/*******************************************************************************
* Function Name  : Read_Ads7846
* Description    : Get TouchPanel X Y
* Input          : None
* Output         : None
* Return         : Coordinate *
* Attention      : None
*******************************************************************************/
Coordinate *Read_Ads7846(void)
{
  static Coordinate  screen;
  int m0,m1,m2,TP_X[1],TP_Y[1],temp[3];
  uint8_t count=0;
  int buffer[2][9]={{0},{0}};
  
  do
  {       
	TP_GetAdXY_IT(TP_X,TP_Y); // STEPIEN: Interrupt driven version
    buffer[0][count]=TP_X[0];  
    buffer[1][count]=TP_Y[0];
    count++;  
  }
  while(!TP_INT_IN&& count<9);  /* TP_INT_IN  */
  if(count==9)   /* Average X Y  */ 
  {
    /* Average X  */
    temp[0]=(buffer[0][0]+buffer[0][1]+buffer[0][2])/3;
    temp[1]=(buffer[0][3]+buffer[0][4]+buffer[0][5])/3;
    temp[2]=(buffer[0][6]+buffer[0][7]+buffer[0][8])/3;
   
    m0=temp[0]-temp[1];
    m1=temp[1]-temp[2];
    m2=temp[2]-temp[0];
   
    m0=m0>0?m0:(-m0);
    m1=m1>0?m1:(-m1);
    m2=m2>0?m2:(-m2);
   
    if( m0>THRESHOLD  &&  m1>THRESHOLD  &&  m2>THRESHOLD ) return 0;
   
    if(m0<m1)
    {
      if(m2<m0) 
        screen.x=(temp[0]+temp[2])/2;
      else 
        screen.x=(temp[0]+temp[1])/2;  
    }
    else if(m2<m1) 
      screen.x=(temp[0]+temp[2])/2;
    else 
      screen.x=(temp[1]+temp[2])/2;
   
    /* Average Y  */
    temp[0]=(buffer[1][0]+buffer[1][1]+buffer[1][2])/3;
    temp[1]=(buffer[1][3]+buffer[1][4]+buffer[1][5])/3;
    temp[2]=(buffer[1][6]+buffer[1][7]+buffer[1][8])/3;
    m0=temp[0]-temp[1];
    m1=temp[1]-temp[2];
    m2=temp[2]-temp[0];
    m0=m0>0?m0:(-m0);
    m1=m1>0?m1:(-m1);
    m2=m2>0?m2:(-m2);
    if(m0>THRESHOLD&&m1>THRESHOLD&&m2>THRESHOLD) return 0;
   
    if(m0<m1)
    {
      if(m2<m0) 
        screen.y=(temp[0]+temp[2])/2;
      else 
        screen.y=(temp[0]+temp[1])/2;  
      }
    else if(m2<m1) 
       screen.y=(temp[0]+temp[2])/2;
    else
       screen.y=(temp[1]+temp[2])/2;
   
    return &screen;
  }
  return 0; 
}
   
// STEPIEN: Interrupt version

//uint8_t Read_Ads7846_IT(Coordinate *screen)
//{
//  static int TP_X,TP_Y;
//	  int m0,m1,m2,temp[3];
//  static uint8_t count=0;
//  static int buffer[2][9]={{0},{0}};
//  static uint8_t state=0;
//
//  switch (state)
//  {
//	case 0:
//	case 1:
//      if (TP_GetAdXY_IT(&TP_X,&TP_Y) == 0)
//      {
//        buffer[0][count]=TP_X;
//        buffer[1][count]=TP_Y;
//        count++;
//      }
//      if (count < 9)
//      {
//    	  state = 1;
//      }
//      else
//      {
//
//    /* Average X  */
//    temp[0]=(buffer[0][0]+buffer[0][1]+buffer[0][2])/3;
//    temp[1]=(buffer[0][3]+buffer[0][4]+buffer[0][5])/3;
//    temp[2]=(buffer[0][6]+buffer[0][7]+buffer[0][8])/3;
//
//    m0=temp[0]-temp[1];
//    m1=temp[1]-temp[2];
//    m2=temp[2]-temp[0];
//
//    m0=m0>0?m0:(-m0);
//    m1=m1>0?m1:(-m1);
//    m2=m2>0?m2:(-m2);
//
//    if( m0>THRESHOLD  &&  m1>THRESHOLD  &&  m2>THRESHOLD ) return 0;
//
//    if(m0<m1)
//    {
//      if(m2<m0)
//        screen->x=(temp[0]+temp[2])/2;
//      else
//        screen->x=(temp[0]+temp[1])/2;
//    }
//    else if(m2<m1)
//      screen->x=(temp[0]+temp[2])/2;
//    else
//      screen->x=(temp[1]+temp[2])/2;
//
//    /* Average Y  */
//    temp[0]=(buffer[1][0]+buffer[1][1]+buffer[1][2])/3;
//    temp[1]=(buffer[1][3]+buffer[1][4]+buffer[1][5])/3;
//    temp[2]=(buffer[1][6]+buffer[1][7]+buffer[1][8])/3;
//    m0=temp[0]-temp[1];
//    m1=temp[1]-temp[2];
//    m2=temp[2]-temp[0];
//    m0=m0>0?m0:(-m0);
//    m1=m1>0?m1:(-m1);
//    m2=m2>0?m2:(-m2);
//    if(m0>THRESHOLD&&m1>THRESHOLD&&m2>THRESHOLD) return 0;
//
//    if(m0<m1)
//    {
//      if(m2<m0)
//        screen->y=(temp[0]+temp[2])/2;
//      else
//        screen->y=(temp[0]+temp[1])/2;
//      }
//    else if(m2<m1)
//       screen->y=(temp[0]+temp[2])/2;
//    else
//       screen->y=(temp[1]+temp[2])/2;
//    state = 0;
//    count = 0;
//      }
//  }
//
//return state;
//
//}

/*******************************************************************************
* Function Name  : setCalibrationMatrix
* Description    : Calculate K A B C D E F
* Input          : None
* Output         : None
* Return         : 
* Attention      : None
*******************************************************************************/
FunctionalState setCalibrationMatrix( Coordinate * displayPtr,
                          Coordinate * screenPtr,
                          Matrix * matrixPtr)
{

  FunctionalState retTHRESHOLD = ENABLE ;
  /* K=(X0-X2) (Y1-Y2)-(X1-X2) (Y0-Y2) */
  matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                       ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
  if( matrixPtr->Divider == 0 )
  {
    retTHRESHOLD = DISABLE;
  }
  else
  {
    /* A=((XD0-XD2) (Y1-Y2)-(XD1-XD2) (Y0-Y2))/K  */
    matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
  /* B=((X0-X2) (XD1-XD2)-(XD0-XD2) (X1-X2))/K  */
    matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
                    ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;
    /* C=(Y0(X2XD1-X1XD2)+Y1(X0XD2-X2XD0)+Y2(X1XD0-X0XD1))/K */
    matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;
    /* D=((YD0-YD2) (Y1-Y2)-(YD1-YD2) (Y0-Y2))/K  */
    matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;
    /* E=((X0-X2) (YD1-YD2)-(YD0-YD2) (X1-X2))/K  */
    matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
                    ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;
    /* F=(Y0(X2YD1-X1YD2)+Y1(X0YD2-X2YD0)+Y2(X1YD0-X0YD1))/K */
    matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;
  }
  return( retTHRESHOLD ) ;
}

/*******************************************************************************
* Function Name  : getDisplayPoint
* Description    : Touch panel X Y to display X Y
* Input          : None
* Output         : None
* Return         : 
* Attention      : None
*******************************************************************************/
FunctionalState getDisplayPoint(Coordinate * displayPtr,
                     Coordinate * screenPtr,
                     Matrix * matrixPtr )
{
  FunctionalState retTHRESHOLD =ENABLE ;
  /*
  An=168
  */
  if( matrixPtr->Divider != 0 )
  {
    /* XD = AX+BY+C */        
    displayPtr->x = ( (matrixPtr->An * screenPtr->x) + 
                      (matrixPtr->Bn * screenPtr->y) + 
                       matrixPtr->Cn 
                    ) / matrixPtr->Divider ;
    /* YD = DX+EY+F */        
    displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) + 
                      (matrixPtr->En * screenPtr->y) + 
                       matrixPtr->Fn 
                    ) / matrixPtr->Divider ;
  }
  else
  {
    retTHRESHOLD = DISABLE;
  }
  return(retTHRESHOLD);
} 


/*******************************************************************************
* Function Name  : TouchPanel_Calibrate
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void TouchPanel_Calibrate(void)
{
  uint8_t i;
  Coordinate * Ptr;

  for(i=0;i<3;i++)
  {
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE); 
    // STEPIEN: Changed to centre string
    // BSP_LCD_DisplayStringAtLine(1, (uint8_t*)"        Touch crosshair to calibrate");
    BSP_LCD_DisplayStringAt(BSP_LCD_GetXSize()/2, 0, (uint8_t *)"Touch crosshair to calibrate", CENTER_MODE);
    BSP_LCD_SetFont(&Font24);
    // STEPIEN: Changed to centre string
    // BSP_LCD_DisplayStringAtLine(5, (uint8_t*)"   Waveshare LCD");
    BSP_LCD_DisplayStringAt(BSP_LCD_GetXSize()/2, BSP_LCD_GetYSize()/2, (uint8_t*)"Waveshare LCD", CENTER_MODE);
    HAL_Delay(200);
    DrawCross(DisplaySample[i].x,DisplaySample[i].y);
    do
    {
      Ptr=Read_Ads7846();
      // HAL_Delay(1); // STEPIEN: TEST
    }
    // while( 1 );
    while( Ptr == (void*)0 ); // STEPIEN: TEST
    ScreenSample[i].x= Ptr->x; ScreenSample[i].y= Ptr->y;
  }
  setCalibrationMatrix( &DisplaySample[0],&ScreenSample[0],&matrix );
  BSP_LCD_Clear(LCD_COLOR_BLACK);
} 

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
