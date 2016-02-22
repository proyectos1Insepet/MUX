/*
*********************************************************************************************************
*                                           GRP550/700 CODE
*
*                             (c) Copyright 2013; Sistemas Insepet LTDA
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                             GRP550/700 CODE
*
*                                             CYPRESS PSoC5LP
*                                                with the
*                                            CY8C5969AXI-LP035
*
* Filename      : I2C.c
* Version       : V1.00
* Programmer(s) : 
                  
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <device.h>
#include "VariablesG.h"
#include "LCD.h"

/*
*********************************************************************************************************
*                                         uint8 leer_hora( void )
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
uint8 leer_hora(){
	uint8 status,i;
	for(i=1;i<=2;i++){
        I2C_1_MasterClearStatus();
        status = I2C_1_MasterSendStart(0x68, I2C_1_WRITE_XFER_MODE);
        if(I2C_1_MSTR_NO_ERROR == status)								 		/* Check if transfer completed without errors */
    	{
            status = I2C_1_MasterWriteByte(i);
            if(status != I2C_1_MSTR_NO_ERROR)
            {
                return 0;
            }
			else{
		        I2C_1_MasterSendStop(); 										/* Send Stop */
		        CyDelay(10);
		        status = I2C_1_MasterSendStart(0x68, I2C_1_READ_XFER_MODE);
		        if(I2C_1_MSTR_NO_ERROR == status){
		            hora[i-1] = I2C_1_MasterReadByte(I2C_1_NAK_DATA);
		        }
				else{
					return 0;
				}
		        I2C_1_MasterSendStop();	
			}
        }
		else{
			return 0;
		}		
	}
	return 1;
}

/*
*********************************************************************************************************
*                                         uint8 leer_fecha( void )
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
uint8 leer_fecha(){
	uint8 status,i;
	for(i=4;i<=6;i++){
        I2C_1_MasterClearStatus();
        status = I2C_1_MasterSendStart(0x68, I2C_1_WRITE_XFER_MODE);
        if(I2C_1_MSTR_NO_ERROR == status)								 		/* Check if transfer completed without errors */
    	{
            status = I2C_1_MasterWriteByte(i);
            if(status != I2C_1_MSTR_NO_ERROR)
            {
                return 0;
            }
			else{
		        I2C_1_MasterSendStop(); 										/* Send Stop */
		        CyDelay(10);
		        status = I2C_1_MasterSendStart(0x68, I2C_1_READ_XFER_MODE);
		        if(I2C_1_MSTR_NO_ERROR == status){
		            fecha[i-4] = I2C_1_MasterReadByte(I2C_1_NAK_DATA);
		        }
				else{
					return 0;
				}
		        I2C_1_MasterSendStop();	
			}
        }
		else{
			return 0;
		}		
	}
	return 1;
}

/*
*********************************************************************************************************
*                                         uint8 write_hora( void )
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/

uint8 write_hora( void ){
	uint8 status, dato[3],i;
	dato[0]=1;
	dato[1]=hora[0];
	dato[2]=hora[1];
	I2C_1_MasterClearStatus();
    status = I2C_1_MasterSendStart(0x68, I2C_1_WRITE_XFER_MODE);
    if(I2C_1_MSTR_NO_ERROR == status) 
    {
        for(i=0; i<3; i++)
        {
            status = I2C_1_MasterWriteByte(dato[i]);
            if(status != I2C_1_MSTR_NO_ERROR)
            {
                return 0;
            }
        }
    }
    else{
		return 0;
    }
    I2C_1_MasterSendStop();	
	return 1;
}

/*
*********************************************************************************************************
*                                         uint8 write_fecha( void )
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/

uint8 write_fecha( void ){
	uint8 status, dato[4],i;
	dato[0]=4;
	dato[1]=fecha[0];
	dato[2]=fecha[1];
	dato[3]=fecha[2];
	I2C_1_MasterClearStatus();
    status = I2C_1_MasterSendStart(0x68, I2C_1_WRITE_XFER_MODE);
    if(I2C_1_MSTR_NO_ERROR == status) 
    {
        for(i=0; i<4; i++)
        {
            status = I2C_1_MasterWriteByte(dato[i]);
            if(status != I2C_1_MSTR_NO_ERROR)
            {
                return 0;
            }
        }
    }
    else{
		return 0;
    }
    I2C_1_MasterSendStop();	
	return 1;
}

/*
*********************************************************************************************************
*                              uint8 write_eeprom(uint16 page, uint8 valor)
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/

uint8 write_eeprom(uint16 page, uint8 *valor){
	uint8 status, dato[4],i;
	dato[1]=(page&0xFF);
	page=(page>>8)&0xFF;
	dato[0]=page;
	Pin_WP_Write(0);
	I2C_1_MasterClearStatus();
    status = I2C_1_MasterSendStart(0x54, I2C_1_WRITE_XFER_MODE);
    if(I2C_1_MSTR_NO_ERROR == status) 
    {
        for(i=0; i<2; i++)
        {
            status = I2C_1_MasterWriteByte(dato[i]);
            if(status != I2C_1_MSTR_NO_ERROR)
            {
                return 0;
            }
        }
        for(i=0; i<=valor[0]; i++)
        {
            status = I2C_1_MasterWriteByte(valor[i]);
            if(status != I2C_1_MSTR_NO_ERROR)
            {
                return 0;
            }
			CyDelay(5);
        }		
    }
    else{
		return 0;
    }
    I2C_1_MasterSendStop();	
	CyDelay(6);
	Pin_WP_Write(1);
	return 1;
}

/*
*********************************************************************************************************
*                               uint8 leer_eeprom(uint16 page, uint8 size))
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
uint8 leer_eeprom(uint16 page, volatile uint8 size){
	uint16 status, dato[4],i;
	dato[1]=(page&0xFF);
	page=(page>>8)&0xFF;
	dato[0]=page;	
    I2C_1_MasterClearStatus();
    status = I2C_1_MasterSendStart(0x54, I2C_1_WRITE_XFER_MODE);
    if(I2C_1_MSTR_NO_ERROR == status)								 		/* Check if transfer completed without errors */
	{
        for(i=0;i<2;i++)
        {
            status = I2C_1_MasterWriteByte(dato[i]);
            if(status != I2C_1_MSTR_NO_ERROR)
            {
            	return 0;
            }
			CyDelay(5);
        }		
    }
	else{
		return 0;
	}
	I2C_1_MasterSendStop();	
    CyDelay(50);
    status = I2C_1_MasterSendStart(0x54, I2C_1_READ_XFER_MODE);
    if(I2C_1_MSTR_NO_ERROR == status){
		for(i=0;i<size;i++){
	        buffer_i2c[i] = I2C_1_MasterReadByte(I2C_1_ACK_DATA);
			CyDelay(5);
		}
    }
	else{
		return 0;
	}
    I2C_1_MasterSendStop();	
	CyDelay(6);	
	return 1;
}

/*
*********************************************************************************************************
*                                   uint8 write_psoc1( uint8 puerto, uint8 valor )
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/

uint8 write_psoc1( uint8 puerto, uint8 valor ){
	uint8 status, dato[2],i;
	dato[0]=puerto;
	dato[1]=valor;
	I2C_1_MasterClearStatus();
    status = I2C_1_MasterSendStart(7u, I2C_1_WRITE_XFER_MODE);
    if(I2C_1_MSTR_NO_ERROR == status) 
    {
        for(i=0; i<2; i++)
        {
            status = I2C_1_MasterWriteByte(dato[i]);
            if(status != I2C_1_MSTR_NO_ERROR)
            {
                return 0;
            }
        }
    }
    else{
		return 0;
    }
    I2C_1_MasterSendStop();	
	return 1;
}

/* [] END OF FILE */
