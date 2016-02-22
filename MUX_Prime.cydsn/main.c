/*
*********************************************************************************************************
*                                           GRP550M CODE2
*											CÓDIGO PARA MUX
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
*                                               GRP550M CODE
*
*                                             CYPRESS PSoC5LP
*                                                with the
*                                            CY8C5969AXI-LP035
*
* Filename      : main.c
* Version       : V2.00
* Programmer(s) : 
                  
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <device.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Protocolo.h"
#include "VariablesG.h"
#include "Print.h"
#include "LCD.h"
#include "ibutton.h"
#include "i2c.h"

/*
*********************************************************************************************************
*                                             DECLARACION DE FUNCIONES
*********************************************************************************************************
*/
uint8 letras[26]={0x25,0x42,0x31,0x27,0x1D,0x28,0x29,0x2A,0x22,0x2B,0x2C,0x2D,0x35,0x34,0x23,0x24,0x1B,0x1E,0x26,0x1F,0x21,0x32,0x1C,0x30,0x20,0x2F};
uint8 clave_config[5]="02412";
uint8 espera;
uint8 test[18]="Test de Impresora";
uint8 msn_errort[21]="No pudo tomar Totales";
uint8 msn_errorv[19]="No pudo tomar venta";
uint8 estado_lcd1,estado_lcd2, estado_pos[4];
CY_ISR(animacion);
CY_ISR(animacion2);
CY_ISR(modo_mux);
uint8 idaux[8];
uint8 state_rf;
/*
*********************************************************************************************************
*                                         init( void )
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
void init(void){
	uint8 x,y,i;
    CyGlobalIntEnable;
    Surtidor_EnableRxInt();
    PC_EnableRxInt();
    I2C_1_Start();
    Surtidor_Start();
    Impresora_Start();
    LCD_1_Start();
    LCD_2_Start();
    LCD_1_EnableRxInt();
	LCD_2_EnableRxInt();
	Timer_Modo_Start();
	/***********Iniciacionde variables***************/
	lado1.grado[0][0]=0;
	lado1.grado[1][0]=0;
	lado1.grado[2][0]=0;
	lado2.grado[0][0]=0;
	lado2.grado[1][0]=0;
	lado2.grado[2][0]=0;
	for(y=0;y<=2;y++){
		for(x=0;x<=23;x++){
			lado1.totales[y][x]=0;
		}
	}
	for(y=0;y<=2;y++){
		for(x=0;x<=23;x++){
			lado2.totales[y][x]=0;
		}
	}	
    CyDelay(5);
	/******************Lectura de EEPROM*************/
	leer_eeprom(0,32);									//Nombre Estacion
    if((buffer_i2c[0]>0)&&(buffer_i2c[0]<=32)){
    	for(x=0;x<=buffer_i2c[0];x++){
    		nombre[x]=buffer_i2c[x];
    	}
    }
	leer_eeprom(32,15);									//Nit
    if((buffer_i2c[0]>0)&&(buffer_i2c[0]<=15)){
    	for(x=0;x<=buffer_i2c[0];x++){
    		nit[x]=buffer_i2c[x];		
    	}
    }
	leer_eeprom(64,32);									//Direccion
    if((buffer_i2c[0]>0)&&(buffer_i2c[0]<=32)){
    	for(x=0;x<=buffer_i2c[0];x++){
    		direccion[x]=buffer_i2c[x];		
    	}
    }
	leer_eeprom(47,12);									//Telefono
    if((buffer_i2c[0]>0)&&(buffer_i2c[0]<=12)){
    	for(x=0;x<=buffer_i2c[0];x++){
    		telefono[x]=buffer_i2c[x];		
    	}
    }
	leer_eeprom(128,32);								//Lema 1
    if((buffer_i2c[0]>0)&&(buffer_i2c[0]<=32)){
    	for(x=0;x<=buffer_i2c[0];x++){
    		lema1[x]=buffer_i2c[x];	
    	}
    }
	leer_eeprom(160,32);								//Lema 2
    if((buffer_i2c[0]>0)&&(buffer_i2c[0]<=32)){
    	for(x=0;x<=buffer_i2c[0];x++){
    		lema2[x]=buffer_i2c[x];
    	}
    }
	leer_eeprom(455,2);									//logo
	if(buffer_i2c[0]==1){
		 id_logo[0]=buffer_i2c[0]; 
		 id_logo[1]=buffer_i2c[1];
	}
	leer_eeprom(756,2);									//km
	if(buffer_i2c[0]==1){
		 km[0]=buffer_i2c[0]; 
		 km[1]=buffer_i2c[1];
	}		
	leer_eeprom(96,9);									//Pasword
	if((buffer_i2c[0]<=0x09)&&(buffer_i2c[0]>=0x01)){
		for(x=0;x<=7;x++){
			password_lcd[x+1]=buffer_i2c[x+1];
		}
		password_lcd[0]=buffer_i2c[0];
	}
	else{
		for(x=1;x<=8;x++){
			password_lcd[x]=x;
		}	
	}
	leer_eeprom(678,4);										//Numero de grados y codigo de cada uno 1
    if((buffer_i2c[0]<=4)&&(buffer_i2c[0]>0)){
    	lado1.mangueras=buffer_i2c[0];
    	for(x=0;x<buffer_i2c[0];x++){
    		lado1.grado[x][0]=buffer_i2c[x+1];	
    	}
    	i=0;
    	if((lado1.grado[0][0]==1)||(lado1.grado[1][0]==1)||(lado1.grado[2][0]==1)){
    		i=1;
    	}
    	if((lado1.grado[0][0]==2)||(lado1.grado[1][0]==2)||(lado1.grado[2][0]==2)){
    		i|=2;
    	}
    	if((lado1.grado[0][0]==3)||(lado1.grado[1][0]==3)||(lado1.grado[2][0]==3)){
    		i|=4;
    	}
    	switch(i){
			case 1:
				imagen_grado1=1;	
			break;
			
			case 2:
				imagen_grado1=1;	
			break;
			
			case 4:
				imagen_grado1=1;	
			break;	
			
    		case 5:
    			imagen_grado1=124;			
    		break;		
    		
    		case 3:
    			imagen_grado1=125;	
    		break;
    		
    		case 6:
    			imagen_grado1=127;	
    		break;	
    		
    		case 7:
    			imagen_grado1=126;	
    		break;		
    	}
    }
	leer_eeprom(682,4);										//Numero de grado y codigo de cada uno 2
    if((buffer_i2c[0]<=4)&&(buffer_i2c[0]>0)){
    	lado2.mangueras=buffer_i2c[0];
    	for(x=0;x<buffer_i2c[0];x++){
    		lado2.grado[x][0]=buffer_i2c[x+1];
    	}
    	i=0;
    	if((lado2.grado[0][0]==1)||(lado2.grado[1][0]==1)||(lado2.grado[2][0]==1)){
    		i=1;
    	}
    	if((lado2.grado[0][0]==2)||(lado2.grado[1][0]==2)||(lado2.grado[2][0]==2)){
    		i|=2;
    	}
    	if((lado2.grado[0][0]==3)||(lado2.grado[1][0]==3)||(lado2.grado[2][0]==3)){
    		i|=4;
    	}
    	switch(i){
			case 1:             //Cambio lado1 por lado2
				imagen_grado2=1;	
			break;
			
			case 2:
				imagen_grado2=1;	
			break;
			
			case 4:
				imagen_grado2=1;	
			break;	
			
    		case 5:
    			imagen_grado2=124;			
    		break;		
    		
    		case 3:
    			imagen_grado2=125;	
    		break;
    		
    		case 6:
    			imagen_grado2=127;	
    		break;	
    		
    		case 7:
    			imagen_grado2=126;	
    		break;		
    	}		
    }
	leer_eeprom(758,4);										//Numero de grado y codigo de cada uno 3
    if((buffer_i2c[0]<=4)&&(buffer_i2c[0]>0)){
    	lado3.mangueras=buffer_i2c[0];
    	for(x=0;x<buffer_i2c[0];x++){
    		lado3.grado[x][0]=buffer_i2c[x+1];
    	}
    	i=0;
    	if((lado3.grado[0][0]==1)||(lado3.grado[1][0]==1)||(lado3.grado[2][0]==1)){
    		i=1;
    	}
    	if((lado3.grado[0][0]==2)||(lado3.grado[1][0]==2)||(lado3.grado[2][0]==2)){
    		i|=2;
    	}
    	if((lado3.grado[0][0]==3)||(lado3.grado[1][0]==3)||(lado3.grado[2][0]==3)){
    		i|=4;
    	}
    	switch(i){
			case 1:
				imagen_grado3=1;	//Cambio lado1 a lado3
			break;
			
			case 2:
				imagen_grado3=1;	
			break;
			
			case 4:
				imagen_grado3=1;	
			break;	
			
    		case 5:
    			imagen_grado3=124;			
    		break;		
    		
    		case 3:
    			imagen_grado3=125;	
    		break;
    		
    		case 6:
    			imagen_grado3=127;	
    		break;	
    		
    		case 7:
    			imagen_grado3=126;	
    		break;		
    	}		
    }
	leer_eeprom(762,4);										//Numero de grado y codigo de cada uno 4
    if((buffer_i2c[0]<=4)&&(buffer_i2c[0]>0)){
    	lado4.mangueras=buffer_i2c[0];
    	for(x=0;x<buffer_i2c[0];x++){
    		lado4.grado[x][0]=buffer_i2c[x+1];
    	}
    	i=0;
    	if((lado4.grado[0][0]==1)||(lado4.grado[1][0]==1)||(lado4.grado[2][0]==1)){
    		i=1;
    	}
    	if((lado4.grado[0][0]==2)||(lado4.grado[1][0]==2)||(lado4.grado[2][0]==2)){
    		i|=2;
    	}
    	if((lado4.grado[0][0]==3)||(lado4.grado[1][0]==3)||(lado4.grado[2][0]==3)){
    		i|=4;
    	}
    	switch(i){
			case 1:
				imagen_grado4=1;	//Cambio lado1 por lado4
			break;
			    
			case 2:
				imagen_grado4=1;	
			break;
			
			case 4:
				imagen_grado4=1;	
			break;	
			
    		case 5:
    			imagen_grado4=124;			
    		break;		
    		
    		case 3:
    			imagen_grado4=125;	
    		break;
    		
    		case 6:
    			imagen_grado4=127;	
    		break;	
    		
    		case 7:
    			imagen_grado4=126;	
    		break;		
    	}		
    }	
	leer_eeprom(444,2);										//PPU X10
	if(buffer_i2c[0]==1){
		 ppux10=(buffer_i2c[1]&0x0f);
	}
	leer_eeprom(686,17);									//Nombre Codigo de Producto 1
    if((buffer_i2c[0]<=17)&&(buffer_i2c[0]>0)){
    	for(x=0;x<=buffer_i2c[0];x++){
    		nombre_pro1[x]=buffer_i2c[x];
    	}
    }
	leer_eeprom(704,17);									//Nombre Codigo de Producto 2
    if((buffer_i2c[0]<=17)&&(buffer_i2c[0]>0)){
    	for(x=0;x<=buffer_i2c[0];x++){
    		nombre_pro2[x]=buffer_i2c[x];
    	}
    }
	leer_eeprom(721,17);									//Nombre Codigo de Producto 3
    if((buffer_i2c[0]<=17)&&(buffer_i2c[0]>0)){    
    	for(x=0;x<=buffer_i2c[0];x++){
    		nombre_pro3[x]=buffer_i2c[x];
    	}
    }
	leer_eeprom(602,6);										//Version decimales dinero y decimales volumen
	if((buffer_i2c[0]==3)||(buffer_i2c[0]==1)||(buffer_i2c[0]==5)){
		 version[1]=(buffer_i2c[1]&0x0f);
		 version[2]=(buffer_i2c[2]&0x0f);
		 version[3]=(buffer_i2c[3]&0x0f);
	}
	leer_eeprom(608,2);							//Lado A
	if(buffer_i2c[0]==1){									
		lado1.dir=buffer_i2c[1];	
	}
	else{
		lado1.dir=0xFF;
	}
	leer_eeprom(610,2);							//Lado B
	if(buffer_i2c[0]==1){									
		lado1.dir=buffer_i2c[1];	
	}
	else{
		lado1.dir=0xFF;
	}
	leer_eeprom(804,2);							//Lado C
	if(buffer_i2c[0]==1){									
		lado3.dir=buffer_i2c[1];	
	}
	else{
		lado3.dir=0xFF;
	}
	leer_eeprom(806,2);							//Lado D	
	if(buffer_i2c[0]==1){									
		lado4.dir=buffer_i2c[1];	
	}
	else{
		lado4.dir=0xFF;
	}
	leer_eeprom(666,2);							//Impresoras
	if(buffer_i2c[0]==1){
		 print1[1]=(buffer_i2c[1]&0x0f);
	}
	leer_eeprom(613,2);
	if(buffer_i2c[0]==1){
		 print2[1]=(buffer_i2c[1]&0x0f);
	}
	rventa1.autorizado=100;
	rventa2.autorizado=100;
	rventa3.autorizado=100;
	rventa4.autorizado=100;	
    CyDelay(5);
}


/*
*********************************************************************************************************
*                             uint8 verificar_check(uint8 *datos, uint16 size)
*
* Description : calcula el checksum
*               

*********************************************************************************************************
*/
uint8 verificar_check(uint8 *datos, uint16 size){
	uint8 checksum,index;
	uint16 i;
    uint8 table[256] = { 
    0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
    35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
    70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
    17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
    50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
    87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};	
	checksum=0;
	for(i=0;i<(size-1);i++){
		index = (uint8)(checksum ^ datos[i]);
		checksum = table[index];				
	}
	return checksum;
}

/*
************************************************************************************************************
*                                         void error_op()
*
* Description : Muestra en la pantalla el mensaje de operación incorrecta y regresa al inicio del Flujo LCD
*               
*
* Argument(s) : uint8 lcd, para elegir cual pantalla entra en esta función
*
* Return(s)   : none
*
* Caller(s)   : Desde cualquier momento de la operacion donde ocurra un error por parte del usuario
*
* Note(s)     : none.
************************************************************************************************************
*/
void error_op(uint8 lcd, uint16 imagen){
	if(lcd==1){
	    set_imagen(1,imagen);
	    flujo_LCD1=100;
		flujo_LCD3=100;
		count_protector=1;
	    isr_3_StartEx(animacion);  
	    Timer_Animacion_Start();
	}
	else{
	    set_imagen(2,imagen);
	    flujo_LCD2=100;
		flujo_LCD4=100;
		count_protector2=1;
	    isr_4_StartEx(animacion2);  
	    Timer_Animacion2_Start();	
	}
}

/*
*********************************************************************************************************
*                                         init_surt( void )
*
* Description : Busca las posiciones del surtidor y las graba en lado.a.dir y lado.b.dir
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : main()
*
* Note(s)     : Falta generar codigo para los casos 1 y 2
*********************************************************************************************************
*/
void init_surt(void){
	uint8  ok=0, aux[15];
    /*lado1.dir=1;
    lado2.dir=2;*/
	while(ok!=2){
		ok=0;
		//if((lado1.dir==0xFF)||(lado2.dir==0xFF)||(lado3.dir==0xFF)||(lado4.dir==0xFF)){
			while(ver_pos()==0);
			CyDelay(50);
			totales(lado1.dir, lado1.mangueras);
			CyDelay(50);	
			totales(lado2.dir, lado2.mangueras);
			CyDelay(50);
			totales(lado3.dir, lado3.mangueras);
			CyDelay(50);
			totales(lado4.dir, lado4.mangueras);
			CyDelay(50);			
			aux[0]=1;
			aux[1]=lado1.dir;
			if(write_eeprom(608,aux)){
				set_imagen(1,60);								
			}
			else{
				set_imagen(1,85);
			}
			aux[0]=1;
			aux[1]=lado2.dir;
			if(write_eeprom(610,aux)){
				set_imagen(2,60);								
			}
			else{
				set_imagen(2,85);
			}
			aux[0]=1;
			aux[1]=lado3.dir;
			if(write_eeprom(804,aux)){
				set_imagen(1,60);								
			}
			else{
				set_imagen(1,85);
			}
			aux[0]=1;
			aux[1]=lado4.dir;
			if(write_eeprom(806,aux)){
				set_imagen(2,60);								
			}
			else{
				set_imagen(2,85);
			}			
			flujo_LCD1=0;
			flujo_LCD2=0;
			flujo_LCD3=0;
			flujo_LCD4=0;
			set_imagen(2,46);				//Pos A y B
			set_imagen(1,46);
			estado_lcd1=0;
			estado_lcd2=0;
			estado_pos[0]=0;
			estado_pos[1]=0;
			estado_pos[2]=0;
			estado_pos[3]=0;
			ok=2;			
		//}
		/*else{
			set_imagen(2,46);				//Pos A y B
			set_imagen(1,46);
			flujo_LCD2=0;
			flujo_LCD1=0;
			flujo_LCD3=0;
			flujo_LCD4=0;			
			ok=2;
		}*/
		/*else{
			estado=get_estado(lado1.dir);										
			switch(estado){
		         case 0:                     
					lado1.dir=0xFF;
				 break;	
			
				case 0x06:
					venta(lado1.dir);
					CyDelay(50);	
					totales(lado1.dir, lado1.mangueras);
					CyDelay(50);	
					flujo_LCD1=0;					
					ok++;
				break;
					
				case 0x07:	
					totales(lado1.dir, lado1.mangueras);
					CyDelay(50);	
					flujo_LCD1=0;					
					ok++;
				break;
				
				case 0x08:
			     	flujo_LCD1=11;
					ok++;
				break;
					
				case 0x09:
			     	flujo_LCD1=11;
					ok++;
				break;
				
		         case 0x0B:                     //Termino venta
					flujo_LCD1=12;
					ok++;
				 break;	
					
		         case 0x0A:						//Termino venta
					flujo_LCD1=12;
					ok++;
				 break;
					
		         case 0x0C:						//Bloqueado por surtiendo al iniciar
			     	flujo_LCD1=11;
				 	set_imagen(1,8);
					ok++;
				 break;					
					
			}
			estado=get_estado(lado2.dir);										
			switch(estado){
		         case 0:                     
					lado2.dir=0xFF;
				 break;	
			
				case 0x06:
					venta(lado2.dir);
					CyDelay(50);		
					totales(lado2.dir, lado2.mangueras);
					CyDelay(50);	
					flujo_LCD2=0;				
					ok++;
				break;
					
				case 0x07:	
					totales(lado2.dir, lado2.mangueras);
					CyDelay(50);	
					flujo_LCD2=0;					
					ok++;
				break;
				
				case 0x08:
			     	flujo_LCD2=11;
				 	set_imagen(2,8);
					ok++;
				break;
					
				case 0x09:
			     	flujo_LCD2=11;
				 	set_imagen(2,8);
					ok++;
				break;
				
		         case 0x0B:                     //Termino venta
					flujo_LCD2=12;
					ok++;
				 break;	
					
		         case 0x0A:						//Termino venta
					flujo_LCD2=12;
					ok++;
				 break;
					
		         case 0x0C:						//Bloqueado por surtiendo al iniciar
			     	flujo_LCD2=11;
				 	set_imagen(2,8);
					ok++;
				 break;										
			}			
		}*/
	}
}

/*
*********************************************************************************************************
*                                         uint8 polling_rf(void)
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

void polling_rf(void){
	uint16 status1, status2,size,i,x;
	uint8  buffer_rf[1500],aux[15],programar_ok;
	if(PC_GetRxBufferSize()>=6){
		status1=PC_GetRxBufferSize();
		CyDelayUs(3000);
		status2=PC_GetRxBufferSize();
		if(status1==status2){
			size=PC_GetRxBufferSize();
			i=0;
		    while(PC_GetRxBufferSize()>0){
		       buffer_rf[i]=PC_ReadRxData(); 	
			   i++;	
		    }			
			if((buffer_rf[0]==0xBC)&&(buffer_rf[1]==0xBB)&&(buffer_rf[2]==0xB8)){
				if((buffer_rf[3]==lado1.dir)||(buffer_rf[3]==lado2.dir)||(buffer_rf[3]==lado3.dir)||(buffer_rf[3]==lado4.dir)){
					if(verificar_check(buffer_rf,size)==buffer_rf[size-1]){
						state_rf=0;
						rf_mod[0]=0xB8;
						rf_mod[1]=0xAD;
						rf_mod[2]=0xAF;
						rf_mod[3]=buffer_rf[3];
						ok_datosRF=0;
						switch(buffer_rf[4]){
							case cestado:
								if(buffer_rf[3]==lado1.dir){
									rf_mod[4]=0xD1;
									rf_mod[5]=lado1.estado;	
									rf_mod[6]=verificar_check(rf_mod,7);
									ok_datosRF=1;
									size=7;
								}
								else if(buffer_rf[3]==lado2.dir){
									rf_mod[4]=0xD1;
									rf_mod[5]=lado2.estado;	
									rf_mod[6]=verificar_check(rf_mod,7);
									ok_datosRF=1;
									size=7;									
								}
								else if(buffer_rf[3]==lado3.dir){
									rf_mod[4]=0xD1;
									rf_mod[5]=lado3.estado;	
									rf_mod[6]=verificar_check(rf_mod,7);
									ok_datosRF=1;
									size=7;									
								}
								else if(buffer_rf[3]==lado4.dir){
									rf_mod[4]=0xD1;
									rf_mod[5]=lado4.estado;	
									rf_mod[6]=verificar_check(rf_mod,7);
									ok_datosRF=1;
									size=7;									
								}									
							break;	
							
							case cdatospr:
								if(buffer_rf[3]==lado1.dir){
									rf_mod[4]=0xD8;
									rf_mod[5]=rventa1.manguera;	
									rf_mod[6]=rventa1.preset[0];
									for(x=1;x<=7;x++){
										rf_mod[6+x]=rventa1.preset[x];
									}
									rf_mod[14]=0x54;
									for(x=0;x<=11;x++){
										rf_mod[15+x]=((lado1.totales[rventa1.manguera-1][12+x])&0x0f);
									}
									for(x=0;x<=11;x++){
										rf_mod[27+x]=((lado1.totales[rventa1.manguera-1][x])&0x0f);
									}									
									rf_mod[39]=0x50;
									for(x=0;x<=4;x++){
										rf_mod[40+x]=((lado1.ppu[rventa1.manguera-1][x])&0x0f);
									}
									rf_mod[45]=0x48;
									rf_mod[46]=hora[0];
									rf_mod[47]=hora[1];
									rf_mod[48]=0x46;
									rf_mod[49]=fecha[0];
									rf_mod[50]=fecha[1];
									rf_mod[51]=fecha[2];
									rf_mod[52]=verificar_check(rf_mod,53);
									ok_datosRF=1;
									size=53;
								}
								else if(buffer_rf[3]==lado2.dir){
									rf_mod[4]=0xD8;
									rf_mod[5]=rventa2.manguera;	
									rf_mod[6]=rventa2.preset[0];
									for(x=1;x<=7;x++){
										rf_mod[6+x]=rventa2.preset[x];
									}
									rf_mod[14]=0x54;
									for(x=0;x<=11;x++){
										rf_mod[15+x]=((lado2.totales[rventa2.manguera-1][12+x])&0x0f);
									}
									for(x=0;x<=11;x++){
										rf_mod[27+x]=((lado2.totales[rventa2.manguera-1][x])&0x0f);
									}									
									rf_mod[39]=0x50;
									for(x=0;x<=4;x++){
										rf_mod[40+x]=((lado2.ppu[rventa2.manguera-1][x])&0x0f);
									}
									rf_mod[45]=0x48;
									rf_mod[46]=hora[0];
									rf_mod[47]=hora[1];
									rf_mod[48]=0x46;
									rf_mod[49]=fecha[0];
									rf_mod[50]=fecha[1];
									rf_mod[51]=fecha[2];
									rf_mod[52]=verificar_check(rf_mod,53);
									ok_datosRF=1;
									size=53;								
								}
								else if(buffer_rf[3]==lado3.dir){
									rf_mod[4]=0xD8;
									rf_mod[5]=rventa3.manguera;	
									rf_mod[6]=rventa3.preset[0];
									for(x=1;x<=7;x++){
										rf_mod[6+x]=rventa3.preset[x];
									}
									rf_mod[14]=0x54;
									for(x=0;x<=11;x++){
										rf_mod[15+x]=((lado3.totales[rventa3.manguera-1][12+x])&0x0f);
									}
									for(x=0;x<=11;x++){
										rf_mod[27+x]=((lado3.totales[rventa3.manguera-1][x])&0x0f);
									}									
									rf_mod[39]=0x50;
									for(x=0;x<=4;x++){
										rf_mod[40+x]=((lado3.ppu[rventa3.manguera-1][x])&0x0f);
									}
									rf_mod[45]=0x48;
									rf_mod[46]=hora[0];
									rf_mod[47]=hora[1];
									rf_mod[48]=0x46;
									rf_mod[49]=fecha[0];
									rf_mod[50]=fecha[1];
									rf_mod[51]=fecha[2];
									rf_mod[52]=verificar_check(rf_mod,53);
									ok_datosRF=1;
									size=53;								
								}
								else if(buffer_rf[3]==lado4.dir){
									rf_mod[4]=0xD8;
									rf_mod[5]=rventa4.manguera;	
									rf_mod[6]=rventa4.preset[0];
									for(x=1;x<=7;x++){
										rf_mod[6+x]=rventa4.preset[x];
									}
									rf_mod[14]=0x54;
									for(x=0;x<=11;x++){
										rf_mod[15+x]=((lado4.totales[rventa4.manguera-1][12+x])&0x0f);
									}
									for(x=0;x<=11;x++){
										rf_mod[27+x]=((lado4.totales[rventa4.manguera-1][x])&0x0f);
									}									
									rf_mod[39]=0x50;
									for(x=0;x<=4;x++){
										rf_mod[40+x]=((lado4.ppu[rventa4.manguera-1][x])&0x0f);
									}
									rf_mod[45]=0x48;
									rf_mod[46]=hora[0];
									rf_mod[47]=hora[1];
									rf_mod[48]=0x46;
									rf_mod[49]=fecha[0];
									rf_mod[50]=fecha[1];
									rf_mod[51]=fecha[2];
									rf_mod[52]=verificar_check(rf_mod,53);
									ok_datosRF=1;
									size=53;								
								}
							break;

							case cdatosprid:
								if(buffer_rf[3]==lado1.dir){
									rf_mod[4]=0xD9;
									rf_mod[5]=rventa1.manguera;	
									rf_mod[6]=rventa1.preset[0];
									for(x=1;x<=7;x++){
										rf_mod[6+x]=rventa1.preset[x];
									}
									rf_mod[14]=0x54;
									for(x=0;x<=11;x++){
										rf_mod[15+x]=((lado1.totales[rventa1.manguera-1][12+x])&0x0f);
									}
									for(x=0;x<=11;x++){
										rf_mod[27+x]=((lado1.totales[rventa1.manguera-1][x])&0x0f);
									}									
									rf_mod[39]=0x50;
									for(x=0;x<=4;x++){
										rf_mod[40+x]=((lado1.ppu[rventa1.manguera-1][x])&0x0f);
									}
									rf_mod[45]=0x48;
									rf_mod[46]=hora[0];
									rf_mod[47]=hora[1];
									rf_mod[48]=0x46;
									rf_mod[49]=fecha[0];
									rf_mod[50]=fecha[1];
									rf_mod[51]=fecha[2];
									rf_mod[52]=0x49;
									for(x=0;x<=7;x++){
										rf_mod[53+x]=rventa1.id[x];
									}
									rf_mod[61]=0x4B;
									for(x=0;x<=9;x++){
										rf_mod[62+x]=rventa1.km[x+1];
									}									
									rf_mod[72]=verificar_check(rf_mod,73);
									ok_datosRF=1;
									size=73;
								}
								else if(buffer_rf[3]==lado2.dir){
									rf_mod[4]=0xD9;
									rf_mod[5]=rventa2.manguera;	
									rf_mod[6]=rventa2.preset[0];
									for(x=1;x<=7;x++){
										rf_mod[6+x]=rventa2.preset[x];
									}
									rf_mod[14]=0x54;
									for(x=0;x<=11;x++){
										rf_mod[15+x]=((lado2.totales[rventa2.manguera-1][12+x])&0x0f);
									}
									for(x=0;x<=11;x++){
										rf_mod[27+x]=((lado2.totales[rventa2.manguera-1][x])&0x0f);
									}									
									rf_mod[39]=0x50;
									for(x=0;x<=4;x++){
										rf_mod[40+x]=((lado2.ppu[rventa2.manguera-1][x])&0x0f);
									}
									rf_mod[45]=0x48;
									rf_mod[46]=hora[0];
									rf_mod[47]=hora[1];
									rf_mod[48]=0x46;
									rf_mod[49]=fecha[0];
									rf_mod[50]=fecha[1];
									rf_mod[51]=fecha[2];
									rf_mod[52]=0x49;
									for(x=0;x<=7;x++){
										rf_mod[53+x]=rventa2.id[x];
									}
									rf_mod[61]=0x4B;
									for(x=0;x<=9;x++){
										rf_mod[62+x]=rventa2.km[x+1];
									}									
									rf_mod[72]=verificar_check(rf_mod,73);
									ok_datosRF=1;
									size=73;								
								}	
								else if (buffer_rf[3]==lado3.dir){
									rf_mod[4]=0xD9;
									rf_mod[5]=rventa3.manguera;	
									rf_mod[6]=rventa3.preset[0];
									for(x=1;x<=7;x++){
										rf_mod[6+x]=rventa3.preset[x];
									}
									rf_mod[14]=0x54;
									for(x=0;x<=11;x++){
										rf_mod[15+x]=((lado3.totales[rventa3.manguera-1][12+x])&0x0f);
									}
									for(x=0;x<=11;x++){
										rf_mod[27+x]=((lado3.totales[rventa3.manguera-1][x])&0x0f);
									}									
									rf_mod[39]=0x50;
									for(x=0;x<=4;x++){
										rf_mod[40+x]=((lado3.ppu[rventa3.manguera-1][x])&0x0f);
									}
									rf_mod[45]=0x48;
									rf_mod[46]=hora[0];
									rf_mod[47]=hora[1];
									rf_mod[48]=0x46;
									rf_mod[49]=fecha[0];
									rf_mod[50]=fecha[1];
									rf_mod[51]=fecha[2];
									rf_mod[52]=0x49;
									for(x=0;x<=7;x++){
										rf_mod[53+x]=rventa3.id[x];
									}
									rf_mod[61]=0x4B;
									for(x=0;x<=9;x++){
										rf_mod[62+x]=rventa3.km[x+1];
									}									
									rf_mod[72]=verificar_check(rf_mod,73);
									ok_datosRF=1;
									size=73;								
								}
								else if(buffer_rf[3]==lado4.dir){
									rf_mod[4]=0xD9;
									rf_mod[5]=rventa4.manguera;	
									rf_mod[6]=rventa4.preset[0];
									for(x=1;x<=7;x++){
										rf_mod[6+x]=rventa4.preset[x];
									}
									rf_mod[14]=0x54;
									for(x=0;x<=11;x++){
										rf_mod[15+x]=((lado4.totales[rventa4.manguera-1][12+x])&0x0f);
									}
									for(x=0;x<=11;x++){
										rf_mod[27+x]=((lado4.totales[rventa4.manguera-1][x])&0x0f);
									}									
									rf_mod[39]=0x50;
									for(x=0;x<=4;x++){
										rf_mod[40+x]=((lado4.ppu[rventa4.manguera-1][x])&0x0f);
									}
									rf_mod[45]=0x48;
									rf_mod[46]=hora[0];
									rf_mod[47]=hora[1];
									rf_mod[48]=0x46;
									rf_mod[49]=fecha[0];
									rf_mod[50]=fecha[1];
									rf_mod[51]=fecha[2];
									rf_mod[52]=0x49;
									for(x=0;x<=7;x++){
										rf_mod[53+x]=rventa4.id[x];
									}
									rf_mod[61]=0x4B;
									for(x=0;x<=9;x++){
										rf_mod[62+x]=rventa4.km[x+1];
									}									
									rf_mod[72]=verificar_check(rf_mod,73);
									ok_datosRF=1;
									size=73;								
								}
							break;
								
							case cventa:
								if(buffer_rf[3]==lado1.dir){
									rf_mod[4]=0xDA;
									rf_mod[5]=rventa1.producto;	
									rf_mod[6]=0x44;
									if(version[1]==6){
										for(x=0;x<=6;x++){
											rf_mod[7+x]=rventa1.dinero[x] & 0x0f;
										}
									}
									else{
										for(x=1;x<=7;x++){
											rf_mod[6+x]=rventa1.dinero[x] & 0x0f;
										}									
									}
									rf_mod[14]=0x56;
									for(x=0;x<=6;x++){
										rf_mod[15+x]=rventa1.volumen[x] & 0x0f;
									}
									rf_mod[22]=0x54;
									for(x=0;x<=11;x++){
										rf_mod[23+x]=((lado1.totales[rventa1.producto-1][x+12])&0x0f);
									}
									for(x=0;x<=11;x++){
										rf_mod[35+x]=((lado1.totales[rventa1.producto-1][x])&0x0f);
									}									
									rf_mod[47]=0x50;
									for(x=0;x<=5;x++){
										rf_mod[48+x]=((lado1.ppu[rventa1.producto-1][x])&0x0f);
									}									
									rf_mod[53]=0x48;
									rf_mod[54]=hora[0];
									rf_mod[55]=hora[1];
									rf_mod[56]=0x46;
									rf_mod[57]=fecha[0];
									rf_mod[58]=fecha[1];
									rf_mod[59]=fecha[2];
									rf_mod[60]=0x50;
									for(x=1;x<=8;x++){
										rf_mod[60+x]=rventa1.placa[x];
									}
									rf_mod[68]=0x49;
									rf_mod[69]=rventa1.imprimir;
									rf_mod[70]=0x4B;
									for(x=0;x<=9;x++){
										rf_mod[71+x]=rventa1.km[x+1];
									}									
									rf_mod[81]=verificar_check(rf_mod,82);
									ok_datosRF=1;
									size=82;
								}
								else if(buffer_rf[3]==lado2.dir){
									rf_mod[4]=0xDA;
									rf_mod[5]=rventa2.producto;	
									rf_mod[6]=0x44;
									if(version[1]==6){
										for(x=0;x<=6;x++){
											rf_mod[7+x]=rventa2.dinero[x] & 0x0f;
										}
									}
									else{
										for(x=1;x<=7;x++){
											rf_mod[6+x]=rventa2.dinero[x] & 0x0f;
										}									
									}
									rf_mod[14]=0x56;
									for(x=0;x<=6;x++){
										rf_mod[15+x]=rventa2.volumen[x] & 0x0f;
									}
									rf_mod[22]=0x54;
									for(x=0;x<=11;x++){
										rf_mod[23+x]=((lado2.totales[rventa2.producto-1][x+12])&0x0f);
									}
									for(x=0;x<=11;x++){
										rf_mod[35+x]=((lado2.totales[rventa2.producto-1][x])&0x0f);
									}									
									rf_mod[47]=0x50;
									for(x=0;x<=5;x++){
										rf_mod[48+x]=((lado2.ppu[rventa2.producto-1][x])&0x0f);
									}									
									rf_mod[53]=0x48;
									rf_mod[54]=hora[0];
									rf_mod[55]=hora[1];
									rf_mod[56]=0x46;
									rf_mod[57]=fecha[0];
									rf_mod[58]=fecha[1];
									rf_mod[59]=fecha[2];
									rf_mod[60]=0x50;
									for(x=1;x<=8;x++){
										rf_mod[60+x]=rventa2.placa[x];
									}
									rf_mod[68]=0x49;
									rf_mod[69]=rventa2.imprimir;
									rf_mod[70]=0x4B;
									for(x=0;x<=9;x++){
										rf_mod[71+x]=rventa2.km[x+1];
									}									
									rf_mod[81]=verificar_check(rf_mod,82);
									ok_datosRF=1;
									size=82;								
								}
								else if(buffer_rf[3]==lado3.dir){
									rf_mod[4]=0xDA;
									rf_mod[5]=rventa3.producto;	
									rf_mod[6]=0x44;
									if(version[1]==6){
										for(x=0;x<=6;x++){
											rf_mod[7+x]=rventa3.dinero[x] & 0x0f;
										}
									}
									else{
										for(x=1;x<=7;x++){
											rf_mod[6+x]=rventa3.dinero[x] & 0x0f;
										}									
									}
									rf_mod[14]=0x56;
									for(x=0;x<=6;x++){
										rf_mod[15+x]=rventa3.volumen[x] & 0x0f;
									}
									rf_mod[22]=0x54;
									for(x=0;x<=11;x++){
										rf_mod[23+x]=((lado3.totales[rventa3.producto-1][x+12])&0x0f);
									}
									for(x=0;x<=11;x++){
										rf_mod[35+x]=((lado3.totales[rventa3.producto-1][x])&0x0f);
									}									
									rf_mod[47]=0x50;
									for(x=0;x<=5;x++){
										rf_mod[48+x]=((lado3.ppu[rventa3.producto-1][x])&0x0f);
									}									
									rf_mod[53]=0x48;
									rf_mod[54]=hora[0];
									rf_mod[55]=hora[1];
									rf_mod[56]=0x46;
									rf_mod[57]=fecha[0];
									rf_mod[58]=fecha[1];
									rf_mod[59]=fecha[2];
									rf_mod[60]=0x50;
									for(x=1;x<=8;x++){
										rf_mod[60+x]=rventa3.placa[x];
									}
									rf_mod[68]=0x49;
									rf_mod[69]=rventa3.imprimir;
									rf_mod[70]=0x4B;
									for(x=0;x<=9;x++){
										rf_mod[71+x]=rventa3.km[x+1];
									}									
									rf_mod[81]=verificar_check(rf_mod,82);
									ok_datosRF=1;
									size=82;								
								}	
								else if(buffer_rf[3]==lado4.dir){
									rf_mod[4]=0xDA;
									rf_mod[5]=rventa4.producto;	
									rf_mod[6]=0x44;
									if(version[1]==6){
										for(x=0;x<=6;x++){
											rf_mod[7+x]=rventa4.dinero[x] & 0x0f;
										}
									}
									else{
										for(x=1;x<=7;x++){
											rf_mod[6+x]=rventa4.dinero[x] & 0x0f;
										}									
									}
									rf_mod[14]=0x56;
									for(x=0;x<=6;x++){
										rf_mod[15+x]=rventa4.volumen[x] & 0x0f;
									}
									rf_mod[22]=0x54;
									for(x=0;x<=11;x++){
										rf_mod[23+x]=((lado4.totales[rventa4.producto-1][x+12])&0x0f);
									}
									for(x=0;x<=11;x++){
										rf_mod[35+x]=((lado4.totales[rventa4.producto-1][x])&0x0f);
									}									
									rf_mod[47]=0x50;
									for(x=0;x<=5;x++){
										rf_mod[48+x]=((lado4.ppu[rventa4.producto-1][x])&0x0f);
									}									
									rf_mod[53]=0x48;
									rf_mod[54]=hora[0];
									rf_mod[55]=hora[1];
									rf_mod[56]=0x46;
									rf_mod[57]=fecha[0];
									rf_mod[58]=fecha[1];
									rf_mod[59]=fecha[2];
									rf_mod[60]=0x50;
									for(x=1;x<=8;x++){
										rf_mod[60+x]=rventa4.placa[x];
									}
									rf_mod[68]=0x49;
									rf_mod[69]=rventa4.imprimir;
									rf_mod[70]=0x4B;
									for(x=0;x<=9;x++){
										rf_mod[71+x]=rventa4.km[x+1];
									}									
									rf_mod[81]=verificar_check(rf_mod,82);
									ok_datosRF=1;
									size=82;								
								}								
							break;	
								
							case chora:
								hora[0]=buffer_rf[5];
								hora[1]=buffer_rf[6];
								fecha[0]=buffer_rf[7];
								fecha[1]=buffer_rf[8];
								fecha[2]=buffer_rf[9];
								rf_mod[4]=0xD2;
								rf_mod[5]=0x04;					//Nack	
								rf_mod[6]=verificar_check(rf_mod,7);
								if(write_hora()==1){
									if(write_fecha()==1){
										rf_mod[5]=0x03;			//Ack
									}
								}
								ok_datosRF=1;
								size=7;								
							break;
							
							case cdatosestacion:
								x=5;
								i=1;
								while(buffer_rf[x]!='*'){
									nombre[i]=buffer_rf[x];
									x++;
									i++;
								}
								nombre[0]=i-1;
								write_eeprom(0,nombre);
								x++;
								i=1;
								while(buffer_rf[x]!='*'){
									direccion[i]=buffer_rf[x];
									x++;
									i++;
								}
								direccion[0]=i-1;
								write_eeprom(64,direccion);
								x++;
								i=1;
								while(buffer_rf[x]!='*'){
									telefono[i]=buffer_rf[x];
									x++;
									i++;
								}	
								telefono[0]=i-1;
								write_eeprom(47,telefono);
								x++;
								i=1;
								while(buffer_rf[x]!='*'){
									nit[i]=buffer_rf[x];
									x++;
									i++;
								}
								nit[0]=i-1;
								write_eeprom(32,nit);
								x++;
								i=1;
								while(buffer_rf[x]!='*'){
									nombre_pro1[i]=buffer_rf[x];
									x++;
									i++;
								}
								nombre_pro1[0]=i-1;
								write_eeprom(686,nombre_pro1);
								x++;
								i=1;
								while(buffer_rf[x]!='*'){
									nombre_pro2[i]=buffer_rf[x];
									x++;
									i++;
								}
								nombre_pro2[0]=i-1;
								write_eeprom(704,nombre_pro2);
								x++;
								i=1;
								while(buffer_rf[x]!='*'){
									nombre_pro3[i]=buffer_rf[x];
									x++;
									i++;
								}
								nombre_pro3[0]=i-1;
								write_eeprom(721,nombre_pro3);
								x++;
								i=1;
								while(buffer_rf[x]!='*'){
									id_logo[i]=buffer_rf[x];
									x++;
									i++;
								}
								id_logo[0]=1;
								write_eeprom(455,id_logo);
								x++;
								i=1;
								while(buffer_rf[x]!='*'){
									km[i]=buffer_rf[x]-48;
									x++;
									i++;
								}
								km[0]=1;
								write_eeprom(756,km);								
								rf_mod[4]=0xD3;
								rf_mod[5]=0x03;					//ack	
								rf_mod[6]=verificar_check(rf_mod,7);
								ok_datosRF=1;
								size=7;								
							break;
							
							case cdatosimpresion:
							
							break;								
								
							case cdatossurt:
								version[1]=buffer_rf[5]&0x0F;
								version[2]=buffer_rf[7]&0x0F;
								version[3]=buffer_rf[9]&0x0F;
								version[0]=3;
								write_eeprom(602,version);
								if(buffer_rf[3]==lado1.dir){
									lado1.mangueras=buffer_rf[11]&0x0F;
									aux[0]=lado1.mangueras;
									lado1.grado[0][0]=buffer_rf[13]&0x0F;
									aux[1]=lado1.grado[0][0];
									lado1.grado[1][0]=buffer_rf[15]&0x0F;
									aux[2]=lado1.grado[1][0];
									lado1.grado[2][0]=buffer_rf[17]&0x0F;
									aux[3]=lado1.grado[2][0];
									write_eeprom(678,aux);
									i=0;
									if((aux[1]==1)||(aux[2]==1)||(aux[3]==1)){
										i=1;
									}
									if((aux[1]==2)||(aux[2]==2)||(aux[3]==2)){
										i|=2;
									}
									if((aux[1]==3)||(aux[2]==3)||(aux[3]==3)){
										i|=4;
									}
									switch(i){
										case 1:
											imagen_grado1=1;	
										break;
										
										case 2:
											imagen_grado1=1;	
										break;
										
										case 4:
											imagen_grado1=1;	
										break;	
										
										case 5:
											imagen_grado1=124;	
										break;		
										
										case 3:
											imagen_grado1=125;	
										break;
										
										case 6:
											imagen_grado1=127;	
										break;	
										
										case 7:
											imagen_grado1=126;	
										break;		
									}									
								}
								else if(buffer_rf[3]==lado2.dir){
									lado2.mangueras=buffer_rf[11]&0x0F;
									aux[0]=lado2.mangueras;
									lado2.grado[0][0]=buffer_rf[13]&0x0F;
									aux[1]=lado2.grado[0][0];
									lado2.grado[1][0]=buffer_rf[15]&0x0F;
									aux[2]=lado2.grado[1][0];
									lado2.grado[2][0]=buffer_rf[17]&0x0F;
									aux[3]=lado2.grado[2][0];
									write_eeprom(682,aux);
									i=0;
									if((aux[1]==1)||(aux[2]==1)||(aux[3]==1)){
										i=1;
									}
									if((aux[1]==2)||(aux[2]==2)||(aux[3]==2)){
										i|=2;
									}
									if((aux[1]==3)||(aux[2]==3)||(aux[3]==3)){
										i|=4;
									}
									switch(i){
										case 1:
											imagen_grado2=1;	
										break;
										
										case 2:
											imagen_grado2=1;	
										break;
										
										case 4:
											imagen_grado2=1;	
										break;	
										
										case 5:
											imagen_grado2=124;	
										break;		
										
										case 3:
											imagen_grado2=125;	
										break;
										
										case 6:
											imagen_grado2=127;	
										break;	
										
										case 7:
											imagen_grado2=126;	
										break;		
									}									
								}
								else if(buffer_rf[3]==lado3.dir){
									lado3.mangueras=buffer_rf[11]&0x0F;
									aux[0]=lado3.mangueras;
									lado3.grado[0][0]=buffer_rf[13]&0x0F;
									aux[1]=lado3.grado[0][0];
									lado3.grado[1][0]=buffer_rf[15]&0x0F;
									aux[2]=lado3.grado[1][0];
									lado3.grado[2][0]=buffer_rf[17]&0x0F;
									aux[3]=lado3.grado[2][0];
									write_eeprom(758,aux);
									i=0;
									if((aux[1]==1)||(aux[2]==1)||(aux[3]==1)){
										i=1;
									}
									if((aux[1]==2)||(aux[2]==2)||(aux[3]==2)){
										i|=2;
									}
									if((aux[1]==3)||(aux[2]==3)||(aux[3]==3)){
										i|=4;
									}
									switch(i){
										case 1:
											imagen_grado3=1;	
										break;
										
										case 2:
											imagen_grado3=1;	
										break;
										
										case 4:
											imagen_grado3=1;	
										break;	
										
										case 5:
											imagen_grado3=124;	
										break;		
										
										case 3:
											imagen_grado3=125;	
										break;
										
										case 6:
											imagen_grado3=127;	
										break;	
										
										case 7:
											imagen_grado3=126;	
										break;		
									}									
								}
								else if(buffer_rf[3]==lado4.dir){
									lado4.mangueras=buffer_rf[11]&0x0F;
									aux[0]=lado4.mangueras;
									lado4.grado[0][0]=buffer_rf[13]&0x0F;
									aux[1]=lado4.grado[0][0];
									lado4.grado[1][0]=buffer_rf[15]&0x0F;
									aux[2]=lado4.grado[1][0];
									lado4.grado[2][0]=buffer_rf[17]&0x0F;
									aux[3]=lado4.grado[2][0];
									write_eeprom(762,aux);
									i=0;
									if((aux[1]==1)||(aux[2]==1)||(aux[3]==1)){
										i=1;
									}
									if((aux[1]==2)||(aux[2]==2)||(aux[3]==2)){
										i|=2;
									}
									if((aux[1]==3)||(aux[2]==3)||(aux[3]==3)){
										i|=4;
									}
									switch(i){
										case 1:
											imagen_grado4=1;	
										break;
										
										case 2:
											imagen_grado4=1;	
										break;
										
										case 4:
											imagen_grado4=1;	
										break;	
										
										case 5:
											imagen_grado4=124;	
										break;		
										
										case 3:
											imagen_grado4=125;	
										break;
										
										case 6:
											imagen_grado4=127;	
										break;	
										
										case 7:
											imagen_grado4=126;	
										break;		
									}									
								}								
								ppux10=buffer_rf[19]&0x0F;
								aux[0]=1;
								aux[1]=ppux10;
								write_eeprom(444,aux);							
								rf_mod[4]=0xD4;
								rf_mod[5]=0x03;					//ack	
								rf_mod[6]=verificar_check(rf_mod,7);
								ok_datosRF=1;
								size=7;								
							break;
							
							case ctotales:
								rf_mod[4]=0xDC;
								rf_mod[5]=0x31;
								if(buffer_rf[3]==lado1.dir){
									for(x=0;x<=11;x++){
										rf_mod[x+6]=lado1.totales[0][x+12] & 0x0F;
									}
									for(x=0;x<=11;x++){
										rf_mod[x+18]=lado1.totales[0][x] & 0x0F;
									}
									rf_mod[30]=0x32;
									for(x=0;x<=11;x++){
										rf_mod[x+31]=lado1.totales[1][x+12] & 0x0F;
									}
									for(x=0;x<=11;x++){
										rf_mod[x+43]=lado1.totales[1][x] & 0x0F;
									}
									rf_mod[55]=0x33;
									for(x=0;x<=11;x++){
										rf_mod[x+56]=lado1.totales[2][x+12] & 0x0F;
									}
									for(x=0;x<=11;x++){
										rf_mod[x+68]=lado1.totales[2][x] & 0x0F;
									}
								}
								else if(buffer_rf[3]==lado2.dir){
									for(x=0;x<=11;x++){
										rf_mod[x+6]=lado2.totales[0][x+12] & 0x0F;
									}
									for(x=0;x<=11;x++){
										rf_mod[x+18]=lado2.totales[0][x] & 0x0F;
									}
									rf_mod[30]=0x32;
									for(x=0;x<=11;x++){
										rf_mod[x+31]=lado2.totales[1][x+12] & 0x0F;
									}
									for(x=0;x<=11;x++){
										rf_mod[x+43]=lado2.totales[1][x] & 0x0F;
									}
									rf_mod[55]=0x33;
									for(x=0;x<=11;x++){
										rf_mod[x+56]=lado2.totales[2][x+12] & 0x0F;
									}
									for(x=0;x<=11;x++){
										rf_mod[x+68]=lado2.totales[2][x] & 0x0F;
									}								
								}
								else if(buffer_rf[3]==lado3.dir){
									for(x=0;x<=11;x++){
										rf_mod[x+6]=lado3.totales[0][x+12] & 0x0F;
									}
									for(x=0;x<=11;x++){
										rf_mod[x+18]=lado3.totales[0][x] & 0x0F;
									}
									rf_mod[30]=0x32;
									for(x=0;x<=11;x++){
										rf_mod[x+31]=lado3.totales[1][x+12] & 0x0F;
									}
									for(x=0;x<=11;x++){
										rf_mod[x+43]=lado3.totales[1][x] & 0x0F;
									}
									rf_mod[55]=0x33;
									for(x=0;x<=11;x++){
										rf_mod[x+56]=lado3.totales[2][x+12] & 0x0F;
									}
									for(x=0;x<=11;x++){
										rf_mod[x+68]=lado3.totales[2][x] & 0x0F;
									}								
								}
								else if(buffer_rf[3]==lado4.dir){
									for(x=0;x<=11;x++){
										rf_mod[x+6]=lado4.totales[0][x+12] & 0x0F;
									}
									for(x=0;x<=11;x++){
										rf_mod[x+18]=lado4.totales[0][x] & 0x0F;
									}
									rf_mod[30]=0x32;
									for(x=0;x<=11;x++){
										rf_mod[x+31]=lado4.totales[1][x+12] & 0x0F;
									}
									for(x=0;x<=11;x++){
										rf_mod[x+43]=lado4.totales[1][x] & 0x0F;
									}
									rf_mod[55]=0x33;
									for(x=0;x<=11;x++){
										rf_mod[x+56]=lado4.totales[2][x+12] & 0x0F;
									}
									for(x=0;x<=11;x++){
										rf_mod[x+68]=lado4.totales[2][x] & 0x0F;
									}								
								}								
								rf_mod[80]=0x34;
								rf_mod[81]=verificar_check(rf_mod,82);
								ok_datosRF=1;
								size=82;								
							break;
							
							case ccambioprecio:
								aux[0]=buffer_rf[6]-48;
								aux[1]=buffer_rf[7]-48;
								aux[2]=buffer_rf[8]-48;
								aux[3]=buffer_rf[9]-48;
								aux[4]=buffer_rf[10]-48;
								/*aux[0]=0;
								aux[1]=0;
								aux[2]=0;
								aux[3]=0;
								aux[4]=3;*/								
								if(cambiar_precio(buffer_rf[3], aux, buffer_rf[5])==1){
									rf_mod[4]=0xDD;
									rf_mod[5]=0x03;					//ack	
									rf_mod[6]=verificar_check(rf_mod,7);
									ok_datosRF=1;
									size=7;
									
									if(buffer_rf[3]==lado1.dir){
										lado1.ppu[buffer_rf[5]-1][0]=aux[0];
										lado1.ppu[buffer_rf[5]-1][1]=aux[1];
										lado1.ppu[buffer_rf[5]-1][2]=aux[2];
										lado1.ppu[buffer_rf[5]-1][3]=aux[3];
										lado1.ppu[buffer_rf[5]-1][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										switch(buffer_rf[5]){
											case 1:
												write_eeprom(423,aux);
											break;
											case 2:
												write_eeprom(429,aux);
											break;
											case 3:
												write_eeprom(435,aux);
											break;												
										}
									}
									else if(buffer_rf[3]==lado2.dir){
										lado2.ppu[buffer_rf[5]-1][0]=aux[0];
										lado2.ppu[buffer_rf[5]-1][1]=aux[1];
										lado2.ppu[buffer_rf[5]-1][2]=aux[2];
										lado2.ppu[buffer_rf[5]-1][3]=aux[3];
										lado2.ppu[buffer_rf[5]-1][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										switch(buffer_rf[5]){
											case 1:
												write_eeprom(738,aux);
											break;
											case 2:
												write_eeprom(744,aux);
											break;
											case 3:
												write_eeprom(750,aux);
											break;												
										}										
									}
									else if(buffer_rf[3]==lado3.dir){
										lado3.ppu[buffer_rf[5]-1][0]=aux[0];
										lado3.ppu[buffer_rf[5]-1][1]=aux[1];
										lado3.ppu[buffer_rf[5]-1][2]=aux[2];
										lado3.ppu[buffer_rf[5]-1][3]=aux[3];
										lado3.ppu[buffer_rf[5]-1][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										switch(buffer_rf[5]){
											case 1:
												write_eeprom(768,aux);
											break;
											case 2:
												write_eeprom(774,aux);
											break;
											case 3:
												write_eeprom(780,aux);
											break;												
										}										
									}
									else if(buffer_rf[3]==lado4.dir){
										lado4.ppu[buffer_rf[5]-1][0]=aux[0];
										lado4.ppu[buffer_rf[5]-1][1]=aux[1];
										lado4.ppu[buffer_rf[5]-1][2]=aux[2];
										lado4.ppu[buffer_rf[5]-1][3]=aux[3];
										lado4.ppu[buffer_rf[5]-1][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										switch(buffer_rf[5]){
											case 1:
												write_eeprom(786,aux);
											break;
											case 2:
												write_eeprom(792,aux);
											break;
											case 3:
												write_eeprom(798,aux);
											break;												
										}										
									}
								}	
							break;
								
							case csolicitaprecio:
								rf_mod[4]=0xDF;
								rf_mod[5]=buffer_rf[5];	
								if(buffer_rf[3]==lado1.dir){
									rf_mod[6]=lado1.ppu[buffer_rf[5]-1][0]&0x0F;
									rf_mod[7]=lado1.ppu[buffer_rf[5]-1][1]&0x0F;
									rf_mod[8]=lado1.ppu[buffer_rf[5]-1][2]&0x0F;
									rf_mod[9]=lado1.ppu[buffer_rf[5]-1][3]&0x0F;
									rf_mod[10]=lado1.ppu[buffer_rf[5]-1][4]&0x0F;
								}
								else if(buffer_rf[3]==lado2.dir){
									rf_mod[6]=lado2.ppu[buffer_rf[5]-1][0]&0x0F;
									rf_mod[7]=lado2.ppu[buffer_rf[5]-1][1]&0x0F;
									rf_mod[8]=lado2.ppu[buffer_rf[5]-1][2]&0x0F;
									rf_mod[9]=lado2.ppu[buffer_rf[5]-1][3]&0x0F;
									rf_mod[10]=lado2.ppu[buffer_rf[5]-1][4]&0x0F;								
								}
								else if(buffer_rf[3]==lado3.dir){
									rf_mod[6]=lado3.ppu[buffer_rf[5]-1][0]&0x0F;
									rf_mod[7]=lado3.ppu[buffer_rf[5]-1][1]&0x0F;
									rf_mod[8]=lado3.ppu[buffer_rf[5]-1][2]&0x0F;
									rf_mod[9]=lado3.ppu[buffer_rf[5]-1][3]&0x0F;
									rf_mod[10]=lado3.ppu[buffer_rf[5]-1][4]&0x0F;								
								}
								else if(buffer_rf[3]==lado4.dir){
									rf_mod[6]=lado4.ppu[buffer_rf[5]-1][0]&0x0F;
									rf_mod[7]=lado4.ppu[buffer_rf[5]-1][1]&0x0F;
									rf_mod[8]=lado4.ppu[buffer_rf[5]-1][2]&0x0F;
									rf_mod[9]=lado4.ppu[buffer_rf[5]-1][3]&0x0F;
									rf_mod[10]=lado4.ppu[buffer_rf[5]-1][4]&0x0F;								
								}								
								rf_mod[11]=verificar_check(rf_mod,12);
								ok_datosRF=1;
								size=12;
							break;	
							
							case cautorizar:
								if(get_estado(buffer_rf[3])==7){
									CyDelay(10);
									aux[1]=buffer_rf[7];
									aux[2]=buffer_rf[8];
									aux[3]=buffer_rf[9];
									if(buffer_rf[6]==1){
										aux[4]='.';
										aux[5]=buffer_rf[10];
										aux[6]=buffer_rf[11];
										aux[7]=buffer_rf[12];
										aux[8]=buffer_rf[13];
										aux[9]=buffer_rf[14];										
									}
									else{
										aux[4]=buffer_rf[10];
										aux[5]=buffer_rf[11];
										aux[6]=buffer_rf[12];
										aux[7]=buffer_rf[13];
										aux[8]=buffer_rf[14];
									}
									programar_ok=1;
									if(buffer_rf[3]==lado1.dir){
										if(rventa1.tipo_venta==2){
											if(buffer_rf[5]!=rventa1.manguera){
												programar_ok=0;
											}
										}
									}
									else if(buffer_rf[3]==lado2.dir){
										if(rventa2.tipo_venta==2){
											if(buffer_rf[5]!=rventa2.manguera){
												programar_ok=0;
											}
										}									
									}
									else if(buffer_rf[3]==lado3.dir){
										if(rventa3.tipo_venta==2){
											if(buffer_rf[5]!=rventa3.manguera){
												programar_ok=0;
											}
										}									
									}
									else if(buffer_rf[3]==lado4.dir){
										if(rventa4.tipo_venta==2){
											if(buffer_rf[5]!=rventa4.manguera){
												programar_ok=0;
											}
										}									
									}									
									if(programar_ok==1){
										if(programar(buffer_rf[3],buffer_rf[5],aux,buffer_rf[6])==1){
											CyDelay(10);
											Surtidor_PutChar(0x10|buffer_rf[3]);
											if(buffer_rf[3]==lado1.dir){
												lado1.estado=11;
												flujo_LCD1=11;
												//set_imagen(1,8);
												estado_lcd1=0;
												set_imagen(1,46);								//imagen de pos a  pos b												
											}
											else if(buffer_rf[3]==lado2.dir){
												lado2.estado=11;
												flujo_LCD2=11;
												//set_imagen(2,8);
												estado_lcd2=0;
												set_imagen(2,46);								//imagen de pos a  pos b												
											}
											else if(buffer_rf[3]==lado3.dir){
												lado3.estado=11;
												flujo_LCD3=11;
												//set_imagen(2,8);
												estado_lcd1=0;
												set_imagen(1,46);								//imagen de pos a  pos b												
											}
											else if(buffer_rf[3]==lado4.dir){
												lado4.estado=11;
												flujo_LCD4=11;
												//set_imagen(2,8);
												estado_lcd2=0;
												set_imagen(2,46);								//imagen de pos a  pos b												
											}											
											rf_mod[4]=0xDE;
											rf_mod[5]=0x03;					//ack	
											rf_mod[6]=verificar_check(rf_mod,7);
											ok_datosRF=1;
											size=7;											
										}
										else{
											if(buffer_rf[3]==lado1.dir){
												lado1.estado=1;
	                                            error_op(1,85);
											}
											else if(buffer_rf[3]==lado2.dir){
												lado2.estado=1;
	                                            error_op(2,85);
											}   
											else if(buffer_rf[3]==lado3.dir){
												lado3.estado=1;
	                                            error_op(1,85);
											} 
											else if(buffer_rf[3]==lado4.dir){
												lado4.estado=1;
	                                            error_op(2,85);
											} 											
										}
									}
									else{
										rf_mod[4]=0xDE;
										rf_mod[5]=0x04;					//Nack	
										rf_mod[6]=verificar_check(rf_mod,7);
										ok_datosRF=1;
										size=7;									
									}
								}
                                else if((get_estado(buffer_rf[3])==9)){
									rf_mod[4]=0xDE;
									rf_mod[5]=0x03;					//ack	
									rf_mod[6]=verificar_check(rf_mod,7);
									ok_datosRF=1;
									size=7;                                    
                                }
							break;
								
							case cimprimir:
								rf_mod[4]=0xD0;
								rf_mod[5]=0x03;					//ack	
								rf_mod[6]=verificar_check(rf_mod,7);
								ok_datosRF=0;
								size=7;	
							    for(x=0;x<size;x++){
								   PC_PutChar(rf_mod[x]);
							    }								
								if(buffer_rf[3]==lado1.dir){
									if(lado1.estado!=11){										
										flujo_LCD1=0;
							            isr_3_Stop(); 
							            Timer_Animacion_Stop(); 
										estado_lcd1=0;
										if((lado1.estado==25)||(lado1.estado==27)){
											set_imagen(1,46);
											estado_pos[0]=0;
										}
										lado1.estado=1;										
									}									
									print_logo(id_logo[1],0,943,print1[1]);
									write_psoc1(print1[1],0x1B);
									write_psoc1(print1[1],0x40);
									write_psoc1(print1[1],10);
									write_psoc1(print1[1],10);									
					            	for(x=0;x<((buffer_rf[5]|((uint32)buffer_rf[6]<<8)));x++){
					                	write_psoc1(print1[1],buffer_rf[7+x]);
					            	}		
								}
								else if(buffer_rf[3]==lado2.dir){
									if(lado2.estado!=11){										
										flujo_LCD2=0;
							            isr_4_Stop(); 
							            Timer_Animacion2_Stop();
										estado_lcd2=0;	
										if((lado2.estado==25)||(lado2.estado==27)){
											set_imagen(2,46);
											estado_pos[1]=0;
										}
										lado2.estado=1;											
									}									
									print_logo(id_logo[1],0,943,print2[1]);
									write_psoc1(print2[1],0x1B);
									write_psoc1(print2[1],0x40);
									write_psoc1(print2[1],10);
									write_psoc1(print2[1],10);									
					            	for(x=0;x<((buffer_rf[5]|((uint32)buffer_rf[6]<<8)));x++){
					                	write_psoc1(print2[1],buffer_rf[7+x]);
					            	}	
								}
								else if(buffer_rf[3]==lado3.dir){
									if(lado3.estado!=11){										
										flujo_LCD3=0;
							            isr_3_Stop(); 
							            Timer_Animacion_Stop();
										estado_lcd1=0;	
										if((lado3.estado==25)||(lado3.estado==27)){
											set_imagen(1,46);
											estado_pos[2]=0;
										}
										lado3.estado=1;											
									}									
									print_logo(id_logo[1],0,943,print1[1]);
									write_psoc1(print1[1],0x1B);
									write_psoc1(print1[1],0x40);
									write_psoc1(print1[1],10);
									write_psoc1(print1[1],10);									
					            	for(x=0;x<((buffer_rf[5]|((uint32)buffer_rf[6]<<8)));x++){
					                	write_psoc1(print1[1],buffer_rf[7+x]);
					            	}	
								}								
								else if(buffer_rf[3]==lado4.dir){
									if(lado4.estado!=11){									
										flujo_LCD4=0;
							            isr_4_Stop(); 
							            Timer_Animacion2_Stop();
										estado_lcd2=0;
										if((lado4.estado==25)||(lado4.estado==27)){
											set_imagen(2,46);
											estado_pos[3]=0;
										}
										lado4.estado=1;											
									}									
									print_logo(id_logo[1],0,943,print2[1]);
									write_psoc1(print2[1],0x1B);
									write_psoc1(print2[1],0x40);
									write_psoc1(print2[1],10);
									write_psoc1(print2[1],10);									
					            	for(x=0;x<((buffer_rf[5]|((uint32)buffer_rf[6]<<8)));x++){
					                	write_psoc1(print2[1],buffer_rf[7+x]);
					            	}	
								}								
							break;
							
							case cbloquear:
								rf_mod[5]=0x04;
								if(buffer_rf[3]==lado1.dir){
									if(buffer_rf[5]==1){
										if(lado1.estado==1){
											lado1.estado=0;
											//set_imagen(1,0);
											//flujo_LCD1=102;
											estado_pos[0]=1;
											rf_mod[5]=0x03;
										}
									}
									else{
										if(lado1.estado==0){
											lado1.estado=1;
											//flujo_LCD1=0;
											rf_mod[5]=0x03;
											estado_pos[0]=0;
										}
									}
								}
								else if(buffer_rf[3]==lado2.dir){
									if(buffer_rf[5]==1){
										if(lado2.estado==1){
											lado2.estado=0;
											//set_imagen(2,0);
											//flujo_LCD2=102;
											estado_pos[1]=1;
											rf_mod[5]=0x03;
										}
									}
									else{
										if(lado2.estado==0){
											lado2.estado=1;
											//flujo_LCD2=0;
											rf_mod[5]=0x03;
											estado_pos[1]=0;
										}
									}																	
								}
								else if(buffer_rf[3]==lado3.dir){
									if(buffer_rf[5]==1){
										if(lado3.estado==1){
											lado3.estado=0;
											//set_imagen(1,0);
											//flujo_LCD3=102;
											rf_mod[5]=0x03;
											estado_pos[2]=1;
										}
									}
									else{
										if(lado3.estado==0){
											lado3.estado=1;
											//flujo_LCD3=0;
											rf_mod[5]=0x03;
											estado_pos[2]=0;
										}
									}																	
								}
								else if(buffer_rf[3]==lado4.dir){
									if(buffer_rf[5]==1){
										if(lado4.estado==1){
											lado4.estado=0;
											//set_imagen(2,0);
											//flujo_LCD4=102;
											rf_mod[5]=0x03;
											estado_pos[3]=1;
										}
									}
									else{
										if(lado4.estado==0){
											lado4.estado=1;
											//flujo_LCD4=0;
											rf_mod[5]=0x03;
											estado_pos[3]=0;
										}
									}																	
								}								
								rf_mod[4]=0xE0;	
								rf_mod[6]=verificar_check(rf_mod,7);
								ok_datosRF=1;
								size=7;								
							break;	
							
							case cturno:
								rf_mod[5]=0x04;
								if(buffer_rf[3]==lado1.dir){
									if(buffer_rf[5]==1){				//Abrir Turno
										lado1.estado=15;
										set_imagen(1,35);
										flujo_LCD1=101;
										prox_caso[0][0]=2;
										prox_caso[0][1]=36;
						                isr_3_StartEx(animacion); 
						                Timer_Animacion_Start();
										count_protector=0;
										rf_mod[5]=0x03;
										rventa1.turno=1;
									}
									else{								//Cerrar Turno
										lado1.estado=15;
										set_imagen(1,42);
										flujo_LCD1=101;
										prox_caso[0][0]=2;
										prox_caso[0][1]=36;
						                isr_3_StartEx(animacion); 
						                Timer_Animacion_Start();
										count_protector=0;
										rf_mod[5]=0x03;
										rventa1.turno=0;
									}
								}
								else if(buffer_rf[3]==lado2.dir){
									if(buffer_rf[5]==1){
										lado2.estado=15;
										set_imagen(2,35);
										flujo_LCD2=101;
										prox_caso[1][0]=2;
										prox_caso[1][1]=36;
						                isr_4_StartEx(animacion2); 
						                Timer_Animacion2_Start();
										count_protector2=0;
										rf_mod[5]=0x03;
										rventa2.turno=1;
									}
									else{
										lado2.estado=15;
										set_imagen(2,42);
										flujo_LCD2=101;
										prox_caso[1][0]=2;
										prox_caso[1][1]=36;
						                isr_4_StartEx(animacion2); 
						                Timer_Animacion2_Start();
										count_protector2=0;
										rf_mod[5]=0x03;
										rventa2.turno=0;
									}																	
								}
								else if(buffer_rf[3]==lado3.dir){
									if(buffer_rf[5]==1){
										lado3.estado=15;
										set_imagen(1,35);
										flujo_LCD3=101;
										prox_caso[2][0]=2;
										prox_caso[2][1]=36;
						                isr_3_StartEx(animacion); 
						                Timer_Animacion_Start();
										count_protector=0;
										rf_mod[5]=0x03;
										rventa3.turno=1;
									}
									else{
										lado3.estado=15;
										set_imagen(1,42);
										flujo_LCD3=101;
										prox_caso[2][0]=2;
										prox_caso[2][1]=36;
						                isr_3_StartEx(animacion); 
						                Timer_Animacion_Start();
										count_protector=0;
										rf_mod[5]=0x03;
										rventa3.turno=0;
									}																	
								}	
								else if(buffer_rf[3]==lado4.dir){
									if(buffer_rf[5]==1){
										lado4.estado=15;
										set_imagen(2,35);
										flujo_LCD4=101;
										prox_caso[3][0]=2;
										prox_caso[3][1]=36;
						                isr_4_StartEx(animacion2); 
						                Timer_Animacion2_Start();
										count_protector2=0;
										rf_mod[5]=0x03;
										rventa4.turno=1;
									}
									else{
										lado4.estado=15;
										set_imagen(2,42);
										flujo_LCD4=101;
										prox_caso[3][0]=2;
										prox_caso[3][1]=36;
						                isr_4_StartEx(animacion2); 
						                Timer_Animacion2_Start();
										count_protector2=0;
										rf_mod[5]=0x03;
										rventa4.turno=0;
									}																	
								}								
								rf_mod[4]=0xE1;	
								rf_mod[6]=verificar_check(rf_mod,7);
								ok_datosRF=1;
								size=7;									
							break;
								
							case cdatosturno:
								rf_mod[4]=0xDB;	
								if(buffer_rf[3]==lado1.dir){
									rf_mod[5]=rventa1.turno;
									rf_mod[6]=0x43;
									for(x=1;x<=10;x++){
										rf_mod[6+x]=rventa1.cedula[x];
									}
									rf_mod[17]=0x50;
									for(x=1;x<=8;x++){
										rf_mod[17+x]=rventa1.password[x];
									}
								}
								else if(buffer_rf[3]==lado2.dir){
									rf_mod[5]=rventa2.turno;
									rf_mod[6]=0x43;
									for(x=1;x<=10;x++){
										rf_mod[6+x]=rventa2.cedula[x];
									}
									rf_mod[17]=0x50;
									for(x=1;x<=8;x++){
										rf_mod[17+x]=rventa2.password[x];
									}																	
								}
								else if(buffer_rf[3]==lado3.dir){
									rf_mod[5]=rventa3.turno;
									rf_mod[6]=0x43;
									for(x=1;x<=10;x++){
										rf_mod[6+x]=rventa3.cedula[x];
									}
									rf_mod[17]=0x50;
									for(x=1;x<=8;x++){
										rf_mod[17+x]=rventa3.password[x];
									}																	
								}
								else if(buffer_rf[3]==lado4.dir){
									rf_mod[5]=rventa4.turno;
									rf_mod[6]=0x43;
									for(x=1;x<=10;x++){
										rf_mod[6+x]=rventa4.cedula[x];
									}
									rf_mod[17]=0x50;
									for(x=1;x<=8;x++){
										rf_mod[17+x]=rventa4.password[x];
									}																	
								}								
								rf_mod[26]=verificar_check(rf_mod,27);
								ok_datosRF=1;
								size=27;								
							break;
							
							case crturno:
								rf_mod[5]=0x04;
								if(buffer_rf[3]==lado1.dir){
									lado1.estado=1;
									flujo_LCD1=100;
					                isr_3_StartEx(animacion); 
					                Timer_Animacion_Start();
									count_protector=0;
									rf_mod[5]=0x03;									
									switch(buffer_rf[5]){				
										case 1:					//Operacion Correcta apertura
											set_imagen(1,60);
											rventa1.autorizado='1';
											rventa2.autorizado='1';
											rventa3.autorizado='1';
											rventa4.autorizado='1';
										break;
											
										case 2:					//Usuario invalido		
											set_imagen(1,67);
										break;
											
										case 3:					//Islero invalido		
											set_imagen(1,66);
										break;
										
										case 4:					//Error de apertura		
											set_imagen(1,85);
										break;
											
										case 5:					//Error Turno	
											set_imagen(1,65);
										break;	
											
										case 6:					//Operacion Correcta cierre	
											set_imagen(1,60);
											rventa1.autorizado='0';
											rventa2.autorizado='0';	
											rventa3.autorizado='0';
											rventa4.autorizado='0';											
										break;											
									}
								}
								else if(buffer_rf[3]==lado2.dir){
									lado2.estado=1;
									flujo_LCD2=100;
					                isr_4_StartEx(animacion2); 
					                Timer_Animacion2_Start();
									count_protector2=0;
									rf_mod[5]=0x03;									
									switch(buffer_rf[5]){				
										case 1:					//Operacion Correcta
											set_imagen(2,60);
											rventa1.autorizado='1';
											rventa2.autorizado='1';	
											rventa3.autorizado='1';
											rventa4.autorizado='1';												
										break;
											
										case 2:					//Usuario invalido		
											set_imagen(2,67);
										break;
											
										case 3:					//Islero invalido		
											set_imagen(2,66);
										break;
										
										case 4:					//Error de apertura		
											set_imagen(2,85);
										break;
											
										case 5:					//Error Turno	
											set_imagen(2,65);
										break;
											
										case 6:					//Operacion Correcta cierre	
											set_imagen(2,60);
											rventa1.autorizado='0';
											rventa2.autorizado='0';	
											rventa3.autorizado='0';
											rventa4.autorizado='0';												
										break;												
									}																	
								}
								else if(buffer_rf[3]==lado3.dir){
									lado3.estado=1;
									flujo_LCD3=100;
					                isr_3_StartEx(animacion2); 
					                Timer_Animacion_Start();
									count_protector=0;
									rf_mod[5]=0x03;									
									switch(buffer_rf[5]){				
										case 1:					//Operacion Correcta
											set_imagen(1,60);
											rventa1.autorizado='1';
											rventa2.autorizado='1';	
											rventa3.autorizado='1';
											rventa4.autorizado='1';												
										break;
											
										case 2:					//Usuario invalido		
											set_imagen(1,67);
										break;
											
										case 3:					//Islero invalido		
											set_imagen(1,66);
										break;
										
										case 4:					//Error de apertura		
											set_imagen(1,85);
										break;
											
										case 5:					//Error Turno	
											set_imagen(1,65);
										break;
											
										case 6:					//Operacion Correcta cierre	
											set_imagen(1,60);
											rventa1.autorizado='0';
											rventa2.autorizado='0';	
											rventa3.autorizado='0';
											rventa4.autorizado='0';												
										break;												
									}																	
								}
								else if(buffer_rf[3]==lado4.dir){
									lado4.estado=1;
									flujo_LCD4=100;
					                isr_4_StartEx(animacion2); 
					                Timer_Animacion2_Start();
									count_protector2=0;
									rf_mod[5]=0x03;									
									switch(buffer_rf[5]){				
										case 1:					//Operacion Correcta
											set_imagen(2,60);
											rventa1.autorizado='1';
											rventa2.autorizado='1';	
											rventa3.autorizado='1';
											rventa4.autorizado='1';												
										break;
											
										case 2:					//Usuario invalido		
											set_imagen(2,67);
										break;
											
										case 3:					//Islero invalido		
											set_imagen(2,66);
										break;
										
										case 4:					//Error de apertura		
											set_imagen(2,85);
										break;
											
										case 5:					//Error Turno	
											set_imagen(2,65);
										break;
											
										case 6:					//Operacion Correcta cierre	
											set_imagen(2,60);
											rventa1.autorizado='0';
											rventa2.autorizado='0';	
											rventa3.autorizado='0';
											rventa4.autorizado='0';												
										break;												
									}																	
								}									
								rf_mod[4]=0xE2;	
								rf_mod[6]=verificar_check(rf_mod,7);
								ok_datosRF=1;
								size=7;								
							break;
							
							case cplaca:
								rf_mod[5]=0x04;
								if(buffer_rf[3]==lado1.dir){
									lado1.estado=26;
									flujo_LCD1=13;
									rf_mod[5]=0x03;
									count_teclas1=0;
									teclas1=7;
									posx1=4;
									posy1=3;
									sizeletra1=1;					
									set_imagen(1,10);									
								}
								else if(buffer_rf[3]==lado2.dir){
									lado2.estado=26;
									flujo_LCD2=13;
									rf_mod[5]=0x03;
									count_teclas2=0;
									teclas2=7;
									posx2=4;
									posy2=3;
									sizeletra2=1;					
									set_imagen(2,10);									
								}
								else if(buffer_rf[3]==lado3.dir){
									lado3.estado=26;
									flujo_LCD3=13;
									rf_mod[5]=0x03;
									count_teclas2=0;
									teclas1=7;
									posx1=4;
									posy1=3;
									sizeletra1=1;					
									set_imagen(1,10);									
								}
								else if(buffer_rf[3]==lado4.dir){
									lado4.estado=26;
									flujo_LCD4=13;
									rf_mod[5]=0x03;
									count_teclas2=0;
									teclas2=7;
									posx2=4;
									posy2=3;
									sizeletra2=1;					
									set_imagen(2,10);									
								}								
								rf_mod[4]=0xE3;	
								rf_mod[6]=verificar_check(rf_mod,7);
								ok_datosRF=1;
								size=7;	
							break;
								
							case obtenerplaca:
								rf_mod[4]=0xE4;	
								if(buffer_rf[3]==lado1.dir){
									rf_mod[5]=0x50;
									for(x=1;x<8;x++){
										rf_mod[5+x]=rventa1.placa[x];
									}
								}
								else if(buffer_rf[3]==lado2.dir){
									rf_mod[5]=0x50;
									for(x=1;x<8;x++){
										rf_mod[5+x]=rventa2.placa[x];
									}																	
								}
								else if(buffer_rf[3]==lado3.dir){
									rf_mod[5]=0x50;
									for(x=1;x<8;x++){
										rf_mod[5+x]=rventa3.placa[x];
									}																	
								}
								else if(buffer_rf[3]==lado4.dir){
									rf_mod[5]=0x50;
									for(x=1;x<8;x++){
										rf_mod[5+x]=rventa4.placa[x];
									}																	
								}								
								rf_mod[13]=verificar_check(rf_mod,14);
								ok_datosRF=1;
								size=14;								
							break;
							
							case cventauto:
								rf_mod[5]=0x04;
								if((buffer_rf[3]==lado1.dir)&&(lado1.estado==1)&&((flujo_LCD1==0)||(flujo_LCD1==3))&&(estado_pos[0]==0)&&(estado_lcd1==0)){
									if(rventa1.autorizado=='1'){
										flujo_LCD1=6;
										lado1.estado=30;
										set_imagen(1,7);
										rventa1.tipo_venta=2;
										estado_pos[0]=1;
										estado_lcd1=1;										
										rf_mod[5]=0x03;
										for(x=0;x<=10;x++){
											rventa1.km[x]=0;
										}										
									}
									else{
										lado1.estado=28;	
									}
								}
								if((buffer_rf[3]==lado2.dir)&&(lado2.estado==1)&&((flujo_LCD2==0)||(flujo_LCD2==3))&&(estado_pos[1]==0)&&(estado_lcd2==0)){
									if(rventa2.autorizado=='1'){
										flujo_LCD2=6;
										lado2.estado=30;
										set_imagen(2,7);
										rventa2.tipo_venta=2;
										estado_pos[1]=1;
										estado_lcd2=1;										
										rf_mod[5]=0x03;
										for(x=0;x<=10;x++){
											rventa2.km[x]=0;
										}										
									}
									else{
										lado2.estado=28;	
									}
								}
								if((buffer_rf[3]==lado3.dir)&&(lado3.estado==1)&&((flujo_LCD3==0)||(flujo_LCD3==3))&&(estado_pos[2]==0)&&(estado_lcd1==0)){
									if(rventa3.autorizado=='1'){
										flujo_LCD3=6;
										lado3.estado=30;
										set_imagen(1,7);
										rventa3.tipo_venta=2;
										estado_pos[2]=1;
										estado_lcd1=1;										
										rf_mod[5]=0x03;
										for(x=0;x<=10;x++){
											rventa3.km[x]=0;
										}										
									}
									else{
										lado3.estado=28;	
									}
								}								
								if((buffer_rf[3]==lado4.dir)&&(lado4.estado==1)&&((flujo_LCD4==0)||(flujo_LCD4==3))&&(estado_pos[3]==0)&&(estado_lcd2==0)){
									if(rventa4.autorizado=='1'){
										flujo_LCD4=6;
										lado4.estado=30;
										set_imagen(2,7);
										rventa4.tipo_venta=2;
										estado_pos[3]=1;
										estado_lcd2=1;										
										rf_mod[5]=0x03;
										for(x=0;x<=10;x++){
											rventa4.km[x]=0;
										}										
									}
									else{
										lado4.estado=28;	
									}
								}								
								rf_mod[4]=0xE6;	
								rf_mod[6]=verificar_check(rf_mod,7);
								ok_datosRF=1;
								size=7;								
							break;	
								
							case cconfig:
								rf_mod[4]=0xE5;
								if((buffer_rf[5]=='0')||(buffer_rf[5]=='1')){
									version[1]=buffer_rf[6]-48;
									version[0]=1;
									write_eeprom(602,version);
								}								
								if(buffer_rf[3]==lado1.dir){		
									rf_mod[5]=0x03;
									rventa1.autorizado=buffer_rf[5];
                                    if(lado1.estado==28){
									    lado1.estado=1;
                                    }    
									if((buffer_rf[5]=='0')||(buffer_rf[5]=='1')){
										lado1.mangueras=buffer_rf[10]-48;
										aux[0]=lado1.mangueras;
										lado1.grado[0][0]=buffer_rf[12]-48;
										aux[1]=lado1.grado[0][0];
										lado1.grado[1][0]=buffer_rf[18]-48;
										aux[2]=lado1.grado[1][0];
										lado1.grado[2][0]=buffer_rf[24]-48;
										aux[3]=lado1.grado[2][0];
										write_eeprom(678,aux);
										i=0;
										if((aux[1]==1)||(aux[2]==1)||(aux[3]==1)){
											i=1;
										}
										if((aux[1]==2)||(aux[2]==2)||(aux[3]==2)){
											i|=2;
										}
										if((aux[1]==3)||(aux[2]==3)||(aux[3]==3)){
											i|=4;
										}
										switch(i){
											case 1:
												imagen_grado1=1;	
											break;
											
											case 2:
												imagen_grado1=1;	
											break;
											
											case 4:
												imagen_grado1=1;	
											break;
											
											case 5:
												imagen_grado1=124;	
											break;		
											
											case 3:
												imagen_grado1=125;	
											break;
											
											case 6:
												imagen_grado1=127;	
											break;	
											
											case 7:
												imagen_grado1=126;	
											break;		
										}									
									}									
									if(buffer_rf[12]!='0'){
										aux[0]=buffer_rf[13]-48;
										aux[1]=buffer_rf[14]-48;
										aux[2]=buffer_rf[15]-48;
										aux[3]=buffer_rf[16]-48;
										aux[4]=buffer_rf[17]-48;									
										lado1.ppu[0][0]=aux[0];
										lado1.ppu[0][1]=aux[1];
										lado1.ppu[0][2]=aux[2];
										lado1.ppu[0][3]=aux[3];
										lado1.ppu[0][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										write_eeprom(423,aux);
										
									}
									if(buffer_rf[18]!='0'){
										aux[0]=buffer_rf[19]-48;
										aux[1]=buffer_rf[20]-48;
										aux[2]=buffer_rf[21]-48;
										aux[3]=buffer_rf[22]-48;
										aux[4]=buffer_rf[23]-48;									
										lado1.ppu[1][0]=aux[0];
										lado1.ppu[1][1]=aux[1];
										lado1.ppu[1][2]=aux[2];
										lado1.ppu[1][3]=aux[3];
										lado1.ppu[1][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										write_eeprom(429,aux);									
									}
									if(buffer_rf[24]!='0'){
										aux[0]=buffer_rf[25]-48;
										aux[1]=buffer_rf[26]-48;
										aux[2]=buffer_rf[27]-48;
										aux[3]=buffer_rf[28]-48;
										aux[4]=buffer_rf[29]-48;									
										lado1.ppu[2][0]=aux[0];
										lado1.ppu[2][1]=aux[1];
										lado1.ppu[2][2]=aux[2];
										lado1.ppu[2][3]=aux[3];
										lado1.ppu[2][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										write_eeprom(435,aux);									
									}									
								}
								else if(buffer_rf[3]==lado2.dir){
									rf_mod[5]=0x03;
									rventa2.autorizado=buffer_rf[5];
                                    if(lado2.estado==28){
									    lado2.estado=1;
                                    }    
									if((buffer_rf[5]=='0')||(buffer_rf[5]=='1')){
										lado2.mangueras=buffer_rf[10]&0x0F;
										aux[0]=lado2.mangueras;
										lado2.grado[0][0]=buffer_rf[12]&0x0F;
										aux[1]=lado2.grado[0][0];
										lado2.grado[1][0]=buffer_rf[18]&0x0F;
										aux[2]=lado2.grado[1][0];
										lado2.grado[2][0]=buffer_rf[24]&0x0F;
										aux[3]=lado2.grado[2][0];
										write_eeprom(682,aux);
										i=0;
										if((aux[1]==1)||(aux[2]==1)||(aux[3]==1)){
											i=1;
										}
										if((aux[1]==2)||(aux[2]==2)||(aux[3]==2)){
											i|=2;
										}
										if((aux[1]==3)||(aux[2]==3)||(aux[3]==3)){
											i|=4;
										}
										switch(i){

											case 1:
												imagen_grado2=1;	
											break;
											
											case 2:
												imagen_grado2=1;	
											break;
											
											case 4:
												imagen_grado2=1;	
											break;											
											
											case 5:
												imagen_grado2=124;	
											break;		
											
											case 3:
												imagen_grado2=125;	
											break;
											
											case 6:
												imagen_grado2=127;	
											break;	
											
											case 7:
												imagen_grado2=126;	
											break;		
										}										
									}									
									if(buffer_rf[12]!='0'){
										aux[0]=buffer_rf[13]-48;
										aux[1]=buffer_rf[14]-48;
										aux[2]=buffer_rf[15]-48;
										aux[3]=buffer_rf[16]-48;
										aux[4]=buffer_rf[17]-48;									
										lado2.ppu[0][0]=aux[0];
										lado2.ppu[0][1]=aux[1];
										lado2.ppu[0][2]=aux[2];
										lado2.ppu[0][3]=aux[3];
										lado2.ppu[0][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										write_eeprom(738,aux);
									}
									if(buffer_rf[18]!='0'){
										aux[0]=buffer_rf[19]-48;
										aux[1]=buffer_rf[20]-48;
										aux[2]=buffer_rf[21]-48;
										aux[3]=buffer_rf[22]-48;
										aux[4]=buffer_rf[23]-48;									
										lado2.ppu[1][0]=aux[0];
										lado2.ppu[1][1]=aux[1];
										lado2.ppu[1][2]=aux[2];
										lado2.ppu[1][3]=aux[3];
										lado2.ppu[1][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										write_eeprom(744,aux);									
									}
									if(buffer_rf[24]!='0'){
										aux[0]=buffer_rf[25]-48;
										aux[1]=buffer_rf[26]-48;
										aux[2]=buffer_rf[27]-48;
										aux[3]=buffer_rf[28]-48;
										aux[4]=buffer_rf[29]-48;									
										lado2.ppu[2][0]=aux[0];
										lado2.ppu[2][1]=aux[1];
										lado2.ppu[2][2]=aux[2];
										lado2.ppu[2][3]=aux[3];
										lado2.ppu[2][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										write_eeprom(750,aux);									
									}
								}
								else if(buffer_rf[3]==lado3.dir){		
									rf_mod[5]=0x03;
									rventa3.autorizado=buffer_rf[5];
                                    if(lado3.estado==28){
									    lado3.estado=1;
                                    }    
									if((buffer_rf[5]=='0')||(buffer_rf[5]=='1')){
										lado3.mangueras=buffer_rf[10]-48;
										aux[0]=lado3.mangueras;
										lado3.grado[0][0]=buffer_rf[12]-48;
										aux[1]=lado3.grado[0][0];
										lado3.grado[1][0]=buffer_rf[18]-48;
										aux[2]=lado3.grado[1][0];
										lado3.grado[2][0]=buffer_rf[24]-48;
										aux[3]=lado3.grado[2][0];
										write_eeprom(758,aux);
										i=0;
										if((aux[1]==1)||(aux[2]==1)||(aux[3]==1)){
											i=1;
										}
										if((aux[1]==2)||(aux[2]==2)||(aux[3]==2)){
											i|=2;
										}
										if((aux[1]==3)||(aux[2]==3)||(aux[3]==3)){
											i|=4;
										}
										switch(i){
											case 1:
												imagen_grado3=1;	
											break;
											
											case 2:
												imagen_grado3=1;	
											break;
											
											case 4:
												imagen_grado3=1;	
											break;
											
											case 5:
												imagen_grado3=124;	
											break;		
											
											case 3:
												imagen_grado3=125;	
											break;
											
											case 6:
												imagen_grado3=127;	
											break;	
											
											case 7:
												imagen_grado3=126;	
											break;		
										}									
									}									
									if(buffer_rf[12]!='0'){
										aux[0]=buffer_rf[13]-48;
										aux[1]=buffer_rf[14]-48;
										aux[2]=buffer_rf[15]-48;
										aux[3]=buffer_rf[16]-48;
										aux[4]=buffer_rf[17]-48;									
										lado3.ppu[0][0]=aux[0];
										lado3.ppu[0][1]=aux[1];
										lado3.ppu[0][2]=aux[2];
										lado3.ppu[0][3]=aux[3];
										lado3.ppu[0][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										write_eeprom(768,aux);
										
									}
									if(buffer_rf[18]!='0'){
										aux[0]=buffer_rf[19]-48;
										aux[1]=buffer_rf[20]-48;
										aux[2]=buffer_rf[21]-48;
										aux[3]=buffer_rf[22]-48;
										aux[4]=buffer_rf[23]-48;									
										lado3.ppu[1][0]=aux[0];
										lado3.ppu[1][1]=aux[1];
										lado3.ppu[1][2]=aux[2];
										lado3.ppu[1][3]=aux[3];
										lado3.ppu[1][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										write_eeprom(774,aux);									
									}
									if(buffer_rf[24]!='0'){
										aux[0]=buffer_rf[25]-48;
										aux[1]=buffer_rf[26]-48;
										aux[2]=buffer_rf[27]-48;
										aux[3]=buffer_rf[28]-48;
										aux[4]=buffer_rf[29]-48;									
										lado3.ppu[2][0]=aux[0];
										lado3.ppu[2][1]=aux[1];
										lado3.ppu[2][2]=aux[2];
										lado3.ppu[2][3]=aux[3];
										lado3.ppu[2][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										write_eeprom(780,aux);									
									}									
								}
								else if(buffer_rf[3]==lado4.dir){		
									rf_mod[5]=0x03;
									rventa4.autorizado=buffer_rf[5];
                                    if(lado4.estado==28){
									    lado4.estado=1;
                                    }    
									if((buffer_rf[5]=='0')||(buffer_rf[5]=='1')){
										lado4.mangueras=buffer_rf[10]-48;
										aux[0]=lado4.mangueras;
										lado4.grado[0][0]=buffer_rf[12]-48;
										aux[1]=lado4.grado[0][0];
										lado4.grado[1][0]=buffer_rf[18]-48;
										aux[2]=lado4.grado[1][0];
										lado4.grado[2][0]=buffer_rf[24]-48;
										aux[3]=lado4.grado[2][0];
										write_eeprom(762,aux);
										i=0;
										if((aux[1]==1)||(aux[2]==1)||(aux[3]==1)){
											i=1;
										}
										if((aux[1]==2)||(aux[2]==2)||(aux[3]==2)){
											i|=2;
										}
										if((aux[1]==3)||(aux[2]==3)||(aux[3]==3)){
											i|=4;
										}
										switch(i){
											case 1:
												imagen_grado4=1;	
											break;
											
											case 2:
												imagen_grado4=1;	
											break;
											
											case 4:
												imagen_grado4=1;	
											break;
											
											case 5:
												imagen_grado4=124;	
											break;		
											
											case 3:
												imagen_grado4=125;	
											break;
											
											case 6:
												imagen_grado4=127;	
											break;	
											
											case 7:
												imagen_grado4=126;	
											break;		
										}									
									}									
									if(buffer_rf[12]!='0'){
										aux[0]=buffer_rf[13]-48;
										aux[1]=buffer_rf[14]-48;
										aux[2]=buffer_rf[15]-48;
										aux[3]=buffer_rf[16]-48;
										aux[4]=buffer_rf[17]-48;									
										lado4.ppu[0][0]=aux[0];
										lado4.ppu[0][1]=aux[1];
										lado4.ppu[0][2]=aux[2];
										lado4.ppu[0][3]=aux[3];
										lado4.ppu[0][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										write_eeprom(786,aux);
										
									}
									if(buffer_rf[18]!='0'){
										aux[0]=buffer_rf[19]-48;
										aux[1]=buffer_rf[20]-48;
										aux[2]=buffer_rf[21]-48;
										aux[3]=buffer_rf[22]-48;
										aux[4]=buffer_rf[23]-48;									
										lado4.ppu[1][0]=aux[0];
										lado4.ppu[1][1]=aux[1];
										lado4.ppu[1][2]=aux[2];
										lado4.ppu[1][3]=aux[3];
										lado4.ppu[1][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										write_eeprom(792,aux);									
									}
									if(buffer_rf[24]!='0'){
										aux[0]=buffer_rf[25]-48;
										aux[1]=buffer_rf[26]-48;
										aux[2]=buffer_rf[27]-48;
										aux[3]=buffer_rf[28]-48;
										aux[4]=buffer_rf[29]-48;									
										lado4.ppu[2][0]=aux[0];
										lado4.ppu[2][1]=aux[1];
										lado4.ppu[2][2]=aux[2];
										lado4.ppu[2][3]=aux[3];
										lado4.ppu[2][4]=aux[4];
										aux[5]=aux[4];
										aux[4]=aux[3];
										aux[3]=aux[2];
										aux[2]=aux[1];
										aux[1]=aux[0];
										aux[0]=5;
										write_eeprom(798,aux);									
									}									
								}								
								rf_mod[6]=verificar_check(rf_mod,7);
								ok_datosRF=1;
								size=7;								
							break;	
								
							case creset:
								if(buffer_rf[3]==lado1.dir){
									switch (buffer_rf[5]){
										case 1:
											lado1.estado=11;
										break;
										
										case 2:
											if(rventa1.tipo_venta>=1){
												lado1.estado=7;
											}
											else{
												lado1.estado=1;
												flujo_LCD1=0;
												estado_pos[0]=0;
												
											}
										break;
										
										case 3:
											lado1.estado=10;
										break;
										
										case 4:
							                flujo_LCD1=100;
							                //set_imagen(1,43);
							                count_protector=0;              
							                isr_3_StartEx(animacion); 
							                Timer_Animacion_Start();
											if((lado1.estado!=28)&&(lado1.estado!=11)){
												lado1.estado=1;
											}	
										break;	
										
										case 5:
											lado1.estado=17;
										break;
											
										case 7:
											lado1.estado=1;
											flujo_LCD1=0;
											estado_pos[0]=0;
											set_imagen(1,46);
											estado_lcd1=0;											
										break;											
									}
								}
								else if(buffer_rf[3]==lado2.dir){
									switch (buffer_rf[5]){
										case 1:
											lado2.estado=11;
										break;
										
										case 2:
											if(rventa2.tipo_venta>=1){
												lado2.estado=7;
											}
											else{
												lado2.estado=1;
												flujo_LCD2=0;
												estado_pos[1]=0;		
											}
										break;
										
										case 3:
											lado2.estado=10;
										break;
										
										case 4:
							                flujo_LCD2=100;
							                //set_imagen(2,43);
							                count_protector2=0;              
							                isr_4_StartEx(animacion2); 
							                Timer_Animacion2_Start();
											if((lado2.estado!=28)&&(lado2.estado!=11)){
												lado2.estado=1;
											}											
										break;	
										
										case 5:
											lado2.estado=17;
										break;
											
										case 7:
											lado2.estado=1;
											flujo_LCD2=0;
											estado_pos[1]=0;
											set_imagen(2,46);
											estado_lcd2=0;											
										break;											
									}
								}
								else if(buffer_rf[3]==lado3.dir){
									switch (buffer_rf[5]){
										case 1:
											lado3.estado=11;
										break;
										
										case 2:
											if(rventa3.tipo_venta>=1){
												lado3.estado=7;
											}
											else{
												lado3.estado=1;
												flujo_LCD3=0;
												estado_pos[2]=0;		
											}
										break;
										
										case 3:
											lado3.estado=10;
										break;
										
										case 4:
							                flujo_LCD3=100;
							                //set_imagen(1,43);
							                count_protector=0;              
							                isr_3_StartEx(animacion); 
							                Timer_Animacion_Start();
											if((lado3.estado!=28)&&(lado3.estado!=11)){
												lado3.estado=1;
											}											
										break;	
										
										case 5:
											lado3.estado=17;
										break;
											
										case 7:
											lado3.estado=1;
											flujo_LCD3=0;
											estado_pos[2]=0;
											set_imagen(1,46);
											estado_lcd1=0;
										break;											
									}
								}
								else if(buffer_rf[3]==lado4.dir){
									switch (buffer_rf[5]){
										case 1:
											lado4.estado=11;
										break;
										
										case 2:
											if(rventa4.tipo_venta>=1){
												lado4.estado=7;
											}
											else{
												lado4.estado=1;
												flujo_LCD4=0;
												estado_pos[3]=0;		
											}
										break;
										
										case 3:
											lado4.estado=10;
										break;
										
										case 4:
							                flujo_LCD4=100;
							                //set_imagen(2,43);
							                count_protector=0;              
							                isr_4_StartEx(animacion2); 
							                Timer_Animacion2_Start();
											if((lado4.estado!=28)&&(lado4.estado!=11)){
												lado4.estado=1;
											}											
										break;	
										
										case 5:
											lado4.estado=17;
										break;
											
										case 7:
											lado4.estado=1;
											flujo_LCD4=0;
											estado_pos[3]=0;
											set_imagen(2,46);
											estado_lcd2=0;
										break;											
									}
								}								
							break;								
								
						}
						if(ok_datosRF==1){
						    for(x=0;x<size;x++){
							   PC_PutChar(rf_mod[x]);
						    }							
						}
					}	
				}	
			}
			PC_ClearRxBuffer();
			status1=0;
			status2=0;			
		}		
	}	
}

/*
*********************************************************************************************************
*                                         uint8 polling_mod(void)
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

void polling_mod(void){
	if(espera==1){
		espera=0;
		if(estado_pos[0]==0){
        	get_estado(lado1.dir);
			CyDelay(5);
		}
		if(estado_pos[1]==0){
        	get_estado(lado2.dir);
			CyDelay(5);
		}
		if(estado_pos[2]==0){
        	get_estado(lado3.dir);
			CyDelay(5);
		}
		if(estado_pos[3]==0){
        	get_estado(lado4.dir);
			CyDelay(5);
		}		
    }
}

/*
*********************************************************************************************************
*                                         void polling_pos1(void)
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

void polling_pos1(void){ 
    uint8 x,y,ppu[5],estado,aux[10];
	char numero[8];
	double num_decimal;
    switch(flujo_LCD1){   
        case 0:
         /*isr_3_StartEx(animacion); 
         Timer_Animacion_Start();
         count_protector=0;*/
         flujo_LCD1=1;
		 for(x=0;x<=7;x++){
			numero[x]=0;
		 }
		 for(x=0;x<=10;x++){
			rventa1.cedula[x]=0;
		 }	
		 for(x=0;x<=8;x++){
			rventa1.password[x]=0;
		 }
		 estado=get_estado(lado1.dir);										
		 switch(estado){	
          case 0x0B:                     	//Termino venta
			flujo_LCD1=11;
		  break;	
			
          case 0x0A:						//Termino venta
			flujo_LCD1=11;
		  break;										
		 }        
        break;
        
        case 1:
		 if((rventa1.autorizado==100)&&(flujo_LCD2<=1)&&(flujo_LCD3<=1)&&(flujo_LCD4<=1)){
			if(lado1.estado!=12){
				lado1.estado=28;
			}
			rventa1.autorizado=0;			
			PC_Start();
		 }
         //if(LCD_1_GetRxBufferSize()==8){ 
             isr_3_Stop(); 
             Timer_Animacion_Stop();
	         flujo_LCD1=3;
	         set_imagen(1,4);
			 CyDelay(100);
             LCD_1_ClearRxBuffer();				
         //}
        break;
        
        case 2: 
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
				count_protector=0;
                if(count_teclas1<teclas1){									
                    if(LCD_1_rxBuffer[3]<=9){								//Numero de 1-9
                        count_teclas1++;
                        Buffer_LCD1[count_teclas1]=LCD_1_rxBuffer[3];
                        write_LCD(1,(LCD_1_rxBuffer[3]+0x30), posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
                    }
                    if(LCD_1_rxBuffer[3]==0x0A){            				//Comando de 0
						if((id_teclado1==1)&&(count_teclas1==1)&&(Buffer_LCD1[1]==0)){
						}
						else{
	                        count_teclas1++;
	                        Buffer_LCD1[count_teclas1]=0;
							write_LCD(1,0x30, posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
						}
                    }  
                    if(LCD_1_rxBuffer[3]==0x51){            	 			//Comando de Coma
                        if(count_teclas1>=1 && comas1==0){
                            count_teclas1++;
                            Buffer_LCD1[count_teclas1]=id_coma1;
							write_LCD(1,id_coma1, posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
                            comas1=1;
                        }
                    }                    
                }
                if(LCD_1_rxBuffer[3]==0x0B){								//Cancel
                    if(count_teclas1==0){
						switch(id_teclado1){
							case 1:
							set_imagen(1,5);
							flujo_LCD1=4;
							break;
							
							case 2:
								if(rventa1.tipo_venta==1){
		                        	flujo_LCD1=4;	
		                        	set_imagen(1,5);
								}
								else{
									count_teclas1=0;
									teclas1=7;
									posx1=4;
									posy1=3;
									sizeletra1=1;				
									set_imagen(1,10);
									flujo_LCD1=13;
									count_protector=0;
								}
							break;
							
							case 3:
							//set_imagen(1,0);
							flujo_LCD1=0;
							lado1.estado=1;
							estado_lcd1=0;
							set_imagen(1,46);								//imagen de pos a  pos b
							estado_pos[0]=0;
							break;
							
							case 4:
							//set_imagen(1,0);
							flujo_LCD1=0;
							lado1.estado=1;
							estado_lcd1=0;
							set_imagen(1,46);								//imagen de pos a  pos b
							estado_pos[0]=0;							
							break;							
						}
                    }
                    else{
						write_LCD(1,0x20, posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
                        if(Buffer_LCD1[count_teclas1]==id_coma1){
                            comas1=0;
                        }
						Buffer_LCD1[count_teclas1]=0;
                        count_teclas1--;
                    }
                }
                if(LCD_1_rxBuffer[3]==0x0C){								//Enter
					switch(id_teclado1){
						case 1:
							for(x=0;x<=7;x++){
								numero[x]=0;
							}						
							if((count_teclas1>=1)&&(Buffer_LCD1[count_teclas1]!='.')){
								for(x=1;x<=count_teclas1;x++){
									if(Buffer_LCD1[x]!='.'){
										numero[x-1]=Buffer_LCD1[x]+48;
									}
									else{
										numero[x-1]=Buffer_LCD1[x];
									}
								}
								num_decimal=atof(numero);
								if(((rventa1.preset[0]==2)&&(num_decimal>=800))||((rventa1.preset[0]==1)&&(num_decimal<=900)&&(num_decimal>0))){
									for(x=count_teclas1;x>=1;x--){
										rventa1.preset[x]=Buffer_LCD1[(count_teclas1-x)+1];
									}
									flujo_LCD1=5;								
									if(rventa1.tipo_venta==0){
										if((lado1.mangueras!=1)&&(imagen_grado1!=1)){
			                        		set_imagen(1,imagen_grado1);
										}
										else{
											flujo_LCD1=12;
											//set_imagen(1,7);
											rventa1.producto=lado1.grado[0][0];	
										}
									}
									else{
						                flujo_LCD1=6;                         
						                set_imagen(1,7);
									}
								}
							}
						break;
							
						case 2:							
							for(x=count_teclas1;x>=1;x--){
								rventa1.km[x]=Buffer_LCD1[(count_teclas1-x)+1];
							}
							if(rventa1.tipo_venta==1){
	                        	flujo_LCD1=4;	
	                        	set_imagen(1,5);
							}
							else{
								count_teclas1=0;
								teclas1=7;
								posx1=4;
								posy1=3;
								sizeletra1=1;				
								set_imagen(1,10);
								flujo_LCD1=13;
								count_protector=0;
							}
						break;
							
						case 3:
							for(x=count_teclas1;x>=1;x--){
								rventa1.cedula[x]=Buffer_LCD1[(count_teclas1-x)+1]+48;
							}
                        	flujo_LCD1=2;	
                        	set_imagen(1,37);
							count_teclas1=0;
							id_teclado1=4;
							teclas1=10;
							posx1=2;
							posy1=3;
							sizeletra1=1;								
						break;
							
						case 4:
							for(x=count_teclas1;x>=1;x--){
								rventa1.password[x]=Buffer_LCD1[(count_teclas1-x)+1]+48;
							}
                        	flujo_LCD1=102;	
                        	set_imagen(1,57);
							lado1.estado=16;
						break;							
							
					}					
                }
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }
		 if((count_protector>=30)&&(rventa1.tipo_venta==0)&&(id_teclado1==2)){
			count_teclas1=0;
			teclas1=7;
			posx1=4;
			posy1=3;
			sizeletra1=1;				
			set_imagen(1,10);
			flujo_LCD1=13;
			count_protector=0;
		 }		
        break;
        
        case 3:  
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
                    case 0x0D:								//Efectivo
					  for(x=0;x<=10;x++){
					 	  rventa1.km[x]=0;
					  }					
					  if(rventa1.autorizado=='1'){	
	                      flujo_LCD1=4;                                   
	                      set_imagen(1,5);
						  rventa1.tipo_venta=0;
					  }
					  else if(rventa1.autorizado=='0'){
						lado1.estado=28;
						error_op(1,65);   //Error de turno
					  }
					  else if((rventa1.autorizado=='2')||(rventa1.autorizado==0)||(rventa1.autorizado==1)||(rventa1.autorizado==100)){
                      	flujo_LCD1=4;                                
                      	set_imagen(1,5);
					  	rventa1.tipo_venta=0;
						lado1.estado=28;
					  }					
                    break;
                    
                    case 0x0E:								//Credito 
					  for(x=0;x<=10;x++){
					 	  rventa1.km[x]=0;
					  }					 
					  if(rventa1.autorizado=='1'){
	                      flujo_LCD1=16;                                
	                      set_imagen(1,18);
						  rventa1.tipo_venta=1;	
					  }
					  else if (rventa1.autorizado=='0'){
						lado1.estado=28;
						error_op(1,65);
					  }	
					  else if ((rventa1.autorizado=='2')||(rventa1.autorizado==0)||(rventa1.autorizado==1)||(rventa1.autorizado==100)){
						//error_op(1,85);
                        flujo_LCD1 = 100;
                        set_imagen(1,85);
                        count_protector=1;
	                    isr_3_StartEx(animacion);  
	                    Timer_Animacion_Start();
						lado1.estado=28;   //Error de operación
					  }					
                    break;
                    
                    case 0x45:
                      flujo_LCD1=17;                                    
                      set_imagen(1,131);                    //Otras Opciones
                    break; 
					
                    case 0x7E:                         		//Volver a menu                   
					  flujo_LCD1=0; 	
					  estado_pos[0]=0;
					  set_imagen(1,46);
					  estado_lcd1=0; 
                    break; 
					
                }
            }
           // CyDelay(100);
            LCD_1_ClearRxBuffer();
         }       
        break;
        
        case 4:   
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
                    case 0x0F:                               	//$
					  for(x=0;x<=7;x++){
					 	rventa1.preset[x]=0;
					  }					
                      flujo_LCD1=2; 
					  count_teclas1=0;							//Inicia el contador de teclas	
					  id_teclado1=1;
					  id_coma1='.';	
                      teclas1=6; 						//cantidad de teclas q puede digitar
					  rventa1.preset[0]=2;
                      comas1=0;									
                      set_imagen(1,6); 
					  posx1=4;
					  posy1=3;
					  sizeletra1=1;	
					  write_LCD(1,'$', 3, 4, 1);
                    break;
                    
                    case 0x10: 									//G 
					  for(x=0;x<=7;x++){
					 	rventa1.preset[x]=0;
					  }						
                      flujo_LCD1=2; 
					  count_teclas1=0;							//Inicia el contador de teclas	
					  id_teclado1=1;
					  id_coma1='.';	
                      teclas1=version[1]; 
					  rventa1.preset[0]=1;
                      comas1=0;									
                      set_imagen(1,13); 
					  posx1=4;
					  posy1=3;
					  sizeletra1=1;					
					  write_LCD(1,'G', 3, 4, 1);
                    break;
                    
                    case 0x43:  								//Full 
					  for(x=0;x<=7;x++){
					 	rventa1.preset[x]=0;
					  }
                      rventa1.preset[0]=3;					
					  for(x=5;x<=6;x++){
						rventa1.preset[x]=9;		
					  }						
					  if(rventa1.tipo_venta==0){				
						if((lado1.mangueras!=1)&&(imagen_grado1!=1)){
                    		set_imagen(1,imagen_grado1);
							flujo_LCD1=5;
						}
						else{
							flujo_LCD1=12;
							//set_imagen(1,7);
							rventa1.producto=lado1.grado[0][0];	
						}
					  }
					  else{
		                flujo_LCD1=6;                         
		                set_imagen(1,7);						
					  }
                    break;
                    
                    case 0x3B:                                //Cancel                     
                      flujo_LCD1=3;
                      set_imagen(1,4);                  //Vuelve a opciones de tanqueo
                    break;                              //Volver a inicio?
                }
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         } 
        break;
		
		case 5:
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
                    case 0x80:                          //Corriente
					  rventa1.producto=1;	
                      flujo_LCD1=12;                         
                      //set_imagen(1,7); 
                    break;
                    
                    case 0x81:                          //Diesel
					  rventa1.producto=3;
                      flujo_LCD1=12;                         
                      //set_imagen(1,7); 
                    break;
                   
                    case 0x7F:                          //Extra  
					  rventa1.producto=2;
                      flujo_LCD1=12;                         
                      //set_imagen(1,7); 
                    break; 										
                }
                CyDelay(50);
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }		
		break;
		
		case 6:
		 CyDelay(50);
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                if(LCD_1_rxBuffer[3]==0x7E){					//Cancel
					flujo_LCD1=0;
					estado_pos[0]=0;
					set_imagen(1,46);
					estado_lcd1=0;					
                }												
			}
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
			break;
		 }
         x=get_estado(lado1.dir);
         if(x==7){								//Espera a que este en listo el equipo	
			flujo_LCD1=7;
            CyDelay(50);
         }
         else if(x==0){
			CyDelay(50);
			programar(lado1.dir,1,aux,2);
         }
         else if(x==0x0C){
            error_op(1,85);
         }
		 else if(x==0x0B){                    //Termino venta
			flujo_LCD1=11;
         }						
		 else if(x==0x0A){					//Termino venta
			flujo_LCD1=11;
		 }        
        break;
		
		case 7:
		 CyDelay(10);
		 rventa1.manguera=estado_ex(lado1.dir);
		 if(rventa1.manguera!=0){
			 if(rventa1.tipo_venta==0){
				if(rventa1.producto==lado1.grado[rventa1.manguera-1][0]){
					flujo_LCD1=8;
                    CyDelay(10);
				}
				else{
					error_op(1,85);
				}
			 }
			 else if(rventa1.tipo_venta==1){
				flujo_LCD1=8; 
                CyDelay(10);
			 }
			 else if(rventa1.tipo_venta==2){
				flujo_LCD1=102;
				set_imagen(1,57);
				lado1.estado=31; 
			 }			
		 }	
		break;
		
		case 8:
        CyDelay(10);
		if(get_estado(lado1.dir)==7){
			 if(rventa1.tipo_venta==0){
				switch(rventa1.manguera){
					case 1:
						leer_eeprom(423,6);
					break;
					case 2:
						leer_eeprom(429,6);	
					break;
					case 3:
						leer_eeprom(435,6);					
					break;				
				}
				ppu[0]=buffer_i2c[1];
				ppu[1]=buffer_i2c[2];
				ppu[2]=buffer_i2c[3];
				ppu[3]=buffer_i2c[4];
				ppu[4]=buffer_i2c[5];
				if(buffer_i2c[0]==5){                    
					if(cambiar_precio(lado1.dir, ppu, rventa1.manguera)==1){
						flujo_LCD1=9;
					 	leer_hora();
					 	leer_fecha();
					}
                    else{
					    error_op(1,85);					                         
                    }
				}
				else{
					error_op(1,85);
					lado1.estado=28;
				}
			 }
			 else{
				flujo_LCD1=102;   //Flujo venta a credito
				set_imagen(1,57); //Ocupado
				lado1.estado=9;  //Esperando
				leer_hora();
				leer_fecha();				
			 }
		}
		break;
		
		case 9:
         CyDelay(10);
		 if(get_estado(lado1.dir)==7){
			 CyDelay(10);
			 if(programar(lado1.dir,rventa1.manguera,rventa1.preset,rventa1.preset[0])==0){
				error_op(1,85);	
			 }
			 else{
				CyDelay(10);
				Surtidor_PutChar(0x10|lado1.dir);				//Autoriza el surtidor
			    flujo_LCD1=11;
				//set_imagen(1,8);				
				lado1.estado=8;
				estado_lcd1=0;
				set_imagen(1,46);								//imagen de pos a  pos b
			 }
		}
		break;
		
		case 10:
		
		break;		
		
		case 11:
		 CyDelay(50);
		 switch(get_estado(lado1.dir)){
	         case 0x0B:                     //Termino venta
				CyDelay(100);
				for(x=0;x<=8;x++){
					rventa1.dinero[x]=0;
					rventa1.volumen[x]=0;
				}					
				if(venta(lado1.dir)==1){
					flujo_LCD1=15;
				}
				else{
					for(x=0;x<=18;x++){										//imprime no pudo tomar venta
						write_psoc1(3,msn_errorv[x]);
					}
				    write_psoc1(3,10);
					write_psoc1(3,10);
					write_psoc1(3,10);
				}				
			 break;	
				
	         case 0x0A:						//Termino venta
				for(x=0;x<=8;x++){
					rventa1.dinero[x]=0;
					rventa1.volumen[x]=0;
				}				
				if(venta(lado1.dir)==1){
					flujo_LCD1=15;
				}
				else{
					for(x=0;x<=18;x++){										//imprime no pudo tomar venta
						write_psoc1(3,msn_errorv[x]);
					}
				    write_psoc1(3,10);
					write_psoc1(3,10);
					write_psoc1(3,10);
				}				
			 break;

	         case 0x06:                     //No hizo venta
				flujo_LCD1=100;
				leer_hora();
				leer_fecha();
				venta(lado1.dir);
				lado1.estado=12;
		        isr_3_StartEx(animacion); 
		        Timer_Animacion_Start();
		        count_protector=1;				
			 break;				 	
         }		 	
		break;
		
		case 12:
		 if(rventa1.tipo_venta==0){
			if(km[1]==0){
				count_teclas1=0;
				teclas1=7;
				posx1=4;
				posy1=3;
				sizeletra1=1;				
				set_imagen(1,10);
	            flujo_LCD1=13;
			}
			else {
	            set_imagen(1,14);
				flujo_LCD1=2;
				count_teclas1=0;							//Inicia el contador de teclas	
				id_teclado1=2;
				teclas1=10;
				posx1=2;
				posy1=3;
				sizeletra1=1;				
			}
		 }
		 else if(rventa1.tipo_venta==1){
            flujo_LCD1=15;
			rventa1.imprimir=1;
		 }
		 else if(rventa1.tipo_venta==2){
             set_imagen(1,7); 				//la de suba la manija
             flujo_LCD1=6; 				//Va a suba la manija
			 rventa1.imprimir=0;			
		 }
		 for(x=0;x<=8;x++){
			rventa1.placa[x]=0;
		 }
         isr_3_StartEx(animacion); 
         Timer_Animacion_Start();
         count_protector=0;						
		break;
		
		case 13:
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
				count_protector=0;
                if(count_teclas1<teclas1){
                    if(LCD_1_rxBuffer[3]<=9){
                        count_teclas1++;
                        Buffer_LCD1[count_teclas1]=LCD_1_rxBuffer[3]+0x30;
                        write_LCD(1,(LCD_1_rxBuffer[3]+0x30), posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);                       
                    }
                    if(LCD_1_rxBuffer[3]==0x0A){                                        //Comando de 0
                        count_teclas1++;                       
                        Buffer_LCD1[count_teclas1]=0x30;
                        write_LCD(1,0x30, posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);                        
                    }  
                    if(LCD_1_rxBuffer[3]==0x6A){                                        //Comando de -
                        count_teclas1++;                        
                        Buffer_LCD1[count_teclas1]='-';
                        write_LCD(1,'-', posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);                       
                    } 					
                    if(LCD_1_rxBuffer[3]>=0x1B && LCD_1_rxBuffer[3]<=0x42){            //Comando de Letra
                        for(x=0;x<=25;x++){                                            //Compara el dato que llego con un vector que tiene todas las letras     
                            if(LCD_1_rxBuffer[3]==letras[x]){
                                count_teclas1++;                            
                                Buffer_LCD1[count_teclas1]=x+0x41;
                                write_LCD(1,(x+0x41),posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);                            
                            }
                        }
                    }                    
                }
                if(LCD_1_rxBuffer[3]==0x0B){                             //Borrar - Cancelar
                    if(count_teclas1==0){								//Si no tiene nada pasa a pedir impresion
						if(lado1.estado!=26){
	                        rventa1.placa[0]=0;
							set_imagen(1,11);
	                        flujo_LCD1=14;
						}
                    }
                    else{
                        write_LCD(1,0x20, posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);                        
                        count_teclas1--;
                    }
                }
                if(LCD_1_rxBuffer[3]==0x0C){                           //Enter pasa a imprimir
                    if(count_teclas1>=1){
                        rventa1.placa[0]=teclas1;
						for(x=1;x<=count_teclas1;x++){
							rventa1.placa[x]=Buffer_LCD1[x];
						}
						if(lado1.estado!=26){
	                        flujo_LCD1=14;
	                        set_imagen(1,11);
							count_protector=0;
						}
						else{
	                        flujo_LCD1=102;
	                        set_imagen(1,57);
							lado1.estado=27;
						}
                    }
                }
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }
		 
		 if(count_protector>=30){
            flujo_LCD1=100;
		 }		
		break;
		
		case 14:
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
                    case 0x39:                          //Si Imprime cambiar imagen
                      set_imagen(1,7);					//Suba la manija
                      flujo_LCD1=6;
					  rventa1.imprimir=1;
					  /*if(((rventa1.autorizado=='2')||(rventa1.autorizado==0))&&(rventa1.tipo_venta==0)){
						set_imagen(1,55);
						print_logo(id_logo[1],0,943,print1[1]);
						write_psoc1(print1[1],0x1B);
						write_psoc1(print1[1],0x40);
						write_psoc1(print1[1],10);
						write_psoc1(print1[1],10);						
					  	imprimir(lado1.dir,print1[1], lado1.grado[rventa1.manguera-1][0], rventa1.ppu, rventa1.volumen, rventa1.dinero, rventa1.placa,rventa1.km);
						set_imagen(1,12);
					  }*/
                    break; 
                    
                    case 0x38:                          //No Imprime 
                      set_imagen(1,7); 					//Suba la manija
                      flujo_LCD1=6; 
					  rventa1.imprimir=0;
                    break;                     
                }
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }
		
		 if(count_protector>=30){
            flujo_LCD1=100; 
		 }		 
		break;
		
		case 15:
		 if(totales(lado1.dir, lado1.mangueras)>=lado1.mangueras){
			leer_hora();
			leer_fecha();
			count_protector=0;
			if(rventa1.tipo_venta>=1){
				flujo_LCD1=103;
				set_imagen(1,57);
				rventa1.imprimir=1;
			}
			else{
				flujo_LCD1=104;	
			}	
			lado1.estado=12;
			if(((rventa1.autorizado=='2')||(rventa1.autorizado==0)||(rventa1.autorizado==100))&&(rventa1.tipo_venta==0)&&(rventa1.imprimir==1)){
				//set_imagen(1,55);
				print_logo(id_logo[1],0,943,print1[1]);
				write_psoc1(print1[1],0x1B);
				write_psoc1(print1[1],0x40);
				write_psoc1(print1[1],10);
				write_psoc1(print1[1],10);						
			  	imprimir(lado1.dir,print1[1], lado1.grado[rventa1.manguera-1][0], rventa1.ppu, rventa1.volumen, rventa1.dinero, rventa1.placa,rventa1.km);
				//set_imagen(1,12);
			}			
		 }
		 else{
			for(x=0;x<=20;x++){										//ESTACION DE SERVICIO
				write_psoc1(3,msn_errort[x]);
			}
		    write_psoc1(3,10);
			write_psoc1(3,10);
			write_psoc1(3,10);
		 }
		break;
		
		case 16:
         if(touch_present(1)==1){
             if(touch_write(1,0x33)){
	             for(x=0;x<=7;x++){
	                 rventa1.id[x]=touch_read_byte(1);
	             }
				 crc_total=0;
				 for(x=0;x<7;x++){
				 	crc_total=crc_check(crc_total,rventa1.id[x]);
				 }					
				 if(crc_total==rventa1.id[7]){
		             set_imagen(1,19);              //Esperando ID
		             count_protector=0;              
		             isr_3_StartEx(animacion);       //Lectura de ibutton
		             Timer_Animacion_Start(); 
					 flujo_LCD1=101;
					 prox_caso[0][0]=2;
					 prox_caso[0][1]=14;
					 count_teclas1=0;							//Inicia el contador de teclas	
					 id_teclado1=2;
					 teclas1=10;
					 posx1=2;
					 posy1=3;
					 sizeletra1=1;	
				 }
             }
         }
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
                    case 0x3B:
                     flujo_LCD1=11;
                     set_imagen(1,46);  //Cambio a imagen de pos a pos b
                    break; 
                }
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }		
		break;
		
		case 17:
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
                    case 0x46:                          //Turnos 
						if((rventa1.autorizado=='2')||(rventa1.autorizado==0)){
							error_op(1,85);
						}
						else{
						  set_imagen(1,57); 
						  flujo_LCD1=102;
						  lado1.estado=13;
						  count_teclas1=0;
						  id_teclado1=3;
						  teclas1=10;
						  posx1=2;
						  posy1=3;
						  sizeletra1=1;					  
						}
                    break;
                    
                    case 0x55:                          //Configurar modulo
                      flujo_LCD1=18;     
                      teclas1=0; 
					  set_imagen(1,37);	
					  count_teclas1=0;
					  teclas1=10;
					  posx1=3;
					  posy1=3;
					  sizeletra1=1;					
                    break;
					
                    case 0xB5:                          //Copia de Recibo
						if((rventa1.autorizado=='2')||(rventa1.autorizado==0)){
							error_op(1,85);
						}
						else{
							flujo_LCD1=102;
							set_imagen(1,57);
							lado1.estado=25;
						}
                    break;					
                   
                    case 0x3B:                          //Cancel                     
					  flujo_LCD1=3; 	
					  set_imagen(1,4); 
                    break;                    
                }
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }		 	
		break;
		
		case 18:
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                if(count_teclas1<teclas1){
                    if(LCD_1_rxBuffer[3]<=9){
                        rventa1.password[count_teclas1+1]=LCD_1_rxBuffer[3]+0x30;
                        write_LCD(1,'*', posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
                        count_teclas1++;                        
                    }
                    if(LCD_1_rxBuffer[3]==0x0A){            //Comando de 0
                        rventa1.password[count_teclas1+1]=0x30;
                        write_LCD(1,'*', posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
                        count_teclas1++;                        
                    }                    
                }
                if(LCD_1_rxBuffer[3]==0x0B){                //Cancelar      
                    if(teclas1==0){
                        flujo_LCD1=0;
						estado_pos[0]=0;
						set_imagen(1,46);
						estado_lcd1=0;						
                    }
                    else{
                        count_teclas1--;                                        
                        write_LCD(1,0x20, posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
                    }
                }
                if(LCD_1_rxBuffer[3]==0x0C){                //Enter
                    if(count_teclas1>=1){
                        rventa1.password[0]=count_teclas1;
						y=0;
						if(rventa1.password[0]==4){
							for(x=1;x<=4;x++){
								if(rventa1.password[x]==clave_config[x]){
									y++;
								}
							}
							if(y==4){
		                        flujo_LCD1=19;
		                        set_imagen(1,93);								
							}
							else{
		                        flujo_LCD1=0;							
							}
						}
						else{
	                        flujo_LCD1=0;						
						}
                    }
                }
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }		
		break;
		
		case 19:
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
                    case 0x58:								 	 //Configurar Fecha y Hora	                               
                      flujo_LCD1=20;                         
                      teclas1=0;                            	 //Inicia el contador de teclas  
                      set_imagen(1,62); 
					  if(leer_hora()==1){
						Hora_LCD(1);
					  } 
					  if(leer_fecha()==1){
						Fecha_LCD(1);						
					  }					
                    break;                    
                    
                    case 0x5A:  								//Datos de Recibo

                    break;
                  										
                    case 0x96:                         			//Configurar Surtidor                   

                    break;
					
                    case 0x65:                         			//Test                     
						set_imagen(1,55);
						for(x=0;x<=16;x++){
			        		write_psoc1(3,test[x]);			
						}
					    write_psoc1(3,10);
						write_psoc1(3,10);
						write_psoc1(3,10);
						write_psoc1(3,10);
						for(x=0;x<=16;x++){
			        		write_psoc1(4,test[x]);			
						}
					    write_psoc1(4,10);
						write_psoc1(4,10);
						write_psoc1(4,10);
						write_psoc1(4,10);						
						set_imagen(1,107);
						flujo_LCD1=23;
                    break;					
					
                    case 0x7E:									//ir a menu
                      flujo_LCD1=0;     
                    break;					
                }					
            }
            CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }		  
		break;
		
        case 20:   
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
					case 0x3F:								//Hora
						teclas1=0;
						comas1=0;
						flujo_LCD1=21;
						set_imagen(1,64);							
					break;
					
					case 0x3E:								//Fecha
						teclas1=0;
						comas1=0;
						flujo_LCD1=22;
						set_imagen(1,63);					
					break;
						
                    case 0x7E:								//ir a menu	
                      flujo_LCD1=0;     
                    break;						
                }
            }
            CyDelay(100);            
            LCD_1_ClearRxBuffer();
         } 
        break;
		
        case 21:
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
				switch(teclas1){
	                case 0:										//Decenas de la Hora
	                    if(LCD_1_rxBuffer[3]<=1)					
						{
		                    teclas1++;
		                    write_LCDB(1,(LCD_1_rxBuffer[3]+0x30),teclas1);
		                    hora[1]=LCD_1_rxBuffer[3]<<4;						
	                    }
	                    if(LCD_1_rxBuffer[3]==0x0A){            	//Comando de 0
	                        teclas1++;
	                        hora[1]=0;
	                        write_LCDB(1,0x30,teclas1);
	                    }                     
	                break;
						
	                case 1:											//Unidades de la Hora
	                    if(LCD_1_rxBuffer[3]<=9)					
						{
		                    teclas1++;
		                    write_LCDB(1,(LCD_1_rxBuffer[3]+0x30),teclas1);
		                    hora[1]|=LCD_1_rxBuffer[3];						
	                    }
	                    if(LCD_1_rxBuffer[3]==0x0A){            	//Comando de 0
	                        teclas1++;
	                        write_LCDB(1,0x30,teclas1);
	                    }                     
	                break;
						
	                case 2:											//Decenas de la Minutos					
	                    if(LCD_1_rxBuffer[3]<=5)					
						{
							teclas1++;					
							write_LCDB(1,':',teclas1);							
		                    teclas1++;
		                    write_LCDB(1,(LCD_1_rxBuffer[3]+0x30),teclas1);
		                    hora[0]=LCD_1_rxBuffer[3]<<4;						
	                    }
	                    if(LCD_1_rxBuffer[3]==0x0A){            	//Comando de 0
							teclas1++;					
							write_LCDB(1,':',teclas1);							
	                        teclas1++;
	                        write_LCDB(1,0x30,teclas1);
							hora[0]=0;
	                    } 
	                break;
						
	                case 4:											//Unidades de la Minutos					
	                    if(LCD_1_rxBuffer[3]<=9)					
						{
		                    teclas1++;
		                    write_LCDB(1,(LCD_1_rxBuffer[3]+0x30),teclas1);
		                    hora[0]|=LCD_1_rxBuffer[3];						
	                    }
	                    if(LCD_1_rxBuffer[3]==0x0A){            	//Comando de 0
	                        teclas1++;
	                        write_LCDB(1,0x30,teclas1);
	                    } 
	                break;
				}
                if(LCD_1_rxBuffer[3]==0x40){            		//Comando de Am/Pm
                    if(comas1==0){
                        comas1=1;
                        write_LCDB(1,'p',6);
                    }
					else{
                        comas1=0;
                        write_LCDB(1,'a',6);						
					}
					write_LCDB(1,'m',7);
                } 				
                if(LCD_1_rxBuffer[3]==0x0B){					//Cancel
                    if(teclas1==0){								//Si no ha presionado nada regresa al menu anterior						
                        flujo_LCD1=19;
                        set_imagen(1,93);
                    }
                    else{
                        write_LCDB(1,0x20,(teclas1));			//Si ya presiono borra el dato	
                        teclas1--;
                    }
                }
                if(LCD_1_rxBuffer[3]==0x0C){					//Enter
                    if(teclas1==5){
						hora[1]|=((comas1|2)<<5);
						if(write_hora()==1){
	                        flujo_LCD1=19;                         
		                    set_imagen(1,93);                     
						}
                    }
                }

            }
            CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }        
        break;
		
		case 22:
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
				switch(teclas1){
	                case 0:										//Decenas de dia
	                    if(LCD_1_rxBuffer[3]<=3)					
						{
		                    write_LCDB(1,(LCD_1_rxBuffer[3]+0x30),teclas1);
		                    teclas1++;							
		                    fecha[0]=LCD_1_rxBuffer[3]<<4;						
	                    }
	                    if(LCD_1_rxBuffer[3]==0x0A){            	//Comando de 0
	                        fecha[0]=0;
	                        write_LCDB(1,0x30,teclas1);
	                        teclas1++;							
	                    }                     
	                break;
						
	                case 1:											//Unidades de dia
	                    if(LCD_1_rxBuffer[3]<=9)					
						{
		                    write_LCDB(1,(LCD_1_rxBuffer[3]+0x30),teclas1);
		                    fecha[0]|=LCD_1_rxBuffer[3];	
		                    teclas1++;							
	                    }
	                    if(LCD_1_rxBuffer[3]==0x0A){            	//Comando de 0
	                        write_LCDB(1,0x30,teclas1);
	                        teclas1++;							
	                    }                     
	                break;
						
	                case 2:											//Decenas de mes					
	                    if(LCD_1_rxBuffer[3]<=1)					
						{					
							write_LCDB(1,'/',teclas1);
							teclas1++;							
		                    write_LCDB(1,(LCD_1_rxBuffer[3]+0x30),teclas1);
		                    fecha[1]=LCD_1_rxBuffer[3]<<4;
		                    teclas1++;							
	                    }
	                    if(LCD_1_rxBuffer[3]==0x0A){            	//Comando de 0;					
							write_LCDB(1,'/',teclas1);
							teclas1++;							
	                        write_LCDB(1,0x30,teclas1);
							fecha[1]=0;
	                        teclas1++;							
	                    } 
	                break;
						
	                case 4:											//Unidades de mes					
	                    if(LCD_1_rxBuffer[3]<=9)					
						{
		                    write_LCDB(1,(LCD_1_rxBuffer[3]+0x30),teclas1);
		                    fecha[1]|=LCD_1_rxBuffer[3];	
		                    teclas1++;							
	                    }
	                    if(LCD_1_rxBuffer[3]==0x0A){            	//Comando de 0
	                        write_LCDB(1,0x30,teclas1);
	                        teclas1++;							
	                    } 
	                break;
						
	                case 5:											//Decenas de año					
	                    if(LCD_1_rxBuffer[3]<=9)					
						{					
							write_LCDB(1,'/',teclas1);
							teclas1++;							
		                    write_LCDB(1,(LCD_1_rxBuffer[3]+0x30),teclas1);
		                    fecha[2]=LCD_1_rxBuffer[3]<<4;
		                    teclas1++;							
	                    }
	                    if(LCD_1_rxBuffer[3]==0x0A){            	//Comando de 0;					
							write_LCDB(1,'/',teclas1);
							teclas1++;							
	                        write_LCDB(1,0x30,teclas1);
							fecha[2]=0;
	                        teclas1++;							
	                    } 
	                break;
						
	                case 7:											//Unidades de año					
	                    if(LCD_1_rxBuffer[3]<=9)					
						{
		                    write_LCDB(1,(LCD_1_rxBuffer[3]+0x30),teclas1);
		                    fecha[2]|=LCD_1_rxBuffer[3];	
		                    teclas1++;							
	                    }
	                    if(LCD_1_rxBuffer[3]==0x0A){            	//Comando de 0
	                        write_LCDB(1,0x30,teclas1);
	                        teclas1++;							
	                    } 
	                break;						
				}				
                if(LCD_1_rxBuffer[3]==0x0B){					//Cancel
                    if(teclas1==0){								//Si no ha presionado nada regresa al menu anterior						
                        flujo_LCD1=19;
                        set_imagen(1,93);
                    }
                    else{
                        teclas1--;						
                        write_LCDB(1,0x20,(teclas1));			//Si ya presiono borra el dato	
                    }
                }
                if(LCD_1_rxBuffer[3]==0x0C){					//Enter
                    if(teclas1==8){
						if(write_fecha()==1){
	                        flujo_LCD1=19;                         
		                    set_imagen(1,93);                     
						}
                    }
                }

            }
            CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }			
		break;
		
        case 23:  
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){                               
                    case 0x39:								//SI
					  print1[0]=1;
					  print1[1]=3;
					  print2[0]=1;
					  print2[1]=4;					
					  if(write_eeprom(666,print1)==1){ 
						if(write_eeprom(613,print2)==1){
	                    	set_imagen(1,104); 
						  	CyDelay(500);
						  	flujo_LCD1=0;
						}	
					  }
                    break; 

                    case 0x38:								//NO
                      flujo_LCD1=24;     
					  set_imagen(1,103);					
                    break;					
					
                    case 0x7E:								//ir a menu	
                      flujo_LCD1=0;     
                    break;					
                }
            }
            CyDelay(100);
            LCD_1_ClearRxBuffer();
         }       
        break;	
		
        case 24:  
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){                               
                    case 0x8C:								//La impresora 1
					  print1[0]=1;
					  print1[1]=4;
					  print2[0]=1;
					  print2[1]=4;					
					  if(write_eeprom(666,print1)==1){ 
						if(write_eeprom(613,print2)==1){
	                    	set_imagen(1,106); 
						  	CyDelay(500);
						  	flujo_LCD1=0;
						}	
					  }
                    break; 

                    case 0x8D:								//La impresora 2
					  print1[0]=1;
					  print1[1]=3;
					  print2[0]=1;
					  print2[1]=3;					
					  if(write_eeprom(666,print1)==1){ 
						if(write_eeprom(613,print2)==1){
	                    	set_imagen(1,105); 
						  	CyDelay(500);
						  	flujo_LCD1=0;
						}	
					  }					
                    break;
					
                    case 0x8E:								//Ninguna
					  print1[0]=1;
					  print1[1]=0;
					  print2[0]=1;
					  print2[1]=0;					
					  if(write_eeprom(666,print1)==1){ 
						if(write_eeprom(613,print2)==1){
	                    	set_imagen(1,108); 
						  	CyDelay(500);
						  	flujo_LCD1=0;
						}	
					  }				
                    break;					
					
                    case 0x7E:								//ir a menu	
                      flujo_LCD1=0;     
                    break;					
                }
            }
            CyDelay(100);
            LCD_1_ClearRxBuffer();
         }       
        break;		
		
        case 100:  
         if(count_protector>=4){
            flujo_LCD1=0;
            isr_3_Stop(); 
            Timer_Animacion_Stop(); 
            count_protector=0;
			estado_pos[0]=0;					//Pos libre para volver a programar
			if(lado1.estado!=12){
				set_imagen(1,46);
				estado_lcd1=0;
			}			
         }            
        break;
		
        case 101:  
          if(count_protector>=2){              
            isr_3_Stop(); 
            Timer_Animacion_Stop(); 			
            flujo_LCD1=prox_caso[0][0];  //Sale a caso 2
			set_imagen(1,prox_caso[0][1]); //Prox_caso??? //Imagen Kilometraje
			count_protector=0;			
          }            
        break;		
		
        case 102: 
          if((rventa1.autorizado=='2')||(rventa1.autorizado==0)){
			error_op(1,85);
		  }
          if(lado1.estado==1){
            error_op(1,85);
          }        
        break;
		
		case 103:
			count_protector=0;
            set_imagen(1,46);  //Regresa a pantalla de inicio
            flujo_LCD1 = 11;
		break;
		
		case 104:
          if(rventa1.autorizado==0){
			//error_op(1,85);
			estado_lcd1=0;
			lado1.estado=1;
			estado_pos[0]=0;					//Pos libre para volver a programar;
		  }
		  else if(rventa1.autorizado==100){
		  	PC_Start();
			//error_op(1,85);
			estado_lcd1=0;
			lado1.estado=1;
			estado_pos[0]=0;					//Pos libre para volver a programar;			
		  }
		break;		
    }
}

/*
*********************************************************************************************************
*                                         void polling_pos2(void)
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

void polling_pos2(void){ 
    uint8 x,y,ppu[5],estado,aux[10];
	char numero[8];
	double num_decimal;
    switch(flujo_LCD2){   
        case 0:
         /*isr_4_StartEx(animacion); 
         Timer_Animacion2_Start();
         count_protector2=0;*/
         flujo_LCD2=1;
		 for(x=0;x<=7;x++){
			numero[x]=0;
		 }
		 for(x=0;x<=10;x++){
			rventa2.cedula[x]=0;
		 }	
		 for(x=0;x<=8;x++){
			rventa2.password[x]=0;
		 }
		 estado=get_estado(lado2.dir);										
		 switch(estado){	
          case 0x0B:                     	//Termino venta
			flujo_LCD2=11;
		  break;	
			
          case 0x0A:						//Termino venta
			flujo_LCD2=11;
		  break;										
		 }        
        break;
        
        case 1:
		 if((rventa2.autorizado==100)&&(flujo_LCD1<=1)&&(flujo_LCD3<=1)&&(flujo_LCD4<=1)){
			if(lado2.estado!=12){
				lado2.estado=28;
			}
			rventa2.autorizado=0;			
			PC_Start();
		 }
         /*if(LCD_2_GetRxBufferSize()==8){ 
             isr_4_Stop(); 
             Timer_Animacion2_Stop(); */
	         flujo_LCD2=3;
	         set_imagen(2,4);
			 CyDelay(100);
             LCD_2_ClearRxBuffer();				
         //}
        break;
        
        case 2: 
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
				count_protector2=0;
                if(count_teclas2<teclas2){									
                    if(LCD_2_rxBuffer[3]<=9){								//Numero de 1-9
                        count_teclas2++;
                        Buffer_LCD2[count_teclas2]=LCD_2_rxBuffer[3];
                        write_LCD(2,(LCD_2_rxBuffer[3]+0x30), posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
                    }
                    if(LCD_2_rxBuffer[3]==0x0A){            				//Comando de 0
						if((id_teclado2==1)&&(count_teclas2==1)&&(Buffer_LCD2[1]==0)){
						}
						else{
	                        count_teclas2++;
	                        Buffer_LCD2[count_teclas2]=0;
							write_LCD(2,0x30, posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
						}
                    }  
                    if(LCD_2_rxBuffer[3]==0x51){            	 			//Comando de Coma
                        if(count_teclas2>=1 && comas2==0){
                            count_teclas2++;
                            Buffer_LCD2[count_teclas2]=id_coma2;
							write_LCD(2,id_coma2, posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
                            comas2=1;
                        }
                    }                    
                }
                if(LCD_2_rxBuffer[3]==0x0B){								//Cancel
                    if(count_teclas2==0){
						switch(id_teclado2){
							case 1:
							set_imagen(2,5);     
							flujo_LCD2=4;
							break;
							
							case 2:
								if(rventa2.tipo_venta==1){
		                        	flujo_LCD2=4;	   
		                        	set_imagen(2,5);
								}
								else{
									count_teclas2=0;
									teclas2=7;
									posx2=4;
									posy2=3;
									sizeletra2=1;				
									set_imagen(2,10);
									flujo_LCD2=13;
									count_protector2=0;
								}
							break;
							
							case 3:
							//set_imagen(2,0);
							flujo_LCD2=0;
							lado2.estado=1;
							estado_lcd2=0;
							set_imagen(2,46);								//imagen de pos a  pos b
							estado_pos[1]=0;							
							break;
							
							case 4:
							//set_imagen(2,0);
							flujo_LCD2=0;
							lado2.estado=1;
							estado_lcd2=0;
							set_imagen(2,46);								//imagen de pos a  pos b
							estado_pos[1]=0;							
							break;							
						}
                    }
                    else{
						write_LCD(2,0x20, posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
                        if(Buffer_LCD2[count_teclas2]==id_coma2){
                            comas2=0;
                        }
						Buffer_LCD2[count_teclas2]=0;
                        count_teclas2--;
                    }
                }
                if(LCD_2_rxBuffer[3]==0x0C){								//Enter
					switch(id_teclado2){
						case 1:
							for(x=0;x<=7;x++){
								numero[x]=0;
							}						
							if((count_teclas2>=1)&&(Buffer_LCD2[count_teclas2]!='.')){
								for(x=1;x<=count_teclas2;x++){
									if(Buffer_LCD2[x]!='.'){
										numero[x-1]=Buffer_LCD2[x]+48;
									}
									else{
										numero[x-1]=Buffer_LCD2[x];
									}
								}
								num_decimal=atof(numero);
								if(((rventa2.preset[0]==2)&&(num_decimal>=800))||((rventa2.preset[0]==1)&&(num_decimal<=900)&&(num_decimal>0))){
									for(x=count_teclas2;x>=1;x--){
										rventa2.preset[x]=Buffer_LCD2[(count_teclas2-x)+1];
									}
									flujo_LCD2=5;								
									if(rventa2.tipo_venta==0){
										if((lado2.mangueras!=1)&&(imagen_grado2!=1)){
			                        		set_imagen(2,imagen_grado2);
										}
										else{
											flujo_LCD2=12;
											//set_imagen(2,7);
											rventa2.producto=lado2.grado[0][0];	
										}
									}
									else{
						                flujo_LCD2=6;                         
						                set_imagen(2,7);
									}
								}
							}
						break;
							
						case 2:							
							for(x=count_teclas2;x>=1;x--){
								rventa2.km[x]=Buffer_LCD2[(count_teclas2-x)+1];
							}
							if(rventa2.tipo_venta==1){
	                        	flujo_LCD2=4;	
	                        	set_imagen(2,5);
							}
							else{
								count_teclas2=0;
								teclas2=7;
								posx2=4;
								posy2=3;
								sizeletra2=1;				
								set_imagen(2,10);
								flujo_LCD2=13;
								count_protector2=0;
							}
						break;
							
						case 3:
							for(x=count_teclas2;x>=1;x--){
								rventa2.cedula[x]=Buffer_LCD2[(count_teclas2-x)+1]+48;
							}
                        	flujo_LCD2=2;	
                        	set_imagen(2,37);
							count_teclas2=0;
							id_teclado2=4;
							teclas2=10;
							posx2=2;
							posy2=3;
							sizeletra2=1;								
						break;
							
						case 4:
							for(x=count_teclas2;x>=1;x--){
								rventa2.password[x]=Buffer_LCD2[(count_teclas2-x)+1]+48;
							}
                        	flujo_LCD2=102;	
                        	set_imagen(2,57);
							lado2.estado=16;
						break;							
							
					}					
                }
            }
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }
		 if((count_protector2>=30)&&(rventa2.tipo_venta==0)&&(id_teclado2==2)){
			count_teclas2=0;
			teclas2=7;
			posx2=4;
			posy2=3;
			sizeletra2=1;				
			set_imagen(2,10);
			flujo_LCD2=13;
			count_protector2=0;
		 }		
        break;
        
        case 3:  
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
                    case 0x0D:								//Efectivo
					  for(x=0;x<=10;x++){
					 	  rventa2.km[x]=0;
					  }					
					  if(rventa2.autorizado=='1'){	
	                      flujo_LCD2=4;                                   
	                      set_imagen(2,5);
						  rventa2.tipo_venta=0;
					  }
					  else if(rventa2.autorizado=='0'){
						lado2.estado=28;
						error_op(2,65); //Error de turno
					  }
					  else if((rventa2.autorizado=='2')||(rventa2.autorizado==0)||(rventa2.autorizado==1)||(rventa2.autorizado==100)){
                      	flujo_LCD2=4;                                
                      	set_imagen(2,5);
					  	rventa2.tipo_venta=0;
						lado2.estado=28;
					  }					
                    break;
                    
                    case 0x0E:								//Credito 
					  for(x=0;x<=10;x++){
					 	  rventa2.km[x]=0;
					  }					 
					  if(rventa2.autorizado=='1'){
	                      flujo_LCD2=16;                                
	                      set_imagen(2,18);
						  rventa2.tipo_venta=1;	
					  }
					  else if (rventa2.autorizado=='0'){
						lado2.estado=28;
						error_op(2,65);
					  }	
					  else if ((rventa2.autorizado=='2')||(rventa2.autorizado==0)||(rventa2.autorizado==1)||(rventa2.autorizado==100)){
						//error_op(1,85);
                        flujo_LCD2 = 100;
                        set_imagen(2,85);
                        count_protector2=1;
	                    isr_4_StartEx(animacion2);  
	                    Timer_Animacion2_Start();
						lado2.estado=28;
					  }					
                    break;
                    
                    case 0x45:
                      flujo_LCD2=17;                                    
                      set_imagen(2,131);                    //Otras Opciones
                    break; 
					
                    case 0x7E:                         		//Volver a menu                   
					  flujo_LCD2=0; 	
					  estado_pos[1]=0;
					  set_imagen(2,46);
					  estado_lcd2=0;
                    break; 
					
                }
            }
           // CyDelay(100);
            LCD_2_ClearRxBuffer();
         }       
        break;
        
        case 4:   
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
                    case 0x0F:                               	//$
					  for(x=0;x<=7;x++){
					 	rventa2.preset[x]=0;
					  }					
                      flujo_LCD2=2; 
					  count_teclas2=0;							//Inicia el contador de teclas	
					  id_teclado2=1;
					  id_coma2='.';	
                      teclas2=6; 						//cantidad de teclas q puede digitar
					  rventa2.preset[0]=2;
                      comas2=0;									
                      set_imagen(2,6); 
					  posx2=4;
					  posy2=3;
					  sizeletra2=1;	
					  write_LCD(2,'$', 3, 4, 1);
                    break;
                    
                    case 0x10: 									//G 
					  for(x=0;x<=7;x++){
					 	rventa2.preset[x]=0;
					  }						
                      flujo_LCD2=2; 
					  count_teclas2=0;							//Inicia el contador de teclas	
					  id_teclado2=1;
					  id_coma2='.';	
                      teclas2=version[1]; 
					  rventa2.preset[0]=1;
                      comas2=0;									
                      set_imagen(2,13); 
					  posx2=4;
					  posy2=3;
					  sizeletra2=1;					
					  write_LCD(2,'G', 3, 4, 1);
                    break;
                    
                    case 0x43:  								//Full 
					  for(x=0;x<=7;x++){
					 	rventa2.preset[x]=0;
					  }
                      rventa2.preset[0]=3;					
					  for(x=5;x<=6;x++){
						rventa2.preset[x]=9;		
					  }						
					  if(rventa2.tipo_venta==0){				
						if((lado2.mangueras!=1)&&(imagen_grado2!=1)){
                    		set_imagen(2,imagen_grado2);
							flujo_LCD2=5;
						}
						else{
							flujo_LCD2=12;
							//set_imagen(2,7);
							rventa2.producto=lado2.grado[0][0];	
						}
					  }
					  else{
		                flujo_LCD2=6;                         
		                set_imagen(2,7);						
					  }
                    break;
                    
                    case 0x3B:                                //Cancel                     
                      flujo_LCD2=3;                             //Vuelve a opciones de tanqueo
                      set_imagen(2,4);                          //Volver a inicio?
                    break;                    
                }
            }
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
         } 
        break;
		
		case 5:
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
                    case 0x80:                          //Corriente
					  rventa2.producto=1;	
                      flujo_LCD2=12;                         
                      //set_imagen(2,7); 
                    break;
                    
                    case 0x81:                          //Diesel
					  rventa2.producto=3;
                      flujo_LCD2=12;                         
                      //set_imagen(2,7); 
                    break;
                   
                    case 0x7F:                          //Extra  
					  rventa2.producto=2;
                      flujo_LCD2=12;                         
                      //set_imagen(2,7); 
                    break; 										
                }
                CyDelay(50);
            }
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }		
		break;
		
		case 6:
		 CyDelay(50);
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                if(LCD_2_rxBuffer[3]==0x7E){					//Cancel
					flujo_LCD2=0;
					estado_pos[1]=0;
					set_imagen(2,46);
					estado_lcd2=0;					
                }												
			}
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
			break;
		 }
         x=get_estado(lado2.dir);
         if(x==7){								//Espera a que este en listo el equipo	
			flujo_LCD2=7;
            CyDelay(50);
         }
         else if(x==0){
			CyDelay(50);
			programar(lado2.dir,1,aux,2);
         }
         else if(x==0x0C){
            error_op(2,85);
         }
		 else if(x==0x0B){                    //Termino venta
			flujo_LCD2=11;
         }						
		 else if(x==0x0A){					//Termino venta
			flujo_LCD2=11;
		 }        
        break;
		
		case 7:
		 CyDelay(10);
		 rventa2.manguera=estado_ex(lado2.dir);
		 if(rventa2.manguera!=0){
			 if(rventa2.tipo_venta==0){
				if(rventa2.producto==lado2.grado[rventa2.manguera-1][0]){
					flujo_LCD2=8;
                    CyDelay(10);
				}
				else{
					error_op(2,85);
				}
			 }
			 else if(rventa2.tipo_venta==1){
				flujo_LCD2=8; 
                CyDelay(10);
			 }
			 else if(rventa2.tipo_venta==2){
				flujo_LCD2=102;
				set_imagen(2,57);
				lado2.estado=31; 
			 }			
		 }	
		break;
		
		case 8:
        CyDelay(10);
		if(get_estado(lado2.dir)==7){
			 if(rventa2.tipo_venta==0){
				switch(rventa2.manguera){
					case 1:
						leer_eeprom(738,6);
					break;
					case 2:
						leer_eeprom(744,6);	
					break;
					case 3:
						leer_eeprom(750,6);					
					break;				
				}
				ppu[0]=buffer_i2c[1];
				ppu[1]=buffer_i2c[2];
				ppu[2]=buffer_i2c[3];
				ppu[3]=buffer_i2c[4];
				ppu[4]=buffer_i2c[5];
				if(buffer_i2c[0]==5){                    
					if(cambiar_precio(lado2.dir, ppu, rventa2.manguera)==1){
						flujo_LCD2=9;
					 	leer_hora();
					 	leer_fecha();
					}
                    else{
					    error_op(2,85);					                         
                    }
				}
				else{
					error_op(2,85);
					lado2.estado=28;
				}
			 }
			 else{
				flujo_LCD2=102;
				set_imagen(2,57);
				lado2.estado=9;
				leer_hora();
				leer_fecha();				
			 }
		}
		break;
		
		case 9:
         CyDelay(10);
		 if(get_estado(lado2.dir)==7){
			 CyDelay(10);
			 if(programar(lado2.dir,rventa2.manguera,rventa2.preset,rventa2.preset[0])==0){
				error_op(2,85);	
			 }
			 else{
				CyDelay(10);
				Surtidor_PutChar(0x10|lado2.dir);				//Autoriza el surtidor
			    flujo_LCD2=11;
				//set_imagen(2,8);				
				lado2.estado=8;
				estado_lcd2=0;
				set_imagen(2,46);								//imagen de pos a  pos b
			 }
		}
		break;
		
		case 10:
		
		break;		
		
		case 11:
		 CyDelay(50);
		 switch(get_estado(lado2.dir)){
	         case 0x0B:                     //Termino venta
				CyDelay(100);
				for(x=0;x<=8;x++){
					rventa2.dinero[x]=0;
					rventa2.volumen[x]=0;
				}					
				if(venta(lado2.dir)==1){
					flujo_LCD2=15;
				}
				else{
					for(x=0;x<=18;x++){										//imprime no pudo tomar venta
						write_psoc1(3,msn_errorv[x]);
					}
				    write_psoc1(3,10);
					write_psoc1(3,10);
					write_psoc1(3,10);
				}				
			 break;	
				
	         case 0x0A:						//Termino venta
				for(x=0;x<=8;x++){
					rventa2.dinero[x]=0;
					rventa2.volumen[x]=0;
				}				
				if(venta(lado2.dir)==1){
					flujo_LCD2=15;
				}
				else{
					for(x=0;x<=18;x++){										//imprime no pudo tomar venta
						write_psoc1(3,msn_errorv[x]);
					}
				    write_psoc1(3,10);
					write_psoc1(3,10);
					write_psoc1(3,10);
				}				
			 break;

	         case 0x06:                     //No hizo venta
				flujo_LCD2=100;
				leer_hora();
				leer_fecha();
				venta(lado2.dir);
				lado2.estado=12;
		        isr_4_StartEx(animacion2); 
		        Timer_Animacion2_Start();
		        count_protector2=1;				
			 break;				 	
         }		 	
		break;
		
		case 12:
		 if(rventa2.tipo_venta==0){
			if(km[1]==0){
				count_teclas2=0;
				teclas2=7;
				posx2=4;
				posy2=3;
				sizeletra2=1;				
				set_imagen(2,10);
	            flujo_LCD2=13;
			}
			else {
	            set_imagen(2,14);
				flujo_LCD2=2;
				count_teclas2=0;							//Inicia el contador de teclas	
				id_teclado2=2;
				teclas2=10;
				posx2=2;
				posy2=3;
				sizeletra2=1;				
			}
		 }
		 else if(rventa2.tipo_venta==1){
            flujo_LCD2=15;
			rventa2.imprimir=1;
		 }
		 else if(rventa2.tipo_venta==2){
             set_imagen(2,7); 				//la de suba la manija
             flujo_LCD2=6; 				//Va a suba la manija
			 rventa2.imprimir=0;			
		 }
		 for(x=0;x<=8;x++){
			rventa2.placa[x]=0;
		 }
         isr_4_StartEx(animacion2); 
         Timer_Animacion2_Start();
         count_protector2=0;						
		break;
		
		case 13:
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
				count_protector2=0;
                if(count_teclas2<teclas2){
                    if(LCD_2_rxBuffer[3]<=9){
                        count_teclas2++;
                        Buffer_LCD2[count_teclas2]=LCD_2_rxBuffer[3]+0x30;
                        write_LCD(2,(LCD_2_rxBuffer[3]+0x30), posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);                       
                    }
                    if(LCD_2_rxBuffer[3]==0x0A){                                        //Comando de 0
                        count_teclas2++;                       
                        Buffer_LCD2[count_teclas2]=0x30;
                        write_LCD(2,0x30, posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);                        
                    }  
                    if(LCD_2_rxBuffer[3]==0x6A){                                        //Comando de -
                        count_teclas2++;                        
                        Buffer_LCD2[count_teclas2]='-';
                        write_LCD(2,'-', posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);                       
                    } 					
                    if(LCD_2_rxBuffer[3]>=0x1B && LCD_2_rxBuffer[3]<=0x42){            //Comando de Letra
                        for(x=0;x<=25;x++){                                            //Compara el dato que llego con un vector que tiene todas las letras     
                            if(LCD_2_rxBuffer[3]==letras[x]){
                                count_teclas2++;                            
                                Buffer_LCD2[count_teclas2]=x+0x41;
                                write_LCD(2,(x+0x41),posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);                            
                            }
                        }
                    }                    
                }
                if(LCD_2_rxBuffer[3]==0x0B){                             //Borrar - Cancelar
                    if(count_teclas2==0){								//Si no tiene nada pasa a pedir impresion
						if(lado2.estado!=26){
	                        rventa2.placa[0]=0;
							set_imagen(2,11);
	                        flujo_LCD2=14;
						}
                    }
                    else{
                        write_LCD(2,0x20, posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);                        
                        count_teclas2--;
                    }
                }
                if(LCD_2_rxBuffer[3]==0x0C){                           //Enter pasa a imprimir
                    if(count_teclas2>=1){
                        rventa2.placa[0]=teclas2;
						for(x=1;x<=count_teclas2;x++){
							rventa2.placa[x]=Buffer_LCD2[x];
						}
						if(lado2.estado!=26){
	                        flujo_LCD2=14;
	                        set_imagen(2,11);
							count_protector2=0;
						}
						else{
	                        flujo_LCD2=102;
	                        set_imagen(2,57);
							lado2.estado=27;
						}
                    }
                }
            }
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }
		 
		 if(count_protector2>=30){
            flujo_LCD2=100;
		 }		
		break;
		
		case 14:
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
                    case 0x39:                          //Si Imprime cambiar imagen
                      set_imagen(2,7);					//Suba la manija
                      flujo_LCD2=6;
					  rventa2.imprimir=1;
					  /*if(((rventa2.autorizado=='2')||(rventa2.autorizado==0))&&(rventa2.tipo_venta==0)){
						set_imagen(2,55);
						print_logo(id_logo[1],0,943,print1[1]);
						write_psoc1(print1[1],0x1B);
						write_psoc1(print1[1],0x40);
						write_psoc1(print1[1],10);
						write_psoc1(print1[1],10);						
					  	imprimir(lado2.dir,print1[1], lado2.grado[rventa2.manguera-1][0], rventa2.ppu, rventa2.volumen, rventa2.dinero, rventa2.placa,rventa2.km);
						set_imagen(2,12);
					  }*/
                    break; 
                    
                    case 0x38:                          //No Imprime 
                      set_imagen(2,7); 					//Suba la manija
                      flujo_LCD2=6; 
					  rventa2.imprimir=0;
                    break;                     
                }
            }
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }
		
		 if(count_protector2>=30){
            flujo_LCD2=100; 
		 }		 
		break;
		
		case 15:
		 if(totales(lado2.dir, lado2.mangueras)>=lado2.mangueras){
			leer_hora();
			leer_fecha();
			count_protector2=0;
			if(rventa2.tipo_venta>=1){
				flujo_LCD2=103;
				set_imagen(2,57);
				rventa2.imprimir=1;
			}
			else{
				flujo_LCD2=104;	
			}	
			lado2.estado=12;
			if(((rventa2.autorizado=='2')||(rventa2.autorizado==0)||(rventa2.autorizado==100))&&(rventa2.tipo_venta==0)&&(rventa2.imprimir==1)){
				//set_imagen(2,55);
				print_logo(id_logo[1],0,943,print2[1]);
				write_psoc1(print2[1],0x1B);
				write_psoc1(print2[1],0x40);
				write_psoc1(print2[1],10);
				write_psoc1(print2[1],10);						
			  	imprimir(lado2.dir,print2[1], lado2.grado[rventa2.manguera-1][0], rventa2.ppu, rventa2.volumen, rventa2.dinero, rventa2.placa,rventa2.km);
				//set_imagen(2,12);
			}			
		 }
		 else{
			for(x=0;x<=20;x++){										
				write_psoc1(3,msn_errort[x]);
			}
		    write_psoc1(3,10);
			write_psoc1(3,10);
			write_psoc1(3,10);
		 }		
		break;
		
		case 16:
         if(touch_present(2)==1){
             if(touch_write(2,0x33)){
	             for(x=0;x<=7;x++){
	                 rventa2.id[x]=touch_read_byte(2);
	             }
				 crc_total=0;
				 for(x=0;x<7;x++){
				 	crc_total=crc_check(crc_total,rventa2.id[x]);
				 }					
				 if(crc_total==rventa2.id[7]){
		             set_imagen(2,19);
		             count_protector2=0;              
		             isr_4_StartEx(animacion2); 
		             Timer_Animacion2_Start(); 
					 flujo_LCD2=101;
					 prox_caso[1][0]=2;
					 prox_caso[1][1]=14;
					 count_teclas2=0;							//Inicia el contador de teclas	
					 id_teclado2=2;
					 teclas2=10;
					 posx2=2;
					 posy2=3;
					 sizeletra2=1;	
				 }
             }
         }
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
                    case 0x3B:
                     flujo_LCD2=11;
                     set_imagen(2,46);  //Cambio a imagen de pos a pos b
                    break; 
                }
            }
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }		
		break;
		
		case 17:
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
                    case 0x46:                          //Turnos 
						if((rventa2.autorizado=='2')||(rventa2.autorizado==0)){
							error_op(2,85);
						}
						else{
						  set_imagen(2,57); 
						  flujo_LCD2=102;
						  lado2.estado=13;
						  count_teclas2=0;
						  id_teclado2=3;
						  teclas2=10;
						  posx2=2;
						  posy2=3;
						  sizeletra2=1;					  
						}
                    break;
                    
                    case 0x55:                          //Configurar modulo
                      flujo_LCD2=18;     
                      teclas2=0; 
					  set_imagen(2,37);	
					  count_teclas2=0;
					  teclas2=10;
					  posx2=3;
					  posy2=3;
					  sizeletra2=1;					
                    break;
					
                    case 0xB5:                          //Copia de Recibo
						if((rventa2.autorizado=='2')||(rventa2.autorizado==0)){
							error_op(2,85);
						}
						else{
							flujo_LCD2=102;
							set_imagen(2,57);
							lado2.estado=25;
						}
                    break;					
                   
                    case 0x3B:                          //Cancel                     
					  flujo_LCD2=3; 	
					  set_imagen(2,4); 
                    break;                    
                }
            }
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }		 	
		break;
		
		case 18:
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                if(count_teclas2<teclas2){
                    if(LCD_2_rxBuffer[3]<=9){
                        rventa2.password[count_teclas2+1]=LCD_2_rxBuffer[3]+0x30;
                        write_LCD(2,'*', posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
                        count_teclas2++;                        
                    }
                    if(LCD_2_rxBuffer[3]==0x0A){            //Comando de 0
                        rventa2.password[count_teclas2+1]=0x30;
                        write_LCD(2,'*', posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
                        count_teclas2++;                        
                    }                    
                }
                if(LCD_2_rxBuffer[3]==0x0B){                //Cancelar      
                    if(teclas2==0){
                        flujo_LCD2=0;
						estado_pos[1]=0;
						set_imagen(2,46);
						estado_lcd2=0;						
                    }
                    else{
                        count_teclas2--;                                        
                        write_LCD(2,0x20, posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
                    }
                }
                if(LCD_2_rxBuffer[3]==0x0C){                //Enter
                    if(count_teclas2>=1){
                        rventa2.password[0]=count_teclas2;
						y=0;
						if(rventa2.password[0]==4){
							for(x=1;x<=4;x++){
								if(rventa2.password[x]==clave_config[x]){
									y++;
								}
							}
							if(y==4){
		                        flujo_LCD2=19;
		                        set_imagen(2,93);								
							}
							else{
		                        flujo_LCD2=0;							
							}
						}
						else{
	                        flujo_LCD2=0;						
						}
                    }
                }
            }
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }		
		break;
		
		case 19:
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
                    case 0x58:								 	 //Configurar Fecha y Hora	                               
                      flujo_LCD2=20;                         
                      teclas2=0;                            	 //Inicia el contador de teclas  
                      set_imagen(2,62); 
					  if(leer_hora()==1){
						Hora_LCD(2);
					  } 
					  if(leer_fecha()==1){
						Fecha_LCD(2);						
					  }					
                    break;                    
                    
                    case 0x5A:  								//Datos de Recibo

                    break;
                  										
                    case 0x96:                         			//Configurar Surtidor                   

                    break;
					
                    case 0x65:                         			//Test                     
						set_imagen(2,55);
						for(x=0;x<=16;x++){
			        		write_psoc1(3,test[x]);			
						}
					    write_psoc1(3,10);
						write_psoc1(3,10);
						write_psoc1(3,10);
						write_psoc1(3,10);
						for(x=0;x<=16;x++){
			        		write_psoc1(4,test[x]);			
						}
					    write_psoc1(4,10);
						write_psoc1(4,10);
						write_psoc1(4,10);
						write_psoc1(4,10);						
						set_imagen(2,107);
						flujo_LCD2=23;
                    break;					
					
                    case 0x7E:									//ir a menu
                      flujo_LCD2=0;     
                    break;					
                }					
            }
            CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }		  
		break;
		
        case 20:   
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
					case 0x3F:								//Hora
						teclas2=0;
						comas2=0;
						flujo_LCD2=21;
						set_imagen(2,64);							
					break;
					
					case 0x3E:								//Fecha
						teclas2=0;
						comas2=0;
						flujo_LCD2=22;
						set_imagen(2,63);					
					break;
						
                    case 0x7E:								//ir a menu	
                      flujo_LCD2=0;     
                    break;						
                }
            }
            CyDelay(100);            
            LCD_2_ClearRxBuffer();
         } 
        break;
		
        case 21:
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
				switch(teclas2){
	                case 0:										//Decenas de la Hora
	                    if(LCD_2_rxBuffer[3]<=1)					
						{
		                    teclas2++;
		                    write_LCDB(2,(LCD_2_rxBuffer[3]+0x30),teclas2);
		                    hora[1]=LCD_2_rxBuffer[3]<<4;						
	                    }
	                    if(LCD_2_rxBuffer[3]==0x0A){            	//Comando de 0
	                        teclas2++;
	                        hora[1]=0;
	                        write_LCDB(2,0x30,teclas2);
	                    }                     
	                break;
						
	                case 1:											//Unidades de la Hora
	                    if(LCD_2_rxBuffer[3]<=9)					
						{
		                    teclas2++;
		                    write_LCDB(2,(LCD_2_rxBuffer[3]+0x30),teclas2);
		                    hora[1]|=LCD_2_rxBuffer[3];						
	                    }
	                    if(LCD_2_rxBuffer[3]==0x0A){            	//Comando de 0
	                        teclas2++;
	                        write_LCDB(2,0x30,teclas2);
	                    }                     
	                break;
						
	                case 2:											//Decenas de la Minutos					
	                    if(LCD_2_rxBuffer[3]<=5)					
						{
							teclas2++;					
							write_LCDB(2,':',teclas2);							
		                    teclas2++;
		                    write_LCDB(2,(LCD_2_rxBuffer[3]+0x30),teclas2);
		                    hora[0]=LCD_2_rxBuffer[3]<<4;						
	                    }
	                    if(LCD_2_rxBuffer[3]==0x0A){            	//Comando de 0
							teclas2++;					
							write_LCDB(2,':',teclas2);							
	                        teclas2++;
	                        write_LCDB(2,0x30,teclas2);
							hora[0]=0;
	                    } 
	                break;
						
	                case 4:											//Unidades de la Minutos					
	                    if(LCD_2_rxBuffer[3]<=9)					
						{
		                    teclas2++;
		                    write_LCDB(2,(LCD_2_rxBuffer[3]+0x30),teclas2);
		                    hora[0]|=LCD_2_rxBuffer[3];						
	                    }
	                    if(LCD_2_rxBuffer[3]==0x0A){            	//Comando de 0
	                        teclas2++;
	                        write_LCDB(2,0x30,teclas2);
	                    } 
	                break;
				}
                if(LCD_2_rxBuffer[3]==0x40){            		//Comando de Am/Pm
                    if(comas2==0){
                        comas2=1;
                        write_LCDB(2,'p',6);
                    }
					else{
                        comas2=0;
                        write_LCDB(2,'a',6);						
					}
					write_LCDB(2,'m',7);
                } 				
                if(LCD_2_rxBuffer[3]==0x0B){					//Cancel
                    if(teclas2==0){								//Si no ha presionado nada regresa al menu anterior						
                        flujo_LCD2=19;
                        set_imagen(2,93);
                    }
                    else{
                        write_LCDB(2,0x20,(teclas2));			//Si ya presiono borra el dato	
                        teclas2--;
                    }
                }
                if(LCD_2_rxBuffer[3]==0x0C){					//Enter
                    if(teclas2==5){
						hora[1]|=((comas2|2)<<5);
						if(write_hora()==1){
	                        flujo_LCD2=19;                         
		                    set_imagen(2,93);                     
						}
                    }
                }

            }
            CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }        
        break;
		
		case 22:
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
				switch(teclas2){
	                case 0:										//Decenas de dia
	                    if(LCD_2_rxBuffer[3]<=3)					
						{
		                    write_LCDB(2,(LCD_2_rxBuffer[3]+0x30),teclas2);
		                    teclas2++;							
		                    fecha[0]=LCD_2_rxBuffer[3]<<4;						
	                    }
	                    if(LCD_2_rxBuffer[3]==0x0A){            	//Comando de 0
	                        fecha[0]=0;
	                        write_LCDB(2,0x30,teclas2);
	                        teclas2++;							
	                    }                     
	                break;
						
	                case 1:											//Unidades de dia
	                    if(LCD_2_rxBuffer[3]<=9)					
						{
		                    write_LCDB(2,(LCD_2_rxBuffer[3]+0x30),teclas2);
		                    fecha[0]|=LCD_2_rxBuffer[3];	
		                    teclas2++;							
	                    }
	                    if(LCD_2_rxBuffer[3]==0x0A){            	//Comando de 0
	                        write_LCDB(2,0x30,teclas2);
	                        teclas2++;							
	                    }                     
	                break;
						
	                case 2:											//Decenas de mes					
	                    if(LCD_2_rxBuffer[3]<=1)					
						{					
							write_LCDB(2,'/',teclas2);
							teclas2++;							
		                    write_LCDB(2,(LCD_2_rxBuffer[3]+0x30),teclas2);
		                    fecha[1]=LCD_2_rxBuffer[3]<<4;
		                    teclas2++;							
	                    }
	                    if(LCD_2_rxBuffer[3]==0x0A){            	//Comando de 0;					
							write_LCDB(2,'/',teclas2);
							teclas2++;							
	                        write_LCDB(2,0x30,teclas2);
							fecha[1]=0;
	                        teclas2++;							
	                    } 
	                break;
						
	                case 4:											//Unidades de mes					
	                    if(LCD_2_rxBuffer[3]<=9)					
						{
		                    write_LCDB(2,(LCD_2_rxBuffer[3]+0x30),teclas2);
		                    fecha[1]|=LCD_2_rxBuffer[3];	
		                    teclas2++;							
	                    }
	                    if(LCD_2_rxBuffer[3]==0x0A){            	//Comando de 0
	                        write_LCDB(2,0x30,teclas2);
	                        teclas2++;							
	                    } 
	                break;
						
	                case 5:											//Decenas de año					
	                    if(LCD_2_rxBuffer[3]<=9)					
						{					
							write_LCDB(2,'/',teclas2);
							teclas2++;							
		                    write_LCDB(2,(LCD_2_rxBuffer[3]+0x30),teclas2);
		                    fecha[2]=LCD_2_rxBuffer[3]<<4;
		                    teclas2++;							
	                    }
	                    if(LCD_2_rxBuffer[3]==0x0A){            	//Comando de 0;					
							write_LCDB(2,'/',teclas2);
							teclas2++;							
	                        write_LCDB(2,0x30,teclas2);
							fecha[2]=0;
	                        teclas2++;							
	                    } 
	                break;
						
	                case 7:											//Unidades de año					
	                    if(LCD_2_rxBuffer[3]<=9)					
						{
		                    write_LCDB(2,(LCD_2_rxBuffer[3]+0x30),teclas2);
		                    fecha[2]|=LCD_2_rxBuffer[3];	
		                    teclas2++;							
	                    }
	                    if(LCD_2_rxBuffer[3]==0x0A){            	//Comando de 0
	                        write_LCDB(2,0x30,teclas2);
	                        teclas2++;							
	                    } 
	                break;						
				}				
                if(LCD_2_rxBuffer[3]==0x0B){					//Cancel
                    if(teclas2==0){								//Si no ha presionado nada regresa al menu anterior						
                        flujo_LCD2=19;
                        set_imagen(2,93);
                    }
                    else{
                        teclas2--;						
                        write_LCDB(2,0x20,(teclas2));			//Si ya presiono borra el dato	
                    }
                }
                if(LCD_2_rxBuffer[3]==0x0C){					//Enter
                    if(teclas2==8){
						if(write_fecha()==1){
	                        flujo_LCD2=19;                         
		                    set_imagen(2,93);                     
						}
                    }
                }

            }
            CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }			
		break;
		
        case 23:  
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){                               
                    case 0x39:								//SI
					  print1[0]=1;
					  print1[1]=3;
					  print2[0]=1;
					  print2[1]=4;					
					  if(write_eeprom(666,print1)==1){ 
						if(write_eeprom(613,print2)==1){
	                    	set_imagen(2,104); 
						  	CyDelay(500);
						  	flujo_LCD2=0;
						}	
					  }
                    break; 

                    case 0x38:								//NO
                      flujo_LCD2=24;     
					  set_imagen(2,103);					
                    break;					
					
                    case 0x7E:								//ir a menu	
                      flujo_LCD2=0;     
                    break;					
                }
            }
            CyDelay(100);
            LCD_2_ClearRxBuffer();
         }       
        break;	
		
        case 24:  
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){                               
                    case 0x8C:								//La impresora 1
					  print1[0]=1;
					  print1[1]=4;
					  print2[0]=1;
					  print2[1]=4;					
					  if(write_eeprom(666,print1)==1){ 
						if(write_eeprom(613,print2)==1){
	                    	set_imagen(2,106); 
						  	CyDelay(500);
						  	flujo_LCD2=0;
						}	
					  }
                    break; 

                    case 0x8D:								//La impresora 2
					  print1[0]=1;
					  print1[1]=3;
					  print2[0]=1;
					  print2[1]=3;					
					  if(write_eeprom(666,print1)==1){ 
						if(write_eeprom(613,print2)==1){
	                    	set_imagen(2,105); 
						  	CyDelay(500);
						  	flujo_LCD2=0;
						}	
					  }					
                    break;
					
                    case 0x8E:								//Ninguna
					  print1[0]=1;
					  print1[1]=0;
					  print2[0]=1;
					  print2[1]=0;					
					  if(write_eeprom(666,print1)==1){ 
						if(write_eeprom(613,print2)==1){
	                    	set_imagen(2,108); 
						  	CyDelay(500);
						  	flujo_LCD2=0;
						}	
					  }				
                    break;					
					
                    case 0x7E:								//ir a menu	
                      flujo_LCD2=0;     
                    break;					
                }
            }
            CyDelay(100);
            LCD_2_ClearRxBuffer();
         }       
        break;		
		
        case 100:  
         if(count_protector2>=4){
            flujo_LCD2=0;
            isr_4_Stop(); 
            Timer_Animacion2_Stop(); 
            count_protector2=0;
			estado_pos[1]=0;					//Pos libre para volver a programar
			if(lado2.estado!=12){
				set_imagen(2,46);
				estado_lcd2=0;
			}
         }            
        break;
		
        case 101:  
          if(count_protector2>=2){              
            isr_4_Stop(); 
            Timer_Animacion2_Stop(); 			
            flujo_LCD2=prox_caso[1][0];
			set_imagen(2,prox_caso[1][1]);
			count_protector2=0;
          }            
        break;		
		
        case 102: 
          if((rventa2.autorizado=='2')||(rventa2.autorizado==0)){
			error_op(2,85);
		  }
          if(lado2.estado==1){
            error_op(2,85);
          }        
        break;
		
		case 103:
			count_protector2=0;
            set_imagen(2,46);  //Regresa a pantalla de inicio
            flujo_LCD2 =100;
		break;
		
		case 104:
          if(rventa2.autorizado==0){
			//error_op(2,85);
			estado_lcd2=0;
			lado2.estado=1;
			estado_pos[1]=0;					//Pos libre para volver a programar;
		  }
		  else if(rventa2.autorizado==100){
		  	PC_Start();
			//error_op(2,85);
			estado_lcd2=0;
			lado2.estado=1;
			estado_pos[1]=0;					//Pos libre para volver a programar;			
		  }
		break;		
    }
}

/*
*********************************************************************************************************
*                                         void polling_pos3(void)
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

void polling_pos3(void){
  	uint8 x,ppu[5],estado,aux[10];  ///Falta Y, para qué?
	char numero[8];
	double num_decimal;	
    switch(flujo_LCD3){   
        case 0:
         /*isr_3_StartEx(animacion); 
         Timer_Animacion_Start();
         count_protector=0;*/
         flujo_LCD3=1;
		/* for(x=0;x<=7;x++){     //Sobran inicializaciones o faltan en los otros casos???
		 	rventa3.preset[x]=0;
			numero[x]=0;
		 }*/
		 for(x=0;x<=10;x++){
			rventa3.cedula[x]=0;
		 }	
		 for(x=0;x<=8;x++){
			rventa3.password[x]=0;
			rventa3.placa[x]=0;
		 }
		 estado=get_estado(lado3.dir);										
		 switch(estado){		
          case 0x0B:                     	//Termino venta
			flujo_LCD3=11;
		  break;	                        ///Cambia flujo_LCD3 de 12 a 11, ambos casos
			
          case 0x0A:						//Termino venta
			flujo_LCD3=11;
		  break;										
		 }         
        break;
        
        case 1:
		 if((rventa3.autorizado==100)&&(flujo_LCD1<=1)&&(flujo_LCD2<=1)&&(flujo_LCD4<=1)){
			if(lado3.estado!=12){
				lado3.estado=28;
			}
			rventa3.autorizado=0;			
			PC_Start();
		 }		
         /*if(LCD_1_GetRxBufferSize()==8){ 
             isr_3_Stop(); 
             Timer_Animacion_Stop(); */
	         flujo_LCD3=3;
	         set_imagen(1,4);
			 CyDelay(100);
             LCD_1_ClearRxBuffer();				
         //}
        break;
        
        case 2: 
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
				count_protector=0;
                if(count_teclas1<teclas1){									
                    if(LCD_1_rxBuffer[3]<=9){								//Numero de 1-9
                        count_teclas1++;
                        Buffer_LCD1[count_teclas1]=LCD_1_rxBuffer[3];
                        write_LCD(1,(LCD_1_rxBuffer[3]+0x30), posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
                    }
                    if(LCD_1_rxBuffer[3]==0x0A){            				//Comando de 0
						if((id_teclado1==1)&&(count_teclas1==1)&&(Buffer_LCD1[1]==0)){
						}
						else{
	                        count_teclas1++;
	                        Buffer_LCD1[count_teclas1]=0;
							write_LCD(1,0x30, posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
						}
                    }  
                    if(LCD_1_rxBuffer[3]==0x51){            	 			//Comando de Coma
                        if(count_teclas1>=1 && comas1==0){
                            count_teclas1++;
                            Buffer_LCD1[count_teclas1]=id_coma1;
							write_LCD(1,id_coma1, posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
                            comas1=1;
                        }
                    }                    
                }
                if(LCD_1_rxBuffer[3]==0x0B){								//Cancel
                    if(count_teclas1==0){
						switch(id_teclado1){
							case 1:
							set_imagen(1,5);
							flujo_LCD3=4;
							break;
							
							case 2:
								if(rventa3.tipo_venta==1){
		                        	flujo_LCD3=4;	
		                        	set_imagen(1,5);
								}
								else{
									count_teclas1=0;
									teclas1=7;
									posx1=4;
									posy1=3;
									sizeletra1=1;				
									set_imagen(1,10);
						            flujo_LCD3=13;
									count_teclas1=0;
								}
							break;
							
							case 3:
							//set_imagen(1,0);
							flujo_LCD3=0;
							lado3.estado=1;
							estado_lcd1=0;
							set_imagen(1,46);								//imagen de pos a  pos b
							estado_pos[2]=0;							
							break;
							
							case 4:
							//set_imagen(1,0);
							flujo_LCD3=0;
							lado3.estado=1;
							estado_lcd1=0;
							set_imagen(1,46);								//imagen de pos a  pos b
							estado_pos[2]=0;							
							break;							
						}
                    }
                    else{
						write_LCD(1,0x20, posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
                        if(Buffer_LCD1[count_teclas1]==id_coma1){
                            comas1=0;
                        }
						Buffer_LCD1[count_teclas1]=0;
                        count_teclas1--;
                    }
                }
                if(LCD_1_rxBuffer[3]==0x0C){								//Enter
					switch(id_teclado1){
						case 1:
							for(x=0;x<=7;x++){
								numero[x]=0;
							}						     
							if((count_teclas1>=1)&&(Buffer_LCD1[count_teclas1]!='.')){
								for(x=1;x<=count_teclas1;x++){
									if(Buffer_LCD1[x]!='.'){
										numero[x-1]=Buffer_LCD1[x]+48;
									}
									else{
										numero[x-1]=Buffer_LCD1[x];
									}
								}
								num_decimal=atof(numero);	
								if(((rventa3.preset[0]==2)&&(num_decimal>=800))||((rventa3.preset[0]==1)&&(num_decimal<=900)&&(num_decimal>0))){
									for(x=count_teclas1;x>=1;x--){
										rventa3.preset[x]=Buffer_LCD1[(count_teclas1-x)+1];
									}
		                        	flujo_LCD3=5;
									if(rventa3.tipo_venta==0){
										if((lado3.mangueras!=1)&&(imagen_grado3!=1)){
			                        		set_imagen(1,imagen_grado3);
										}
										else{
											flujo_LCD3=12;
											//set_imagen(1,7);
											rventa3.producto=lado3.grado[0][0];	
										}
									}
									else{
						                flujo_LCD3=6;                         
						                set_imagen(1,7);
									}
								}
							}
						break;
							
						case 2:
							for(x=count_teclas1;x>=1;x--){
								rventa3.km[x]=Buffer_LCD1[(count_teclas1-x)+1];
							}
							if(rventa3.tipo_venta==1){
	                        	flujo_LCD3=4;	
	                        	set_imagen(1,5);
							}
							else{
								count_teclas1=0;
								teclas1=7;
								posx1=4;
								posy1=3;
								sizeletra1=1;				
								set_imagen(1,10);
					            flujo_LCD3=13;
								count_teclas1=0;
							}
						break;
							
						case 3:
							for(x=count_teclas1;x>=1;x--){
								rventa3.cedula[x]=Buffer_LCD1[(count_teclas1-x)+1]+48;
							}
                        	flujo_LCD3=2;	
                        	set_imagen(1,37);
							count_teclas1=0;
							id_teclado1=4;
							teclas1=10;
							posx1=2;
							posy1=3;
							sizeletra1=1;								
						break;
							
						case 4:
							for(x=count_teclas1;x>=1;x--){
								rventa3.password[x]=Buffer_LCD1[(count_teclas1-x)+1]+48;
							}
                        	flujo_LCD3=102;	
                        	set_imagen(1,57);
							lado3.estado=16;
						break;							
							
					}					
                }
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }
		 if((count_protector>=30)&&(rventa3.tipo_venta==0)&&(id_teclado1=2)){
			count_teclas1=0;
			teclas1=7;
			posx1=4;
			posy1=3;
			sizeletra1=1;				
			set_imagen(1,10);
	        flujo_LCD3=13;
		 }		
        break;
        
        case 3:  
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
                    case 0x0D:								//Efectivo
					  for(x=0;x<=10;x++){
					 	  rventa3.km[x]=0;
					  }					
					  if(rventa3.autorizado=='1'){
	                      flujo_LCD3=4;                                   
	                      set_imagen(1,5);
						  rventa3.tipo_venta=0;
					  }
					  else if(rventa3.autorizado=='0'){
						  lado3.estado=28;					 		 			
						  error_op(1,65);		
					  }
					  else if((rventa3.autorizado=='2')||(rventa3.autorizado==0)||(rventa3.autorizado==1)||(rventa3.autorizado==100)){	
	                      flujo_LCD3=4;                                   
	                      set_imagen(1,5);
						  rventa3.tipo_venta=0;
						  lado3.estado=28;
					  }						
                    break;
                    
                    case 0x0E:								//Credito 
					  for(x=0;x<=10;x++){
					 	  rventa3.km[x]=0;
					  }					
					  if(rventa3.autorizado=='1'){
	                     flujo_LCD3=16;                                
	                     set_imagen(1,18);
						 rventa3.tipo_venta=1;
					  }
					  else if(rventa3.autorizado=='0'){
						lado3.estado=28;
						error_op(1,65);
					  }
					  else if((rventa3.autorizado=='2')||(rventa3.autorizado==0)||(rventa3.autorizado==1)||(rventa3.autorizado==100)){
						flujo_LCD3 = 100;
                        set_imagen(1,85);
                        count_protector=1;
	                    isr_3_StartEx(animacion);  
	                    Timer_Animacion_Start();
						lado3.estado=28;   //Error de operación
					  }					
                    break;
                    
                    case 0x45:                          //Otras Opciones
                      flujo_LCD3=17;                                    
                      set_imagen(1,131);                    
                    break; 
					
                    case 0x7E:                         		//Volver a menu                   
					  flujo_LCD3=0; 	
					  estado_pos[2]=0;
					  set_imagen(1,46);
					  estado_lcd1=0; 
                    break; 					
                }
            }
           // CyDelay(100);
            LCD_1_ClearRxBuffer();
         }       
        break;
        
        case 4:   
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
                    case 0x0F:                               	//$
					  for(x=0;x<=7;x++){
					 	rventa3.preset[x]=0;
					  }						
                      flujo_LCD3=2; 
					  count_teclas1=0;							//Inicia el contador de teclas	
					  id_teclado1=1;
					  id_coma1='.';	
                      teclas1=6; 
					  rventa3.preset[0]=2;
                      comas1=0;									
                      set_imagen(1,6); 
					  posx1=4;
					  posy1=3;
					  sizeletra1=1;	
					  write_LCD(1,'$', 3, 4, 1);
                    break;
                    
                    case 0x10: 									//G 
					  for(x=0;x<=7;x++){
					 	rventa3.preset[x]=0;
					  }						
                      flujo_LCD3=2; 
					  count_teclas1=0;							//Inicia el contador de teclas	
					  id_teclado1=1;
					  id_coma1='.';	
                      teclas1=7; 
					  rventa3.preset[0]=1;
                      comas1=0;									
                      set_imagen(1,13); 
					  posx1=4;
					  posy1=3;
					  sizeletra1=1;					
					  write_LCD(1,'G', 3, 4, 1);
                    break;
                    
                    case 0x43:  								//Full 
					  for(x=0;x<=7;x++){
					 	rventa3.preset[x]=0;
					  }	
                      rventa3.preset[0]=3;					
					  for(x=5;x<=6;x++){
						rventa3.preset[x]=9;		
					  }						
					  if(rventa3.tipo_venta==0){				
						if((lado3.mangueras!=1)&&(imagen_grado3!=1)){
	                		set_imagen(1,imagen_grado3);
							flujo_LCD3=5;
						}
						else{
							flujo_LCD3=12;
							//set_imagen(1,7);
							rventa3.producto=lado3.grado[0][0];	
						}
					  }
					  else{
		                flujo_LCD3=6;                         
		                set_imagen(1,7);						
					  }
                    break;
                    
                    case 0x3B:                                //Cancel                     
                      flujo_LCD3=3;
                      set_imagen(1,4);
                    break;                    
                }
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         } 
        break;
		
		case 5:
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
                    case 0x80:                          //Corriente
					  rventa3.producto=1;	
                      flujo_LCD3=12;                         
                      //set_imagen(1,7); 
                    break;
                    
                    case 0x81:                          //Diesel
					  rventa3.producto=3;
                      flujo_LCD3=12;                         
                      //set_imagen(1,7); 
                    break;
                   
                    case 0x7F:                          //Extra  
					  rventa3.producto=2;
                      flujo_LCD3=12;                         
                      //set_imagen(1,7); 
                    break; 										
                }
                CyDelay(50);
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }		
		break;
		
		case 6:
		 CyDelay(50);
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                if(LCD_1_rxBuffer[3]==0x7E){					//Cancel
					flujo_LCD3=0;
					estado_pos[2]=0;
					set_imagen(1,46);
					estado_lcd1=0;					
                }												
			}
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
			break;
		 }		
         x=get_estado(lado3.dir);
         if(x==7){									//Espera a que este en listo el equipo	
			flujo_LCD3=7;
            CyDelay(50);
         }
         else if(x==0){
			CyDelay(50);
			programar(lado3.dir,1,aux,2);
         }
         else if(x==0x0C){
            error_op(1,85);
         } 
		 else if(x==0x0B){                   		 //Termino venta
			flujo_LCD3=11;
         }						
		 else if(x==0x0A){							//Termino venta
			flujo_LCD3=11;
		 }        
        break;
		
		case 7:
		 CyDelay(10);
		 rventa3.manguera=estado_ex(lado3.dir);
		 if(rventa3.manguera!=0){
			 if(rventa3.tipo_venta==0){
				if(rventa3.producto==lado3.grado[rventa3.manguera-1][0]){
					flujo_LCD3=8;
					CyDelay(10);
				}
				else{
					error_op(1,85);
				}
			 }
			 else if(rventa3.tipo_venta==1){
				flujo_LCD3=8;
				CyDelay(10);
			 }
			 else if(rventa3.tipo_venta==2){
				flujo_LCD3=102;
				set_imagen(1,57);
				lado3.estado=31; 
			 }			
		 }	
		break;
		
		case 8:
		 CyDelay(10);
		 if(get_estado(lado3.dir)==7){
			 if(rventa3.tipo_venta==0){
				switch(rventa3.manguera){
					case 1:
						leer_eeprom(768,6);
					break;
					case 2:
						leer_eeprom(774,6);	
					break;
					case 3:
						leer_eeprom(780,6);					
					break;				
				}
				ppu[0]=buffer_i2c[1];
				ppu[1]=buffer_i2c[2];
				ppu[2]=buffer_i2c[3];
				ppu[3]=buffer_i2c[4];
				ppu[4]=buffer_i2c[5];
				if(buffer_i2c[0]==5){
					if(cambiar_precio(lado3.dir, ppu, rventa3.manguera)==1){
						flujo_LCD3=9;
					 	leer_hora();
					 	leer_fecha();
					}
				}
				else{
					error_op(1,85);
					lado3.estado=28;
				}
			 }
			 else{
				flujo_LCD3=102;
				set_imagen(1,57);
				lado3.estado=9;
				leer_hora();
				leer_fecha();				
			 }
		 }
		break;
		
		case 9:
		 CyDelay(10);
		 if(get_estado(lado3.dir)==7){
			 CyDelay(10);
			 if(programar(lado3.dir,rventa3.manguera,rventa3.preset,rventa3.preset[0])==0){
				error_op(1,85);	
			 }
			 else{
				CyDelay(50);
				Surtidor_PutChar(0x10|lado3.dir);				//Autoriza el surtidor
			    flujo_LCD3=11;
				//set_imagen(1,8);
				lado3.estado=8;
				estado_lcd1=0;
				set_imagen(1,46);								//imagen de pos a  pos b				
			 }
		 }
		break;
		
		case 10:
		
		break;		
		
		case 11:
		 CyDelay(50);
		 switch(get_estado(lado3.dir)){
	         case 0x0B:                     //Termino venta
				CyDelay(100);
				for(x=0;x<=8;x++){
					rventa3.dinero[x]=0;
					rventa3.volumen[x]=0;
					rventa3.placa[x]=0;  // Sobra o falta en los otros casos
				}				
				if(venta(lado3.dir)==1){
					flujo_LCD3=15;
				}
				else{
					for(x=0;x<=18;x++){										//imprime no pudo tomar venta
						write_psoc1(3,msn_errorv[x]);
					}
				    write_psoc1(3,10);
					write_psoc1(3,10);
					write_psoc1(3,10);
				}				
			 break;	
				
	         case 0x0A:						//Termino venta
				for(x=0;x<=8;x++){
					rventa3.dinero[x]=0;
					rventa3.volumen[x]=0;
					rventa3.placa[x]=0;  // falta o sobra
				}				
				if(venta(lado3.dir)==1){
					flujo_LCD3=15;
				}
				else{
					for(x=0;x<=18;x++){										//imprime no pudo tomar venta
						write_psoc1(3,msn_errorv[x]);
					}
				    write_psoc1(3,10);
					write_psoc1(3,10);
					write_psoc1(3,10);
				}				
			 break;

	         case 0x06:                     //No hizo venta
				flujo_LCD3=100;
				leer_hora();
				leer_fecha();
				venta(lado3.dir);
				lado3.estado=12;
		        isr_3_StartEx(animacion); 
		        Timer_Animacion_Start();
		        count_protector=1;				
			 break;				 	
         }		 	
		break;
		
		case 12:		
		 if(rventa3.tipo_venta==0){
			if(km[1]==0){
				count_teclas1=0;
				teclas1=7;
				posx1=4;
				posy1=3;
				sizeletra1=1;				
				set_imagen(1,10);
	            flujo_LCD3=13;
			}
			else{
	            set_imagen(1,14); 
				flujo_LCD3=2;
				count_teclas1=0;							//Inicia el contador de teclas	
				id_teclado1=2;
				teclas1=10;
				posx1=2;
				posy1=3;
				sizeletra1=1;					
			}
		 }
		 else if(rventa3.tipo_venta==1){
			rventa3.imprimir=1;
            flujo_LCD3=15;			
		 }
		 else if(rventa3.tipo_venta==2){
             set_imagen(1,12); 				//la de suba la manija
             flujo_LCD3=6; 
			 rventa3.imprimir=0;			
		 }			
         isr_3_StartEx(animacion); 
         Timer_Animacion_Start();
         count_protector=0;								
		break;
		
		case 13:
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
				count_protector=0;
                if(count_teclas1<teclas1){
                    if(LCD_1_rxBuffer[3]<=9){
                        count_teclas1++;
                        Buffer_LCD1[count_teclas1]=LCD_1_rxBuffer[3]+0x30;
                        write_LCD(1,(LCD_1_rxBuffer[3]+0x30), posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);                       
                    }
                    if(LCD_1_rxBuffer[3]==0x0A){                                        //Comando de 0
                        count_teclas1++;                       
                        Buffer_LCD1[count_teclas1]=0x30;
                        write_LCD(1,0x30, posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);                        
                    }  
                    if(LCD_1_rxBuffer[3]==0x6A){                                        //Comando de -
                        count_teclas1++;                        
                        Buffer_LCD1[count_teclas1]='-';
                        write_LCD(1,'-', posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);                       
                    } 					
                    if(LCD_1_rxBuffer[3]>=0x1B && LCD_1_rxBuffer[3]<=0x42){            //Comando de Letra
                        for(x=0;x<=25;x++){                                            //Compara el dato que llego con un vector que tiene todas las letras     
                            if(LCD_1_rxBuffer[3]==letras[x]){
                                count_teclas1++;                            
                                Buffer_LCD1[count_teclas1]=x+0x41;
                                write_LCD(1,(x+0x41),posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);                            
                            }
                        }
                    }                    
                }
                if(LCD_1_rxBuffer[3]==0x0B){                             //Borrar - Cancelar
                    if(count_teclas1==0){								//Si no tiene nada pasa a pedir impresion
						if(lado3.estado!=26){
	                        rventa3.placa[0]=0;
							set_imagen(1,11);
	                        flujo_LCD3=14;
						}
                    }
                    else{
                        write_LCD(1,0x20, posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);                        
                        count_teclas1--;
                    }
                }
                if(LCD_1_rxBuffer[3]==0x0C){                           //Enter pasa a imprimir
                    if(count_teclas1>=1){
                        rventa3.placa[0]=teclas1;
						for(x=1;x<=count_teclas1;x++){
							rventa3.placa[x]=Buffer_LCD1[x];
						}
						if(lado3.estado!=26){
	                        flujo_LCD3=14;
	                        set_imagen(1,11);
							count_protector=0;
						}
						else{
	                        flujo_LCD3=102;
	                        set_imagen(1,57);
							lado3.estado=27;
						}
                    }
                }
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }
		 
		 if(count_protector>=30){
            flujo_LCD3=100;
		 }		
		break;
		
		case 14:
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
                    case 0x39:                          //Si Imprime cambiar imagen
                      set_imagen(1,7);					//la de gracias por su compra
                      flujo_LCD3=6;
					  rventa3.imprimir=1;
					  /*if(((rventa3.autorizado=='2')||(rventa3.autorizado==0))&&(rventa3.tipo_venta==0)){
						set_imagen(1,55);
						print_logo(id_logo[1],0,943,print2[1]);
						write_psoc1(print2[1],0x1B);
						write_psoc1(print2[1],0x40);
						write_psoc1(print2[1],10);
						write_psoc1(print2[1],10);						
						imprimir(lado3.dir,print2[1], lado3.grado[rventa3.manguera-1][0], rventa3.ppu, rventa3.volumen, rventa3.dinero, rventa3.placa,rventa3.km);
						set_imagen(1,12);
					  } */
                    break; 
                    
                    case 0x38:                          //No Imprime 
                      set_imagen(1,7); 				//la de suba la manija
                      flujo_LCD3=6; 
					  rventa3.imprimir=0;
                    break;                     
                }
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }
		
		 if(count_protector>=30){
            flujo_LCD3=100; 
		 }		 
		break;
		
		case 15:
		 if(totales(lado3.dir, lado3.mangueras)>=lado3.mangueras){
			leer_hora();
			leer_fecha();
			count_protector=0;
			if(rventa3.tipo_venta>=1){
				flujo_LCD3=103;
				set_imagen(1,57);
				rventa3.imprimir=1;
			}
			else{
				flujo_LCD3=104;	
			}										
			lado3.estado=12;
			if(((rventa3.autorizado=='2')||(rventa3.autorizado==0)||(rventa3.autorizado==100))&&(rventa3.tipo_venta==0)&&(rventa3.imprimir==1)){
				//set_imagen(1,55);
				print_logo(id_logo[1],0,943,print1[1]);
				write_psoc1(print2[1],0x1B);
				write_psoc1(print2[1],0x40);
				write_psoc1(print2[1],10);
				write_psoc1(print2[1],10);						
			  	imprimir(lado3.dir,print1[1], lado3.grado[rventa3.manguera-1][0], rventa3.ppu, rventa3.volumen, rventa3.dinero, rventa3.placa,rventa3.km);
				//set_imagen(1,12);
			}			
		 }
		 else{
			for(x=0;x<=20;x++){										
				write_psoc1(3,msn_errort[x]);
			}
		    write_psoc1(3,10);
			write_psoc1(3,10);
			write_psoc1(3,10);
		 }		
		break;
		
		case 16:
         if(touch_present(1)==1){
             if(touch_write(1,0x33)){
	             for(x=0;x<=7;x++){
	                 rventa3.id[x]=touch_read_byte(1);
	             }
				 crc_total=0;
				 for(x=0;x<7;x++){
				 	crc_total=crc_check(crc_total,rventa3.id[x]);
				 }					
				 if(crc_total==rventa3.id[7]){
		             set_imagen(1,19);
		             count_protector=0;              
		             isr_3_StartEx(animacion); 
		             Timer_Animacion_Start(); 
					 flujo_LCD3=101;
					 prox_caso[2][0]=2;
					 prox_caso[2][1]=14;
					 count_teclas1=0;							//Inicia el contador de teclas	
					 id_teclado1=2;
					 teclas1=10;
					 posx1=2;
					 posy1=3;
					 sizeletra1=1;	
				 }
             }
         }
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
                    case 0x3B:
                     flujo_LCD3=11;
                     set_imagen(1,46); //Cambio de imagen de pos a pos b  
                    break; 
                }
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }		
		break;
		
		case 17:
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
                    case 0x46:                          //Turnos 
					  if((rventa3.autorizado=='2')||(rventa3.autorizado==0)){
						error_op(1,85);
					  }	
					  else{
						  set_imagen(1,57); 
						  flujo_LCD3=102;
						  lado3.estado=13;
						  count_teclas1=0;
						  id_teclado1=3;
						  teclas1=10;
						  posx1=2;
						  posy1=3;
						  sizeletra1=1;					  
					  }	
                    break;
                    
                    case 0x55:                          //Configurar modulo
                                                        //Se necesita para el caso de la otra POS???
							
                    break;
					
                    case 0xB5:                          //Copia de Recibo
					  if((rventa3.autorizado=='2')||(rventa3.autorizado==0)){
						error_op(1,85);
					  }	
					  else{
						flujo_LCD3=102;
						set_imagen(1,57);
						lado3.estado=25;
					  }	
                    break;					
                   
                    case 0x3B:                          //Cancel                     
					  flujo_LCD3=3; 	
					  set_imagen(1,4); 
                    break;                    
                }
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }		 	
		break;
		
		
        case 100:  
         if(count_protector>=4){
            flujo_LCD3=0;
            isr_3_Stop(); 
            Timer_Animacion_Stop(); 
            count_protector=0;
			estado_pos[2]=0;					//Pos libre para volver a programar
			if(lado3.estado!=12){
				set_imagen(1,46);
				estado_lcd1=0;
			}
			
         }            
        break;
		
        case 101:  
          if(count_protector>=2){              
            isr_3_Stop(); 
            Timer_Animacion_Stop(); 			
            flujo_LCD3=prox_caso[2][0];
			set_imagen(1,prox_caso[2][1]);
			count_protector=0;
          }            
        break;		
		
        case 102:  
         if((rventa3.autorizado=='2')||(rventa3.autorizado==0)){
			error_op(1,85);
		 }
         if(lado3.estado==1){
            error_op(1,85);
         }
        break; 
		
        case 103:  
			count_protector=0;
            set_imagen(1,46);  //Regresa a pantalla de inicio
            flujo_LCD3 = 11;
        break;
		
		case 104:
          if(rventa3.autorizado==0){
			//error_op(1,85);
			estado_lcd1=0;
			lado3.estado=1;
			estado_pos[2]=0;					//Pos libre para volver a programar;
		  }
		  else if(rventa3.autorizado==100){
		  	PC_Start();
			//error_op(1,85);
			estado_lcd1=0;
			lado3.estado=1;
			estado_pos[2]=0;					//Pos libre para volver a programar;			
		  }		
		break;		
		
	}	
}

/*
*********************************************************************************************************
*                                         void polling_pos4(void)
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

void polling_pos4(void){
  	uint8 x,ppu[5],estado,aux[10];
	char numero[8];
	double num_decimal;	
    switch(flujo_LCD4){   
        case 0:
         /*isr_4_StartEx(animacion2); 
         Timer_Animacion2_Start();
         count_protector2=0;*/
         flujo_LCD4=1;
		 for(x=0;x<=7;x++){
		 	rventa4.preset[x]=0;
			numero[x]=0;
		 }
		 for(x=0;x<=10;x++){
			rventa4.cedula[x]=0;
		 }	
		 for(x=0;x<=8;x++){
			rventa4.password[x]=0;
			rventa4.placa[x]=0;
		 }
		 estado=get_estado(lado4.dir);										
		 switch(estado){		
          case 0x0B:                     	//Termino venta
			flujo_LCD4=11;
		  break;	                        //Cambio de flujo a case 11, ant (12)
			
          case 0x0A:						//Termino venta
			flujo_LCD4=11;
		  break;										
		 }         
        break;
        
        case 1:
		 if((rventa4.autorizado==100)&&(flujo_LCD1<=1)&&(flujo_LCD2<=1)&&(flujo_LCD3<=1)){
			if(lado4.estado!=12){
				lado4.estado=28;
			}
			rventa4.autorizado=0;			
			PC_Start();
		 }		
         /*if(LCD_2_GetRxBufferSize()==8){ 
             isr_4_Stop(); 
             Timer_Animacion2_Stop(); */
	         flujo_LCD4=3;
	         set_imagen(2,4);
			 CyDelay(100);
             LCD_2_ClearRxBuffer();				
         //}

        break;
        
        case 2: 
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
				count_protector2=0;
                if(count_teclas2<teclas2){									
                    if(LCD_2_rxBuffer[3]<=9){								//Numero de 1-9
                        count_teclas2++;
                        Buffer_LCD2[count_teclas2]=LCD_2_rxBuffer[3];
                        write_LCD(2,(LCD_2_rxBuffer[3]+0x30), posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
                    }
                    if(LCD_2_rxBuffer[3]==0x0A){            				//Comando de 0
						if((id_teclado2==1)&&(count_teclas2==1)&&(Buffer_LCD2[1]==0)){
						}
						else{
	                        count_teclas2++;
	                        Buffer_LCD2[count_teclas2]=0;
							write_LCD(2,0x30, posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
						}
                    }  
                    if(LCD_2_rxBuffer[3]==0x51){            	 			//Comando de Coma
                        if(count_teclas2>=1 && comas2==0){
                            count_teclas2++;
                            Buffer_LCD2[count_teclas2]=id_coma2;
							write_LCD(2,id_coma2, posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
                            comas2=1;
                        }
                    }                    
                }
                if(LCD_2_rxBuffer[3]==0x0B){								//Cancel
                    if(count_teclas2==0){
						switch(id_teclado2){
							case 1:
							set_imagen(2,5);
							flujo_LCD4=4;
							break;
							
							case 2:
								if(rventa4.tipo_venta==1){
		                        	flujo_LCD4=4;	
		                        	set_imagen(2,5);
								}
								else{
									count_teclas2=0;
									teclas2=7;
									posx2=4;
									posy2=3;
									sizeletra2=1;				
									set_imagen(2,10);
						            flujo_LCD4=13;
									count_teclas2=0;
								}
							break;
							
							case 3:
							//set_imagen(2,0);
							flujo_LCD4=0;
							lado4.estado=1;
							estado_lcd2=0;
							set_imagen(2,46);								//imagen de pos a  pos b
							estado_pos[3]=0;							
							break;
							
							case 4:
							//set_imagen(2,0);
							flujo_LCD4=0;
							lado4.estado=1;
							estado_lcd2=0;
							set_imagen(2,46);								//imagen de pos a  pos b
							estado_pos[3]=0;							
							break;							
						}
                    }
                    else{
						write_LCD(2,0x20, posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
                        if(Buffer_LCD2[count_teclas2]==id_coma2){
                            comas2=0;
                        }
						Buffer_LCD2[count_teclas2]=0;
                        count_teclas2--;
                    }
                }
                if(LCD_2_rxBuffer[3]==0x0C){								//Enter
					switch(id_teclado2){
						case 1:
							for(x=0;x<=7;x++){
								numero[x]=0;
							}						     
							if((count_teclas2>=1)&&(Buffer_LCD2[count_teclas2]!='.')){
								for(x=1;x<=count_teclas2;x++){
									if(Buffer_LCD2[x]!='.'){
										numero[x-1]=Buffer_LCD2[x]+48;
									}
									else{
										numero[x-1]=Buffer_LCD2[x];
									}
								}
								num_decimal=atof(numero);	
								if(((rventa4.preset[0]==2)&&(num_decimal>=800))||((rventa4.preset[0]==1)&&(num_decimal<=900)&&(num_decimal>0))){
									for(x=count_teclas2;x>=1;x--){
										rventa4.preset[x]=Buffer_LCD2[(count_teclas2-x)+1];
									}
		                        	flujo_LCD4=5;
									if(rventa4.tipo_venta==0){
										if((lado4.mangueras!=1)&&(imagen_grado4!=1)){
			                        		set_imagen(2,imagen_grado4);
										}
										else{
											flujo_LCD4=12;
											//set_imagen(2,7);
											rventa4.producto=lado4.grado[0][0];	
										}
									}
									else{
						                flujo_LCD4=6;                         
						                set_imagen(2,7);
									}
								}
							}
						break;
							
						case 2:
							for(x=count_teclas2;x>=1;x--){
								rventa4.km[x]=Buffer_LCD2[(count_teclas2-x)+1];
							}
							if(rventa4.tipo_venta==1){
	                        	flujo_LCD4=4;	
	                        	set_imagen(2,5);
							}
							else{
								count_teclas2=0;
								teclas2=7;
								posx2=4;
								posy2=3;
								sizeletra2=1;				
								set_imagen(2,10);
					            flujo_LCD4=13;
								count_teclas2=0;
							}
						break;
							
						case 3:
							for(x=count_teclas2;x>=1;x--){
								rventa4.cedula[x]=Buffer_LCD2[(count_teclas2-x)+1]+48;
							}
                        	flujo_LCD4=2;	
                        	set_imagen(2,37);
							count_teclas2=0;
							id_teclado2=4;
							teclas2=10;
							posx2=2;
							posy2=3;
							sizeletra2=1;								
						break;
							
						case 4:
							for(x=count_teclas2;x>=1;x--){
								rventa4.password[x]=Buffer_LCD2[(count_teclas2-x)+1]+48;
							}
                        	flujo_LCD4=102;	
                        	set_imagen(2,57);
							lado4.estado=16;
						break;							
							
					}					
                }
            }
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }
		 if((count_protector2>=30)&&(rventa4.tipo_venta==0)&&(id_teclado2=2)){
			count_teclas2=0;
			teclas2=7;
			posx2=4;
			posy2=3;
			sizeletra2=1;				
			set_imagen(2,10);
	        flujo_LCD4=13;
            count_protector2=0;  //Añadir contador
		 }		
        break;
        
        case 3:  
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
                    case 0x0D:								//Efectivo
					  for(x=0;x<=10;x++){
					 	  rventa4.km[x]=0;
					  }					
					  if(rventa4.autorizado=='1'){
	                      flujo_LCD4=4;                                   
	                      set_imagen(2,5);
						  rventa4.tipo_venta=0;
					  }
					  else if(rventa4.autorizado=='0'){
						  lado4.estado=28;		 //Error de turno			 		 			
						  error_op(2,65);		
					  }
					  else if((rventa4.autorizado=='2')||(rventa4.autorizado==0)||(rventa4.autorizado==1)||(rventa4.autorizado==100)){	
	                      flujo_LCD4=4;                                   
	                      set_imagen(2,5);
						  rventa4.tipo_venta=0;
						  lado4.estado=28;
					  }						
                    break;
                    
                    case 0x0E:								//Credito 
					  for(x=0;x<=10;x++){
					 	  rventa4.km[x]=0;
					  }					
					  if(rventa4.autorizado=='1'){
	                     flujo_LCD4=16;                                
	                     set_imagen(2,18);
						 rventa4.tipo_venta=1;
					  }
					  else if(rventa4.autorizado=='0'){
						lado4.estado=28;
						error_op(2,65);
					  }
					  else if((rventa4.autorizado=='2')||(rventa4.autorizado==0)||(rventa4.autorizado==1)||(rventa4.autorizado==100)){
						flujo_LCD4 = 100;
                        set_imagen(2,85);
                        count_protector2=1;
	                    isr_4_StartEx(animacion2);  
	                    Timer_Animacion2_Start();
						lado4.estado=28;
					  }					
                    break;
                    
                    case 0x45:
                      flujo_LCD4=17;                                    
                      set_imagen(2,131);                    //Otras Opciones
                    break; 
					
                    case 0x7E:                         		//Volver a menu                   
					  flujo_LCD4=0; 	
					  estado_pos[3]=0;
					  set_imagen(2,46);
					  estado_lcd2=0; 
                    break; 					
                }
            }
           // CyDelay(100);
            LCD_2_ClearRxBuffer();
         }       
        break;
        
        case 4:   
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
                    case 0x0F:                               	//$
					  for(x=0;x<=7;x++){
					 	rventa4.preset[x]=0;
					  }						
                      flujo_LCD4=2; 
					  count_teclas2=0;							//Inicia el contador de teclas	
					  id_teclado2=1;
					  id_coma2='.';	
                      teclas2=6; 
					  rventa4.preset[0]=2;
                      comas2=0;									
                      set_imagen(2,6); 
					  posx2=4;
					  posy2=3;
					  sizeletra2=1;	
					  write_LCD(2,'$', 3, 4, 1);
                    break;
                    
                    case 0x10: 									//G 
					  for(x=0;x<=7;x++){
					 	rventa4.preset[x]=0;
					  }						
                      flujo_LCD4=2; 
					  count_teclas2=0;							//Inicia el contador de teclas	
					  id_teclado2=1;
					  id_coma2='.';	
                      teclas2=7; 
					  rventa4.preset[0]=1;
                      comas2=0;									
                      set_imagen(2,13); 
					  posx2=4;
					  posy2=3;
					  sizeletra2=1;					
					  write_LCD(2,'G', 3, 4, 1);
                    break;
                    
                    case 0x43:  								//Full 
					  for(x=0;x<=7;x++){
					 	rventa4.preset[x]=0;
					  }	
                      rventa4.preset[0]=3;					
					  for(x=5;x<=6;x++){
						rventa4.preset[x]=9;		
					  }						
					  if(rventa4.tipo_venta==0){				
						if((lado4.mangueras!=1)&&(imagen_grado4!=1)){
	                		set_imagen(2,imagen_grado4);
							flujo_LCD4=5;
						}
						else{
							flujo_LCD4=12;
							//set_imagen(2,7);
							rventa4.producto=lado4.grado[0][0];	
						}
					  }
					  else{
		                flujo_LCD4=6;                         
		                set_imagen(2,7);						
					  }
                    break;
                    
                    case 0x3B:                                //Cancel                     
                      flujo_LCD4=3;
                      set_imagen(2,4);
                    break;                    
                }
            }
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
         } 
        break;
		
		case 5:
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
                    case 0x80:                          //Corriente
					  rventa4.producto=1;	
                      flujo_LCD4=12;                         
                      //set_imagen(2,7); 
                    break;
                    
                    case 0x81:                          //Diesel
					  rventa4.producto=3;
                      flujo_LCD4=12;                         
                      //set_imagen(2,7); 
                    break;
                   
                    case 0x7F:                          //Extra  
					  rventa4.producto=2;
                      flujo_LCD4=12;                         
                      //set_imagen(2,7); 
                    break; 										
                }
                CyDelay(50);
            }
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }		
		break;
		
		case 6:
		 CyDelay(50);
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                if(LCD_2_rxBuffer[3]==0x7E){					//Cancel
					flujo_LCD4=0;
                    estado_pos[1]=0;
					set_imagen(2,46);
					estado_lcd2=0;		   //añadir parte de inicialización
                }												
			}
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
			break;
		 }		
         x=get_estado(lado4.dir);
         if(x==7){									//Espera a que este en listo el equipo	
			flujo_LCD4=7;
            CyDelay(50);
         }
         else if(x==0){
			CyDelay(50);
			programar(lado4.dir,1,aux,2);
         }
         else if(x==0x0C){
            error_op(2,85);
         } 
		 else if(x==0x0B){                   		 //Termino venta
			flujo_LCD4=11;
         }						
		 else if(x==0x0A){							//Termino venta
			flujo_LCD4=11;
		 }        
        break;
		
		case 7:
		 CyDelay(10);
		 rventa4.manguera=estado_ex(lado4.dir);
		 if(rventa4.manguera!=0){
			 if(rventa4.tipo_venta==0){
				if(rventa4.producto==lado4.grado[rventa4.manguera-1][0]){
					flujo_LCD4=8;
					CyDelay(10);
				}
				else{
					error_op(2,85);
				}
			 }
			 else if(rventa4.tipo_venta==1){
				flujo_LCD4=8;
				CyDelay(10);
			 }
			 else if(rventa4.tipo_venta==2){
				flujo_LCD4=102;
				set_imagen(2,57);
				lado4.estado=31; 
			 }			
		 }	
		break;
		
		case 8:
		 CyDelay(10);
		 if(get_estado(lado4.dir)==7){
			 if(rventa4.tipo_venta==0){
				switch(rventa4.manguera){
					case 1:
						leer_eeprom(786,6);
					break;
					case 2:
						leer_eeprom(792,6);	
					break;
					case 3:
						leer_eeprom(798,6);					
					break;				
				}
				ppu[0]=buffer_i2c[1];
				ppu[1]=buffer_i2c[2];
				ppu[2]=buffer_i2c[3];
				ppu[3]=buffer_i2c[4];
				ppu[4]=buffer_i2c[5];
				if(buffer_i2c[0]==5){
					if(cambiar_precio(lado4.dir, ppu, rventa4.manguera)==1){
						flujo_LCD4=9;
					 	leer_hora();
					 	leer_fecha();
					}
				}
				else{
					error_op(2,85);
					lado4.estado=28;
				}
			 }
			 else{
				flujo_LCD4=102;
				set_imagen(2,57);
				lado4.estado=9;
				leer_hora();
				leer_fecha();				
			 }
		 }
		break;
		
		case 9:
		 CyDelay(10);
		 if(get_estado(lado4.dir)==7){
			 CyDelay(10);
			 if(programar(lado4.dir,rventa4.manguera,rventa4.preset,rventa4.preset[0])==0){
				error_op(2,85);	
			 }
			 else{
				CyDelay(50);
				Surtidor_PutChar(0x10|lado4.dir);				//Autoriza el surtidor
			    flujo_LCD4=11;
				//set_imagen(2,8);
				lado4.estado=8;
				estado_lcd2=0;
				set_imagen(2,46);								//imagen de pos a  pos b					
			 }
		 }
		break;
		
		case 10:
		
		break;		
		
		case 11:
		 CyDelay(50);
		 switch(get_estado(lado4.dir)){
	         case 0x0B:                     //Termino venta
				CyDelay(100);
				for(x=0;x<=8;x++){
					rventa4.dinero[x]=0;
					rventa4.volumen[x]=0;
					rventa4.placa[x]=0;
				}				
				if(venta(lado4.dir)==1){
					flujo_LCD4=15;
				}
				else{
					for(x=0;x<=18;x++){										//imprime no pudo tomar venta
						write_psoc1(3,msn_errorv[x]);
					}
				    write_psoc1(3,10);
					write_psoc1(3,10);
					write_psoc1(3,10);
				}				
			 break;	
				
	         case 0x0A:						//Termino venta
				for(x=0;x<=8;x++){
					rventa4.dinero[x]=0;
					rventa4.volumen[x]=0;
					rventa4.placa[x]=0;
				}				
				if(venta(lado4.dir)==1){
					flujo_LCD4=15;
				}
				else{
					for(x=0;x<=18;x++){										//imprime no pudo tomar venta
						write_psoc1(3,msn_errorv[x]);
					}
				    write_psoc1(3,10);
					write_psoc1(3,10);
					write_psoc1(3,10);
				}				
			 break;

	         case 0x06:                     //No hizo venta
				flujo_LCD4=100;
				leer_hora();
				leer_fecha();
				venta(lado4.dir);
				lado4.estado=12;
		        isr_4_StartEx(animacion2); 
		        Timer_Animacion2_Start();
		        count_protector2=1;				
			 break;				 	
         }		 	
		break;
		
		case 12:		
		 if(rventa4.tipo_venta==0){
			if(km[1]==0){
				count_teclas2=0;
				teclas2=7;
				posx2=4;
				posy2=3;
				sizeletra2=1;				
				set_imagen(2,10);
	            flujo_LCD4=13;
			}
			else{
	            set_imagen(2,14); 
				flujo_LCD4=2;
				count_teclas2=0;							//Inicia el contador de teclas	
				id_teclado2=2;
				teclas2=10;
				posx2=2;
				posy2=3;
				sizeletra2=1;					
			}
		 }
		 else if(rventa4.tipo_venta==1){
			rventa4.imprimir=1;
            flujo_LCD4=15;			
		 }
		 else if(rventa4.tipo_venta==2){
             set_imagen(2,7); 				//la de suba la manija estaba con imagen de gracias
             flujo_LCD4=6; 
			 rventa4.imprimir=0;			
		 }			
         isr_4_StartEx(animacion2); 
         Timer_Animacion2_Start();
         count_protector2=0;								
		break;
		
		case 13:
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
				count_protector2=0;
                if(count_teclas2<teclas2){
                    if(LCD_2_rxBuffer[3]<=9){
                        count_teclas2++;
                        Buffer_LCD2[count_teclas2]=LCD_2_rxBuffer[3]+0x30;
                        write_LCD(2,(LCD_2_rxBuffer[3]+0x30), posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);                       
                    }
                    if(LCD_2_rxBuffer[3]==0x0A){                                        //Comando de 0
                        count_teclas2++;                       
                        Buffer_LCD2[count_teclas2]=0x30;
                        write_LCD(2,0x30, posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);                        
                    }  
                    if(LCD_2_rxBuffer[3]==0x6A){                                        //Comando de -
                        count_teclas2++;                        
                        Buffer_LCD2[count_teclas2]='-';
                        write_LCD(2,'-', posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);                       
                    } 					
                    if(LCD_2_rxBuffer[3]>=0x1B && LCD_2_rxBuffer[3]<=0x42){            //Comando de Letra
                        for(x=0;x<=25;x++){                                            //Compara el dato que llego con un vector que tiene todas las letras     
                            if(LCD_2_rxBuffer[3]==letras[x]){
                                count_teclas2++;                            
                                Buffer_LCD2[count_teclas2]=x+0x41;
                                write_LCD(2,(x+0x41),posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);                            
                            }
                        }
                    }                    
                }
                if(LCD_2_rxBuffer[3]==0x0B){                             //Borrar - Cancelar
                    if(count_teclas2==0){								//Si no tiene nada pasa a pedir impresion
						if(lado4.estado!=26){
	                        rventa4.placa[0]=0;
							set_imagen(2,11);
	                        flujo_LCD4=14;
						}
                    }
                    else{
                        write_LCD(2,0x20, posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);                        
                        count_teclas2--;
                    }
                }
                if(LCD_2_rxBuffer[3]==0x0C){                           //Enter pasa a imprimir
                    if(count_teclas2>=1){
                        rventa4.placa[0]=teclas2;
						for(x=1;x<=count_teclas2;x++){
							rventa4.placa[x]=Buffer_LCD2[x];
						}
						if(lado4.estado!=26){
	                        flujo_LCD4=14;
	                        set_imagen(2,11);
							count_protector2=0;
						}
						else{
	                        flujo_LCD4=102;
	                        set_imagen(2,57);
							lado4.estado=27;
						}
                    }
                }
            }
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }
		 
		 if(count_protector2>=30){
            flujo_LCD4=100;
		 }		
		break;
		
		case 14:
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
                    case 0x39:                          //Si Imprime cambiar imagen
                      set_imagen(2,7);					//la de gracias por su compra
                      flujo_LCD4=6;
					  rventa4.imprimir=1;
					  /*if(((rventa4.autorizado=='2')||(rventa4.autorizado==0))&&(rventa4.tipo_venta==0)){
						set_imagen(2,55);
						print_logo(id_logo[1],0,943,print2[1]);
						write_psoc1(print2[1],0x1B);
						write_psoc1(print2[1],0x40);
						write_psoc1(print2[1],10);
						write_psoc1(print2[1],10);						
						imprimir(lado4.dir,print2[1], lado4.grado[rventa4.manguera-1][0], rventa4.ppu, rventa4.volumen, rventa4.dinero, rventa4.placa,rventa4.km);
						set_imagen(2,12);
					  } */
                    break; 
                    
                    case 0x38:                          //No Imprime 
                      set_imagen(2,7); 				//la de suba la manija
                      flujo_LCD4=6; 
					  rventa4.imprimir=0;
                    break;                     
                }
            }
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }
		
		 if(count_protector2>=30){
            flujo_LCD4=100; 
		 }		 
		break;
		
		case 15:
		 if(totales(lado4.dir, lado4.mangueras)>=lado4.mangueras){
			leer_hora();
			leer_fecha();
			count_protector2=0;
			if(rventa4.tipo_venta>=1){
				flujo_LCD4=103;
				set_imagen(2,57);
				rventa4.imprimir=1;
			}
			else{
				flujo_LCD4=104;	
			}										
			lado4.estado=12;
			if(((rventa4.autorizado=='2')||(rventa4.autorizado==0)||(rventa4.autorizado==100))&&(rventa4.tipo_venta==0)&&(rventa4.imprimir==1)){
				//set_imagen(2,55);
				print_logo(id_logo[1],0,943,print2[1]);
				write_psoc1(print2[1],0x1B);
				write_psoc1(print2[1],0x40);
				write_psoc1(print2[1],10);
				write_psoc1(print2[1],10);						
			  	imprimir(lado4.dir,print2[1], lado4.grado[rventa4.manguera-1][0], rventa4.ppu, rventa4.volumen, rventa4.dinero, rventa4.placa,rventa4.km);
				//set_imagen(2,12);
			}			
		 }
		 else{
			for(x=0;x<=20;x++){										
				write_psoc1(3,msn_errort[x]);
			}
		    write_psoc1(3,10);
			write_psoc1(3,10);
			write_psoc1(3,10);
		 }		
		break;
		
		case 16:
         if(touch_present(2)==1){
             if(touch_write(2,0x33)){
	             for(x=0;x<=7;x++){
	                 rventa4.id[x]=touch_read_byte(2);
	             }
				 crc_total=0;
				 for(x=0;x<7;x++){
				 	crc_total=crc_check(crc_total,rventa4.id[x]);
				 }					
				 if(crc_total==rventa4.id[7]){
		             set_imagen(2,19);
		             count_protector2=0;              
		             isr_4_StartEx(animacion2); 
		             Timer_Animacion2_Start(); 
					 flujo_LCD4=101;
					 prox_caso[3][0]=2;
					 prox_caso[3][1]=14;
					 count_teclas2=0;							//Inicia el contador de teclas	
					 id_teclado2=2;
					 teclas2=10;
					 posx2=2;
					 posy2=3;
					 sizeletra2=1;	
				 }
             }
         }
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
                    case 0x3B:
                     flujo_LCD4=11;
                     set_imagen(2,46);  //Cambio a imagen de pos a pos b
                    break; 
                }
            }
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }		
		break;
		
		case 17:
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
                    case 0x46:                          //Turnos 
					  if((rventa4.autorizado=='2')||(rventa4.autorizado==0)){
						error_op(2,85);
					  }	
					  else{
						  set_imagen(2,57); 
						  flujo_LCD4=102;
						  lado4.estado=13;
						  count_teclas2=0;
						  id_teclado2=3;
						  teclas2=10;
						  posx2=2;
						  posy2=3;
						  sizeletra2=1;					  
					  }	
                    break;
                    
                    case 0x55:                          //Configurar modulo
							                            //Hace falta o no?
                    break;
					
                    case 0xB5:                          //Copia de Recibo
					  if((rventa4.autorizado=='2')||(rventa4.autorizado==0)){
						error_op(2,85);
					  }	
					  else{
						flujo_LCD4=102;
						set_imagen(2,57);
						lado4.estado=25;
					  }	
                    break;					
                   
                    case 0x3B:                          //Cancel                     
					  flujo_LCD4=3; 	
					  set_imagen(2,4); 
                    break;                    
                }
            }
            //CyDelay(100);            
            LCD_2_ClearRxBuffer();
         }		 	
		break;
		
		
        case 100:  
         if(count_protector2>=4){
            flujo_LCD4=0;
            isr_4_Stop(); 
            Timer_Animacion2_Stop(); 
            count_protector2=0;
			estado_pos[3]=0;					//Pos libre para volver a programar
			if(lado4.estado!=12){
				set_imagen(2,46);
				estado_lcd2=0;
			}			
         }            
        break;
		
        case 101:  
          if(count_protector2>=2){              
            isr_4_Stop(); 
            Timer_Animacion2_Stop(); 			
            flujo_LCD4=prox_caso[3][0];
			set_imagen(2,prox_caso[3][1]);
			count_protector2=0;
          }            
        break;		
		
        case 102:  
         if((rventa4.autorizado=='2')||(rventa4.autorizado==0)){
			error_op(2,85);
		 }
         if(lado4.estado==1){
            error_op(2,85);
         }
        break; 
		
        case 103:  
			count_protector2=0;
            set_imagen(2,46);  //Regresa a pantalla de inicio
            flujo_LCD4 = 100;
        break;
		
		case 104:
          if(rventa4.autorizado==0){
			//error_op(2,85);
			estado_lcd2=0;
			lado4.estado=1;
			estado_pos[3]=0;					//Pos libre para volver a programar
		  }
		  else if(rventa4.autorizado==100){
		  	PC_Start();
			//error_op(2,85);
			estado_lcd2=0;
			lado4.estado=1;
			estado_pos[3]=0;					//Pos libre para volver a programar
		  }		
		break;		
		
	}	
}



/*
*********************************************************************************************************
*                                         void polling_LCD1(void)
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

void polling_LCD1(void){
	if(estado_lcd1==0){
		if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
					case 0x56:						//Lado A
						if(estado_pos[0]==0){
							estado_pos[0]=1;
							estado_lcd1=1;
							flujo_LCD1=0;
						}
					break;
					
					case 0x57:						//Lado C
						if(estado_pos[2]==0){
							estado_pos[2]=1;
							estado_lcd1=1;
							flujo_LCD3=0;
						}					
					break;
				
				}
			}
			LCD_1_ClearRxBuffer();
		}
	}
}

/*
*********************************************************************************************************
*                                         void polling_LCD2(void)
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

void polling_LCD2(void){
	if(estado_lcd2==0){
		if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
					case 0x56:						//Lado D  //A
						if(estado_pos[3]==0){
							estado_pos[3]=1;
							estado_lcd2=1;
							flujo_LCD4=0;
						}
					break;
					
					case 0x57:						//Lado B  //B
						if(estado_pos[1]==0){
							estado_pos[1]=1;
							estado_lcd2=1;
							flujo_LCD2=0;
						}					
					break;
				
				}
			}
			LCD_2_ClearRxBuffer();
		}
	}	
}

/*
*********************************************************************************************************
*                                        void polling_wd(void)
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
void polling_wd(void){

}

/*
*********************************************************************************************************
*                                         CY_ISR(animacion)
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
CY_ISR(animacion){
    Timer_Animacion_ReadStatusRegister();
    if(flujo_LCD1==1){
        if(count_protector<=6){
            count_protector++;
            set_imagen(1,(iprotector5+count_protector));  
        }
        else{
           count_protector=0; 
           set_imagen(1,(iprotector5+count_protector));  
        }
    }
    else{
        count_protector++;
    }
}

/*
*********************************************************************************************************
*                                         CY_ISR(animacion)
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
CY_ISR(animacion2){
    Timer_Animacion2_ReadStatusRegister();
    if(flujo_LCD2==1){
        if(count_protector2<=6){
            count_protector2++;
            set_imagen(2,(iprotector5+count_protector2));  
        }
        else{
           count_protector2=0; 
           set_imagen(2,(iprotector5+count_protector2));  
        }
    }
    else{
        count_protector2++;
    }
}

/*
*********************************************************************************************************
*                                        CY_ISR(modo_mux)
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
CY_ISR(modo_mux){
	Timer_Modo_ReadStatusRegister();
	state_rf++;
	espera=1;
	if(state_rf==15){
		if((rventa1.autorizado!=100)&&(rventa2.autorizado!=100)&&(rventa3.autorizado!=100)&&(rventa4.autorizado!=100)){
			rventa1.autorizado=0;
			rventa2.autorizado=0;
			rventa3.autorizado=0;
			rventa4.autorizado=0;			
		}	
		state_rf=0;
	}
}

/*
*********************************************************************************************************
*                                         main( void )
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
int main()
{
    init();
	init_surt();
	CyWdtStart(CYWDT_1024_TICKS,CYWDT_LPMODE_NOCHANGE); 
	isr_5_StartEx(modo_mux);
    PC_Start();
    for(;;)
    {
		polling_mod();
       	polling_rf();
       	polling_LCD1();					//Revisa si hay venta en alguna de las dos posiciones
		if(estado_pos[0]==1){				
			polling_pos1();
		}
		CyWdtClear();
		if(estado_pos[1]==1){
			polling_pos2();
		}		
		CyWdtClear();
	   	polling_rf();
	   	polling_LCD2();					//Revisa si hay venta en alguna de las dos posiciones
		if(estado_pos[2]==1){				
			polling_pos3();
		}
		CyWdtClear();
		if(estado_pos[3]==1){
			polling_pos4();
		}		
		CyWdtClear();
        
    }
}

/* [] END OF FILE */
