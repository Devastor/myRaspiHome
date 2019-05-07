{\rtf1\ansi\ansicpg1251\cocoartf1671\cocoasubrtf400
{\fonttbl\f0\fswiss\fcharset0 Helvetica;}
{\colortbl;\red255\green255\blue255;}
{\*\expandedcolortbl;;}
\paperw11900\paperh16840\margl1440\margr1440\vieww29500\viewh19600\viewkind0
\pard\tx566\tx1133\tx1700\tx2267\tx2834\tx3401\tx3968\tx4535\tx5102\tx5669\tx6236\tx6803\pardirnatural\partightenfactor0

\f0\fs24 \cf0 #include <cstdlib>\
#include <stdlib.h>\
#include <iostream>\
#include <sstream>\
#include <string>\
#include <unistd.h>\
//#include <RF24/RF24.h>\
\
#include "/home/pi/libraries/RF24_FRESH/RF24-master/RF24.h"\
#include </home/pi/libraries/bcm2835-1.52/src/bcm2835.h>\
#include <stdio.h>\
#include </home/pi/libraries/curl-7.54.1/include/curl/curl.h>\
#include <fstream>\
#include <stdio.h>\
#include <string.h>    //strlen\
#include <stdlib.h>    //strlen\
#include <cppcms/application.h>\
#include <cppcms/applications_pool.h>\
#include <cppcms/service.h>\
#include <cppcms/http_response.h>\
#include <cppcms/url_dispatcher.h>\
#include <iostream>\
#include "content.h"\
#include <sys/time.h>\
#include <SFML/System.hpp>\
#include <cppcms/rpc_json.h>\
#include <booster/intrusive_ptr.h>\
\
#define sleep(n)    Sleep(n)\
\
#define LED_PIN1 RPI_BPLUS_GPIO_J8_37\
#define LED_PIN2 RPI_BPLUS_GPIO_J8_35\
#define LED_PIN3 RPI_BPLUS_GPIO_J8_33\
#define LED_PIN4 RPI_BPLUS_GPIO_J8_31\
#define LED_PIN5 RPI_BPLUS_GPIO_J8_29\
#define LED_PIN6 RPI_BPLUS_GPIO_J8_22\
#define buzzPin	 RPI_BPLUS_GPIO_J8_12\
		\
#include <boost/bind.hpp>\
\
#include <bluetooth/bluetooth.h>\
#include <bluetooth/rfcomm.h>\
#include <bluetooth/hci.h>\
#include <bluetooth/hci_lib.h>\
#include <sys/socket.h>\
\
#include <unistd.h>			//Used for UART\
#include <fcntl.h>			//Used for UART\
#include <termios.h>		//Used for UART\
\
#include <boost/algorithm/string.hpp>\
\
\
#include <sys/time.h>\
                 \
     \
using namespace std;\
//#include <boost/bind.hpp>\
//using boost::bind;\
\
// Setup for GPIO 15 CE and CE0 CSN with SPI Speed @ 8Mhz\
//RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);\
RF24 radio(22, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);\
\
// RPi generic:\
//RF24 radio(22,0);\
//RF24 radio(22,0);\
\
\
// Radio pipe addresses for the 2 nodes to communicate.\
//const uint8_t pipes[][6] = \{0xAABBCCDD11,0xAABBCCDD12\};\
\
\
char msg[10] = \{0, 0, 0, 0, 0, 0, 0, 0, 0, '\\0'\};\
\
bool LED_state[9] = \{0\};                           	// \uc0\u1089 \u1086 \u1089 \u1090 \u1086 \u1103 \u1085 \u1080 \u1077  \u1089 \u1074 \u1077 \u1090 \u1072 \
bool WND_state[2] = \{0\};                            // \uc0\u1089 \u1086 \u1089 \u1090 \u1086 \u1103 \u1085 \u1080 \u1077  \u1086 \u1082 \u1085 \u1072 \
char readString[] = \{0\};                            // \uc0\u1089 \u1090 \u1088 \u1086 \u1082 \u1072  GET-\u1079 \u1072 \u1087 \u1088 \u1086 \u1089 \u1072 \
char checkStr[] = \{0\};                              // \uc0\u1089 \u1090 \u1088 \u1086 \u1082 \u1072  \u1087 \u1088 \u1086 \u1074 \u1077 \u1088 \u1082 \u1080 \
bool auth = false;                                 // \uc0\u1072 \u1074 \u1090 \u1086 \u1088 \u1080 \u1079 \u1072 \u1094 \u1080 \u1103 \
int startDataRecieved[7] = \{0\};                     // \uc0\u1087 \u1088 \u1080 \u1096 \u1083 \u1080  \u1083 \u1080  \u1085 \u1072 \u1095 \u1072 \u1083 \u1100 \u1085 \u1099 \u1077  \u1076 \u1072 \u1085 \u1085 \u1099 \u1077 \
int dataRecieved[7] = \{0\};                     // \uc0\u1087 \u1088 \u1080 \u1096 \u1083 \u1080  \u1083 \u1080  \u1076 \u1072 \u1085 \u1085 \u1099 \u1077 \
char switcher[] = \{0, 0, 0, 0, 0, 0, 0, 0, '\\0'\};         // \uc0\u1087 \u1077 \u1088 \u1077 \u1082 \u1083 \u1102 \u1095 \u1072 \u1090 \u1077 \u1083 \u1100 \
int co2_lvl;                                        // \uc0\u1091 \u1088 \u1086 \u1074 \u1077 \u1085 \u1100  \u1057 \u1054 2\
int temperature;                                    // \uc0\u1090 \u1077 \u1084 \u1087 \u1077 \u1088 \u1072 \u1090 \u1091 \u1088 \u1072 \
int humidity;                                       // \uc0\u1074 \u1083 \u1072 \u1078 \u1085 \u1086 \u1089 \u1090 \u1100 \
int plants_room_hum;                                // \uc0\u1074 \u1083 \u1072 \u1078 \u1085 \u1086 \u1089 \u1090 \u1100  \u1087 \u1086 \u1095 \u1074 \u1099  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1099 \
int plants_bedroom_hum;                             // \uc0\u1074 \u1083 \u1072 \u1078 \u1085 \u1086 \u1089 \u1090 \u1100  \u1087 \u1086 \u1095 \u1074 \u1099  \u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1080 \
int plants_room_waterLvl;                           // \uc0\u1091 \u1088 \u1086 \u1074 \u1077 \u1085 \u1100  \u1074 \u1086 \u1076 \u1099  \u1074  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1077 \
int plants_bedroom_waterLvl;                        // \uc0\u1091 \u1088 \u1086 \u1074 \u1077 \u1085 \u1100  \u1074 \u1086 \u1076 \u1099  \u1074  \u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1077 \
int pipe_num;                                       // \uc0\u1090 \u1088 \u1091 \u1073 \u1072  \u1088 \u1072 \u1076 \u1080 \u1086 -\u1084 \u1086 \u1076 \u1091 \u1083 \u1103 \
//unsigned int buzzPin = 3;									// \uc0\u1087 \u1080 \u1085  \u1087 \u1080 \u1097 \u1072 \u1083 \u1082 \u1080 \
\
struct timeval start, end;\
long mtime, seconds, useconds;  \
\
int uart0_filestream = -1;\
\
CURL *curl;\
CURLcode res;\
		\
//the thread function\
void *connection_handler(void *);\
char rx_buffer[256];\
\
uint8_t status;\
bool ok;\
\
int minute, old_minute, year, month, old_month, second, old_second, day, old_day, hour, old_hour;\
std::string second_s;\
std::string minute_s;\
std::string hour_s;\
std::string day_s;\
	\
char TimeString[128];\
timeval curTime;\
\
bool showTime = false;\
\
// ===================== \uc0\u1055 \u1056 \u1048 \u1053 \u1048 \u1052 \u1040 \u1045 \u1052  \u1044 \u1040 \u1053 \u1053 \u1067 \u1045  ===================== //\
void accept_Data(void)\
\{\
	cout << "accepting data...\\n";\
	pipe_num = 1;\
	radio.stopListening();\
\
	while (startDataRecieved[0] * startDataRecieved[1] * startDataRecieved[2] * startDataRecieved[3] * startDataRecieved[4] * startDataRecieved[5] * startDataRecieved[6] == 0) \
	\{\
		printf("opened pipe %d... \\n\\r", pipe_num);\
		switch (pipe_num)\
		\{\
			case 1: \
			\{\
				radio.setChannel(10);\
				radio.openWritingPipe(0xAABBCCDD11);\
				delay(20);\
				msg[0] = 9;\
				ok = false;\
				printf("writing for pipe %d... \\n\\r", pipe_num);\
				\
				bool getAnswer = false;\
				while(!ok || !getAnswer)\
				\{\
					delay(200);\
					ok = radio.write(msg, 9);\
\
					if (ok)\
					\{\
						printf("%d write successful!\\n\\r", pipe_num);\
						printf("waiting for answer from pipe %d... \\n\\r", pipe_num);\
						\
						delay(100);\
						\
						bool isPayload = false;\
						while (!isPayload)\
						\{\
							if (radio.isAckPayloadAvailable())\
							\{\
								isPayload = true;\
								radio.read(msg, 9);\
								delay(20);\
								printf("msg = %d\\n", msg[0]);\
								\
								if (startDataRecieved[0] == 0)\
								\{\
									getAnswer = true;\
									cout << "recieve 1\\n\\n";\
									bcm2835_gpio_write(LED_PIN1, 1);\
									for (int i = 0; i < 4; i++)\
									\{\
										LED_state[i] = msg[i + 1];\
										switcher[i] = LED_state[i];\
									\}\
\
									startDataRecieved[0] = 1;\
								\}\
							\}\
						\}\
					\}\
				\}\
				break;\
			\}\
			case 2: \
			\{\
				radio.setChannel(20);\
				radio.openWritingPipe(0xAABBCCDD22);\
				delay(20);\
				msg[0] = 9;\
				ok = false;\
				printf("writing for pipe %d... \\n\\r", pipe_num);\
				\
				bool getAnswer = false;\
				while(!ok || !getAnswer)\
				\{\
					delay(200);\
					ok = radio.write(msg, 9);\
\
					if (ok)\
					\{\
						printf("%d write successful!\\n\\r", pipe_num);\
						printf("waiting for answer from pipe %d... \\n\\r", pipe_num);\
						\
						delay(100);\
						bool isPayload = false;\
						while (!isPayload)\
						\{\
							if (radio.isAckPayloadAvailable())\
							\{\
								isPayload = true;\
								\
								if (startDataRecieved[1] == 0)\
								\{\
									radio.read(msg, 9);\
									getAnswer = true;\
									cout << "recieve 2\\n\\n";\
									bcm2835_gpio_write(LED_PIN2, 1);\
									LED_state[5] = msg[1];\
									switcher[5] = LED_state[5];\
									startDataRecieved[1] = 1;\
								\}\
							\}\
						\}\
					\}\
				\}\
				break;\
			\}\
			case 3: \
			\{\
				radio.setChannel(30);\
				radio.openWritingPipe(0xAABBCCDD33);\
				delay(20);\
				msg[0] = 9;\
				ok = false;\
				printf("writing for pipe %d... \\n\\r", pipe_num);\
				\
				bool getAnswer = false;\
				while(!ok || !getAnswer)\
				\{\
					delay(200);\
					ok = radio.write(msg, 9);\
\
					if (ok && radio.isAckPayloadAvailable())\
					\{\
						printf("%d write successful!\\n\\r", pipe_num);\
						printf("waiting for answer from pipe %d... \\n\\r", pipe_num);\
						\
						radio.read(msg, 9);\
						delay(20);\
						printf("msg = %d\\n", msg[0]);\
						\
						if (startDataRecieved[2] == 0)\
						\{\
							getAnswer = true;\
							cout << "recieve 3\\n\\n";\
							bcm2835_gpio_write(LED_PIN3, 1);\
							WND_state[0] = msg[1];\
							switcher[6] = WND_state[0];\
							startDataRecieved[2] = 1;\
						\}\
					\}\
				\}\
				break;\
			\}\
			case 4: \
			\{\
				radio.setChannel(40);\
				radio.openWritingPipe(0xAABBCCDD44);\
				delay(20);\
				msg[0] = 9;\
				ok = false;\
				printf("writing for pipe %d... \\n\\r", pipe_num);\
				\
				bool getAnswer = false;\
				while(!ok || !getAnswer)\
				\{\
					delay(200);\
					ok = radio.write(msg, 9);\
\
					if (ok && radio.isAckPayloadAvailable())\
					\{\
						printf("%d write successful!\\n\\r", pipe_num);\
						printf("waiting for answer from pipe %d... \\n\\r", pipe_num);\
						\
						delay(100);\
						\
						radio.read(msg, 9);\
						delay(20);\
						printf("msg = %d\\n", msg[0]);\
						\
						if (startDataRecieved[3] == 0)\
						\{\
							getAnswer = true;\
							cout << "recieve 4\\n\\n";\
							bcm2835_gpio_write(LED_PIN4, 1);\
							int co2_str[4] = \{0, 0, 0, 0\};\
							int temp_str[2] = \{0, 0\};\
							int hum_str[2] = \{0, 0\};\
\
							for(int i = 0; i < 4; i++)\
							\{\
								co2_str[i] = msg[i + 5];\
							\}\
\
							temp_str[0] = msg[3];\
							temp_str[1] = msg[4];\
							hum_str[0] = msg[1];\
							hum_str[1] = msg[2];\
\
							temperature = temp_str[1] + temp_str[0]*10;\
							humidity    = hum_str[1] + hum_str[0]*10;\
							co2_lvl     = co2_str[3] + co2_str[2]*10 + co2_str[1]*100 + co2_str[0]*1000;\
\
							startDataRecieved[3] = 1;\
						\}\
					\}\
				\}\
				break;\
			\}\
			case 5: \
			\{\
				radio.setChannel(50);\
				radio.openWritingPipe(0xAABBCCDD55);\
				delay(20);\
				msg[0] = 9;\
				ok = false;\
				printf("writing for pipe %d... \\n\\r", pipe_num);\
				\
				bool getAnswer = false;\
				while(!ok || !getAnswer)\
				\{\
					delay(200);\
					ok = radio.write(msg, 9);\
\
					if (ok && radio.isAckPayloadAvailable())\
					\{\
						printf("%d write successful!\\n\\r", pipe_num);\
						printf("waiting for answer from pipe %d... \\n\\r", pipe_num);\
						\
						delay(100);\
						\
						radio.read(msg, 9);\
						delay(20);\
						printf("msg = %d\\n", msg[0]);\
						\
						if (startDataRecieved[4] == 0)\
						\{\
							getAnswer = true;\
							cout << "recieve 5\\n\\n";\
							bcm2835_gpio_write(LED_PIN5, 1);\
							plants_room_hum = msg[1]*100 + msg[2]*10 + msg[3];\
							plants_room_waterLvl = msg[4];\
							printf("plants_room_hum = %d\\n\\r", plants_room_hum);\
							startDataRecieved[4] = 1;\
						\}\
					\}\
				\}\
				break;\
			\}\
\
			case 6: \
			\{\
				radio.setChannel(60);\
				radio.openWritingPipe(0xAABBCCDD66);\
				delay(20);\
				msg[0] = 9;\
				ok = false;\
				printf("writing for pipe %d... \\n\\r", pipe_num);\
				\
				bool getAnswer = false;\
				while(!ok || !getAnswer)\
				\{\
					delay(200);\
					ok = radio.write(msg, 9);\
\
					if (ok && radio.isAckPayloadAvailable())\
					\{\
						printf("%d write successful!\\n\\r", pipe_num);\
						printf("waiting for answer from pipe %d... \\n\\r", pipe_num);\
						\
						delay(100);\
						\
						radio.read(msg, 9);\
						delay(20);\
						\
						if (startDataRecieved[5] == 0)\
						\{\
							getAnswer = true;\
							cout << "recieve 6\\n\\n";\
							bcm2835_gpio_write(LED_PIN6, 1);\
							\
							\
							plants_bedroom_hum = msg[1]*100 + msg[2]*10 + msg[3];\
							plants_bedroom_waterLvl = msg[4];\
							printf("plants_bedroom_hum = %d\\n\\r", plants_bedroom_hum);\
							\
							startDataRecieved[5] = 1;\
						\}\
					\}\
				\}\
				break;\
			\}\
			\
			\
			case 7: \
			\{\
				radio.setChannel(70);\
				radio.openWritingPipe(0xAABBCCDD77);\
				delay(20);\
				msg[0] = 9;\
				ok = false;\
				printf("writing for pipe %d... \\n\\r", pipe_num);\
				\
				bool getAnswer = false;\
				while(!ok || !getAnswer)\
				\{\
					delay(200);\
					ok = radio.write(msg, 9);\
\
					if (ok && radio.isAckPayloadAvailable())\
					\{\
						printf("%d write successful!\\n\\r", pipe_num);\
						printf("waiting for answer from pipe %d... \\n\\r", pipe_num);\
						\
						if (radio.isAckPayloadAvailable())\
						\{\
							radio.read(msg, 9);\
							delay(20);\
							printf("msg = %d\\n", msg[0]);\
							\
							if (startDataRecieved[6] == 0)\
							\{\
								LED_state[6] = msg[5];\
								switcher[7] = LED_state[6];\
								//bcm2835_gpio_write(LED_PIN7, 1);\
								getAnswer = true;\
								cout << "recieve 7\\n\\n";\
								startDataRecieved[6] = 1;\
								//bcm2835_gpio_write(buzzPin, 0);\
							\}\
						\}\
					\}\
				\}\
				break;\
			\}\
\
\
		\}\
\
\
		if (pipe_num < 7) pipe_num++;\
		else pipe_num = 1; \
		\
\
		delay(100);\
  \}\
  \
  cout << "Server succesfully started!\\n\\n";\
  \
  pipe_num = 4;\
\}\
\
// ===================== \uc0\u1060 \u1054 \u1056 \u1052 \u1048 \u1056 \u1059 \u1045 \u1052  \u1048  \u1055 \u1054 \u1057 \u1067 \u1051 \u1040 \u1045 \u1052  \u1044 \u1040 \u1053 \u1053 \u1067 \u1045  ===================== //\
std::string XML_response(bool auth)\
\{ \
	if (auth)\
	\{\
		std::string str;\
		// LED1\
		str.append("DATA ");\
		str.append("\\nled1 ");\
		if (LED_state[0]) \
		\{\
			str.append("on");\
		\}\
		else \
		\{\
			str.append("off");\
		\}\
		str.append(" led1");\
		str.append("\\nled2 ");\
		// LED2\
		if (LED_state[1]) \
		\{\
			str.append("on");\
		\}\
		else \
		\{\
			str.append("off");\
		\}\
		str.append(" led2");\
		 str.append("\\nled3 ");\
		// LED3\
		if (LED_state[2]) \
		\{\
			str.append("on");\
		\}\
		else \
		\{\
			str.append("off");\
		\}\
		str.append(" led3");\
		str.append("\\nled4 ");\
		// LED4\
		if (LED_state[3]) \
		\{\
			str.append("on");\
		\}\
		else \
		\{\
			str.append("off");\
		\}\
		str.append(" led4");\
		str.append("\\nled5 ");\
		// BATH\
		if (LED_state[4]) \
		\{\
			str.append("on");\
		\}\
		else \
		\{\
			str.append("off");\
		\}\
		str.append(" led5");\
		str.append("\\nled6 ");\
		// ROOM\
		if (LED_state[5]) \
		\{\
			str.append("on");\
		\}\
		else \
		\{\
			str.append("off");\
		\}\
		str.append(" led6");\
		str.append("\\nled7 ");\
		// BEDROOM\
		if (LED_state[6]) \
		\{\
			str.append("on");\
		\}\
		else \
		\{\
			str.append("off");\
		\}\
		str.append(" led7");\
\
		//climat\
		str.append("\\ntemperature ");\
		str.append(std::to_string(temperature));\
		str.append(" temperature");\
		str.append("\\nhumahuma ");\
		str.append(std::to_string(humidity));\
		str.append(" humahuma");\
		str.append("\\nco2 ");\
		str.append(std::to_string(co2_lvl));\
		str.append(" co2");\
\
\
		\
		//plants\
		str.append("\\nplants_room_humidity ");\
		str.append(std::to_string(plants_room_hum));\
		str.append(" plants_room_humidity");\
		str.append("\\nplants_bedroom_humidity ");\
		str.append(std::to_string(plants_bedroom_hum));\
		str.append(" plants_bedroom_humidity");\
		str.append("\\nplants_room_water_level ");\
		str.append(std::to_string(plants_room_waterLvl));\
		str.append(" plants_room_water_level");\
		str.append("\\nplants_bedroom_water_level ");\
		str.append(std::to_string(plants_bedroom_waterLvl));\
		str.append(" plants_bedroom_water_level");\
		\
		// window\
		str.append("\\nwindow ");\
		if (WND_state[0] == 0) \
		\{\
			str.append("on");\
		\}\
		else \
		\{\
			str.append("off");\
		\}\
		str.append(" window");\
		std::string header;\
		header.append(str);\
		\
		//char* tempStr;\
		//strncpy(tempStr, header.c_str(), (sizeof header.c_str())-1);\
		 \
		return header;\
	\}\
	else\
	\{\
		std::string str;\
		// LED1\
		str.append("HTTP/1.0 401 Unauthorized\\n");\
		str.append("WWW-Authenticate: Basic realm=\\"Devastor's Home\\"");\
		std::string header;\
		header.append(str);\
		return header;\
	\}\
\}\
\
void getTime(bool init)\
\{\
	gettimeofday(&curTime, NULL);\
	\
	strftime(TimeString, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));\
	\
	second_s.assign(TimeString);\
	minute_s.assign(TimeString);\
	hour_s.assign(TimeString);\
	day_s.assign(TimeString);\
	\
	second_s = second_s.substr(17, 2);\
	minute_s = minute_s.substr(14, 2);\
	hour_s = hour_s.substr(11, 2);\
	day_s = day_s.substr(8, 2);\
	\
	second = std::stoi(second_s);\
	minute = std::stoi(minute_s);\
	hour = std::stoi(hour_s);\
	day = std::stoi(day_s);\
	\
	if (init)\
	\{\
		old_day = day;\
		old_minute = minute;\
		old_second = second;\
		old_month = month;\
		old_hour = hour;\
	\}\
	\
	\
\}\
\
void updateData()\
\{\
	getTime(false);\
	\
	// \uc0\u1077 \u1089 \u1083 \u1080  \u1087 \u1088 \u1086 \u1096 \u1083 \u1086  \u1074 \u1088 \u1077 \u1084 \u1103  \u1086 \u1073 \u1085 \u1086 \u1074 \u1083 \u1077 \u1085 \u1080 \u1103  \u1076 \u1072 \u1085 \u1085 \u1099 \u1093  (\u1088 \u1072 \u1079  \u1074  10 \u1084 \u1080 \u1085 \u1091 \u1090 )\
	//if (abs(minute - old_minute) >= 10)\
	if (abs(minute - old_minute) >= 10)\
	\{\
		if (hour > 7)\
		\{ \
			bcm2835_gpio_write(buzzPin, 1);\
			delay(100);\
			bcm2835_gpio_write(buzzPin, 0);\
			delay(100);\
			bcm2835_gpio_write(buzzPin, 1);\
			delay(100);\
			bcm2835_gpio_write(buzzPin, 0);\
		\}\
								\
		printf("check data: day:%d time: %d:%d:%d\\n", day, hour, minute, second);\
		\
		dataRecieved[3] = 0;\
		dataRecieved[4] = 0;\
		dataRecieved[5] = 0;\
		pipe_num = 4;\
		\
		radio.stopListening();\
		while (dataRecieved[3] * dataRecieved[4] * dataRecieved[5] == 0) \
		\{\
			printf("opened pipe %d... \\n\\r", pipe_num);\
			switch (pipe_num)\
			\{\
				case 4: \
				\{\
					radio.setChannel(40);\
					radio.openWritingPipe(0xAABBCCDD44);\
					delay(20);\
					msg[0] = 9;\
					ok = false;\
					printf("writing for pipe %d... \\n\\r", pipe_num);\
					\
					bool getAnswer = false;\
					while(!ok || !getAnswer)\
					\{\
						delay(200);\
						ok = radio.write(msg, 9);\
\
						if (ok && radio.isAckPayloadAvailable())\
						\{\
							printf("%d write successful!\\n\\r", pipe_num);\
							printf("waiting for answer from pipe %d... \\n\\r", pipe_num);\
							\
							delay(100);\
							\
							radio.read(msg, 9);\
							delay(20);\
							printf("msg = %d\\n", msg[0]);\
							\
							if (dataRecieved[3] == 0)\
							\{\
								getAnswer = true;\
								dataRecieved[3] = 1;\
								temperature = msg[3]*10 + msg[4];\
								humidity    = msg[1]*10 + msg[2];\
								co2_lvl = msg[5]*1000 + msg[6]*100 + msg[7]*10 + msg[8];\
\
\
								// check if CO2 level in room is high\
								if (temperature > 25 || co2_lvl > 700) \
								\{ \
									if(temperature > 20)\
									\{\
										if (WND_state[0] == 0) \
										\{\
											WND_state[0] = 1;\
											switcher[6] = 1;\
											radio.stopListening();\
											delay(20);\
											radio.setChannel(30);\
											delay(100);\
											radio.openWritingPipe(0xAABBCCDD33);\
											//radio.openReadingPipe(3,0xAABBCCDD34);\
											msg[0] = 8;\
											msg[1] = WND_state[0];\
											ok = radio.write(msg, 9);\
											delay(20);\
											if (ok) printf("window opened!\\n");\
										\}\
									\}\
								\}\
								else if (temperature < 24)\
								\{\
									if (WND_state[0] == 1) \
									\{\
										WND_state[0] = 0;\
										switcher[6] = 0;\
										radio.stopListening();\
										delay(20);\
										radio.setChannel(30);\
										delay(100);\
										radio.openWritingPipe(0xAABBCCDD33);\
										//radio.openReadingPipe(3,0xAABBCCDD34);\
										msg[0] = 8;\
										msg[1] = WND_state[0];\
										ok = radio.write(msg, 9);\
										delay(20);\
										if (ok) printf("window closed!\\n");\
									\}\
								\}\
							\}\
						\}\
					\}\
					break;\
				\}\
				case 5: \
				\{\
					radio.setChannel(50);\
					radio.openWritingPipe(0xAABBCCDD55);\
					delay(20);\
					msg[0] = 9;\
					ok = false;\
					printf("writing for pipe %d... \\n\\r", pipe_num);\
					\
					bool getAnswer = false;\
					while(!ok || !getAnswer)\
					\{\
						delay(200);\
						ok = radio.write(msg, 9);\
\
						if (ok && radio.isAckPayloadAvailable())\
						\{\
							printf("%d write successful!\\n\\r", pipe_num);\
							printf("waiting for answer from pipe %d... \\n\\r", pipe_num);\
							\
							delay(100);\
							\
							radio.read(msg, 9);\
							delay(20);\
							printf("msg = %d\\n", msg[0]);\
							\
							if (dataRecieved[4] == 0)\
							\{\
								getAnswer = true;\
								\
								dataRecieved[4] = 1;\
								plants_room_hum = msg[1]*100 + msg[2]*10 + msg[3];\
								plants_room_waterLvl = msg[4];\
								printf("plants_room_hum = %d\\n\\r", plants_room_hum);\
								\
								printf ("\\nold day = %d, day = %d \\n", old_day, day);\
\
								if (old_day != day)\
								\{\
									bcm2835_gpio_write(buzzPin, 1);\
									delay(100);\
									bcm2835_gpio_write(buzzPin, 0);\
									delay(100);\
									bcm2835_gpio_write(buzzPin, 1);\
									delay(100);\
									bcm2835_gpio_write(buzzPin, 0);\
									delay(100);\
									bcm2835_gpio_write(buzzPin, 1);\
									delay(100);\
									bcm2835_gpio_write(buzzPin, 0);\
									delay(100);\
									bcm2835_gpio_write(buzzPin, 1);\
									delay(100);\
									bcm2835_gpio_write(buzzPin, 0);\
									delay(100);\
									\
									old_day = day;\
									\
									radio.setChannel(50);\
									radio.openWritingPipe(0xAABBCCDD55);\
									delay(50);\
									//radio.openReadingPipe(5,0xAABBCCDD56);\
									msg[0] = 8;\
									ok = radio.write (msg, 9);\
									delay(500);\
									if (ok) printf("plants in room checked!\\n");\
									\
									radio.setChannel(60);\
									radio.openWritingPipe(0xAABBCCDD66);\
									delay(50);\
									//radio.openReadingPipe(5,0xAABBCCDD67);\
									msg[0] = 8;\
									ok = radio.write (msg, 9);\
									delay(20);\
									if (ok) printf("plants in bedroom checked!\\n");\
								\}\
							\}\
						\}\
					\}\
					break;\
				\}\
				case 6: \
				\{\
					radio.setChannel(60);\
					radio.openWritingPipe(0xAABBCCDD66);\
					delay(20);\
					msg[0] = 9;\
					ok = false;\
					printf("writing for pipe %d... \\n\\r", pipe_num);\
					\
					bool getAnswer = false;\
					while(!ok || !getAnswer)\
					\{\
						delay(200);\
						ok = radio.write(msg, 9);\
\
						if (ok && radio.isAckPayloadAvailable())\
						\{\
							printf("%d write successful!\\n\\r", pipe_num);\
							printf("waiting for answer from pipe %d... \\n\\r", pipe_num);\
							\
							delay(100);\
							\
							radio.read(msg, 9);\
							delay(20);\
							\
							if (dataRecieved[5] == 0)\
							\{\
								getAnswer = true;\
								\
								plants_bedroom_hum = msg[1]*100 + msg[2]*10 + msg[3];\
								plants_bedroom_waterLvl = msg[4];\
								printf("plants_bedroom_hum = %d\\n\\r", plants_bedroom_hum);\
								\
								dataRecieved[5] = 1;\
								\
								printf ("\\nold day = %d, day = %d \\n", old_day, day);\
\
								if (old_day != day)\
								\{\
									bcm2835_gpio_write(buzzPin, 1);\
									delay(100);\
									bcm2835_gpio_write(buzzPin, 0);\
									delay(100);\
									bcm2835_gpio_write(buzzPin, 1);\
									delay(100);\
									bcm2835_gpio_write(buzzPin, 0);\
									delay(100);\
									bcm2835_gpio_write(buzzPin, 1);\
									delay(100);\
									bcm2835_gpio_write(buzzPin, 0);\
									delay(100);\
									bcm2835_gpio_write(buzzPin, 1);\
									delay(100);\
									bcm2835_gpio_write(buzzPin, 0);\
									delay(100);\
									\
									old_day = day;\
									radio.setChannel(50);\
									radio.openWritingPipe(0xAABBCCDD55);\
									//radio.openReadingPipe(5,0xAABBCCDD56);\
									delay(50);\
									msg[0] = 8;\
									ok = radio.write (msg, 9);\
									delay(500);\
									if (ok) printf("plants in room checked!\\n");\
									radio.setChannel(60);\
									radio.openWritingPipe(0xAABBCCDD66);\
									delay(50);\
									//radio.openReadingPipe(5,0xAABBCCDD67);\
									msg[0] = 8;\
									msg[1] = 8;\
									ok = radio.write (msg, 9);\
									delay(20);\
									if (ok) printf("plants in bedroom checked!\\n");\
								\}\
							\}\
						\}\
					\}\
					break;\
				\}\
			\}\
\
			if (pipe_num < 6) pipe_num++;\
			else pipe_num = 4;\
			\
			delay(100);\
		\}\
		\
		old_minute = minute;\
	\}\
\}\
\
void checkAlisa(void)\
\{\
	std::string command = "";\
	\
	bool _catch = false;\
	// \uc0\u1095 \u1080 \u1090 \u1072 \u1077 \u1084  \u1095 \u1090 \u1086  \u1087 \u1088 \u1080 \u1096 \u1083 \u1086  \u1087 \u1086  bluetooth\
	if (uart0_filestream != -1)\
	\{\
		\
		int rx_length = read(uart0_filestream, (void*)rx_buffer, 255);		//Filestream, buffer to store in, number of ints to read (max)\
		if (rx_length > 0)\
		\{\
			\
			//setlocale(LC_ALL,"RUS");\
			//setlocale(LC_ALL,"Russian");\
			//setlocale(LC_ALL,"");\
			//ints received\
			rx_buffer[rx_length] = '\\0';\
			//printf("%i ints read : %s\\n", rx_length, rx_buffer);\
			//std::string buf(rx_buffer);\
			for (int i = 0; i < rx_length; i++)\
			\{\
				if (rx_buffer[i] != '\\n' &&\
					rx_buffer[i] != '\\0' &&\
					rx_buffer[i] != ' ') command.append(1, rx_buffer[i]);\
			\}\
			delay(100);\
			\
			\
			if( strstr(rx_buffer, "\uc0\u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1072 \u1103  \u1089 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1072 \u1103  \u1094 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1089 \u1074 \u1077 \u1090  \u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1072 \u1103 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1071  \u1074  \u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1086 \u1081 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1074 \u1077 \u1088 \u1075 \u1086 \u1089 \u1090 \u1080 \u1085 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1042 \u1082 \u1083 \u1102 \u1095 \u1080  \u1057 \u1083 \u1077 \u1085 \u1076 \u1077 \u1088  \u1092 \u1080 \u1083 \u1100 \u1084 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1074 \u1082 \u1083 \u1102 \u1095 \u1080  \u1089 \u1074 \u1077 \u1090  \u1074  \u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1086 \u1081 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1073 \u1072 \u1089 \u1090 \u1080 \u1085 \u1076 \u1072 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1072 \u1103 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1080 \u1094 \u1072 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1075 \u1086 \u1089 \u1090 \u1077 \u1074 \u1072 \u1103 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1075 \u1072 \u1089 \u1080 \u1084  \u1089 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1080 \u1094 \u1099 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1074  \u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1086 \u1081 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1091 \u1102 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1043 \u1072 \u1089 \u1090 \u1077 \u1083 \u1083 \u1086 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1094 \u1074 \u1077 \u1090  \u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1086 \u1081 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1094 \u1074 \u1077 \u1090  \u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1085 \u1086 \u1081 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1080 \u1094 \u1072  \u1089 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1094 \u1074 \u1077 \u1090  \u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1086 \u1081 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1094 \u1074 \u1077 \u1090  \u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1080 \u1094 \u1099 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1074 \u1089 \u1077  \u1076 \u1083 \u1103  \u1089 \u1077 \u1084 \u1100 \u1080 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1047 \u1072 \u1095 \u1077 \u1084  \u1056 \u1072 \u1089 \u1089 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1075 \u1086 \u1089 \u1090 \u1077 \u1074 \u1099 \u1093 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1052 \u1080 \u1090 \u1080 \u1085 \u1086  \u1089 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1074 \u1089 \u1077  \u1082 \u1072 \u1089 \u1090 \u1080 \u1085 \u1075 \u1080 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1042 \u1082 \u1083 \u1102 \u1095 \u1080 \u1090 \u1100  \u1089 \u1074 \u1077 \u1090  \u1074  \u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1086 \u1081 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1072 \u1103  \u1072 \u1082 \u1090 \u1080 \u1074 \u1080 \u1088 \u1086 \u1074 \u1072 \u1090 \u1100 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1072 \u1082 \u1090 \u1080 \u1074 \u1080 \u1088 \u1086 \u1074 \u1072 \u1090 \u1100  \u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1072 \u1103 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1089 \u1074 \u1077 \u1090  \u1074  \u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1086 \u1081 ") != NULL ) \
			\{\
				_catch = true;\
				if (strstr(rx_buffer, "1") != NULL || strstr(rx_buffer, "2") != NULL || strstr(rx_buffer, "3") != NULL)\
				\{\
					if (strstr(rx_buffer, "1") != NULL)\
					\{\
						if (switcher[0] == 1) switcher[0] = 0;\
						else switcher[0] = 1;\
						LED_state[0] = switcher[0];\
					\}\
					if (strstr(rx_buffer, "2") != NULL)\
					\{\
						if (switcher[1] == 1) switcher[1] = 0;\
						else switcher[1] = 1;\
						LED_state[1] = switcher[1];\
					\}\
					if (strstr(rx_buffer, "3") != NULL)\
					\{\
						if (switcher[2] == 1) switcher[2] = 0;\
						else switcher[2] = 1;\
						LED_state[2] = switcher[2];\
					\}\
				\}\
				else\
				\{  \
					if (switcher[3] == 1) switcher[3] = 0;\
					else switcher[3] = 1;\
					\
					LED_state[0] = switcher[3];\
					LED_state[1] = switcher[3];\
					LED_state[2] = switcher[3];\
					LED_state[3] = switcher[3];\
				\}\
				\
				radio.stopListening();\
				delay(20);\
				radio.setChannel(10);\
				delay(20);\
				radio.openWritingPipe(0xAABBCCDD11);\
				msg[0] = 8;\
				for (int i = 0; i < 4; i++)\
				\{\
					msg[i + 1] = LED_state[i];\
				\}\
				\
				ok = radio.write(msg, 9);\
				//cout << "\uc0\u1089 \u1074 \u1077 \u1090  \u1074  \u1075 \u1086 \u1089 \u1090 \u1080 \u1085 \u1086 \u1081 ";\
			\} \
			\
			if( strstr(rx_buffer, "\uc0\u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1103  \u1089 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1057 \u1087 \u1072 \u1083 \u1100 \u1085 \u1103  \u1089 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1057 \u1074 \u1077 \u1090  \u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1103 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1089 \u1074 \u1077 \u1090  \u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1103 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1074 \u1086 \u1076 \u1086 \u1089 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1080 \u1089 \u1087 \u1072 \u1085 \u1077 \u1094 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1074 \u1082 \u1083 \u1102 \u1095 \u1080 \u1090 \u1100  CrossFire") != NULL ||\
				strstr(rx_buffer, "\uc0\u1042 \u1082 \u1083 \u1102 \u1095 \u1080 \u1090 \u1100  \u1089 \u1074 \u1077 \u1090  \u1074  \u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1077 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1042 \u1082 \u1083 \u1102 \u1095 \u1080  \u1089 \u1074 \u1077 \u1090  \u1074  \u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1077 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1074 \u1099 \u1082 \u1083 \u1102 \u1095 \u1080  \u1089 \u1074 \u1077 \u1090  \u1074  \u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1077 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1086 \u1083 \u1080 \u1094 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1094 \u1074 \u1077 \u1090  \u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1080 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1057 \u1072 \u1085 \u1103  \u1089 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1057 \u1072 \u1085 \u1103  \u1094 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1072 \u1084 \u1103 \u1090 \u1100  \u1083 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1086 \u1083 \u1080 \u1094 \u1080 \u1103 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1055 \u1072 \u1103 \u1094 \u1099 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1086 \u1085 \u1080  \u1089 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1089 \u1087 \u1072 \u1075 \u1077 \u1090 \u1090 \u1080 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1086 \u1074 \u1077 \u1088 \u1100 \u1090 \u1077 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1074 \u1089 \u1077  \u1087 \u1086 \u1085 \u1080 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1092 \u1072 \u1083 \u1100 \u1094 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1103 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1054 \u1083 \u1103  \u1089 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1103  \u1094 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1080 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1086 \u1085 \u1103 \u1090 \u1080 \u1103 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1055 \u1072 \u1085 \u1080 \u1085 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1072 \u1084 \u1103 \u1090 \u1085 \u1080 \u1082 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1094 \u1074 \u1077 \u1090 \u1099  \u1076 \u1083 \u1103  \u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1080 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1103  \u1072 \u1082 \u1090 \u1080 \u1074 \u1080 \u1088 \u1086 \u1074 \u1072 \u1090 \u1100 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1057 \u1087 \u1072 \u1083 \u1100 \u1085 \u1103  \u1072 \u1082 \u1090 \u1080 \u1074 \u1080 \u1088 \u1086 \u1074 \u1072 \u1090 \u1100 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1040 \u1082 \u1090 \u1080 \u1074 \u1080 \u1088 \u1086 \u1074 \u1072 \u1090 \u1100  \u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1103 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1072 \u1082 \u1090 \u1080 \u1074 \u1080 \u1088 \u1086 \u1074 \u1072 \u1090 \u1100  \u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1103 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1089 \u1074 \u1077 \u1090  \u1074  \u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1077 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1057 \u1074 \u1077 \u1090  \u1074  \u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1077 ") != NULL ) \
			\{\
				_catch = true;\
				if (switcher[7] == 1) switcher[7] = 0;\
				else switcher[7] = 1;\
				\
				LED_state[6] = switcher[7];\
				radio.stopListening();\
				delay(20);\
				radio.setChannel(70);\
				delay(20);\
				radio.openWritingPipe(0xAABBCCDD77);\
				msg[0] = 8;\
				msg[1] = LED_state[6];\
				\
				ok = radio.write(msg, 9);\
				//cout << "\uc0\u1089 \u1074 \u1077 \u1090  \u1074  \u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1077 ";\
			\} \
\
			if( strstr(rx_buffer, "\uc0\u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1072  \u1089 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1050 \u1086 \u1084 \u1085 \u1072 \u1090 \u1072  \u1089 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1057 \u1074 \u1077 \u1090  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1072 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1089 \u1074 \u1077 \u1090  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1072 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1103  \u1074  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1077 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1094 \u1074 \u1077 \u1090  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1099 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1043 \u1076 \u1077  \u1074  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1077 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1074  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1077 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1072 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1071  \u1074  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1077 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1074 \u1089 \u1077  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1099 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1083 \u1077 \u1089  \u1074  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1077 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1072  \u1089 \u1099 \u1088 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1085 \u1099 \u1081  \u1089 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1085 \u1099 \u1081  \u1094 \u1074 \u1077 \u1090 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1072  \u1072 \u1082 \u1090 \u1080 \u1074 \u1080 \u1088 \u1086 \u1074 \u1072 \u1090 \u1100 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1050 \u1086 \u1084 \u1085 \u1072 \u1090 \u1072  \u1072 \u1082 \u1090 \u1080 \u1074 \u1080 \u1088 \u1086 \u1074 \u1072 \u1090 \u1100 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1040 \u1082 \u1090 \u1080 \u1074 \u1080 \u1088 \u1086 \u1074 \u1072 \u1090 \u1100  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1072 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1072 \u1082 \u1090 \u1080 \u1074 \u1080 \u1088 \u1086 \u1074 \u1072 \u1090 \u1100  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1072 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1089 \u1074 \u1077 \u1090  \u1074  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1077 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1057 \u1074 \u1077 \u1090  \u1074  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1077 ") != NULL  )\
			\{\
				_catch = true;\
				if (switcher[5] == 1) \
				\{ \
				  switcher[5] = 0;\
				  LED_state[5] = 0;\
				\}\
				else \
				\{ \
				  switcher[5] = 1;\
				  LED_state[5] = 1;\
				\}\
				radio.stopListening();\
				delay(20);\
				radio.setChannel(20);\
				delay(20);\
				radio.openWritingPipe(0xAABBCCDD22);\
				msg[0] = 8;\
				msg[1] = LED_state[5];\
				\
				ok = radio.write(msg, 9);\
 				//cout << "\uc0\u1089 \u1074 \u1077 \u1090  \u1074  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1077 ";   \
			\} \
\
			if( strstr(rx_buffer, "\uc0\u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1072  \u1086 \u1082 \u1085 \u1086 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1050 \u1086 \u1084 \u1085 \u1072 \u1090 \u1072  \u1086 \u1082 \u1085 \u1086 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1054 \u1082 \u1085 \u1086  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1072 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1086 \u1082 \u1085 \u1086  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1072 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1086 \u1082 \u1085 \u1086  \u1072 \u1082 \u1090 \u1080 \u1074 \u1080 \u1088 \u1086 \u1074 \u1072 \u1090 \u1100 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1054 \u1082 \u1085 \u1086  \u1072 \u1082 \u1090 \u1080 \u1074 \u1080 \u1088 \u1086 \u1074 \u1072 \u1090 \u1100 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1040 \u1082 \u1090 \u1080 \u1074 \u1080 \u1088 \u1086 \u1074 \u1072 \u1090 \u1100  \u1086 \u1082 \u1085 \u1086 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1072 \u1082 \u1090 \u1080 \u1074 \u1080 \u1088 \u1086 \u1074 \u1072 \u1090 \u1100  \u1086 \u1082 \u1085 \u1086 ") != NULL ||\
				command == "\uc0\u1086 \u1082 \u1085 \u1086  \u1074  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1077 " ||\
				strstr(rx_buffer, "\uc0\u1086 \u1082 \u1085 \u1086 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1086 \u1082 \u1086 \u1096 \u1082 \u1086 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1086 \u1082 \u1086 \u1096 \u1077 \u1095 \u1082 \u1086 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1092 \u1086 \u1088 \u1090 \u1086 \u1095 \u1082 \u1072 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1092 \u1086 \u1088 \u1090 \u1086 \u1095 \u1082 \u1091 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1092 \u1086 \u1088 \u1090 \u1086 \u1095 \u1082 \u1080 ") != NULL ||\
				command == "\uc0\u1054 \u1082 \u1085 \u1086  \u1074  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1077 ") \
			\{\
				_catch = true;\
				if (switcher[6] == 1) \
				\{ \
				  switcher[6] = 0;\
				  WND_state[0] = 0;\
				\}\
				else \
				\{ \
				  switcher[6] = 1;\
				  WND_state[0] = 1;\
				\}\
				radio.stopListening();\
				delay(20);\
				radio.setChannel(30);\
				delay(20);\
				radio.openWritingPipe(0xAABBCCDD33);\
				msg[0] = 8;\
				msg[1] = WND_state[0];\
				\
				ok = radio.write(msg, 9);\
 				//cout << "\uc0\u1086 \u1082 \u1085 \u1086  \u1074  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1077 ";   \
			\} \
\
			if( strstr(rx_buffer, "\uc0\u1087 \u1083 \u1077 \u1081 \u1089 \u1090 \u1077 \u1081 \u1096 \u1077 \u1085 ") != NULL ||\
				strstr(rx_buffer, "PlayStation") != NULL ||\
				strstr(rx_buffer, "Play Station") != NULL ||\
				strstr(rx_buffer, "Mortal Combat") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1083 \u1086 \u1081 \u1082 \u1072 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1083 \u1086 \u1081 \u1082 \u1091 ") != NULL) \
			\{\
				_catch = true;\
				// \uc0\u1075 \u1072 \u1089 \u1080 \u1084  \u1089 \u1074 \u1077 \u1090  \u1074  \u1093 \u1086 \u1083 \u1083 \u1077 \
				if (switcher[3] == 1) switcher[3] = 0;\
				\
				LED_state[0] = switcher[3];\
				LED_state[1] = switcher[3];\
				LED_state[2] = switcher[3];\
				LED_state[3] = switcher[3];\
\
				radio.stopListening();\
				delay(20);\
				radio.setChannel(10);\
				delay(20);\
				radio.openWritingPipe(0xAABBCCDD11);\
				msg[0] = 8;\
				for (int i = 0; i < 4; i++)\
				\{\
					msg[i + 1] = LED_state[i];\
				\}\
				\
				ok = radio.write(msg, 9);\
    \
\
				// \uc0\u1079 \u1072 \u1078 \u1080 \u1075 \u1072 \u1077 \u1084  \u1089 \u1074 \u1077 \u1090  \u1074  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1077 \
				if (switcher[5] == 0) \
				\{ \
				  switcher[5] = 1;\
				  LED_state[5] = 1;\
				\}\
\
				radio.stopListening();\
				delay(20);\
				radio.setChannel(20);\
				delay(20);\
				radio.openWritingPipe(0xAABBCCDD22);\
				msg[0] = 8;\
				msg[1] = LED_state[5];\
				\
				ok = radio.write(msg, 9);\
 				//cout << "\uc0\u1080 \u1075 \u1088 \u1072  \u1074  \u1087 \u1083 \u1086 \u1081 \u1082 \u1091 ";   \
			\} \
\
			if( strstr(rx_buffer, "\uc0\u1089 \u1087 \u1072 \u1090 \u1100 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1086 \u1089 \u1087 \u1083 \u1102 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1089 \u1087 \u1083 \u1102 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1093 \u1086 \u1095 \u1091  \u1089 \u1087 \u1072 \u1090 \u1100 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1093 \u1086 \u1095 \u1091  \u1087 \u1086 \u1089 \u1087 \u1072 \u1090 \u1100 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1086 \u1089 \u1087 \u1072 \u1083 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1086 \u1088 \u1077 \u1083 \u1072 \u1082 \u1089 \u1080 \u1088 \u1091 \u1102 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1088 \u1077 \u1083 \u1072 \u1082 \u1089 \u1080 \u1088 \u1091 \u1102 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1088 \u1077 \u1083 \u1072 \u1082 \u1089 \u1080 \u1088 \u1086 \u1074 \u1072 \u1090 \u1100 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1088 \u1077 \u1083 \u1072 \u1082 \u1089 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1089 \u1086 \u1085 ") != NULL) \
			\{\
				_catch = true;\
				// \uc0\u1075 \u1072 \u1089 \u1080 \u1084  \u1089 \u1074 \u1077 \u1090  \u1074  \u1093 \u1086 \u1083 \u1083 \u1077 \
				if (switcher[3] == 1) switcher[3] = 0;\
				\
				LED_state[0] = switcher[3];\
				LED_state[1] = switcher[3];\
				LED_state[2] = switcher[3];\
				LED_state[3] = switcher[3];\
\
				radio.stopListening();\
				delay(20);\
				radio.setChannel(10);\
				delay(20);\
				radio.openWritingPipe(0xAABBCCDD11);\
				msg[0] = 8;\
				for (int i = 0; i < 4; i++)\
				\{\
					msg[i + 1] = LED_state[i];\
				\}\
				\
				ok = radio.write(msg, 9);\
    \
\
				// \uc0\u1075 \u1072 \u1089 \u1080 \u1084  \u1089 \u1074 \u1077 \u1090  \u1074  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1077 \
				if (switcher[5] == 1) \
				\{ \
				  switcher[5] = 0;\
				  LED_state[5] = 0;\
				\}\
\
				radio.stopListening();\
				delay(20);\
				radio.setChannel(20);\
				delay(20);\
				radio.openWritingPipe(0xAABBCCDD22);\
				msg[0] = 8;\
				msg[1] = LED_state[5];\
				\
				ok = radio.write(msg, 9);\
    \
\
				// \uc0\u1075 \u1072 \u1089 \u1080 \u1084  \u1089 \u1074 \u1077 \u1090  \u1074  \u1089 \u1087 \u1072 \u1083 \u1100 \u1085 \u1077 \
				if (switcher[7] == 1) switcher[7] = 0;\
				\
				radio.stopListening();\
				delay(20);\
				radio.setChannel(70);\
				delay(20);\
				LED_state[6] = switcher[7];\
				radio.openWritingPipe(0xAABBCCDD77);\
				msg[0] = 8;\
				msg[1] = LED_state[6];\
				\
				ok = radio.write(msg, 9);\
 				//cout << "\uc0\u1073 \u1072 \u1102 -\u1073 \u1072 \u1081 ...";   \
			\} \
\
			if( strstr(rx_buffer, "\uc0\u1092 \u1080 \u1083 \u1100 \u1084 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1092 \u1080 \u1083 \u1100 \u1084 \u1099 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1086 \u1089 \u1084 \u1086 \u1090 \u1088 \u1077 \u1090 \u1100 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1085 \u1086 \u1091 \u1090 \u1073 \u1091 \u1082 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1086 \u1088 \u1072 \u1073 \u1086 \u1090 \u1072 \u1090 \u1100 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1088 \u1072 \u1073 \u1086 \u1090 \u1072 \u1090 \u1100 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1088 \u1072 \u1073 \u1086 \u1090 \u1072 \u1102 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1089 \u1084 \u1086 \u1090 \u1088 \u1077 \u1090 \u1100 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1082 \u1080 \u1085 \u1086 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1082 \u1080 \u1085 \u1086 \u1096 \u1082 \u1072 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1082 \u1080 \u1085 \u1086 \u1096 \u1082 \u1091 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1092 \u1080 \u1083 \u1100 \u1084 \u1077 \u1094 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1086 \u1089 \u1084 \u1086 \u1090 \u1088 \u1077 \u1083 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1087 \u1086 \u1075 \u1083 \u1103 \u1078 \u1091 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1092 \u1080 \u1083 \u1100 \u1084 \u1072 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1088 \u1072 \u1073 \u1086 \u1090 \u1072 ") != NULL) \
			\{\
				_catch = true;\
				// \uc0\u1075 \u1072 \u1089 \u1080 \u1084  \u1089 \u1074 \u1077 \u1090  1 \u1074  \u1093 \u1086 \u1083 \u1083 \u1077 , \u1079 \u1072 \u1078 \u1080 \u1075 \u1072 \u1077 \u1084  \u1089 \u1074 \u1077 \u1090  2 \u1080  3 \
				if (switcher[0] == 1) switcher[0] = 0;\
				if (switcher[1] == 0) switcher[1] = 1;\
				if (switcher[2] == 0) switcher[2] = 1;\
				\
				LED_state[0] = switcher[0];\
				LED_state[1] = switcher[1];\
				LED_state[2] = switcher[2];\
\
				radio.stopListening();\
				delay(20);\
				radio.setChannel(10);\
				delay(20);\
				radio.openWritingPipe(0xAABBCCDD11);\
				msg[0] = 8;\
				for (int i = 0; i < 4; i++)\
				\{\
					msg[i + 1] = LED_state[i];\
				\}\
				\
				ok = radio.write(msg, 9);\
    \
\
				// \uc0\u1075 \u1072 \u1089 \u1080 \u1084  \u1089 \u1074 \u1077 \u1090  \u1074  \u1082 \u1086 \u1084 \u1085 \u1072 \u1090 \u1077 \
				if (switcher[5] == 1) \
				\{ \
				  switcher[5] = 0;\
				  LED_state[5] = 0;\
				\}\
				\
				radio.stopListening();\
				delay(20);\
				radio.setChannel(20);\
				delay(20);\
				radio.openWritingPipe(0xAABBCCDD22);\
				msg[0] = 8;\
				msg[1] = LED_state[5];\
				\
				ok = radio.write(msg, 9);\
 				//cout << "\uc0\u1092 \u1080 \u1083 \u1100 \u1084 ";   \
			\}\
			\
			if( strstr(rx_buffer, "\uc0\u1096 \u1086 \u1091 \u1084 \u1072 \u1089 \u1090 \u1086 \u1075 \u1091 \u1086 \u1085 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1096 \u1086 \u1091  \u1084 \u1072 \u1089 \u1090  \u1075 \u1086 \u1091  \u1086 \u1085 "))\
			\{\
				showTime = true;\
				//cout << "\uc0\u1064 \u1054 \u1059  \u1053 \u1040 \u1063 \u1048 \u1053 \u1040 \u1045 \u1058 \u1057 \u1071 !!!!";  \
			\}\
				\
			if( strstr(rx_buffer, "\uc0\u1093 \u1074 \u1072 \u1090 \u1080 \u1090  \u1096 \u1086 \u1091 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1086 \u1089 \u1090 \u1072 \u1085 \u1086 \u1074 \u1080  \u1096 \u1086 \u1091 ") != NULL ||\
				strstr(rx_buffer, "\uc0\u1086 \u1089 \u1090 \u1072 \u1085 \u1086 \u1074 \u1080 \u1089 \u1100 "))\
			\{\
				showTime = false;\
				//cout << "\uc0\u1053 \u1091  \u1074 \u1086 \u1090  (((((";  \
			\}\
			\
			command = ""; \
			rx_length = 0;\
			for (int i = 0; i < sizeof(rx_buffer); i++) \
			\{\
				rx_buffer[i] = '\\0';\
			\}\
		\}\
	\}\
\}\
\
void showTimeIsNow()\
\{\
	int i = rand()%3 + 1;\
	\
	//cout << i; \
	\
	\
	radio.stopListening();\
	delay(20);\
	\
			\
	if (i == 1 || i == 2 || i == 3) \
	\{\
		radio.setChannel(10);\
		delay(20);\
		radio.openWritingPipe(0xAABBCCDD11);\
	\}\
	else if (i == 4)\
	\{\
		radio.setChannel(20);\
		delay(20);\
		radio.openWritingPipe(0xAABBCCDD22);\
	\}\
	else \
	\{\
		radio.setChannel(70);\
		delay(20);\
		radio.openWritingPipe(0xAABBCCDD77);\
	\}\
	\
	msg[0] = 8;\
	\
	if (i == 1)\
	\{\
		LED_state[0] = !LED_state[0];\
		switcher[0] = !switcher[0];\
		msg[1] = LED_state[0];\
		msg[2] = LED_state[1];\
		msg[3] = LED_state[2];\
		msg[4] = LED_state[3];\
		radio.write(msg, 9);\
	\}\
	if (i == 2)\
	\{\
		LED_state[1] = !LED_state[1];\
		switcher[1] = !switcher[1];\
		msg[1] = LED_state[0];\
		msg[2] = LED_state[1];\
		msg[3] = LED_state[2];\
		msg[4] = LED_state[3];\
		radio.write(msg, 9);\
	\}\
	if (i == 3)\
	\{\
		LED_state[2] = !LED_state[2];\
		switcher[2] = !switcher[2];\
		msg[1] = LED_state[0];\
		msg[2] = LED_state[1];\
		msg[3] = LED_state[2];\
		msg[4] = LED_state[3];\
		radio.write(msg, 9);\
	\}\
	if (i == 4)\
	\{\
		LED_state[5] = !LED_state[5];\
		switcher[5] = !switcher[5];\
		msg[1] = LED_state[5];\
		radio.write(msg, 9);\
	\}\
	if (i == 5)\
	\{\
		LED_state[6] = !LED_state[6];\
		switcher[7] = !switcher[7];\
		msg[1] = LED_state[6];\
		radio.write(msg, 9);\
	\}\
	\
	radio.startListening();\
\}\
\
void updateThread(void)\
\{\
	while (1)\
	\{\
		checkAlisa();\
		//if (startDataRecieved[0] * startDataRecieved[1] * startDataRecieved[2] * startDataRecieved[3] * startDataRecieved[4] * startDataRecieved[5]/* * startDataRecieved[6]*/ == 0) \
		//	accept_Data();	\
		if (!showTime) updateData();\
		else \
		\{\
			showTimeIsNow();\
			delay(100);\
		\}\
		\
		//status = radio.get_status();\
		//cout << "status: " << status;\
	\}\
\}\
\
void setParameters(std::string parameters)\
\{\
	bool all_ON = false;\
	\
    if (parameters.find("L4=1")!=std::string::npos)\
    \{\
		all_ON =true;\
		if (LED_state[3] != 1)\
		\{\
			if (switcher[3] == 1) switcher[3] = 0;\
			else switcher[3] = 1;\
			LED_state[3] = 1;\
		\}\
		if (LED_state[0] != 1) \
		\{ \
			LED_state[0] = 1;\
			if (switcher[0] == 1) switcher[0] = 0;\
			else switcher[0] = 1;\
		\}\
		if (LED_state[1] != 1) \
		\{ \
			LED_state[1] = 1;\
			if (switcher[1] == 1) switcher[1] = 0;\
			else switcher[1] = 1;\
		\}\
		if (LED_state[2] != 1) \
		\{ \
			LED_state[2] = 1;\
			if (switcher[2] == 1) switcher[2] = 0;\
			else switcher[2] = 1;\
		\} \
		\
		radio.stopListening();\
		delay(20);\
		radio.setChannel(10);\
		delay(20);\
		radio.openWritingPipe(0xAABBCCDD11);\
		//radio.openReadingPipe(1,0xAABBCCDD12);\
		msg[0] = 8;\
		msg[1] = LED_state[0];\
		msg[2] = LED_state[1];\
		msg[3] = LED_state[2];\
		msg[4] = LED_state[3];\
		ok = radio.write(msg, 9);\
    \
		delay(20);\
    \}\
	else if (parameters.find("L4=0")!=std::string::npos)\
	\{\
		all_ON =true;\
		if (LED_state[3] != 0)\
		\{\
			if (switcher[3] == 1) switcher[3] = 0;\
			else switcher[3] = 1;\
			LED_state[3] = 0;\
		\}\
		if (LED_state[0] != 0) \
		\{ \
			LED_state[0] = 0;\
			if (switcher[0] == 1) switcher[0] = 0;\
			else switcher[0] = 1;\
		\}\
		if (LED_state[1] != 0) \
		\{ \
			LED_state[1] = 0;\
			if (switcher[1] == 1) switcher[1] = 0;\
			else switcher[1] = 1;\
		\}\
		if (LED_state[2] != 0) \
		\{ \
			LED_state[2] = 0;\
			if (switcher[2] == 1) switcher[2] = 0;\
			else switcher[2] = 1;\
		\}  \
\
		radio.stopListening();\
		delay(20);\
		radio.setChannel(10);\
		delay(20);\
		radio.openWritingPipe(0xAABBCCDD11);\
		//radio.openReadingPipe(1,0xAABBCCDD12);\
		msg[0] = 8;\
		msg[1] = LED_state[0];\
		msg[2] = LED_state[1];\
		msg[3] = LED_state[2];\
		msg[4] = LED_state[3];\
		ok = radio.write(msg, 9);\
    \
		delay(20);\
	\}  \
	\
	if (!all_ON)\
	\{\
		if (parameters.find("L1=1")!=std::string::npos)\
		\{ \
			LED_state[0] = 1;\
			if (switcher[0] == 1) switcher[0] = 0;\
			else switcher[0] = 1;\
		\}\
		else if (parameters.find("L1=0")!=std::string::npos) \
		\{\
			LED_state[0] = 0;\
			if (switcher[0] == 1) switcher[0] = 0;\
			else switcher[0] = 1;\
		\}\
		if (parameters.find("L2=1")!=std::string::npos)\
		\{ \
			LED_state[1] = 1;\
			if (switcher[1] == 1) switcher[1] = 0;\
			else switcher[1] = 1;\
		\}\
		else if (parameters.find("L2=0")!=std::string::npos) \
		\{\
			LED_state[1] = 0;\
			if (switcher[1] == 1) switcher[1] = 0;\
			else switcher[1] = 1;\
		\}\
		if (parameters.find("L3=1")!=std::string::npos) \
		\{\
			LED_state[2] = 1;\
			if (switcher[2] == 1) switcher[2] = 0;\
			else switcher[2] = 1;\
		\}\
		else if (parameters.find("L3=0")!=std::string::npos) \
		\{\
			LED_state[2] = 0;\
			if (switcher[2] == 1) switcher[2] = 0;\
			else switcher[2] = 1;\
		\}\
		\
		radio.stopListening();\
		delay(20);\
		radio.setChannel(10);\
		delay(20);\
		radio.openWritingPipe(0xAABBCCDD11);\
		//radio.openReadingPipe(1,0xAABBCCDD12);\
		msg[0] = 8;\
		msg[1] = LED_state[0];\
		msg[2] = LED_state[1];\
		msg[3] = LED_state[2];\
		msg[4] = LED_state[3];\
		ok = radio.write(msg, 9);\
    \
		delay(20);\
	\}\
	\
	if (parameters.find("RL=1")!=std::string::npos)\
	\{\
		LED_state[5] = 1;\
		if (switcher[5] == 1) switcher[5] = 0;\
			else switcher[5] = 1;\
		\
		radio.stopListening();\
		delay(20);\
		radio.setChannel(20);\
		delay(20);\
		radio.openWritingPipe(0xAABBCCDD22);\
		msg[0] = 8;\
		msg[1] = LED_state[5];\
		ok = radio.write(msg, 9);\
		\
		delay(20);\
	\}\
	else if (parameters.find("RL=0")!=std::string::npos) \
	\{\
		LED_state[5] = 0;\
		if (switcher[5] == 1) switcher[5] = 0;\
			else switcher[5] = 1;\
		\
		radio.stopListening();\
		delay(20);\
		radio.setChannel(20);\
		delay(20);\
		radio.openWritingPipe(0xAABBCCDD22);\
		msg[0] = 8;\
		msg[1] = LED_state[5];\
		ok = radio.write(msg, 9);\
		\
		delay(20);\
	\}\
	\
	 \
		\
	if (parameters.find("WND=1")!=std::string::npos)\
	\{ \
		WND_state[0] = 1;\
		if (switcher[6] == 1) switcher[6] = 0;\
			else switcher[6] = 1;\
			\
		radio.stopListening();\
		delay(20);\
		radio.setChannel(30);\
		delay(20);\
		radio.openWritingPipe(0xAABBCCDD33);\
		//radio.openReadingPipe(1,0xAABBCCDD34);\
		msg[0] = 8;\
		msg[1] = WND_state[0];\
		ok = radio.write(msg, 9);\
		\
		delay(20);\
	\}\
	else if (parameters.find("WND=0")!=std::string::npos) \
	\{\
		WND_state[0] = 0;\
		if (switcher[6] == 1) switcher[6] = 0;\
			else switcher[6] = 1;\
		\
		radio.stopListening();\
		delay(20);\
		radio.setChannel(30);\
		delay(20);\
		radio.openWritingPipe(0xAABBCCDD33);\
		//radio.openReadingPipe(1,0xAABBCCDD34);\
		msg[0] = 8;\
		msg[1] = WND_state[0];\
		ok = radio.write(msg, 9);\
		\
		delay(20);\
	\}\
	\
	\
	\
	if (parameters.find("BL=1")!=std::string::npos) \
	\{\
		LED_state[6] = 1;\
		if (switcher[7] == 1) switcher[7] = 0;\
			else switcher[7] = 1;\
			\
		radio.stopListening();\
		delay(20);\
		radio.setChannel(70);\
		delay(20);\
		radio.openWritingPipe(0xAABBCCDD77);\
		//radio.openReadingPipe(1,0xAABBCCDD78);\
		msg[0] = 8;\
		msg[1] = LED_state[6];\
		ok = radio.write(msg, 9);\
		\
		delay(20);\
	\}\
	else if (parameters.find("BL=0")!=std::string::npos) \
	\{\
		LED_state[6] = 0;\
		if (switcher[7] == 1) switcher[7] = 0;\
			else switcher[7] = 1;\
		\
		radio.stopListening();\
		delay(20);\
		radio.setChannel(70);\
		delay(20);\
		radio.openWritingPipe(0xAABBCCDD77);\
		//radio.openReadingPipe(1,0xAABBCCDD78);\
		msg[0] = 8;\
		msg[1] = LED_state[6];\
		ok = radio.write(msg, 9);\
		\
		delay(20);\
	\}\
	\
	\
	\
	\
	if (parameters.find("TEA=1")!=std::string::npos) \
	\{\
		radio.stopListening();\
		delay(20);\
		radio.setChannel(80);\
		delay(20);\
		radio.openWritingPipe(0xAABBCCDD88);\
		msg[0] = 8;\
		ok = radio.write(msg, 9);\
		\
		delay(20);\
	\}\
	\
	if (parameters.find("FLR=1")!=std::string::npos) \
	\{\
		radio.setChannel(50);\
		radio.openWritingPipe(0xAABBCCDD55);\
		delay(50);\
		msg[0] = 8;\
		ok = radio.write (msg, 9);\
		delay(500);\
		if (ok) printf("plants in room checked!\\n");\
	\}\
	if (parameters.find("FLB=1")!=std::string::npos) \
	\{\
		radio.setChannel(60);\
		radio.openWritingPipe(0xAABBCCDD66);\
		delay(50);\
		msg[0] = 8;\
		ok = radio.write (msg, 9);\
		delay(20);\
		if (ok) printf("plants in bedroom checked!\\n");\
	\}\
	\
		\
\}\
\
class json_service : public cppcms::rpc::json_rpc_server \
\{\
public:\
    json_service(cppcms::service &srv) : cppcms::rpc::json_rpc_server(srv)\
    \{\
    	bind("check",cppcms::rpc::json_method(&json_service::check,this),method_role);\
    	bind("change",cppcms::rpc::json_method(&json_service::change,this),method_role);\
    	bind("checkAuth",cppcms::rpc::json_method(&json_service::checkAuth,this),method_role);\
    \}\
    void checkAuth(std::string parameters)\
    \{\
    	return_result(XML_response(false));\
    \}\
    void check(std::string parameters)\
    \{\
    	return_result(XML_response(true));\
    \}\
    void change(std::string parameters)\
    \{\
        setParameters(parameters);\
    	return_result(XML_response(true));\
    \}\
    //client.println(F("HTTP/1.0 401 Unauthorized"));\
    //client.println(F("WWW-Authenticate: Basic realm=\\"Devastor's Home\\""));\
\};\
\
void setupUART(void)\
\{\
	//-------------------------\
	//----- SETUP USART 0 -----\
	//-------------------------\
	//At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively\
	uart0_filestream = -1;\
	\
	//OPEN THE UART\
	//The flags (defined in fcntl.h):\
	//	Access modes (use 1 of these):\
	//		O_RDONLY - Open for reading only.\
	//		O_RDWR - Open for reading and writing.\
	//		O_WRONLY - Open for writing only.\
	//\
	//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status\
	//											if there is no input immediately available (instead of blocking). Likewise, write requests can also return\
	//											immediately with a failure status if the output can't be written immediately.\
	//\
	//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.\
	uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode\
	if (uart0_filestream == -1)\
	\{\
		//ERROR - CAN'T OPEN SERIAL PORT\
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\\n");\
	\}\
	\
	//CONFIGURE THE UART\
	//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):\
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000\
	//	CSIZE:- CS5, CS6, CS7, CS8\
	//	CLOCAL - Ignore modem status lines\
	//	CREAD - Enable receiver\
	//	IGNPAR = Ignore characters with parity errors\
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)\
	//	PARENB - Parity enable\
	//	PARODD - Odd parity (else even)\
	struct termios options;\
	tcgetattr(uart0_filestream, &options);\
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate\
	options.c_iflag = IGNPAR | ICRNL;\
	options.c_oflag = 0;\
	options.c_lflag = 0;\
	tcflush(uart0_filestream, TCIFLUSH);\
	tcsetattr(uart0_filestream, TCSANOW, &options);\
\}\
\
\
int main(int argc,char ** argv)\
\{\
	getTime(true);\
	srand (time(NULL));\
\
	// Setup and configure rf radio\
	radio.begin();\
	radio.enableDynamicPayloads();\
	radio.enableAckPayload();\
	radio.setPALevel(RF24_PA_MAX);\
	//radio.setChannel(10);\
	// optionally, increase the delay between retries & # of retries\
	radio.setRetries(15,15);\
	radio.startListening();\
	radio.stopListening();\
	// Dump the configuration of the rf unit for debugging\
	radio.printDetails(); \
    \
	if (!bcm2835_init())\
		return 1;\
	bcm2835_gpio_fsel(buzzPin, BCM2835_GPIO_FSEL_OUTP);\
\
bcm2835_gpio_fsel(LED_PIN1, BCM2835_GPIO_FSEL_OUTP);\
bcm2835_gpio_fsel(LED_PIN2, BCM2835_GPIO_FSEL_OUTP);\
bcm2835_gpio_fsel(LED_PIN3, BCM2835_GPIO_FSEL_OUTP);\
bcm2835_gpio_fsel(LED_PIN4, BCM2835_GPIO_FSEL_OUTP);\
bcm2835_gpio_fsel(LED_PIN5, BCM2835_GPIO_FSEL_OUTP);\
bcm2835_gpio_fsel(LED_PIN6, BCM2835_GPIO_FSEL_OUTP);\
	\
bcm2835_gpio_write(LED_PIN1, 0);\
bcm2835_gpio_write(LED_PIN2, 0);\
bcm2835_gpio_write(LED_PIN3, 0);\
bcm2835_gpio_write(LED_PIN4, 0);\
bcm2835_gpio_write(LED_PIN5, 0);\
bcm2835_gpio_write(LED_PIN6, 0);\
\
bcm2835_gpio_write(buzzPin, 1);\
	delay(500);\
bcm2835_gpio_write(buzzPin, 0);\
    setupUART();\
    \
	accept_Data();\
	\
	sf::Thread thread(&updateThread);\
    thread.launch();\
    \
    \
    \
	try \
	\{\
        cppcms::service srv(argc,argv);\
    	srv.applications_pool().mount( cppcms::applications_factory<json_service>());\
    	srv.run();\
    \}\
    catch(std::exception const &e) \{\
    	std::cerr << e.what() << std::endl;\
    	return 1;\
    \}\
	\
	\
    return 0;\
  \
\}\
\
}