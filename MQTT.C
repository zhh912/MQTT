#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include  "mqtt.h"


#define	MQTT_CHECK_CLIENT	0

static	unsigned	short	msgId	=	0x1516;

/*
  communicate port registes. 
  You can registe MQTT on Uart1,Uart2,SPI,Or RJ45
*/
struct
{
	unsigned	char	(*f)(unsigned	char *s,unsigned	short len);
}Mqtt_Struct;
//

/*
this function can calculate the length of Mqtt's field eg topic or msg
*/
static  unsigned	char	MQTT_Field_Len(unsigned	char	*str)
{
	unsigned	int	len=0;
	len	=	strlen((const char *)str);
	if(len<128)
		return	1;
	else	if(len<16384)
		return	2;
	else	if(len<2097152)
		return	3;
	else	if(len<268435456)
		return	4;
	else
		return	0x55;
}
//


/*
  communicate port registes. 
  You can registe MQTT on Uart1,Uart2,SPI,Or RJ45
*/
unsigned	char	MQTT_Register_Port(unsigned	char	(*func)(unsigned	char *s,unsigned	short len))
{
	Mqtt_Struct.f	=	func;
	return	0;
}
//




/*
*/
unsigned	char	MQTT_Login(unsigned	char*	client,unsigned short keepAlivetime,unsigned char* user,unsigned char* password)
{

	unsigned	int		index	=	0;
  unsigned	int		len	=	0;
	unsigned	char	*TEMP;

  msgId = 0x1516;
	/*18 is length of MQIsdp string + "MQIsdp" and so as client \user\password */
  
/*
  name                 |         lenth
flag                   |            1
remain_length          |            1-4
lengthMQIsdp           |            2
MQIsdp                 |            6
version                |            2
keepAlivetime          |            2
lengthClient           |            2
Client                 |            x
lengthUser             |            2
USER                   |            2
lengthPassword         |            2
Password               |            x
*/	

  /*MSG length*/
	len=12+	2 +strlen((const char *)client)+
          2 +strlen((const char *)user)+
          2 +strlen((const char *)password);


	if(len<128)
		TEMP	=	(unsigned char *)malloc(len+2);
	else	if(len<16384)
		TEMP	=	(unsigned char *)malloc(len+3);
	else	if(len<2097152)
		TEMP	=	(unsigned char *)malloc(len+4);
	else	if(len<268435456)
		TEMP	=	(unsigned char *)malloc(len+5);
	else
		return	0x55;//too long
	
  
	
	


	TEMP[index]	=	0x10;//??????
	index++;
  do
  {
    TEMP[index]	=	len%128;
    len	=	len/128;
    if(len)
    {
      TEMP[index]	=	TEMP[index] |0x80;
      index++;
      
      return 'C';//协议不支持此长度的报文
    }
    //
    
  }
  while(len);

  
	index++;
  
  
	TEMP[index++]	=	0x00;
	TEMP[index++]	=	0x06;//????????
	TEMP[index++]	=	0x4D;
	TEMP[index++]	=	0x51;
	TEMP[index++]	=	0x49;
	TEMP[index++]	=	0x73;
	TEMP[index++]	=	0x64;
	TEMP[index++]	=	0x70;//????????
	TEMP[index++]=	0x03;//	????
	TEMP[index++]=	0xC2;// 	????????? ???
  
  
	TEMP[index++]=	keepAlivetime/0xff;
	TEMP[index++]=	keepAlivetime%0xff;// 	??????,
  
  
  
  len=strlen((const char *)client);
	TEMP[index++]=	len/0xff;
	TEMP[index++]=	len%0xff;// 	client
  
  
	strcpy((char *)(TEMP+index),(const char *)client);
	
  
	index	=	index+strlen((const char *)client);

	len	=	strlen((const char *)user);

	TEMP[index++]	=	len/256;
	TEMP[index++]	=	len%256;

	strcpy((char *)(TEMP+index),(const char *)user);

	index	=	index+strlen((const char *)user);		

	len	=	strlen((const char *)password);

	TEMP[index++]	=	len/256;
	TEMP[index++]	=	len%256;

	strcpy((char *)(TEMP+index),(const char *)password);

	index	=	index+strlen((const char *)password);	

	len	=	index;

	
	Mqtt_Struct.f(TEMP,len);

	free(TEMP);

	return	0;
	

		
}
//


unsigned	char	MQTT_LoginOK(unsigned	char	*s)
{
	unsigned	char	ack[]={0x20,0x02};
	if(memcmp(s,ack,sizeof(ack)	!=	0))
	{
		return	1;
	}
	
	else
	{
		return	0;
	}
	//

}
//

unsigned	char	MQTT_Subscribe(unsigned	char	*topic,	unsigned	char	Qos)
{
	unsigned	int		len	=	0;
	unsigned	char	*temp;
	len	=	strlen((const char *)topic)+10;
	
	temp	=	(unsigned char *)malloc(len);
	
	//flag	len	msgID	TopicLen	topic	Qos
	
	
	temp[0]	=	0x82;
	
	temp[1]	=	0x02+0x02+strlen((const char *)topic)+0x01;
	
	temp[2]	=	((unsigned	char)(msgId>>8))	&0xFF;
	temp[3]	=	((unsigned	char)(msgId>>0))	&0xFF;	
	
	temp[4]	=	strlen((const char *)topic)/256;
	temp[5]	=	strlen((const char *)topic)%256;
	
	memcpy(temp+6,topic,strlen((const char *)topic));
  
	len	=	6+ (unsigned int)strlen((const char *)topic);
  
	
	temp[len]	=	Qos;
	len++;
	
	Mqtt_Struct.f(temp,len);
	free(temp);
	msgId++;
  
  return 0;
}
//


unsigned	char	MQTT_SubscribeOK(unsigned	char	*s)
{
	if(((*s)&0x90)==0x90)
	{
		msgId=((unsigned	short)(*(s+2))&0xff00) | ((unsigned	short)(*(s+3))&0x00FF);
		return	0;
	}
	//
	return 1;
}
//

unsigned	char	MQTT_Publish(unsigned	char *topic,	unsigned	char *msg )
{
	//flag	len		topiclen	topic	    msgID	      msg
	
	unsigned	int		len		=	0;
	unsigned	int	  index	=	0;
	unsigned	char	*temp;
	
	len	=2+strlen((const char *)topic)+2+strlen((const char *)msg);
	
  temp=(unsigned  char *)malloc(len+5);
  
  
  
	temp[index]=0x32;
	//index++;
	do
  {
    temp[++index]	=	len%128;
    len	=	len/128;
    if(len)
    {
      temp[index]	=	temp[index] |0x80;   
      //index++;
    }
    //
       
  }
  //
	while(len);
  
	
	index++;
	temp[index]	=	strlen((const char *)topic)/256;
	index++;
	temp[index]	=	strlen((const char *)topic)%256;
	
	index++;
	
	memcpy(temp+index,topic,strlen((const char *)topic));
	
	index+=strlen((const char *)topic);
	
	temp[index]	=	(msgId>>8)&0xff;
	index++;
	temp[index]	=	(msgId>>0)&0xff;
	index++;
	
  
  
	memcpy(temp+index,msg,strlen((const char *)msg));
  
  
  index += strlen((const char *)msg);
  
	Mqtt_Struct.f(temp,index);
	
	free(temp);
	return	0;
	

}
//


unsigned	char	MQTT_PublishOK(unsigned	char	*s)
{
	if(((*s)&0x40)==0x40)
	{
		msgId=((unsigned	short)(*(s+2))&0xff00) | ((unsigned	short)(*(s+3))&0x00FF);
		return	0;
	}
	//
	return 1;
}
//

/*
U can search topic's msg
*/
unsigned	int	MQTT_GetCommand(unsigned	char *s,unsigned  int len, unsigned  char* topic)
{
	//寻找到报文中一段字符串，然后把这段字符串丢给sscanf来处理
  //32   0D    00 06   74 6F 70 69 63 31    15 16     61 61 61 
  //flag len   len     topic                  msgId     Msg

  unsigned  int lenOfTopic  = 0;
  unsigned  int loopi = 0;
  
  lenOfTopic  = strlen((const char*)topic);

  for(loopi=0;loopi<len;loopi++)
  {
    if(!memcmp(s+loopi,topic,lenOfTopic))
    {
      break;
    }
    //
  }
  //
  msgId = ((((unsigned short)(*(s+loopi+lenOfTopic  )))<<8)&0xFF00)   |  
          ((((unsigned short)(*(s+loopi+lenOfTopic+1)))<<0)&0x00FF);
  return  (sizeof(msgId)+loopi+lenOfTopic);

}	
//


/*               example                         */
#if 0
MQTT_Register_Port(RS232_2_Send);



MQTT_Login("qweqe",100,"root","password");
    
  
MQTT_Subscribe("topic1",1);
    

MQTT_Publish("topic1","aaa");

{
  unsigned  int num,num1;
  
  //rev msg
  unsigned  char  message[]={0x32,0x0D,0x00 ,0x06,0x74,0x6F,0x70,0x69,0x63,0x31,0x15,0x16,0x61,0x31,0x31};
  
  num = MQTT_GetCommand(message,sizeof(message),"topic1");
  sscanf(message+num,"a%d",&num1);
  num1=num1;
}
//




#endif
/*               example                         */


