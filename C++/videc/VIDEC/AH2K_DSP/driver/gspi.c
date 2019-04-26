#include "gspi.h"


extern GPIO_Handle g_hGPIO;

static GPIO_Handle ghGpio;


static inline Void _outpin(Uns pin,Bool high)
{
	GPIO_pinWrite(ghGpio,pin,high);
}

static inline Bool _inpin(Uns pin)
{
	return GPIO_pinRead(ghGpio,pin);
}

static Void _delay(Uns t)
{
	/*һ��ѭ����Լ1.3ns*/
	static volatile Uns delay[]=
	{
		1,	 /*T0 Ƭѡʱ��1.5ns*/
		20,	 /*T1 ʱ������16.67ns*/
		10,	 /*T2 ����40%clk*/
		1,	 /*T3 ��������*/
		100, /*T4 �ּ� д*/
		200, /*T5 �ּ� ��200ns*/
		1,	 /*T6 �������1.5ns*/
		10, /*T7 Ƭѡȡ��ʱ��*/
		1,   /*T8 ���뱣��1.5ns*/
	};
	volatile Uns i;/*����Ĵ�������*/	

	assert(t<sizeof(delay)/sizeof(delay[0]));
	i=delay[t]; 
	while(i--);
}

static Void _outw(Uns word)
{
	Uns i=16;
	while(i--)
	{
	//	_delay(2);					/*50%��ʱ������*/						
	//	_delay(3);					/*��ʱ����T3(1.5ns)*/
		_outpin(GSPI_SDOUT,(word&(1<<i))?1:0);
		_outpin(GSPI_SCLK,1);		
	//	_delay(2);					/*50%��ʱ������*/	
		_outpin(GSPI_SCLK,0);		
	}
	
}

static Uns _inw()
{
	Uns i=16;
	Uns word=0;
	while(i--)
	{
	//	_delay(2);					/*50%��ʱ������*/					
	//	_delay(3);					/*��ʱ����T3(1.5ns)*/
		word|=_inpin(GSPI_SDIN)<<i;	
		_outpin(GSPI_SCLK,1);	
	//	_delay(2);					/*50%��ʱ������*/
		_outpin(GSPI_SCLK,0);
					
	}
	return word;
}

static Void _cmd(Bool read,Bool inc,Uns addr)
{

/*++
GSPI command
�������ש��ש��ש����ש���������������������
��15  ��14��13��12  ��11-0                ��
�ǩ����贈�贈�贈���贈��������������������
��r/w ��- ��- ��inc �� address            ��
�������ߩ��ߩ��ߩ����ߩ���������������������

r/w: 1-read 0-write
inc: 1-data address inc ,for access multiple data by incremental address
address: register address.
--*/

	Uns shift=0;
	
	shift|=0x8000&(read<<15);
	shift|=0x1000&(inc<<12);
	shift|=0x0FFF&(addr);	 
	
	_outw(shift);
	
}

Bool GSPI_init()
{
//	ghGpio=GPIO_open(GPIO_DEV0,GPIO_OPEN_RESET);

	ghGpio=g_hGPIO;	 
	if(ghGpio==INV)
		return 0;

	GPIO_pinEnable(ghGpio,GSPI_SDIN | GSPI_SDOUT | GSPI_SCLK | GSPI_CS);
	GPIO_pinDirection(ghGpio,GSPI_SDOUT | GSPI_SCLK | GSPI_CS,GPIO_OUTPUT);
	GPIO_pinDirection(ghGpio,GSPI_SDIN ,GPIO_INPUT);


	

	_outpin(GSPI_CS,1); /*ȡ��Ƭѡ*/
	_outpin(GSPI_SCLK,0); 
	_outpin(GSPI_SDOUT,0); 
	_delay(7);	
	return 1;
}

Bool GSPI_write(Uns addr,Uns* bufptr,Uns bufcnt)
{

	_outpin(GSPI_CS,0); /*Ƭѡ*/
	_delay(0);
		
	_cmd(0,1,addr);

	_delay(4);
	while(bufcnt--)
	{
		_outw(*bufptr++);
		_delay(4);
	}
	_outpin(GSPI_CS,1); /*ȡ��Ƭѡ*/
	_delay(7);	
	return 1;	
}

Bool GSPI_read(Uns addr,Uns* bufptr,Uns bufcnt)
{
	
	_outpin(GSPI_CS,0); /*Ƭѡ*/
	_delay(0);
	
	_cmd(1,1,addr);

	_delay(5);
	while(bufcnt--)
	{
		*bufptr++=_inw();
		_delay(5);
	}
	_outpin(GSPI_CS,1); /*ȡ��Ƭѡ*/
	_delay(7);	
	return 1;	
}


