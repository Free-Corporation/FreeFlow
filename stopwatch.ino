#include  <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <Key.h>
#include <Keypad.h>
LiquidCrystal_I2C lcd(0x27,16,2);

bool startState = LOW;
bool resetState = LOW;

unsigned long startMillis;
unsigned long currentMillis;
unsigned long elapsedMillis;

int reciverPin = 11;
IRrecv irrecv(reciverPin);

decode_results results;

String weight;
float flow;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','n'},
  {'4','5','6','n'},
  {'7','8','9','r'},
  {'#','0','*','s'}
};

byte rowPins[ROWS] = { 2, 3, 4, 5 };
byte colPins[COLS] = { 6, 7, 8, 9 };

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  delay(500);
  lcd.clear();
  lcd.setCursor(3, 0);
  Serial.begin(9600);
  irrecv.enableIRIn();
  kpd.setHoldTime(500);  
  kpd.setDebounceTime(250);
}

void loop() {

  if (startState)
  {
    currentMillis = millis();
    elapsedMillis = (currentMillis - startMillis);
    unsigned long durMS = (elapsedMillis%1000);       //Milliseconds
    unsigned long durSS = (elapsedMillis/1000)%60;    //Seconds
    unsigned long durMM = (elapsedMillis/(60000))%60; //Minutes
    String durMilliSec = timeMillis(durMM, durSS,durMS);
    lcd.setCursor(3, 0);
    lcd.print(durMilliSec);
    delay(150);
  }

  if (resetState)
  {
    // clear screen for the next loop:
    lcd.clear();
    lcd.setCursor(3, 0);
    delay(100);
    resetState = LOW;
  }
  
  if (irrecv.decode(&results)) {
  Serial.println(results.value, HEX);
  if(results.value == 0xFF30CF || results.value == 0x9716BE3F) weight += 1;
  if(results.value == 0xFF18E7 || results.value == 0x3D9AE3F7) weight += 2;
  if(results.value == 0xFF7A85 || results.value == 0x6182021B) weight += 3;
  if(results.value == 0xFF10EF || results.value == 0x8C22657B) weight += 4;
  if(results.value == 0xFF38C7 || results.value == 0x488F3CBB) weight += 5;
  if(results.value == 0xFF5AA5 || results.value == 0x449E79F)  weight += 6;
  if(results.value == 0xFF42BD || results.value == 0x32C6FDF7) weight += 7;
  if(results.value == 0xFF4AB5 || results.value == 0x1BC0157B) weight += 8;
  if(results.value == 0xFF52AD || results.value == 0x3EC3FC1B) weight += 9;
  if(results.value == 0xFF6897 || results.value == 0xC101E57B) weight += 0;
  if(results.value == 0xFFE01F || results.value == 0xF076C13B) weight.remove(weight.length()-1);
  if(results.value == 0xFFC23D || results.value == 0x20FE4DBB) (startState = !startState) && (startMillis = millis());
  if(weight.length() == 5) weight.remove(weight.length()-1);
  lcd.clear();
  lcd.setCursor(3, 0);
  unsigned long durMS = (elapsedMillis%1000);       //Milliseconds
  unsigned long durSS = (elapsedMillis/1000)%60;    //Seconds
  unsigned long durMM = (elapsedMillis/(60000))%60; //Minutes
  String durMilliSec = timeMillis(durMM, durSS,durMS);
  if(results.value == 0xFF22DD || results.value == 0x52A3D41F) (weight = "") && (flow = '0');    
  lcd.print(durMilliSec);
  lcd.setCursor(3, 1);
  lcd.print(weight);
  lcd.setCursor(8, 1);
  float elapsedMillisFloat = (float) elapsedMillis;
  flow = 3600 * weight.toFloat() / elapsedMillisFloat;
  lcd.print(flow);        
  irrecv.resume(); // готовы принимать следующий сигнал
  }
  
  char key = kpd.getKey();
  if(key)  
  {
    switch (key)
    {
      case '*':
      lcd.clear();
      weight.remove(weight.length()-1);
        break;
      case '#':
        break;
      case 'n':
        break;
      case 's':
      (startState = !startState) && (startMillis = millis());
      case 'r':
      (weight = "") && (flow = '0');    
      break;
      default:
        weight += key;
    }
  lcd.clear();
  lcd.setCursor(3, 0);
  unsigned long durMS = (elapsedMillis%1000);       //Milliseconds
  unsigned long durSS = (elapsedMillis/1000)%60;    //Seconds
  unsigned long durMM = (elapsedMillis/(60000))%60; //Minutes
  String durMilliSec = timeMillis(durMM, durSS,durMS);
  if(weight.length() == 5) weight.remove(weight.length()-1);
  lcd.print(durMilliSec);    
  lcd.setCursor(3, 1);  
  lcd.print(weight);    
  lcd.setCursor(8, 1);
  float elapsedMillisFloat = (float) elapsedMillis;
  flow = 3600 * weight.toFloat() / elapsedMillisFloat;
  lcd.print(flow);        
  }
}

String timeMillis(unsigned long Mintime,unsigned long Sectime,unsigned long MStime)
{
  String dataTemp = "";
  if (Mintime < 10)
  {
    dataTemp = dataTemp + "0" + String(Mintime)+ ":";
  }
  else{
    dataTemp = dataTemp + String(Mintime)+ ":";
  }
  if (Sectime < 10)
  {
    dataTemp = dataTemp + "0" + String(Sectime)+ ":";
  }
  else{
    dataTemp = dataTemp + String(Sectime)+ ":";
  }
  dataTemp = dataTemp + String(MStime);
  return dataTemp;
}
