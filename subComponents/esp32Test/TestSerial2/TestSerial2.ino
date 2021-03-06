#include "HardwareSerial.h"

// unsigned long maxTimeout = 20; //20 mS timeOut
// int speed;
// int limit=0;

// #define UART_NUM 2
// #define UART_RX 16
// #define UART_TX 17
#define UART2_NUM 1
#define UART2_RX 16
#define UART2_TX 4 // 17 does not seems to emit (TX2 labeled pin)

#define UART3_NUM 2
#define UART3_RX 26
#define UART3_TX 14

HardwareSerial Serial2(UART2_NUM);
HardwareSerial Serial3(UART3_NUM);

// void setTimeOut(unsigned long n){
//   maxTimeout = n;
// }

// unsigned long getTimeOut(void){
//   return maxTimeout;
// }

// int serial_read(uint8_t *dataStream, int size){
//   int count = 0;
//   unsigned long timeOut = getTimeOut() + millis(); //Actual time
  
//   while((millis() < timeOut) && (count < size)){// Wait until data recived and no timeout
//     if(Serial2.available()){
//       dataStream[count]=Serial2.read();
//       count++;
//     }
//     yield();
//   }
//   return count;                                   
// }

// void setSerial(int bus_speed, byte parity){ //TODO velocity changes hangs on
//   speed = bus_speed;
//   //Serial2.flush(); //wait to the send buffer
//   delay(2); //wait for the last char
//   if(parity){
//     Serial2.begin(bus_speed, SERIAL_8E1);
//   }else{
//     Serial2.begin(bus_speed, SERIAL_8N1);
//   }
//   yield();
// }

void setup() {
  //Serial2.begin(19200, SERIAL_8N1);
  Serial2.begin(9600, SERIAL_8N1, UART2_RX, UART2_TX, false);

  Serial3.begin(115200, SERIAL_8N1, UART3_RX, UART3_TX, false);

  Serial.begin(115200, SERIAL_8N1);
  pinMode(2, OUTPUT);
}

void loop() {
    Serial.write("Software started\n");

// reads but not writes!!!!!!
//Serial2.println("Hello World !!!");
Serial2.write("Hello World UART#2 !!!\n");
Serial3.write("Hello World UART#3 !!!\n");

while( true ) {
  while(Serial2.available() > 0) {
    Serial.print( (char)Serial2.read() );
  }
  while(Serial3.available() > 0) {
    Serial.print( (char)Serial3.read() );
  }
  while(Serial.available() > 0) {
    char ch = (char)Serial.read();
    Serial2.print( ch );
    Serial3.print( ch );
  }
}


  // uint8_t comm1[]={0x02, 0x1D, 0x58, 0xC2, 0x92, 0x42};
  // uint8_t resp[128];
  // uint8_t ack[]={0xCA};
  // int readed, i;
  // char cad[100], *p;

  // p = cad;
  // Serial2.write(comm1, 6);
  // //Serial2.flush();
  // readed = serial_read(resp, 1);
  // if((readed == 1) && (resp[0]==0x1A)){
  //   digitalWrite(2, HIGH);
  //   readed += serial_read(&resp[readed], 7);
  //   Serial2.write(ack, 1);
  //   digitalWrite(2, LOW);
  // }
  // for(i=0; i<readed; i++){
  //   snprintf(p, 50, "%02X", resp[i]);
  //   p+=2;
  // }
  // if(readed > 0){
  //   sprintf(p, "\n");
  //   Serial.write(cad);
  // }
  delay(100);
//   if(++limit >= 10){
//     Serial.write("Software Reset\n");
//     delay(100);
//     ESP.restart();
//   }
Serial.write("Software worked\n");
}

