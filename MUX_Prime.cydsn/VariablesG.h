/*
*********************************************************************************************************
*                                           GRP500 CODE
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
*                                               GRP500 CODE
*
*                                             CYPRESS PSoC5LP
*                                                with the
*                                            CY8C5969AXI-LP035
*
* Filename      : VariablesG.h
* Version       : V1.00
* Programmer(s) : 
*********************************************************************************************************
*/

#ifndef VARIABLESG_H
#define VARIABLESG_H
#include <device.h>	
/*****************Imagenes*********************/	
#define iprotector1     0
#define iprotector2     1
#define iprotector3     2
#define iprotector4     3
#define iprotector5     20
#define iprotector6     21
#define iprotector7     22
#define iprotector8     23
#define iprotector9     24
#define iprotector10    25
#define iprotector11    26
#define iprotector12    27
	
/***************Comandos Modulo**********************/
#define cestado			0xA1
#define chora			0xA2
#define cdatosestacion	0xA3
#define cdatossurt		0xA4
#define cdatosproducto	0xA5
#define cdatosimpresion	0xA6
#define creset			0xA7	
#define cdatospr		0xA8
#define cdatosprid		0xA9	
#define cventa			0xAA
#define cdatosturno		0xAB	
#define ctotales		0xAC
#define	ccambioprecio	0xAD	
#define	cautorizar		0xAE
#define	csolicitaprecio 0xAF
#define	cimprimir 		0xA0	
#define	cbloquear 		0xB0
#define	cturno	 		0xB1
#define	crturno			0xB2
#define	cplaca			0xB3	
#define	obtenerplaca	0xB4
#define cconfig			0xB5
#define cventauto		0xB6	

/****************************************************************************************************************
									Variables para manejo de LCD
****************************************************************************************************************/	
volatile uint8 	count_teclas1, comas1, comas2, id_coma1, id_teclado1, count_teclas2, id_coma2, id_teclado2;															
volatile uint16 posx1,posy1,sizeletra1,posx2,posy2,sizeletra2;	
volatile uint8 	teclas1,teclas2;
volatile uint8 	flujo_LCD1,flujo_LCD2,flujo_LCD3,flujo_LCD4;
volatile uint8 	Buffer_LCD1[30];
volatile uint8 	Buffer_LCD2[30];
volatile uint8 	count_protector,count_protector2;
volatile uint8 	password_lcd[9];
volatile uint16 imagen_grado1,imagen_grado2,imagen_grado3,imagen_grado4;										//Imagen que a mostrar para elegir grado
volatile uint16 prox_caso[4][2];									//0 flujo 1 imagen	
volatile uint8 	bloqueo_LCD[3];	
/****************************************************************************************************************
						Variables para manejo de datos del surtidor
****************************************************************************************************************/	
volatile uint8 	ppux10;
volatile uint8 	nombre_pro1[17],nombre_pro2[17],nombre_pro3[17];	//Nombre de cada producto
volatile uint8 	version[4];											//1 version - 2 Decimal Dinero - 3 Decimal Volumen

struct surtidor{
    volatile uint8 dir;
    volatile uint8 estado;
    volatile uint8 totales[3][24];   	//los 12 primeros son volumen y los segundos pesos
	volatile uint8 ppu[3][5];
	volatile uint8 grado[3][1];			//Contiene el codigo del producto en cada grado
	volatile uint8 mangueras;
};
struct surtidor lado1;
struct surtidor lado2;
struct surtidor lado3;
struct surtidor lado4;
/****************************************************************************************************************
								Variables para manejo de recibo
****************************************************************************************************************/
volatile uint8 nombre[33];
volatile uint8 nit[15];
volatile uint8 telefono[20];
volatile uint8 direccion[33];
volatile uint8 lema1[32];
volatile uint8 lema2[32];
volatile uint8 id_logo[2];
volatile uint8 km[2];
uint8 fecha[3];
uint8 hora[2];
struct recibo{					//Datos venta
    uint8 posicion;
    uint8 grado;
    uint8 ppu[5];
    uint8 dinero[9];	
    uint8 volumen[9];
	uint8 producto;
	uint8 manguera;
    uint8 preset[8]; 		//el bit 0 es el tipo de preset	
	uint8 id[16];
	uint8 km[11];
	uint8 placa[9];
	uint8 imprimir;			//Bandera de impresion
	uint8 tipo_venta;		// 0 Contado 1 credito 2 autoservicio
	uint8 cedula[11];
	uint8 password[11];
	uint8 turno;
	uint8 autorizado;		//Autorizado 0 venta mux local - 0x30 turno cerrado - 0x31 venta con nsx y turno abierto - 0x32 venta CDG   	
};
struct recibo rventa1;   	//datos de la venta actual lado 1
struct recibo rventa2;   	//datos de la venta actual lado 2
struct recibo rventa3;   	//datos de la venta actual lado 1
struct recibo rventa4;   	//datos de la venta actual lado 2

/****************************************************************************************************************
								Variables para manejo de RF
****************************************************************************************************************/
uint8 	ok_RF;      		 //Bandera para indicar que hay comunicacion RF
uint8 	ok_datosRF;			 //Hay datos para enviar al CDG	
uint8  	rf_mod[200];		 //Datos que van al CDG

/****************************************************************************************************************
								Variables para manejo de I2C
****************************************************************************************************************/
uint8 	buffer_i2c[64];												//Buffer I2C de lectura

/****************************************************************************************************************
								Variables para manejo de Ibutton
****************************************************************************************************************/
uint8	crc_total;													//Checksum Ibutton
/****************************************************************************************************************
								Variables para manejo de Impresora
****************************************************************************************************************/
uint8	print1[2];													//Puerto de la impresora
uint8	print2[2];



#endif

//[] END OF FILE
