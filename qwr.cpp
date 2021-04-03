//https://news.naver.com/main/read.nhn?mode=LS2D&mid=shm&sid1=102&sid2=257&oid=449&aid=0000206467

#include <SoftwareSerial.h>
SoftwareSerial bt = SoftwareSerial(7, 8);
//--블루투스 통신을 위한 입출력 핀----
int rxpin = 7;
int txpin = 8;
//--------------------------------
//--액추에이터 및 센서 입출력 핀-----
int GasSenVin = 12;
int GasSenOut = A0;
int Piezo = 4;
//--------------------------------
//--센서값 보정을 위한 변수----------
double senVal = 0;
double senVol = 0;
double rs_gas;
double r0;
double ratio;
int R2 = 2000;
double ppm;
//--------------------------------
//--일산화탄소 노출 시간 변수--------
unsigned long passedTime1 = 0;
unsigned long passedTime2 = 0;
unsigned long passedTime3 = 0;
int danger3 = 0;
int danger2 = 0;
int danger1 = 0;
//--------------------------------
//--블루투스 제어변수---------------
int onoff = 1;
double getppm();
void setup(){
  
  pinMode(GasSenVin, OUTPUT);
  digitalWrite(GasSenVin, HIGH);
  Serial.begin(9600);
  bt.begin(9600);
  //----------------------------------------
  //-------ppm계산을 위한 센서값 보정----------
  //delay(60000); //히터 60초 동안 예열
  for(int i = 0;i<100;i++){
    senVal += analogRead(GasSenOut);
    delay(90); // 90초 동안 센서값 입력
  }
  senVal /= 100.0;
  senVol = senVal*5.0/1024.0;//전압으로 변경
  rs_gas = ((5.0 * R2)/senVol) -R2;
  r0 = rs_gas / 1;
  //----------------------------------------
  //----------------------------------------
}
void loop(){
  ppm = getppm();
  Serial.print("PPM: ");
  Serial.println(ppm);
  delay(1000);
  
  if(bt.available());
    
  if(onoff){
    if(ppm > 90){
      if(danger2){
        passedTime3 = map(passedTime2, 0, 60*60, 0, 60*15);
        danger2 = 0;
      }
      else if(danger1){
      	passedTime3 = map(passedTime1, 0, 60*60*8, 0, 60*15);
        danger1 = 0;
      }
      danger3 = 1;
      passedTime3 += 1;
      Serial.print("\nTime : ");
      Serial.print(passedTime3);
      if(passedTime3 > 60*15)
        tone(Piezo,700,500);
    }
    else if(ppm > 25){
      if(danger3){
        passedTime2 = map(passedTime3, 0, 60*15, 0, 60*60);
        danger3 = 0;
      }
      else if(danger1){
      	passedTime2 = map(passedTime1, 0, 60*60*8, 0, 60*60);
        danger1 = 0;
      }
      danger2 = 1;
      passedTime2 += 1;
      Serial.print("\nTime : ");
      Serial.print(passedTime2);
      if(passedTime2 > 60*60)
        tone(Piezo,700,500);
    }
    else if(ppm > 10){
      if(danger3){
        passedTime1 = map(passedTime3, 0, 60*15, 0, 60*60*8);
        danger3 = 0;
      }
      else if(danger2){
      	passedTime1 = map(passedTime2, 0, 60*60, 0, 60*60*8);
        danger2 = 0;
      }
      danger1 = 1;
      passedTime1 += 1;
      Serial.print("\nTime : ");
      Serial.print(passedTime1);
      if(passedTime1 > 60*60*8)
        tone(Piezo,700,500);
    }
    else{
      passedTime1 = 0;
      passedTime2 = 0;
      passedTime3 = 0;
    }
  }
}
double getppm(){
  senVal = analogRead(A0);
  senVol = senVal/1024*5.0;
  rs_gas = (5.0-senVol)/senVol;
  ratio = rs_gas/r0;
  double x = 1538.46 * ratio;
  double ppm = pow(x,-1.709);
  return ppm;
}
