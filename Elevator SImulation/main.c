#include "reg51.h"    //���ļ��ж����˵�Ƭ����һЩ���⹦�ܼĴ���
#include "lcd.h"

typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;
					
sbit A1=P1^0; //���岽��������Ӷ˿�
sbit B1=P1^1;
sbit C1=P1^2;
sbit D1=P1^3;

sbit LEDO=P2^0;
sbit LEDC=P2^1;
sbit LEDU=P2^6;
sbit LEDD=P2^7;

#define Coil_A1 {A1=1;B1=0;C1=0;D1=0;}//A��ͨ�磬������ϵ�
#define Coil_B1 {A1=0;B1=1;C1=0;D1=0;}//B��ͨ�磬������ϵ�
#define Coil_C1 {A1=0;B1=0;C1=1;D1=0;}//C��ͨ�磬������ϵ�
#define Coil_D1 {A1=0;B1=0;C1=0;D1=1;}//D��ͨ�磬������ϵ�
#define Coil_AB1 {A1=1;B1=1;C1=0;D1=0;}//AB��ͨ�磬������ϵ�
#define Coil_BC1 {A1=0;B1=1;C1=1;D1=0;}//BC��ͨ�磬������ϵ�
#define Coil_CD1 {A1=0;B1=0;C1=1;D1=1;}//CD��ͨ�磬������ϵ�
#define Coil_DA1 {A1=1;B1=0;C1=0;D1=1;}//D��ͨ�磬������ϵ�
#define Coil_OFF {A1=0;B1=0;C1=0;D1=0;}//ȫ���ϵ�

#define DataPort P0 //��������ݶ˿� ����������DataPort ����P0 �滻
#define SegPort  P3 //����λ���ݶ˿�

#define GPIO_DIG P0
#define GPIO_KEY P1

//sbit LSA=P2^2;
//sbit LSB=P2^3;
//sbit LSC=P2^4;
//sbit LEDC=P2^4;//����ָʾ��
//sbit LEDO=P2^5;//����ָʾ��
//sbit LEDU=P1^6;//����ָʾ��
//sbit LEDD=P1^7;//����ָʾ��					

u8 code DuanMa[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,
		                  	         0x77,0x7c,0x39,0x5e,0x79,0x71};// ��ʾ����ֵ0~F
u8 code WeiMa[]={0,1,2,3,4,5,6,7};

u8 KeyValue;	//������Ŷ�ȡ���ļ�ֵ
u8 N[5] = {0,0,0,0,0};			//�ڲ�����
u8 F[5] = {0,0,0,0,0};			//¥��
u8 out_up[5] = {0,0,0,0,0};  //�ⲿ����
u8 out_down[5] = {0,0,0,0,0};
u8 door[3]={0,0,0};
u8 state=0;
u8 Speed=2;
u8 floor=1;
u8 jump=0;
u8 j=0;
u8 i=0;


u8 TempData[8]; //�洢��ʾֵ��ȫ�ֱ���
u8 Judge(u8 s[]);
u8 Maxline(u8 s[]);
u8 Minline(u8 s[]);
u8 Status (u8 s);
void atfloar(void);
//void Ctrl_door(u8 s);
//void Open_door(void);
//void Close_door(void);
void WaitDelayMs(u8 t);
void Clear(u8 floor);
void DelayMs1(u16 t); 
void Machine (u8 s);
void KeyDown(void);
void KeyTrans(void);
void Latern(void);
void state_LED();
void Display(u8 FirstBit,u8 Num);
void Init_Timer0(void);

void main()
{	
	while(1)
	{
		 Init_Timer0();
		 Coil_OFF
 		 TempData[6]=DuanMa[floor];
		 if(j==1)
		 {
		 	LEDO=0;
		  	DelayMs1(3000);
			LEDO=1;
			LEDC=0;
			DelayMs1(1000);
			LEDC=1;
			j=0;
			i=0;
		 }
		 
		 state=Status(state);
		 state_LED();		 
		 Machine(state);
		 	
	}		
}


/**************************************************************************************
*		              ���ݼ�������												  *
�������ţ�S1��S8Ϊ�����ⲿ����
		  S9-S13Ϊ�ڲ�����
		  S14-S15Ϊ�����ſ���																				  
***************************************************************************************/
/*u8 code smgduan[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
					0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};//��ʾ0~F��ֵ*/
/*******************************************************************************
* �� �� ��         : delay
* ��������		   : ��ʱ������i=1ʱ����Լ��ʱ10us
*******************************************************************************/
void delay(u16 i)
{
	while(i--);	
}
/*******************************************************************************
* �� �� ��         : KeyDown
* ��������		   : ����а������²���ȡ��ֵ
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void KeyDown(void)
{	
	char a=0;
	KeyValue = 15;
//	u8 i[5]={0};
//	u8 j[5]={0};
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)//��ȡ�����Ƿ���
	{
		delay(1000);//��ʱ10ms��������
		if(GPIO_KEY!=0x0f)//�ٴμ������Ƿ���
		{	
			//������
			GPIO_KEY=0X0f;
			switch(GPIO_KEY)
			{
				case(0X07): KeyValue=0;break;
				case(0X0b):	KeyValue=1;break;
				case(0X0d): KeyValue=2;break;
				case(0X0e):	KeyValue=3;break;
			}
			//������
			GPIO_KEY=0Xf0;
			switch(GPIO_KEY)
			{
				case(0X70):	KeyValue=KeyValue;break;
				case(0Xb0):	KeyValue=KeyValue+4;break;
				case(0Xd0): KeyValue=KeyValue+8;break;
				case(0Xe0):	KeyValue=KeyValue+12;break;
			}
			while((a<50)&&(GPIO_KEY!=0xf0))	 //��ⰴ�����ּ��
			{
				delay(1000);
				a++;
			}
		}
	}
}
/*******************************************************************************
* �� �� ��         : KeyTrans
* ��������		   : ����ֵת��Ϊ��Ӧ����
* ע	��		   ��NΪ�ڲ����룬nΪ�ⲿ����,i���ڵ�ǰ¥���ⲿ�ϱ�����ʱ��1��j���ڵ�ǰ¥���ⲿ�±�����ʱ��1		
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void KeyTrans(void)
{
	switch(KeyValue)
	{
	 	case (0):  {F[0]=1;out_up[0]=1;} break;
		case (1):  {F[1]=1;out_up[1]=1;} break;
		case (2):  {F[1]=1;out_down[1]=1;} break;
		case (3):  {F[2]=1;out_up[2]=1;} break;
		case (4):  {F[2]=1;out_down[2]=1;} break;
		case (5):  {F[3]=1;out_up[3]=1;} break;
		case (6):  {F[3]=1;out_down[3]=1;} break;
		case (7):  {F[4]=1;out_down[4]=1;} break;
		case (8):  		N[0] = 1;		break;
		case (9):  		N[1] = 1;		break;
		case (10):  	N[2] = 1;		break;
		case (11):  	N[3] = 1;		break;
		case (12):  	N[4] = 1;		break;
		default :		break;
	}
}
/*******************************************************************************
* �� �� ��         : Status
* ��������		   : �жϵ���Ŀǰ������������л�������
* ע	��		   ��s=1��ʾ���У�s=2��ʾ����
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
u8 Status (u8 s)
{
	if(s==0)
	{	
		if (Judge(N))
		{
			if(Maxline(N)+1 > floor)
				s=1;
			else if(Maxline(N)+1 == floor)
				{
					s=0;
					N[floor-1]=0;
					j=1;
				}
			else 
				s=2;
		}
		else
		{
			if(Judge(F))
			{										 
				if((Maxline(F)+1) > floor)
					s=1;
				else if((Maxline(F)+1) == floor)
					{
						s=0;
						out_up[floor-1]=0;
						out_down[floor-1]=0;
						F[floor-1]=0;
						j=1;
					}
				else
					s=2;
			}
			else 
				s=0;
		}		
	}
	else if(s==2)
	{
		if (Judge(N))
		{
			if(Minline(N)+1 < floor)
				s=s;
			else 
				s=1;
		}
		else
		{
			if (Judge(out_down))
			{
				if(Minline(out_down)+1 < floor)
					s=s;
				else
					s=1;
			}
			else if(Judge(out_up))
				s=1;
			else 
				s=0;
		}
	}	
	else if(s==1)
	{
		if (Judge(N))
		{
			if(Maxline(N)+1 > floor)
				s=1;
			else 
				s=2;
		}
		else
		{
			if (Judge(F))
			{
				if(Maxline(F)+1 > floor)
					s=1;
				else
					s=2;
			}
			else if(Judge(out_down))
				s=2;
			else 
				s=0;	
		}			
	}
	return s;
}
/*******************************************************************************
* �� �� ��         : Judge
* ��������		   : �ж�һ�������Ƿ�ȫΪ��
* ע	��		   ����ȫΪ0����0�����򷵻�1
* ��    ��         : һ������
* ��    ��         : һ��ʵ��1��0
*******************************************************************************/
u8 Judge(u8 s[])
{
	u8 i = 0;
	while (i<5)		   
	{
		if(s[i] != 0)
			return 1;
		i++;
	}
	return 0;
}
/*******************************************************************************
* �� �� ��         : Maxline
* ��������		   : �ҳ�������������
* ע	��		   ����
* ��    ��         : һ������
* ��    ��         : һ��������������
*******************************************************************************/
u8 Maxline(u8 a[])
{
	u8 max = 0;
	u8 i = 0;
	while (i<5)
	{
		if(a[i]!=0)
			max = i;
		i++;		
	}
	return max;
}
/*******************************************************************************
* �� �� ��         : Minline
* ��������		   : �ҳ���������С����
* ע	��		   ����
* ��    ��         : һ������
* ��    ��         : һ����������С����
*******************************************************************************/
u8 Minline(u8 b[])
{
	u8 i = 0;
	while (i<5)
	{
		if(b[i]!=0)
			return i;
		i++;		
	}
	return 0;
}
/*------------------------------------------------
 uS��ʱ����������������� unsigned char t���޷���ֵ
 unsigned char �Ƕ����޷����ַ���������ֵ�ķ�Χ��
 0~255 ����ʹ�þ���12M����ȷ��ʱ��ʹ�û��,������ʱ
 �������� T=tx2+5 uS 
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
 while(--t);
}
/*------------------------------------------------
 mS��ʱ����������������� unsigned char t���޷���ֵ
 unsigned char �Ƕ����޷����ַ���������ֵ�ķ�Χ��
 0~255 ����ʹ�þ���12M����ȷ��ʱ��ʹ�û��
------------------------------------------------*/
void DelayMs(unsigned char t)
{
     
 while(t--)
 {
     //������ʱ1mS
     DelayUs2x(245);
	 DelayUs2x(245);
 }
}
void DelayMs1(unsigned int t)
{
     
 while(t--)
 {
     //������ʱ1mS
     DelayUs2x(245);
	 DelayUs2x(245);
 }
}
/*******************************************************************************
* �� �� ��         : Run_up
* ��������		   : ��������
* ע	��		   ����
* ��    ��         : ��
* ��    ��         : �������ת
*******************************************************************************/
void Run_up(void)
{
	unsigned int r=1;
 	unsigned int i=512;//��תһ��ʱ��
	Coil_OFF
  	while(i--)  //����
  	{         
    	Coil_A1                //����Coil_A1  ��{A1=1;B1=0;C1=0;D1=0;}����
    	DelayMs(Speed);         //�ı�����������Ե������ת�� ,����ԽС��ת��Խ��,����ԽС
    	Coil_B1
    	DelayMs(Speed);
    	Coil_C1
    	DelayMs(Speed);
    	Coil_D1
    	DelayMs(Speed);	
  	} 
  	Clear(floor);
	floor++;
 }
 /*******************************************************************************
* �� �� ��         : Run_down
* ��������		   : ��������
* ע	��		   ����
* ��    ��         : ��
* ��    ��         : �������ת
*******************************************************************************/
void Run_down(void)
{
	unsigned int r=0x01;
 	unsigned int i=512*r;//��תһ��ʱ��
	Coil_OFF
  	while(i--)  //����
  	{         
    	Coil_D1                //����Coil_A1  ��{A1=1;B1=0;C1=0;D1=0;}����
    	DelayMs(Speed);         //�ı�����������Ե������ת�� ,����ԽС��ת��Խ��,����ԽС
    	Coil_C1
    	DelayMs(Speed);
    	Coil_B1
    	DelayMs(Speed);
    	Coil_A1
    	DelayMs(Speed);	
  	}
  	Clear(floor-2);
	floor--;
 }
 /*******************************************************************************
* �� �� ��         : Machine
* ��������		   : ���Ƶ���
* ע	��		   ����
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Machine(u8 c)
{
	unsigned int r=0x01;
 	unsigned int i=512*r;//��תһ��ʱ��
 	if (c==0)
 	{
 		Coil_OFF
 	}
 	if (c==1)
		Run_up();
 	Coil_OFF
  	i=512*r;
  	if (c==2)
  	{
		Run_down();
  	}
 }

/*------------------------------------------------
              ��ʱ����ʼ���ӳ���
------------------------------------------------*/
void Init_Timer0(void)
{
 TMOD |= 0x01;	  //ʹ��ģʽ1��16λ��ʱ����ʹ��"|"���ſ�����ʹ�ö����ʱ��ʱ����Ӱ��		     
 //TH0=0x00;	      //������ֵ
 //TL0=0x00;
 EA=1;            //���жϴ�
 ET0=1;           //��ʱ���жϴ�
 TR0=1;           //��ʱ�����ش�
}
/*------------------------------------------------
                 ��ʱ���ж��ӳ���
------------------------------------------------*/
void Timer0_isr(void) interrupt 1 
{
 KeyDown();
 KeyTrans();

 TH0=(65536-2000)/256;		  //���¸�ֵ 2ms
 TL0=(65536-2000)%256;
 
 Display(0,8);       // ���������ɨ��

}

/*------------------------------------------------
 ��ʾ���������ڶ�̬ɨ�������
 ������� FirstBit ��ʾ��Ҫ��ʾ�ĵ�һλ���縳ֵ2��ʾ�ӵ���������ܿ�ʼ��ʾ
 ������0��ʾ�ӵ�һ����ʾ��
 Num��ʾ��Ҫ��ʾ��λ��������Ҫ��ʾ99��λ��ֵ���ֵ����2
------------------------------------------------*/
void Display(unsigned char FirstBit,unsigned char Num)
{
      static unsigned char i=0;
	  

	   DataPort=0;   //������ݣ���ֹ�н�����Ӱ


       SegPort=WeiMa[i+FirstBit]; //ȡλ�� 


       DataPort=TempData[i]; //ȡ��ʾ���ݣ�����

       
	   i++;
       if(i==Num)
	      i=0;
}
void Clear(u8 floor)
{	j=0;
	i=0;
	if(out_up[floor]&&out_down[floor])
	{
	 	if(state==1)
			{
				out_up[floor]=0;
				j=1;
				i=1;
			}
		if(state==2)
		{
			out_down[floor]=0;
			j=1;
			i=1;
		}
	}
	else if(out_up[floor])
	{
		if(state==1)
			{
				out_up[floor]=0;
				j=1;
				i=1;
			}
		if(state==2)
		{
			if(Minline(N)<floor)
				;
			else 
			{
				out_up[floor]=0;
				j=1;
				i=1;
			}
		}
	}
	else if(out_down[floor])
	{
	   if(state==2)
	   {
			out_down[floor]=0;
			j=1;
			i=1;
		}
		if(state==1)
		{
			if(Maxline(N)>floor)
				;
			else if(Maxline(out_down)>floor)
				;
			else 
			{
				out_down[floor]=0;
				j=1;
				i=1;
				
			}
		}
	}
	if(N[floor]==1)
	{
		i=1;
		j=1;
	}
	N[floor]=0;
	F[floor]=0;
} 


 void state_LED()
 {
 	if (i==0&&state==1)
	{
		LEDU=0;
		LEDD=1;
	}
	if (i==0&&state==2)
	{
		LEDU=1;
		LEDD=0;
	}
	if(i==1)
	{
	   	LEDU=1;
		LEDD=1;
	}
		
 }
