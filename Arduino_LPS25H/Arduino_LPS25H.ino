#include <Wire.h>

uint8_t DEVICE_ADDRESS = 0x5D;//SA0=VDD
bool WHO_AM_I = false;

float zero_height = 0;
float rc = 0;
boolean calibrated = false;
void calibrate_zero(void){
  float zero=0;
  for(int i=0; i<50; i++){
    zero += get_height();
    //Serial.println(zero);
    delay(100);
  }
  zero_height = zero/50;
  //Serial.println(zero_height);
  calibrated = true;
}

float get_height(void){
  int i;
  uint8_t RegTbl[5];

  for (i = 0; i < 5; i++)
  {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write(0x28 + i);
    Wire.endTransmission();

    Wire.requestFrom(DEVICE_ADDRESS, 1);
    while (!Wire.available())
      ;
    RegTbl[i] = Wire.read();
  }

  uint16_t lo = RegTbl[1] << 8 | RegTbl[0];
  uint32_t hi = RegTbl[2] * 65536;
  float P = (hi + lo) / 4096.0;

  int16_t temperature = (RegTbl[4] << 8 | RegTbl[3]);
  float T = 42.5 + (temperature / 480.0);

  float H = ((pow(1013.25 / P, 1 / 5.257) - 1) * (T + 273.15)) / 0.0065;
  return H*100;
}

void setup()
{
  Serial.begin(115200);
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

void loop()
{
  uint16_t val=0;
  if(calibrated){
    int tmp;
    tmp = 0;
    rc = rc*0.98+ get_height()*0.02;
    tmp = (int)(rc - zero_height);
    if(tmp<0){
      tmp = 0;
    }
    val = (uint16_t)tmp;
//    Serial.print("rc=");
//    Serial.print(rc);
//    Serial.print(" zerop=");
//    Serial.print(zero_height);
//    Serial.print(" val=");
//    Serial.println(val);
    delay(10);
  }
  
  
  if(Serial.available()>0){
    char mozi = Serial.read();
    switch (mozi){
    case 'C':
      if(!WHO_AM_I){
        Serial.write('N');
      } else {
        calibrate_zero();
        Serial.write('O');
      }
      //Serial.print("zero=");
      //Serial.println(zero_height);
      break;

    case 'G':
      uint8_t H;
      H = (val & 0xF0) >> 8;
      uint8_t L;
      L = val & 0x0F;
      //Serial.println(val);
      Serial.print(H);
      Serial.println(L);
      
      break;

    default:
      
      break;
    }
  }
}
