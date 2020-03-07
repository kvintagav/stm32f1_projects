/******************** (C) COPYRIGHT 2014 Kazakov Andrey ********************
* File Name          : console.c
* Author             : Kazakov Andrey
* Version            : V1.0
* Date               : 28/05/2014
* Description        : This file provides a set of functions needed to manage the
*                      communication between user and device via console
*******************************************************************************/

#include "console.h"

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#include "version_programm.h"


#define FORMAT_DEC true
#define FORMAT_HEX false


#define MAX_SIZE_CMD	 	40



extern bool test_work;
extern bool console_mode;
extern char console_out_buffer[100];
/*
uint8_t number_channel = 0;
uint32_t timeout_test = 1000;
*/

#define MEMORY_TEST  0x80


#pragma pack(push,1)  // выравнивание по одному байту


//#define MNT_NAND 1
/*****************************************************
*  @brief  Список тестов
*****************************************************/

typedef struct testMain_{
uint8_t id;
uint8_t sub_id;
uint8_t error;
uint8_t status;
} TestMain;

#define TEST_LIST \
		{"all",		"",{		0xFF, 			0,					0, 0},"всего"				}
/*


  	  	{"mem",		"",{		DSP_TEST, 		0,					0, 0, 0},"записи в память DSP"			},\
		{"nand",	"",{			NAND_TEST, 		0,		0, 0},			"NAND",						},\
		{"nand",	"id",{		NAND_TEST, 		NAND_ID_TEST,0, 0},		"NAND ID"},\
		{"nand",	"erase",{	NAND_TEST, 		NAND_ERASE_TEST,0, 0},	"стирания NAND"},\
		{"nand",	"rw",{ 		NAND_TEST, 		NAND_RW_TEST,0, 0},		"чтения/записи NAND"},\
		{"gxb",		"",{			GXB_TEST,		0,0, 0},					"гигабитников"},\
		{"sdram",	"",{		`	SDRAM_TEST, 	0,0, 0},					"SDRAM"},\
*/


TestMain ts;
struct test_struct
{
	char * name;
	char * sec_name;
	TestMain main;
	char * help;
} test_list[] = {TEST_LIST};


#define MOUNT_TEST ( sizeof test_list / sizeof test_list[0])

/*****************************************************
*  @brief  Список команд
*****************************************************/
#define CMD_HELP		0
#define CMD_BINARY		1
#define CMD_VERSION		4




#define COMMAND_LIST \
		{"help",		CMD_HELP,		"Справка"},\
		{"binary",		CMD_BINARY,		"Перевести UART в бинарный режим"},\
		{"version",		CMD_VERSION,	"Версия программы"},/*\
		{"write",		CMD_WRITE,		"Команда записи"},\
		{"read",		CMD_READ,		"Команда чтения"},\
		{"reset",		CMD_RESET,		"Сброс процессоров"},\
		{"temp",		CMD_TEMP,		"Чтение температуры"},\
		{"delayset",	CMD_DELAY_SET,	"Авто-Настройка задержек"},\
		{"delay",		CMD_DELAY,		"Настройка задержек"},\
		{"phasel",		CMD_PLLLEFT,	"сдвиг частоты влево"},\
		{"phaser",		CMD_PLLRIGHT,	"сдвиг чатоты вправо"},\
		{"phaseset",	CMD_PLLSET,		"подстройка частот"}
*/
/*
{"write rand",	CMD_WRITE_RAND,	"Команда записи случайных чисел"},\
{"linkcfg",		CMD_LINK_CFG,	"Настройка временных задержек"},\

*/

struct command_struct
{
	char * name;
	int numb;
	char * help;
} cmd_list[] = {COMMAND_LIST};

#define MOUNT_CMD ( sizeof cmd_list / sizeof cmd_list[0])

/*****************************************************
*  @brief  Список опций
*****************************************************/
#define OPT_INFINITY 	0
#define OPT_LITE_ONLY 	1
#define OPT_HARD_ONLY 	2
#define OPT_DSP_ONLY 	3
#define OPT_FPGA_ONLY 	4
#define OPT_RAND_DATA 	5
#define OPT_CNT_DATA 	6
#define OPT_ZERO_FFFF	7
#define OPT_DELAY_OUT	8

struct optin_struct
{
	char * name;
	int numb;
	char * help;
	bool enable;
} opt_list[] = {\
		{"i",	OPT_INFINITY,	"Infinity test", 	false},/*\
		{"l",	OPT_LITE_ONLY,	"Lite test", 		false},\
		{"h",	OPT_HARD_ONLY,	"Hard test", 		false},\
		{"d",	OPT_DSP_ONLY,	"dsp->fpga", 		false},\
		{"f",	OPT_FPGA_ONLY,	"fpga->dsp", 		false},\
		{"r",	OPT_RAND_DATA,	"random data",		false},\
		{"c",	OPT_CNT_DATA,	"counter data",		false},\
		{"z",	OPT_ZERO_FFFF,	"0000 - FFFF",		false},\
		{"o",	OPT_DELAY_OUT,	"DELAY OUTPUT",		false}
		*/
};

#define MOUNT_OPT ( sizeof opt_list / sizeof opt_list[0])

/*****************************************************
*  @brief  Список параметров тестов
*****************************************************/
#define	P_CHAN		 	0
#define P_TIME 			1
#define P_PATTERN		2
#define P_MOUNT 		3
#define P_ADDR			4
#define P_F_ADDR		5
#define P_DSP			6
#define P_MEM			7

struct param_struct
{
	char * name;
	uint8_t numb;
	char * help;
	uint32_t value;
	uint32_t default_v;
	uint32_t max;
	bool is_hex;
} par_list[] =  {\
		{"c",	P_CHAN,			"Number channel",			0,		-1, 	 	16,			false	},\
		{"t",	P_TIME,			"Timeout test",				0,		1, 			1000,		false	},\
		{"p",	P_PATTERN,		"Pattern",					0,		0,			0xFFFFFFFF, true	},\
		{"l",	P_MOUNT,		"Mount packets/tick",		0, 		1,	 	  	1024,		false 	},\
		{"a",	P_ADDR,			"Address in memory",		0, 		0,			0xFFFFFFFF,	true	},\
		{"f",	P_F_ADDR,		"Finish address in memory",	0, 		0,			0xFFFFFFFF,	true	},\
		{"d",	P_DSP,			"number dsp",				0, 		-1,			7,			false	},\
		{"m",	P_MEM,			"number mem",				0, 		-1,			5,			false	}\

};



#define MOUNT_PAR ( sizeof par_list / sizeof par_list[0])

#pragma pack(push,4)


/******************************************************************************

*******************************************************************************/
uint32_t console_read_hex_value(char * buffer)
{
	uint32_t result=0;
	for (int i=0 ; i<strlen(buffer) ; i++)
	{
		result=result<<4;
		if ((buffer[i]>=0x30)&&(buffer[i]<=0x39))
			result+=(buffer[i]-0x30);
		else if ((buffer[i]>=0x41)&&(buffer[i]<=0x46))
			result+= buffer[i]-(0x41-0xA);
		else if((buffer[i]>=0x61)&&(buffer[i]<=0x66))
		  result+= buffer[i]-(0x61-0xA);
	}
	return result;
}

/******************************************************************************

*******************************************************************************/
uint32_t find_cmd_par( char *argv[], uint8_t argc, bool is_hex)
{

	for (int i=0 ; i<argc ; i++)
	{
		if ((argv[i][0]=='-')||(argv[i][1]=='='))
			continue;

		int result = 0 , j = 0;
		bool ok = true;
		while(argv[i][j]!=0 && ok)
		{
			result=(is_hex) ? (result<<4) : result*10;
			if ((argv[i][j]>=0x30)&&(argv[i][j]<=0x39))
				result+=(argv[i][j]-0x30);
			else if ((argv[i][j]>='A')&&(argv[i][j]<='A'))
				result+= argv[i][j]-'A'+0xA;
			else if((argv[i][j]>='a')&&(argv[i][j]<='a'))
			  result+= argv[i][j]-'a'+0xA;
			else
				ok = false;
			j++;
		}
		if (ok)
			return result;
	}
	return 0;

}



/******************************************************************************

*******************************************************************************/
void reset_parameters()
{
	for (int j=0 ; j<MOUNT_PAR ; j++)
		par_list[j].value = par_list[j].default_v;
}

/******************************************************************************

*******************************************************************************/
void find_parameters( char *argv[], uint8_t argc)
{
	reset_parameters();
	for (int i=0 ; i<argc ; i++)
		if (strlen(argv[i])>2)
			for (int j=0 ; j<MOUNT_PAR ; j++)
				if ((argv[i][0]==par_list[j].name[0])&&(argv[i][1]=='='))
				{
					if (par_list[j].is_hex)
						par_list[j].value = console_read_hex_value(&argv[i][2]);
					else
						par_list[j].value = atoi(&argv[i][2]);
					if (par_list[j].value>par_list[j].max)
						par_list[j].value = par_list[j].max;
					break;
				}

}




/******************************************************************************

*******************************************************************************/
void find_options( char *argv[], uint8_t argc)
{
	for (int i=0 ; i<argc ; i++)
		if (strlen(argv[i])==2)
			for (int j=0 ; j<MOUNT_OPT ; j++)
				if ((argv[i][0]=='-')&&(argv[i][1]==opt_list[j].name[0]))
				{
					opt_list[j].enable = true;
					break;
				}

}

/******************************************************************************

*******************************************************************************/
int find_sub_test(char *argv[], uint8_t argc)
{
	if (argc<=2)
		return -1;
	else if (argv[2][0]=='-')
		return -1;
	else if (argv[2][1]=='=')
		return -1;
	else
	{
		for (int j=0 ; j<MOUNT_TEST ; j++)
			if (strncmp(test_list[j].name, argv[1], strlen(test_list[j].name))==0)
				if (strncmp(test_list[j].sec_name, argv[2], strlen(argv[2]))==0)
					return j;
		return -1;
	}
}

/******************************************************************************

*******************************************************************************/
int get_sub_test(uint8_t id , uint8_t sub_id)
{
	for (int j=0 ; j<MOUNT_TEST ; j++)
		if ((test_list[j].main.id == id)&&(test_list[j].main.sub_id == sub_id))
			return j;
	return 0;
}

/******************************************************************************

*******************************************************************************/
void reset_options()
{
	for (int j=0 ; j<MOUNT_OPT ; j++)
		opt_list[j].enable = false;
}

/******************************************************************************
*  @brief   read parameters from message
*  @retval result
*******************************************************************************/
int parser (char *point, char *argv[], int max_len)
{
	int number=0;
	if ((*point=='\0')||(*point=='\n')||(*point=='\r'))
		return number;
	else
		argv[number++]=point++;

	for (int i=0 ; i<max_len;i++){
		if ((*point =='\n')||(*point =='\r')||(*point ==0x00)){
			*point = '\0';
			return number;
		}
		else if ((*point==' ')||(*point=='.')||(*point==',')){
			*point = '\0';
			argv[number++]=++point;
		}
		else
			point++;
	}
	return number;
}




/******************************************************************************

*******************************************************************************/
bool analyze_command(char * buffer)
{
	//tTestBox tb;
	bool cmd_true = true;

	char *argv[10];
	uint8_t argc = parser(buffer, argv, 100);
	find_options(argv,argc);
	find_parameters(argv,argc);

	int sub_test = find_sub_test(argv,argc);

	uart_send("\r\n");

	test_work = true;
	do
	{
		if (strncmp("test", argv[0],4)==0)
		{
			
			int numb;
			for (numb=0 ; numb<MOUNT_TEST ; numb++)
				if (strncmp(test_list[numb].name, argv[1], strlen(test_list[numb].name))==0)
					break;

			if (numb>=MOUNT_TEST)
			{
				sprintf(console_out_buffer,"Bad cmd");
				uart_send_str(console_out_buffer);
				cmd_true = false;
			}
			else
			{
				
			}
			
		}
		else
		{
			int numb;
			for (numb=0 ; numb<MOUNT_CMD ; numb++)
				if (strncmp(cmd_list[numb].name, argv[0], strlen(cmd_list[numb].name))==0)
					break;

			if (numb>=MOUNT_CMD)
			{
				sprintf(console_out_buffer,"Bad cmd");
				uart_send_str(console_out_buffer);
				cmd_true = false;
			}
			else
			{
				switch(cmd_list[numb].numb)
				{
				case CMD_HELP:
					sprintf(console_out_buffer,"---------- COMMANDS ----------");
					uart_send_str(console_out_buffer);
					for (int i=0 ; i<MOUNT_CMD ; i++)
					{
						sprintf(console_out_buffer,"%s \t- %s",cmd_list[i].name,cmd_list[i].help);
						uart_send_str(console_out_buffer);
					}
					sprintf(console_out_buffer,"\r\n------------ TESTS -----------");
					uart_send_str(console_out_buffer);
					for (int i=0 ; i<MOUNT_TEST ; i++)
					{
						sprintf(console_out_buffer,"test %s %s\t- Проверка %s",test_list[i].name,test_list[i].sec_name,test_list[i].help);
						uart_send_str(console_out_buffer);
					}
					sprintf(console_out_buffer,"\r\n--------- PARAMETERS ---------");
					uart_send_str(console_out_buffer);
					for (int i=0 ; i<MOUNT_PAR ; i++)
					{
						sprintf(console_out_buffer,"%s= \t- %s",par_list[i].name,par_list[i].help);
						uart_send_str(console_out_buffer);
					}
					sprintf(console_out_buffer,"\r\n----------- OPTIONS ----------");
					uart_send_str(console_out_buffer);
					for (int i=0 ; i<MOUNT_OPT ; i++)
					{
						sprintf(console_out_buffer,"-%s \t- %s",opt_list[i].name,opt_list[i].help);
						uart_send_str(console_out_buffer);
					}
					break;
				case CMD_VERSION:
					sprintf(console_out_buffer,"Data: %08X, сommit: %08X",(VERSION_DATA),(VERSION_PROGRAMM));
					uart_send_str(console_out_buffer);
					break;
				case CMD_BINARY:
					console_mode = false;
					break;
				}
			}
		}


		if (!test_work)
			opt_list[OPT_INFINITY].enable=false;

	} while (opt_list[OPT_INFINITY].enable);

	test_work = false;
	uart_send_byte('>');
	reset_parameters();
	reset_options();

	return cmd_true;
}





#define DEPTH_CONS_FIFO 20

uint8_t curr_mnt_cmd = 0;
int8_t curr_point_write = 0;
int8_t curr_point_read = 0;

char fifo_cmd[DEPTH_CONS_FIFO][MAX_SIZE_CMD]={0};
char resive_buf[MAX_SIZE_CMD]="";
char resive_comm[MAX_SIZE_CMD]="";
char data_p[3]={0};

/************************************
* @brief
* @param  none
* @retval none
************************************/

void full_fifo_console(void)
{
	for (int i=0 ; i<DEPTH_CONS_FIFO ; i++)
		strcpy(fifo_cmd[i],"");
}


/************************************
* @brief
* @param  none
* @retval none
************************************/

void read_data_fifo(bool point)
{
	if (curr_mnt_cmd==0)
		return;
	if (point==true)
	{
		curr_point_read--;
		if (curr_point_read<0)
			curr_point_read= curr_mnt_cmd-1;
	}
	else
	{
		curr_point_read++;
		if (curr_point_read>=curr_mnt_cmd)
			curr_point_read= 0;
	}
	for (int i=strlen(resive_buf);i>0;i--)  //Очистка от мусора
	{
		uart_send_byte(0x7F);
	}

	uart_send("\r>");
	strcpy(resive_buf,fifo_cmd[curr_point_read]);
	uart_send(resive_buf);

}


/************************************
* @brief
* @param  none
* @retval none
************************************/

void write_data_fifo(void)
{

	if (curr_mnt_cmd==0)
	{
		strcpy(fifo_cmd[0],resive_buf);
		curr_point_write=1;
		curr_mnt_cmd = 1;
		return;
	}

	int prev = curr_point_write-1;
	if (prev<0)
		prev = curr_mnt_cmd-1;

	if (strncmp(resive_buf,fifo_cmd[prev],strlen(resive_buf))!=0)
	{
		strcpy(fifo_cmd[curr_point_write],resive_buf);
		curr_point_write++;
		if (curr_point_write>=DEPTH_CONS_FIFO)
			curr_point_write=0;
		curr_mnt_cmd++;
		if (curr_mnt_cmd>DEPTH_CONS_FIFO)
			curr_mnt_cmd = DEPTH_CONS_FIFO;
	}
	curr_point_read = curr_point_write;

}

/************************************
* @brief  UART IRQ Handler
* @param  none
* @retval none
************************************/

void UART1_IRQHandler(void)
{
	char data=0;
	
		data_p[2]=data_p[1];
		data_p[1]=data_p[0];
		data_p[0]=data;

		if ((data_p[2]==0x1B)&&(data_p[1]==0x5B)&&(data_p[0]==0x41)) //pointer up
			read_data_fifo(true);
		else if ((data_p[2]==0x1B)&&(data_p[1]==0x5B)&&(data_p[0]==0x42))//pointer down
			read_data_fifo(false);
		else if (data==0x1B) {}
		else if (data==0x5B) {}
		else if (data==0x03)  //Прекращение выполнения тестов Ctrl+C
		{
			opt_list[OPT_INFINITY].enable = false;
			test_work = false;
		}
		else
		{
			if (data=='\r')
			{
				if (strlen(resive_buf)>0)
				{
					strcpy(resive_comm, resive_buf);
					if (analyze_command(resive_comm))
						write_data_fifo();
					memset(resive_buf,0,MAX_SIZE_CMD);
					//strcpy(resive_buf,""); //Очистка команды
				}
				else
				{
					uart_send("\n\r>");
				}
			}
			else
			{
				if (data==0x7F) //Delete
				{
					if(strlen(resive_buf)!=0)
					{
						resive_buf[strlen(resive_buf)-1]='\0'; //Затираем позиции
						uart_send_byte(data);
					}
				}
				else
				{
					strncat(resive_buf,&data,1);
					uart_send_byte(data);
				}
			}
		}

}
