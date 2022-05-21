#include <stdio.h>
#include "stm32f4xx.h"
#include "esp82xx_driver.h"
#include "fifo.h"
#include "circular_buffer.h"
#include "esp82xx_lib.h"
#include "hardware_modules.h"
#include "adc.h"

#define SSID_NAME  "TP-Link_9A4E"
#define PASSKEY    "94933581"

uint32_t sensor_value,filtered_value;
uint32_t packet[2];

int main()
{
    /*Initialize debug uart*/
	debug_uart_init();

	/*Initialize esp82xx uart*/
	esp_uart_init();

	/*Initialize RS pin*/
	esp_rs_pin_init();

	/*Initialize hardware modules*/
	light_init();
	valve_init();
	pump_init();
	gripper_init();

	/*Initialize adc*/
	pa1_adc_init();

	/*Start conversion*/
	start_conversion();

	/*Enable RS pin*/
	esp_rs_pin_enable();

    /*Initialize device*/
	esp82xx_device_init(SSID_NAME,PASSKEY);

	while(1)
	{

		sensor_value = adc_read();
		filtered_value = sensor_value + 100;

		packet[0] = sensor_value;
		packet[1] = filtered_value;

		espxx_thingspeak_send_multi_field("A8LYPSITQ12KERVY",2,packet);

		systick_delay_ms(3000);
	}

}

