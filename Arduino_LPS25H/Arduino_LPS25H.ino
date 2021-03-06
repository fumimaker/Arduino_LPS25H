#include <Wire.h>
//#define debug
uint8_t DEVICE_ADDRESS = 0x5D;//SA0=VDD
bool WHO_AM_I = false;

float zero_height = 0;

void initLPS25H(void){
  Wire.begin();
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(0x0F);
  Wire.endTransmission();

  Wire.requestFrom(DEVICE_ADDRESS, 1);

  uint16_t timeout=0;
  while (!Wire.available()&&(timeout<1000)){
    timeout++;
  }
  uint8_t live = Wire.read();

  if (0xBD != live){
    WHO_AM_I = false;
    return;
  }else{
    WHO_AM_I = true;
  }

  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(0x20);
  Wire.write(0xB0); // 12.5Hz
  Wire.endTransmission();
}

void calibrate_zero(void){
  float zero=0;
  for(int i=0; i<10; i++){
    zero += get_height();
    delay(100);
  }
  zero_height = zero/10;
  #ifdef debug
  Serial.print("zero ");
  Serial.println(zero_height);
  #endif
}

float get_height(void){
  int i; uint8_t RegTbl[5];   

  for (i=0; i< 5; i++){
    Wire.beginTransmission(DEVICE_ADDRESS);
      Wire.write(0x28 + i );
    Wire.endTransmission();
  
    Wire.requestFrom(DEVICE_ADDRESS, 1);
    while(Wire.available() == 0) {}
    RegTbl[i] = Wire.read();     
  }
 

  uint16_t lo = RegTbl[1] << 8 | RegTbl[0];
  uint32_t hi = RegTbl[2] * 65536; // 16bitの左シフト
  float P = (hi +  lo) / 4096.0; 

  int temperature = (RegTbl[4] << 8 |  RegTbl[3]);
  float T = 42.5 + (temperature / 480.0); 
 
  float H = ((pow(1013.25 / P, 1 / 5.257) - 1) * (T+ 273.15)) / 0.0065;//m
  return H;//[m]
}

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  if(Serial.available()>0){
    char mozi = Serial.read();
    switch (mozi){
    case 'C':
      initLPS25H();
      if(!WHO_AM_I){
        Serial.write('N');
        //Serial.println("N");
      } else {
        calibrate_zero();
        Serial.write('O');
        //Serial.print("Hello Wolrd");
        //Serial.println("O");
      }
      break;

    case 'G':
      float tmp = 0;
//      for (int i = 0; i < 10; i++){
//        tmp += get_height();
//      }
//      tmp = tmp / 10;
      float val = get_height();
      val = get_height() - zero_height;
      if(val<0){
        val = 0;
      }

      #ifdef debug
      Serial.print("float ");
      Serial.println(val);
      #endif
      
      uint16_t temp = (uint16_t)val;
      //H = (val & 0xFF) >> 8;
      #ifdef debug
      Serial.print("temp ");
      Serial.println(temp);
      #endif
      uint8_t H = (temp>>8)&0xFF;
      uint8_t L = temp & 0xFF;
      #ifdef debug
      Serial.print("H:");
      Serial.print(H,HEX);
      Serial.print(" L:");
      Serial.println(L,HEX);
      #endif
      
      #ifndef debug
      Serial.write(H);
      Serial.write(L);
      //Serial.println(temp);
      #endif
      
      break;

    default:
      break;
    }
  }
}
