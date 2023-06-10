


#define BLYNK_TEMPLATE_ID "TMPL5kojJYqWE"
#define BLYNK_DEVICE_NAME "VOLTMETER"
#define BLYNK_AUTH_TOKEN "-nuQJ6TFGcVsYKY13vPtGmf_-3jsCPSD"

char ssid[] = "SFR_EDD0";
char pass[] = "vigen201402";


#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiManager.h>
#include <ACS712.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x23, 16, 2);
 ACS712 sensor(ACS712_20A,35);



  const int  relayPin = 5;
  const int  buttonPin = 4;
  const int  voltage_pin = 34 ;

/*
#define VPIN_volt       V1
#define VPIN_current    V2
#define VPIN_amphors    V3
#define VPIN_watt       V4
 */
  int  voltRead = 0;
  int  ampRead = 0;
  long current = 0.0;         
  long voltage = 0.0;
  float watt  = 0.0;
  float VOLTtotal = 0.0;
  float AMPtotal = 0.0;
  float voltcorect = 6.470;
  float  ampcorrect = 107.0;
  float ampHours = 0.0;
unsigned long previousMillis = 0;
unsigned long interval = 3600; 
  
   
   bool relayActive = false;
   bool buttonState = false;
   bool prevButtonState = true;
 

char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;

void volt_update() {
  for(int i=0;i<200;i++) {
  voltRead = analogRead(voltage_pin);
  voltage += voltRead ;
  delay(1);
  } 
  //voltage = map(rawVoltage, 0, 4095, 0, 3300) / 1000.0;
   voltage = voltage / 200 ;
   VOLTtotal = voltage *(3.30 / 4095);
   VOLTtotal = VOLTtotal * voltcorect ;
   Blynk.virtualWrite(V1,VOLTtotal);
   Serial.print("VOLTtotal");
   Serial.println(VOLTtotal);
}
  void amp_update(){
  for(int i=0;i<200;i++) {
  ampRead =  sensor.getCurrentDC();
  current += ampRead ;
  delay(1);
  } 
   
   current = current / 200 ;
   AMPtotal = current *(1.0 / 4095);
   AMPtotal = AMPtotal * ampcorrect ;
  Serial.print("AMPtotal");
  Serial.println(AMPtotal);
 Blynk.virtualWrite(V2,AMPtotal);
 }
   
    void watt_update(){
    for (int i = 0; i < 10; i++) {
    watt = AMPtotal * VOLTtotal ;
    delay(1);
  } 

Blynk.virtualWrite(V4,watt);
Serial.print("watt");
Serial.println(watt);

}

void AH_update(){
unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    float currentHours = AMPtotal / 3600.0;
    ampHours += currentHours;
    previousMillis = currentMillis;
  }
   Serial.print("ampHours");
   Serial.println(ampHours);
   Blynk.virtualWrite(V3,ampHours);
}

void updatebutton() {
if (AMPtotal > 5 && !relayActive) {
    digitalWrite(relayPin, HIGH);
    relayActive = true;
  }
  
  if (buttonState == LOW && prevButtonState == HIGH) {
    digitalWrite(relayPin, LOW);
    relayActive = false;
  }
  
  buttonState = digitalRead(buttonPin);
  prevButtonState = buttonState;


 }
  /*
void sendsensors() {

        
  Blynk.virtualWrite(VPIN_volt,VOLTtotal);
  Blynk.virtualWrite(VPIN_current,AMPtotal);
  Blynk.virtualWrite(VPIN_watt,watt);
  Blynk.virtualWrite(VPIN_amphors,ampHours);

}
  */

void updateVoltage() {
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(VOLTtotal);
  lcd.print("     ");
}

 
void updateCurrent() {
  lcd.setCursor(0, 1);
  lcd.print("I:");
  lcd.print(AMPtotal);
  lcd.print("     ");
  if (relayActive) {
  lcd.setCursor(0, 1); 
  lcd.print("Rely:ON "); 
  }
}
 
void updatepower() {
  lcd.setCursor(9, 0);
  lcd.print("P:");
  lcd.print(watt);
  lcd.print("     ");
}

void updateEnergy(){

  lcd.setCursor(9, 1);
  lcd.print("H:");
  lcd.print(ampHours);
  lcd.print("     ");
}



void setup(){
    Serial.begin(9600);
    WiFi.begin(ssid, pass);
    Serial.print("Connecting to WiFi");
    delay(5000);
    WiFiManager wifiManager;
    wifiManager.autoConnect("BLYNK-IOT");
    Serial.println(WiFi.localIP());  
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  delay(1000);
    Blynk.config(auth);
  delay(500);
  Blynk.virtualWrite(V1,VOLTtotal);
  Blynk.virtualWrite(V2,AMPtotal);
  Blynk.virtualWrite(V3,ampHours);
  Blynk.virtualWrite(V4,watt);
  delay(500);
  pinMode(voltage_pin, INPUT);
//pinMode(current_pin, INPUT);     
  pinMode(relayPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(relayPin, LOW);  
  //timer.setInterval(1000L, sendsensors);
  delay(1000);

}
  

void loop()
{
        volt_update();
        amp_update();
        watt_update();
        AH_update();
        
      
 
        
        updateVoltage();
        updateCurrent();
        updatepower();
        updateEnergy();
        updatebutton();
  
  Blynk.run();
  timer.run(); // Initiates SimpleTimer

  delay(500);
}
