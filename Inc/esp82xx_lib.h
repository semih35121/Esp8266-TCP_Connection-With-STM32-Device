#ifndef ESP82XX_LIB_H_
#define ESP82XX_LIB_H_
#include <stdint.h>

void esp82xx_sever_init(char * ssid, char *password);
void server_begin(void);
void espxx_thingspeak_send(char *apikey, int field_no, uint32_t value);
void esp82xx_device_init(char * ssid, char *password);
void espxx_thingspeak_send_multi_field(char *apikey, int num_of_fields, uint32_t value[]);

#endif /* ESP82XX_LIB_H_ */
