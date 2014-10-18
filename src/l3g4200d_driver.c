/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : L3G4200D.c
* Author             : MSH Application Team
* Author             : Andrea Labombarda
* Version            : $Revision:$
* Date               : $Date:$
* Description        : L3G4200D driver file
*                      
* HISTORY:
* Date               |	Modification                    |	Author
* 22/03/2011         |	Initial Revision                |	Andrea Labombarda

********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOFTWARE IS SPECIFICALLY DESIGNED FOR EXCLUSIVE USE WITH ST PARTS.
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"
#include "l3g4200d_driver.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name		: L3G4200D_ReadReg
* Description		: Generic Reading function. It must be fullfilled with either
*					: I2C or SPI reading functions					
* Input				: Register Address
* Output			: Data REad
* Return			: None
*******************************************************************************/
u8_t L3G4200D_ReadReg(u8_t Reg, u8_t* Data) {
  
  //To be completed with either I2c or SPI reading function
  //i.e.: *Data = SPI_Mems_Read_Reg( Reg );
 	if(!I2C_ByteRead(L3G4200D_I2C, Data, L3G4200D_I2C_ADDR, Reg))
		return MEMS_ERROR;
	return MEMS_SUCCESS;
}

/*******************************************************************************
* Function Name		: L3G4200D_WriteReg
* Description		: Generic Writing function. It must be fullfilled with either
*					: I2C or SPI writing function
* Input				: Register Address, Data to be written
* Output			: None
* Return			: None
*******************************************************************************/
u8_t L3G4200D_WriteReg(u8_t Reg, u8_t Data) {
    
  //To be completed with either I2c or SPI writing function
  //i.e.: SPI_Mems_Write_Reg(Reg, Data);
  
  if (!I2C_ByteWrite(L3G4200D_I2C, &Data, L3G4200D_I2C_ADDR,  Reg))
  	return MEMS_ERROR;
  return MEMS_SUCCESS;
}
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : L3G4200D_SetODR
* Description    : Sets L3G4200D Output Data Rate
* Input          : Output Data Rate
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetODR(L3G4200D_ODR_t ov){
  u8_t value;

  if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG1, &value) )
    return MEMS_ERROR;

  value &= 0x0f;
  value |= ov<<4;

  if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG1, value) )
    return MEMS_ERROR;

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_SetMode
* Description    : Sets L3G4200D Operating Mode
* Input          : Modality (NORMAL, SLEEP, POWER_DOWN)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetMode(L3G4200D_Mode_t md) {
  u8_t value;
  
  if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG1, &value) )
    return MEMS_ERROR;
                  
  switch(md) {
  
  case L3G4200D_POWER_DOWN:		
    value &= 0xF7;
    value |= (MEMS_RESET<<L3G4200D_PD);
    break;
          
  case L3G4200D_NORMAL:
    value &= 0xF7;
    value |= (MEMS_SET<<L3G4200D_PD);
    break;
          
  case L3G4200D_SLEEP:		
    value &= 0xF0;
    value |= ( (MEMS_SET<<L3G4200D_PD) | (MEMS_RESET<<L3G4200D_ZEN) | (MEMS_RESET<<L3G4200D_YEN) | (MEMS_RESET<<L3G4200D_XEN) );
    break;
          
  default:
    return MEMS_ERROR;
  }
  
  if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG1, value) )
    return MEMS_ERROR;
                  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_SetAxis
* Description    : Enable/Disable L3G4200D Axis
* Input          : X_ENABLE/X_DISABLE | Y_ENABLE/Y_DISABLE | Z_ENABLE/Z_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetAxis(L3G4200D_Axis_t axis) {
  u8_t value;
  
  if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG1, &value) )
    return MEMS_ERROR;
    
  value &= 0xf8;
  value |= axis;
  
  if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG1, value) )
    return MEMS_ERROR;  
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_SetFullScale
* Description    : Sets the L3G4200D FullScale
* Input          : FS_250/FS_500/FS_2000
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetFullScale(L3G4200D_Fullscale_t fs) {
  u8_t value;
  
  if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG4, &value) )
    return MEMS_ERROR;
                  
  value &= 0xCF;	
  value |= (fs<<L3G4200D_FS);
  
  if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_SetBDU
* Description    : Enable/Disable Block Data Update Functionality
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetBDU(State_t bdu) {
  u8_t value;
  
  if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG4, &value) )
    return MEMS_ERROR;
 
  value &= 0x7F;
  value |= (bdu<<L3G4200D_BDU);

  if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG4, value) )
    return MEMS_ERROR;

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_SetBLE
* Description    : Set Endianess (MSB/LSB)
* Input          : BLE_LSB / BLE_MSB
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetBLE(L3G4200D_Endianess_t ble) {
  u8_t value;
  
  if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG4, &value) )
    return MEMS_ERROR;
                  
  value &= 0xBF;	
  value |= (ble<<L3G4200D_BLE);
  
  if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_HPFEnable
* Description    : Enable/Disable High Pass Filter
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_HPFEnable(State_t hpf) {
  u8_t value;
  
  if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG5, &value) )
    return MEMS_ERROR;
                  
  value &= 0xEF;
  value |= (hpf<<L3G4200D_HPEN);
  
  if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG5, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_SetHPFMode
* Description    : Set High Pass Filter Modality
* Input          : HPM_NORMAL_MODE_RES/HPM_REF_SIGNAL/HPM_NORMAL_MODE/HPM_AUTORESET_INT
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetHPFMode(L3G4200D_HPFMode_t hpf) {
  u8_t value;
  
  if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG2, &value) )
    return MEMS_ERROR;
                  
  value &= 0xCF;
  value |= (hpf<<L3G4200D_HPM);
  
  if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_SetHPFCutOFF
* Description    : Set High Pass CUT OFF Freq
* Input          : HPFCF_0,HPFCF_1,HPFCF_2... See Table 27 of the datasheet
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetHPFCutOFF(L3G4200D_HPFCutOffFreq_t hpf) {
  u8_t value;
  
  if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG2, &value) )
    return MEMS_ERROR;
                  
  value &= 0xF0;
  value |= (hpf<<L3G4200D_HPFC0);
  
  if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
  
}


/*******************************************************************************
* Function Name  : L3G4200D_SetIntPinMode
* Description    : Set Interrupt Pin Modality (push pull or Open drain)
* Input          : PUSH_PULL/OPEN_DRAIN
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetIntPinMode(L3G4200D_IntPinMode_t pm) {
  u8_t value;
  
  if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG3, &value) )
    return MEMS_ERROR;
                  
  value &= 0xEF;
  value |= (pm<<L3G4200D_PP_OD);
  
  if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG3, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_SetInt1Pin
* Description    : Set Interrupt1 pin Function
* Input          : L3G4200D_I1_ON_PIN_INT1_ENABLE | L3G4200D_I1_BOOT_ON_INT1 | L3G4200D_INT1_ACTIVE_HIGH
* example        : L3G4200D_SetInt1Pin(L3G4200D_I1_ON_PIN_INT1_ENABLE | L3G4200D_I1_BOOT_ON_INT1_ENABLE | L3G4200D_INT1_ACTIVE_LOW) 
* to enable Interrupt 1 or Bootsatus on interrupt 1 pin
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetInt1Pin(L3G4200D_Int1PinConf_t pinConf) {
  u8_t value;
  
  if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG3, &value) )
    return MEMS_ERROR;
                  
  value &= 0x1F;
  value |= pinConf;
  
  if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG3, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_SetInt2Pin
* Description    : Set Interrupt2 pin Function
* Input          : L3G4200D_I2_DRDY_ON_INT2_ENABLE/DISABLE | 
                   L3G4200D_WTM_ON_INT2_ENABLE/DISABLE | 
                   L3G4200D_OVERRUN_ON_INT2_ENABLE/DISABLE | 
                   L3G4200D_EMPTY_ON_INT2_ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetInt2Pin(L3G4200D_Int2PinConf_t pinConf) {
  u8_t value;
  
  if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG3, &value) )
    return MEMS_ERROR;
                  
  value &= 0xF0;
  value |= pinConf;
  
  if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG3, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : L3G4200D_Int1LatchEnable
* Description    : Enable Interrupt 1 Latching function
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_Int1LatchEnable(State_t latch) {
  u8_t value;
  
  if( !L3G4200D_ReadReg(L3G4200D_INT1_CFG, &value) )
    return MEMS_ERROR;
                  
  value &= 0xBF;
  value |= latch<<L3G4200D_LIR;
  
  if( !L3G4200D_WriteReg(L3G4200D_INT1_CFG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_ResetInt1Latch
* Description    : Reset Interrupt 1 Latching function
* Input          : None
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_ResetInt1Latch(void) {
  u8_t value;
  
  if( !L3G4200D_ReadReg(L3G4200D_INT1_SRC, &value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_SetIntConfiguration
* Description    : Interrupt 1 Configuration
* Input          : AND/OR, INT1_LIR ZHIE_ENABLE/DISABLE | INT1_ZLIE_ENABLE/DISABLE...
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetIntConfiguration(L3G4200D_Int1Conf_t ic) {
  u8_t value;
  
  value = ic;

  if( !L3G4200D_WriteReg(L3G4200D_INT1_CFG, value) )
    return MEMS_ERROR;

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_SetInt1Threshold
* Description    : Sets Interrupt 1 Threshold
* Input          : Threshold = [0,31]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetInt1Threshold(L3G4200D_IntThsAxis axis, u16_t ths) {
  u8_t value;
  
  switch (axis) {
    
    case L3G4200D_THS_X:
      //write the threshold LSB
      value = (u8_t)( ths & 0x00ff); 
      if( !L3G4200D_WriteReg(L3G4200D_INT1_THS_XL, value) )
        return MEMS_ERROR;
      
      //write the threshold LSB
      value = (u8_t)( ths >> 8); 
      if( !L3G4200D_WriteReg(L3G4200D_INT1_THS_XH, value) )
        return MEMS_ERROR;
      
      break;
      
    case L3G4200D_THS_Y:
      //write the threshold LSB
      value = (u8_t)( ths & 0x00ff); 
      if( !L3G4200D_WriteReg(L3G4200D_INT1_THS_YL, value) )
        return MEMS_ERROR;
      
      //write the threshold LSB
      value = (u8_t)( ths >> 8); 
      if( !L3G4200D_WriteReg(L3G4200D_INT1_THS_YH, value) )
        return MEMS_ERROR;
      
      break;
      
    case L3G4200D_THS_Z:
      //write the threshold LSB
      value = (u8_t)( ths & 0x00ff); 
      if( !L3G4200D_WriteReg(L3G4200D_INT1_THS_ZL, value) )
        return MEMS_ERROR;
      
      //write the threshold LSB
      value = (u8_t)( ths >> 8); 
      if( !L3G4200D_WriteReg(L3G4200D_INT1_THS_ZH, value) )
        return MEMS_ERROR;
      
      break;     

        
  }

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_SetInt1Duration
* Description    : Sets Interrupt 1 Duration
* Input          : Duration value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetInt1Duration(L3G4200D_Int1Conf_t id) {
 
  if (id > 127)
    return MEMS_ERROR;

  if( !L3G4200D_WriteReg(L3G4200D_INT1_DURATION, id) )
    return MEMS_ERROR;

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_FIFOModeEnable
* Description    : Sets Fifo Modality
* Input          : 
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_FIFOModeEnable(L3G4200D_FifoMode_t fm) {
  u8_t value;  
  
  if(fm == L3G4200D_FIFO_DISABLE) {
    
    if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG5, &value) )
      return MEMS_ERROR;
                    
    value &= 0xBF;    
    
    if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG5, value) )
      return MEMS_ERROR;
    
  }
  else {
    
    if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG5, &value) )
      return MEMS_ERROR;
                    
    value &= 0xBF;
    value |= MEMS_SET<<L3G4200D_FIFO_EN;
    
    if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG5, value) )
      return MEMS_ERROR;
    
    
    if( !L3G4200D_ReadReg(L3G4200D_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1f;
    value |= (fm<<L3G4200D_FM0);
    
    if( !L3G4200D_WriteReg(L3G4200D_FIFO_CTRL_REG, value) )
      return MEMS_ERROR;
  }

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_SetWaterMark
* Description    : Sets Watermark Value
* Input          : Watermark = [0,31]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetWaterMark(u8_t wtm) {
  u8_t value;
  
  if(wtm > 31)
    return MEMS_ERROR;  
  
  if( !L3G4200D_ReadReg(L3G4200D_FIFO_CTRL_REG, &value) )
    return MEMS_ERROR;
                  
  value &= 0xE0;
  value |= wtm; 
  
  if( !L3G4200D_WriteReg(L3G4200D_FIFO_CTRL_REG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_GetSatusReg
* Description    : Read the status register
* Input          : None
* Output         : status register buffer
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_GetSatusReg(u8_t* buff) {
  if( !L3G4200D_ReadReg(L3G4200D_STATUS_REG, buff) )
      return MEMS_ERROR;
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : L3G4200D_GetAngRateRaw
* Description    : Read the Angular Rate Registers
* Input          : None
* Output         : Angular Rate Registers buffer
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_GetAngRateRaw(AxesRaw_t* buff) {
  u8_t valueL;
  u8_t valueH;
  

  if( !L3G4200D_ReadReg(L3G4200D_OUT_X_L, &valueL) )
      return MEMS_ERROR;
  
  if( !L3G4200D_ReadReg(L3G4200D_OUT_X_H, &valueH) )
      return MEMS_ERROR;
  
  buff->AXIS_X = (i16_t)( (valueH << 8) | valueL );
  
  if( !L3G4200D_ReadReg(L3G4200D_OUT_Y_L, &valueL) )
      return MEMS_ERROR;
  
  if( !L3G4200D_ReadReg(L3G4200D_OUT_Y_H, &valueH) )
      return MEMS_ERROR;
  
  buff->AXIS_Y = (i16_t)( (valueH << 8) | valueL );
  
   if( !L3G4200D_ReadReg(L3G4200D_OUT_Z_L, &valueL) )
      return MEMS_ERROR;
  
  if( !L3G4200D_ReadReg(L3G4200D_OUT_Z_H, &valueH) )
      return MEMS_ERROR;
  
  buff->AXIS_Z = (i16_t)( (valueH << 8) | valueL );
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : L3G4200D_GetInt1Src
* Description    : Reset Interrupt 1 Latching function
* Input          : None
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_GetInt1Src(u8_t* buff) {
  
  if( !L3G4200D_ReadReg(L3G4200D_INT1_SRC, buff) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_GetFifoSourceReg
* Description    : Read Fifo source Register
* Input          : None
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_GetFifoSourceReg(u8_t* buff) {
  
  if( !L3G4200D_ReadReg(L3G4200D_FIFO_SRC_REG, buff) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3G4200D_SetOutputDataAndFifoFilters
* Description    : ENABLE/DISABLE HIGH PASS and LOW PASS filters applied to output and fifo registers
*                : See Table 8 of AN3393 for more details
* Input          : L3G4200D_NONE, L3G4200D_HPH, L3G4200D_LPF2, L3G4200D_HPFLPF2
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetOutputDataAndFifoFilters(L3G4200D_HPF_LPF2_Enable hpf){
  u8_t value;
  
  //HPF
  if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG5, &value) )
    return MEMS_ERROR;
  
  switch(hpf) {
    
  case L3G4200D_NONE:
    value &= 0xfc;
    value |= 0x00; //hpen = x, Out_sel_1 = 0, Out_sel_0 = 0
    break;
    
  case L3G4200D_HPF:
    value &= 0xfc;
    value |= 0x01; //hpen = x, Out_sel_1 = 0, Out_sel_0 = 1
    break;

  case L3G4200D_LPF2:
    value &= 0xed;
    value |= 0x02; //hpen = 0, Out_sel_1 = 1, Out_sel_0 = x
    break;    
   
  case L3G4200D_HPFLPF2:
    value &= 0xed;
    value |= 0x12; //hpen = 1, Out_sel_1 = 1, Out_sel_0 = x
    break;    
  }
  
  if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG5, value) )
    return MEMS_ERROR;
  
  
  return MEMS_SUCCESS;
  
}


/*******************************************************************************
* Function Name  : L3G4200D_SetInt1Filters
* Description    : ENABLE/DISABLE HIGH PASS and LOW PASS filters applied to Int1 circuitery
*                : See Table 9 of AN3393 for more details
* Input          : NONE, HPH, LPF2, HPFLPF2
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetInt1Filters(L3G4200D_HPF_LPF2_Enable hpf){
  u8_t value;
  
  //HPF
  if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG5, &value) )
    return MEMS_ERROR;
  
  switch(hpf) {
    
  case L3G4200D_NONE:
    value &= 0xf3;
    value |= 0x00<<L3G4200D_INT1_SEL0; //hpen = x, Int1_sel_1 = 0, Int1_sel_0 = 0
    break;
    
  case L3G4200D_HPF:
    value &= 0xf3;
    value |= 0x01<<L3G4200D_INT1_SEL0; //hpen = x, Int1_sel_1 = 0, Int1_sel_0 = 1
    break;

  case L3G4200D_LPF2:
    value &= 0xe7;
    value |= 0x02<<L3G4200D_INT1_SEL0; //hpen = 0, Int1_sel_1 = 1, Int1_sel_0 = x
    break;    
   
  case L3G4200D_HPFLPF2:
    value &= 0xe7;
    value |= 0x01<<L3G4200D_HPEN;
    value |= 0x02<<L3G4200D_INT1_SEL0; //hpen = 1, Int1_sel_1 = 1, Int1_sel_0 = x
    break;    
  }
  
  if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG5, value) )
    return MEMS_ERROR;
  
  
  return MEMS_SUCCESS;
  
}


/*******************************************************************************
* Function Name  : L3G4200D_SetSPIInterface
* Description    : Set SPI mode: 3 Wire Interface OR 4 Wire Interface
* Input          : L3G4200D_SPI_3_WIRE, L3G4200D_SPI_4_WIRE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3G4200D_SetSPIInterface(L3G4200D_SPIMode_t spi) {
  u8_t value;
  
  if( !L3G4200D_ReadReg(L3G4200D_CTRL_REG4, &value) )
    return MEMS_ERROR;
                  
  value &= 0xFE;
  value |= spi<<L3G4200D_SIM;
  
  if( !L3G4200D_WriteReg(L3G4200D_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  
  return MEMS_SUCCESS;
}
