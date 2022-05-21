#include <stdio.h>
#include <string.h>
#include "esp82xx_driver.h"
#include "circular_buffer.h"
#include "hardware_modules.h"



extern portType esp82xx_port;
extern portType debug_port;


static void esp82xx_reset(void);
static void esp82xx_startup_test(void);
static  void esp82xx_sta_mode(void);
static  void esp82xx_ap_connect(char *ssid, char *password);
static void esp82xx_get_local_ip(void);
static void esp82xx_multi_conn_en(void);
static void esp82xx_create_tcp_server(void);
static int8_t send_server_data(char *str, int Link_ID);
static void esp82xx_single_conn_en(void);

char *home = " <html><head><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\
<title>HARDWARE CONTROL SERVER </title><style>\
 html { font-family: Helvetica;display: inline-block;margin: 0px auto;text-align: center;width:100%}\
 body{ margin-top: 50px;float: left;}  h1{color: #444444;margin: 50px auto 30px;}h3{color: #444444;margin-bottom: 50px;}\
 .button{display: block;width: 80px;background-color:#1abc9c;border: none;color: white;padding: 13px 30px;\
	   text-decoration: none;\
	   font-size: 25px;\
	   margin: 30px 30px 30px 30px;\
	   cursor: pointer;\
	   border-radius: 4px;\
	   float: left;\
  }\
  .button-on{ background-color: #1abc77;}.button-on:active{background-color: #16a085;}\
  .button-off {background-color: #ff2200;}\
  .button-off:active{background-color: #ff0000;}{ font-size: 14px;color: #888;margin-bottom: 10px;}\
  .module_label{margin: 30px 30px 30px 30px;}\
 </style></head><body><h1>HARDWARE CONTROL SERVER </h1>\
<p>VALVE  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp GRIPPER  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp PUMP State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp LIGHT  State: OFF</p>\
<a class=\"button button-on\" href=\"/valve\">ON</a>\
<a class=\"button button-on\" href=\"/gripper\">ON</a>\
<a class=\"button button-on\" href=\"/pump\">ON</a>\
<a class=\"button button-on\" href=\"/light\">ON</a>\
</body></html>";

char *valve = " <html><head><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\
<title>HARDWARE CONTROL SERVER </title><style>\
 html { font-family: Helvetica;display: inline-block;margin: 0px auto;text-align: center;width:100%}\
 body{ margin-top: 50px;float: left;}  h1{color: #444444;margin: 50px auto 30px;}h3{color: #444444;margin-bottom: 50px;}\
 .button{display: block;width: 80px;background-color:#1abc9c;border: none;color: white;padding: 13px 30px;\
	   text-decoration: none;\
	   font-size: 25px;\
	   margin: 30px 30px 30px 30px;\
	   cursor: pointer;\
	   border-radius: 4px;\
	   float: left;\
  }\
  .button-on{ background-color: #1abc77;}.button-on:active{background-color: #16a085;}\
  .button-off {background-color: #ff2200;}\
  .button-off:active{background-color: #ff0000;}{ font-size: 14px;color: #888;margin-bottom: 10px;}\
  .module_label{margin: 30px 30px 30px 30px;}\
 </style></head><body><h1>HARDWARE CONTROL SERVER </h1>\
<p>VALVE  State: ON &nbsp;&nbsp;&nbsp;&nbsp;&nbsp GRIPPER  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp PUMP State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp LIGHT  State: OFF</p>\
<a class=\"button button-off\" href=\"/\">OFF</a>\
<a class=\"button button-on\" href=\"/gripper\">ON</a>\
<a class=\"button button-on\" href=\"/pump\">ON</a>\
<a class=\"button button-on\" href=\"/light\">ON</a>\
</body></html>";

char *pump = " <html><head><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\
<title>HARDWARE CONTROL SERVER </title><style>\
 html { font-family: Helvetica;display: inline-block;margin: 0px auto;text-align: center;width:100%}\
 body{ margin-top: 50px;float: left;}  h1{color: #444444;margin: 50px auto 30px;}h3{color: #444444;margin-bottom: 50px;}\
 .button{display: block;width: 80px;background-color:#1abc9c;border: none;color: white;padding: 13px 30px;\
	   text-decoration: none;\
	   font-size: 25px;\
	   margin: 30px 30px 30px 30px;\
	   cursor: pointer;\
	   border-radius: 4px;\
	   float: left;\
  }\
  .button-on{ background-color: #1abc77;}.button-on:active{background-color: #16a085;}\
  .button-off {background-color: #ff2200;}\
  .button-off:active{background-color: #ff0000;}{ font-size: 14px;color: #888;margin-bottom: 10px;}\
  .module_label{margin: 30px 30px 30px 30px;}\
 </style></head><body><h1>HARDWARE CONTROL SERVER </h1>\
<p>VALVE  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp GRIPPER  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp PUMP State: ON &nbsp;&nbsp;&nbsp;&nbsp;&nbsp LIGHT  State: OFF</p>\
<a class=\"button button-on\" href=\"/valve\">ON</a>\
<a class=\"button button-on\" href=\"/gripper\">ON</a>\
<a class=\"button button-off\" href=\"/\">OFF</a>\
<a class=\"button button-on\" href=\"/light\">ON</a>\
</body></html>";


char *light = " <html><head><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\
<title>HARDWARE CONTROL SERVER </title><style>\
 html { font-family: Helvetica;display: inline-block;margin: 0px auto;text-align: center;width:100%}\
 body{ margin-top: 50px;float: left;}  h1{color: #444444;margin: 50px auto 30px;}h3{color: #444444;margin-bottom: 50px;}\
 .button{display: block;width: 80px;background-color:#1abc9c;border: none;color: white;padding: 13px 30px;\
	   text-decoration: none;\
	   font-size: 25px;\
	   margin: 30px 30px 30px 30px;\
	   cursor: pointer;\
	   border-radius: 4px;\
	   float: left;\
  }\
  .button-on{ background-color: #1abc77;}.button-on:active{background-color: #16a085;}\
  .button-off {background-color: #ff2200;}\
  .button-off:active{background-color: #ff0000;}{ font-size: 14px;color: #888;margin-bottom: 10px;}\
  .module_label{margin: 30px 30px 30px 30px;}\
 </style></head><body><h1>HARDWARE CONTROL SERVER </h1>\
<p>VALVE  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp GRIPPER  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp PUMP State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp LIGHT  State: ON</p>\
<a class=\"button button-on\" href=\"/valve\">ON</a>\
<a class=\"button button-on\" href=\"/gripper\">ON</a>\
<a class=\"button button-on\" href=\"/pump\">ON</a>\
<a class=\"button button-off\" href=\"/\">OFF</a>\
</body></html>";

char *gripper = " <html><head><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\
<title>HARDWARE CONTROL SERVER </title><style>\
 html { font-family: Helvetica;display: inline-block;margin: 0px auto;text-align: center;width:100%}\
 body{ margin-top: 50px;float: left;}  h1{color: #444444;margin: 50px auto 30px;}h3{color: #444444;margin-bottom: 50px;}\
 .button{display: block;width: 80px;background-color:#1abc9c;border: none;color: white;padding: 13px 30px;\
	   text-decoration: none;\
	   font-size: 25px;\
	   margin: 30px 30px 30px 30px;\
	   cursor: pointer;\
	   border-radius: 4px;\
	   float: left;\
  }\
  .button-on{ background-color: #1abc77;}.button-on:active{background-color: #16a085;}\
  .button-off {background-color: #ff2200;}\
  .button-off:active{background-color: #ff0000;}{ font-size: 14px;color: #888;margin-bottom: 10px;}\
  .module_label{margin: 30px 30px 30px 30px;}\
 </style></head><body><h1>HARDWARE CONTROL SERVER </h1>\
<p>VALVE  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp GRIPPER  State: ON &nbsp;&nbsp;&nbsp;&nbsp;&nbsp PUMP State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp LIGHT  State: OFF</p>\
<a class=\"button button-on\" href=\"/valve\">ON</a>\
<a class=\"button button-off\" href=\"/\">OFF</a>\
<a class=\"button button-on\" href=\"/pump\">ON</a>\
<a class=\"button button-on\" href=\"/light\">ON</a>\
</body></html>";

void esp82xx_sever_init(char * ssid, char *password)
{
	circular_buffer_init();

	esp82xx_reset();
	esp82xx_startup_test();
	esp82xx_sta_mode();
	esp82xx_ap_connect(ssid,password);
	esp82xx_get_local_ip();
	esp82xx_multi_conn_en();

}

void esp82xx_device_init(char * ssid, char *password)
{
	circular_buffer_init();
	esp82xx_reset();
	esp82xx_startup_test();
	esp82xx_sta_mode();
	esp82xx_ap_connect(ssid,password);
	esp82xx_single_conn_en();
}

/*Reset esp82xx*/

 static void esp82xx_reset(void)
{
	/*Send reset command*/
	buffer_send_string("AT+RST\r\n",esp82xx_port);

	/*Delay for 1 second*/
	systick_delay_ms(1000);

	/*Wait for "OK" response*/
	while(! (is_response("OK\r\n"))){}

	buffer_send_string("Reset was successful...\n\r",debug_port);
}


 /*Test AT startup*/
 static void esp82xx_startup_test(void)
 {
	 /*Clear esp uart buffer*/
	 buffer_clear(esp82xx_port);

	 /*Send test command*/
	buffer_send_string("AT\r\n",esp82xx_port);



     /*Wait for "OK" response*/
	while(! (is_response("OK\r\n"))){}

	buffer_send_string("AT Startup test successful...\n\r",debug_port);

}

 /*Enable station mode*/
 static  void esp82xx_sta_mode(void)
 {
	 /*Clear esp uart buffer*/
	 buffer_clear(esp82xx_port);

	 /*Send STA command*/
	buffer_send_string("AT+CWMODE=1\r\n",esp82xx_port);

    /*Wait for "OK" response*/
	while(! (is_response("OK\r\n"))){}

	buffer_send_string("STA Mode set successfully...\n\r",debug_port);

 }


 /*Connect to Access Apoint (AP)*/

 static  void esp82xx_ap_connect(char *ssid, char *password)
 {
	 char data[80];

	 /*Clear esp uart buffer*/
	 buffer_clear(esp82xx_port);

	 buffer_send_string("Connecting to access point...\n\r",debug_port);

	 /*Put ssid,password  and command into one packet*/
	 sprintf(data,"AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,password);

	 /*Send entire packet to esp uart*/
	 buffer_send_string(data,esp82xx_port);

	  /*Wait for "OK" response*/
	 while(! (is_response("OK\r\n"))){}

	 sprintf(data, "Connected : \"%s\"\r\n",ssid);

	 buffer_send_string(data,debug_port);

 }

 /*Get device local IP address*/

 static void esp82xx_get_local_ip(void)
 {
	 char data[80];
	 char buffer[20];
	 int len;

	 /*Clear buffer*/
	 buffer_clear(esp82xx_port);

	 /*Send 'get ip address ' command*/
	 buffer_send_string("AT+CIFSR\r\n",esp82xx_port);

	 /*Wait for 'Station IP'  response*/
	 while(!(is_response("CIFSR:STAIP,\""))){}

	 /*Copy IP address portion*/
	 while(!(copy_up_to_string("\"",buffer))){}

	  /*Wait for "OK" response*/
	 while(! (is_response("OK\r\n"))){}

	 len = strlen(buffer);

	 buffer[len - 1] = '\0';

	 sprintf(data,"Local IP Address :  %s \r\n",buffer);

	 buffer_send_string(data,debug_port);


 }


 /*Enable single connections*/
 static void esp82xx_single_conn_en(void)
 {
	 /*Clear buffer*/
	 buffer_clear(esp82xx_port);

	 /*Send 'enable single conn cmd'*/
	 buffer_send_string("AT+CIPMUX=0\r\n",esp82xx_port);

	  /*Wait for "OK" response*/
	 while(! (is_response("OK\r\n"))){}

	 buffer_send_string("Single connection enabled....\n\r",debug_port);

 }

 /*Enable multiple connections*/
 static void esp82xx_multi_conn_en(void)
 {
	 /*Clear buffer*/
	 buffer_clear(esp82xx_port);

	 /*Send 'enable multi conn cmd'*/
	 buffer_send_string("AT+CIPMUX=1\r\n",esp82xx_port);

	  /*Wait for "OK" response*/
	 while(! (is_response("OK\r\n"))){}

	 buffer_send_string("Multiple connections enabled....\n\r",debug_port);

 }


 /*Creat tcp server*/

 static void esp82xx_create_tcp_server(void)
 {
	 /*Clear buffer*/
	 buffer_clear(esp82xx_port);

	 /*Send 'create tcp server cmd'*/
	 buffer_send_string("AT+CIPSERVER=1,80\r\n",esp82xx_port);

	  /*Wait for "OK" response*/
	 while(! (is_response("OK\r\n"))){}

	 buffer_send_string("Please connect to the Local IP above....\n\r",debug_port);

 }


 /*Send data to the server*/

static int8_t send_server_data(char *str, int Link_ID)
 {
	 char data[80];

	 /*Get length of data to send*/
	 int len = strlen(str);

	 /*Send link id and data length*/
	 sprintf(data,"AT+CIPSEND=%d,%d\r\n",Link_ID,len);
	 buffer_send_string(data,esp82xx_port);

	  /*Wait for ">" response*/
	  while(! (is_response(">"))){}

	  /*Send data*/
	  buffer_send_string(str,esp82xx_port);

	  /*Wait for "SEND OK" response*/
	   while(! (is_response("SEND OK"))){}

	  /*Close connection*/
	   sprintf(data,"AT+CIPCLOSE=5\r\n");
	   buffer_send_string(data,esp82xx_port);

		/*Wait for "OK" response*/
	   while(! (is_response("OK\r\n"))){}

	   return 1;

 }

void process_server_data(char *str,int link_id)
{
	/*Check if strings are the same*/
	if((strcmp(str,"/valve")) == 0)
	{
		/*Serve valve page*/
		send_server_data(valve,link_id);
	}

	else if((strcmp(str,"/gripper")) == 0)
	{
		/*Serve gripper page*/
		send_server_data(gripper,link_id);
	}

	else if((strcmp(str,"/pump")) == 0)
	{
		/*Serve pump page*/
		send_server_data(pump,link_id);
	}

	else if((strcmp(str,"/light")) == 0)
	{
		/*Serve light page*/
		send_server_data(light,link_id);
	}
	else
	{
		/*Serve home page*/
		send_server_data(home,link_id);

	}

}

 void server_begin(void)
 {
	 char dest_buffer[64] ={0};
	 char link_id;

	 while(!(get_next_strs("+IPD,",1,&link_id))){}

	 link_id -=48;

	 while(!(copy_up_to_string("HTTP/1.1",dest_buffer))){};

	 if(find_str("/valve",dest_buffer)== 1)
	 {
		 process_server_data("/valve",link_id);

		 //Do something..
		 valve_on();
		 pump_off();
		 light_off();
		 gripper_off();

		 buffer_send_string("\r\nValve module diagnostics starting...\r\n",debug_port);
	 }
	 else if(find_str("/gripper",dest_buffer)== 1)
		{

		 process_server_data("/gripper",link_id);

		 //Do something..
			 valve_off();
			 pump_off();
			 light_off();
			 gripper_on();

			 buffer_send_string("\r\nGripper module diagnostics starting...\r\n",debug_port);

		}

	 else if(find_str("/light",dest_buffer)== 1)
		{

		 process_server_data("/light",link_id);

		 //Do something..
			 valve_off();
			 pump_off();
			 light_on();
			 gripper_off();

			 buffer_send_string("\r\nLight module diagnostics starting...\r\n",debug_port);
		}


	 else if(find_str("/pump",dest_buffer)== 1)
		{

		 process_server_data("/pump",link_id);

		 //Do something..
			 valve_off();
			 pump_on();
			 light_off();
			 gripper_off();

			 buffer_send_string("\r\nPump module diagnostics starting...\r\n",debug_port);
		}
	 else
	 {
		 process_server_data("/",link_id);
	 }


 }

static void buffer_reset(char *buffer)
{
	int len = strlen(buffer);

	for(int i=0 ; i<len ; i++)
	{
		buffer[i] = '\0';
	}
}

void espxx_thingspeak_send_multi_field(char *apikey, int num_of_fields, uint32_t value[])
{
	char local_buf1[100] ={0};
	char local_buf2[100] ={0};
	char local_buf3[100] ={0};

	/*Establish TCP connection with ThingSpeak*/
	buffer_send_string("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n",esp82xx_port);

	 /*Wait for "OK" response*/
	 while(! (is_response("OK\r\n"))){}

	 sprintf(local_buf1,"GET /update?api_key=%s",apikey);

	 for(int i=0 ;i<num_of_fields;i++)
	 {
		sprintf(local_buf3,"&field%d=%lu",i+1,value[i]);
		strcat(local_buf1,local_buf3);
	 }

	 strcat(local_buf1,"\r\n");
	 int len = strlen(local_buf1);

	 sprintf(local_buf2,"AT+CIPSEND=%d\r\n",len);

	 /*Send buffer length*/
	 buffer_send_string(local_buf2,esp82xx_port);

	 /*Wait for ">" response*/
	  while(! (is_response(">"))){}

	  /*Send parameters*/
	  buffer_send_string(local_buf1,esp82xx_port);

	 /*Wait for "SEND OK" response*/
	 while(! (is_response("SEND OK\r\n"))){}

	  /*Wait for "CLOSED" response*/
	  while(! (is_response("CLOSED"))){}


	 /*Clear buffers*/
	 buffer_reset(local_buf1);
	 buffer_reset(local_buf2);
	 buffer_reset(local_buf3);


	 /*Re-initialize circular buffer*/
	 circular_buffer_init();
}

void espxx_thingspeak_send(char *apikey, int field_no, uint32_t value)
{
	char local_buf1[100] ={0};
	char local_buf2[100] ={0};

	/*Establish TCP connection with ThingSpeak*/
	buffer_send_string("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n",esp82xx_port);

	 /*Wait for "OK" response*/
	 while(! (is_response("OK\r\n"))){}

	 sprintf(local_buf1,"GET /update?api_key=%s&field%d=%lu\r\n",apikey,field_no,value);

	 int len = strlen(local_buf1);
	 sprintf(local_buf2,"AT+CIPSEND=%d\r\n",len);

	 /*Send buffer length*/
	 buffer_send_string(local_buf2,esp82xx_port);

	 /*Wait for ">" response*/
	  while(! (is_response(">"))){}

	  /*Send parameters*/
	  buffer_send_string(local_buf1,esp82xx_port);

	 /*Wait for "SEND OK" response*/
	 while(! (is_response("SEND OK\r\n"))){}

	 /*Wait for "CLOSED" response*/
	  while(! (is_response("CLOSED"))){}


	 /*Clear buffers*/
	 buffer_reset(local_buf1);
	 buffer_reset(local_buf2);

	 /*Re-initialize circular buffer*/
	 circular_buffer_init();


}
void USART1_IRQHandler(void)
{
	esp82_uart_callback();
}


void USART2_IRQHandler(void)
{
	debug_uart_callback();
}





