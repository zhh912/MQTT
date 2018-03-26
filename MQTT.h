#ifndef __MQTT_H__
#define __MQTT_H__



extern  unsigned	char	MQTT_Register_Port(unsigned	char	(*func)(unsigned	char *s,unsigned	short len));
extern  unsigned	char	MQTT_Login(unsigned	char*	client,unsigned short keepAlivetime,unsigned char* user,unsigned char* password);
extern  unsigned	char	MQTT_LoginOK(unsigned	char	*s);
extern  unsigned	char	MQTT_Subscribe(unsigned	char	*topic,	unsigned	char	Qos);
extern  unsigned	char	MQTT_SubscribeOK(unsigned	char	*s);
extern  unsigned	char	MQTT_Publish(unsigned	char *topic,	unsigned	char *msg );
extern  unsigned	char	MQTT_PublishOK(unsigned	char	*s);
extern  unsigned	int	  MQTT_GetCommand(unsigned	char *s,unsigned  int len, unsigned  char* topic);





#endif


