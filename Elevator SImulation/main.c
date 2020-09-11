#include "reg51.h"    //此文件中定义了单片机的一些特殊功能寄存器
#include "lcd.h"

typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;
					
sbit A1=P1^0; //定义步进电机连接端口
sbit B1=P1^1;
sbit C1=P1^2;
sbit D1=P1^3;

sbit LEDO=P2^0;
sbit LEDC=P2^1;
sbit LEDU=P2^6;
sbit LEDD=P2^7;

#define Coil_A1 {A1=1;B1=0;C1=0;D1=0;}//A相通电，其他相断电
#define Coil_B1 {A1=0;B1=1;C1=0;D1=0;}//B相通电，其他相断电
#define Coil_C1 {A1=0;B1=0;C1=1;D1=0;}//C相通电，其他相断电
#define Coil_D1 {A1=0;B1=0;C1=0;D1=1;}//D相通电，其他相断电
#define Coil_AB1 {A1=1;B1=1;C1=0;D1=0;}//AB相通电，其他相断电
#define Coil_BC1 {A1=0;B1=1;C1=1;D1=0;}//BC相通电，其他相断电
#define Coil_CD1 {A1=0;B1=0;C1=1;D1=1;}//CD相通电，其他相断电
#define Coil_DA1 {A1=1;B1=0;C1=0;D1=1;}//D相通电，其他相断电
#define Coil_OFF {A1=0;B1=0;C1=0;D1=0;}//全部断电

#define DataPort P0 //定义段数据端口 程序中遇到DataPort 则用P0 替换
#define SegPort  P3 //定义位数据端口

#define GPIO_DIG P0
#define GPIO_KEY P1

//sbit LSA=P2^2;
//sbit LSB=P2^3;
//sbit LSC=P2^4;
//sbit LEDC=P2^4;//关门指示灯
//sbit LEDO=P2^5;//开门指示灯
//sbit LEDU=P1^6;//上行指示灯
//sbit LEDD=P1^7;//下行指示灯					

u8 code DuanMa[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,
		                  	         0x77,0x7c,0x39,0x5e,0x79,0x71};// 显示段码值0~F
u8 code WeiMa[]={0,1,2,3,4,5,6,7};

u8 KeyValue;	//用来存放读取到的键值
u8 N[5] = {0,0,0,0,0};			//内部按键
u8 F[5] = {0,0,0,0,0};			//楼层
u8 out_up[5] = {0,0,0,0,0};  //外部上升
u8 out_down[5] = {0,0,0,0,0};
u8 door[3]={0,0,0};
u8 state=0;
u8 Speed=2;
u8 floor=1;
u8 jump=0;
u8 j=0;
u8 i=0;


u8 TempData[8]; //存储显示值的全局变量
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
*		              电梯键盘设置												  *
按键安排：S1—S8为各层外部按键
		  S9-S13为内部按键
		  S14-S15为开关门控制																				  
***************************************************************************************/
/*u8 code smgduan[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
					0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};//显示0~F的值*/
/*******************************************************************************
* 函 数 名         : delay
* 函数功能		   : 延时函数，i=1时，大约延时10us
*******************************************************************************/
void delay(u16 i)
{
	while(i--);	
}
/*******************************************************************************
* 函 数 名         : KeyDown
* 函数功能		   : 检测有按键按下并读取键值
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void KeyDown(void)
{	
	char a=0;
	KeyValue = 15;
//	u8 i[5]={0};
//	u8 j[5]={0};
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)//读取按键是否按下
	{
		delay(1000);//延时10ms进行消抖
		if(GPIO_KEY!=0x0f)//再次检测键盘是否按下
		{	
			//测试列
			GPIO_KEY=0X0f;
			switch(GPIO_KEY)
			{
				case(0X07): KeyValue=0;break;
				case(0X0b):	KeyValue=1;break;
				case(0X0d): KeyValue=2;break;
				case(0X0e):	KeyValue=3;break;
			}
			//测试行
			GPIO_KEY=0Xf0;
			switch(GPIO_KEY)
			{
				case(0X70):	KeyValue=KeyValue;break;
				case(0Xb0):	KeyValue=KeyValue+4;break;
				case(0Xd0): KeyValue=KeyValue+8;break;
				case(0Xe0):	KeyValue=KeyValue+12;break;
			}
			while((a<50)&&(GPIO_KEY!=0xf0))	 //检测按键松手检测
			{
				delay(1000);
				a++;
			}
		}
	}
}
/*******************************************************************************
* 函 数 名         : KeyTrans
* 函数功能		   : 将键值转化为相应操作
* 注	释		   ：N为内部输入，n为外部输入,i下在当前楼层外部上被按下时置1，j下在当前楼层外部下被按下时置1		
* 输    入         : 无
* 输    出         : 无
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
* 函 数 名         : Status
* 函数功能		   : 判断电梯目前运行情况，上行还是下行
* 注	释		   ：s=1表示上行，s=2表示下行
* 输    入         : 无
* 输    出         : 无
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
* 函 数 名         : Judge
* 函数功能		   : 判断一个数组是否全为零
* 注	释		   ：若全为0返回0，否则返回1
* 输    入         : 一个数组
* 输    出         : 一个实数1或0
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
* 函 数 名         : Maxline
* 函数功能		   : 找出数组中最大的项
* 注	释		   ：无
* 输    入         : 一个数组
* 输    出         : 一个整数，最大的项
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
* 函 数 名         : Minline
* 函数功能		   : 找出数组中最小的项
* 注	释		   ：无
* 输    入         : 一个数组
* 输    出         : 一个整数，最小的项
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
 uS延时函数，含有输入参数 unsigned char t，无返回值
 unsigned char 是定义无符号字符变量，其值的范围是
 0~255 这里使用晶振12M，精确延时请使用汇编,大致延时
 长度如下 T=tx2+5 uS 
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
 while(--t);
}
/*------------------------------------------------
 mS延时函数，含有输入参数 unsigned char t，无返回值
 unsigned char 是定义无符号字符变量，其值的范围是
 0~255 这里使用晶振12M，精确延时请使用汇编
------------------------------------------------*/
void DelayMs(unsigned char t)
{
     
 while(t--)
 {
     //大致延时1mS
     DelayUs2x(245);
	 DelayUs2x(245);
 }
}
void DelayMs1(unsigned int t)
{
     
 while(t--)
 {
     //大致延时1mS
     DelayUs2x(245);
	 DelayUs2x(245);
 }
}
/*******************************************************************************
* 函 数 名         : Run_up
* 函数功能		   : 电梯上行
* 注	释		   ：无
* 输    入         : 无
* 输    出         : 电机正向转
*******************************************************************************/
void Run_up(void)
{
	unsigned int r=1;
 	unsigned int i=512;//旋转一周时间
	Coil_OFF
  	while(i--)  //正向
  	{         
    	Coil_A1                //遇到Coil_A1  用{A1=1;B1=0;C1=0;D1=0;}代替
    	DelayMs(Speed);         //改变这个参数可以调整电机转速 ,数字越小，转速越大,力矩越小
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
* 函 数 名         : Run_down
* 函数功能		   : 电梯下行
* 注	释		   ：无
* 输    入         : 无
* 输    出         : 电机反向转
*******************************************************************************/
void Run_down(void)
{
	unsigned int r=0x01;
 	unsigned int i=512*r;//旋转一周时间
	Coil_OFF
  	while(i--)  //反向
  	{         
    	Coil_D1                //遇到Coil_A1  用{A1=1;B1=0;C1=0;D1=0;}代替
    	DelayMs(Speed);         //改变这个参数可以调整电机转速 ,数字越小，转速越大,力矩越小
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
* 函 数 名         : Machine
* 函数功能		   : 控制电梯
* 注	释		   ：无
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Machine(u8 c)
{
	unsigned int r=0x01;
 	unsigned int i=512*r;//旋转一周时间
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
              定时器初始化子程序
------------------------------------------------*/
void Init_Timer0(void)
{
 TMOD |= 0x01;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响		     
 //TH0=0x00;	      //给定初值
 //TL0=0x00;
 EA=1;            //总中断打开
 ET0=1;           //定时器中断打开
 TR0=1;           //定时器开关打开
}
/*------------------------------------------------
                 定时器中断子程序
------------------------------------------------*/
void Timer0_isr(void) interrupt 1 
{
 KeyDown();
 KeyTrans();

 TH0=(65536-2000)/256;		  //重新赋值 2ms
 TL0=(65536-2000)%256;
 
 Display(0,8);       // 调用数码管扫描

}

/*------------------------------------------------
 显示函数，用于动态扫描数码管
 输入参数 FirstBit 表示需要显示的第一位，如赋值2表示从第三个数码管开始显示
 如输入0表示从第一个显示。
 Num表示需要显示的位数，如需要显示99两位数值则该值输入2
------------------------------------------------*/
void Display(unsigned char FirstBit,unsigned char Num)
{
      static unsigned char i=0;
	  

	   DataPort=0;   //清空数据，防止有交替重影


       SegPort=WeiMa[i+FirstBit]; //取位码 


       DataPort=TempData[i]; //取显示数据，段码

       
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
