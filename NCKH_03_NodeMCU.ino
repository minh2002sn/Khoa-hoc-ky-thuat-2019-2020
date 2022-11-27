#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "nckhltn";
const char* pass = "LTN190903";
const char* mqtt = "soldier.cloudmqtt.com";

int relay = D0;
int buttonpin1 = D5;
int buttonpin2 = D6;
int trig = D7;
int echo = D8;
int n = 50;
float distance;
unsigned long timer = 0;
int counter1 = 0;
int counter2 = 0;
int buttonstatus1 = 0;
int lastbuttonstatus1 = 0;
int buttonstatus2 = 0;
int lastbuttonstatus2 = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  lcd.begin();
  lcd.backlight();
  lcd.clear();

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(buttonpin1, INPUT);
  pinMode(buttonpin2, INPUT);
  pinMode(relay, OUTPUT);
  
  lcd.print("WiFi Connecting");
  setWiFi();
  
  client.setServer(mqtt, 12894);
  client.setCallback(callback);

  if(WiFi.status() == WL_CONNECTED){
    mqttconnect();
    lcd.setCursor(0,1);
    lcd.print("MQTT Connected");
  }
  delay(2000);
  
  lcd.clear();
  lcd.print("Safe distance: ");
  lcd.setCursor(4,1);
  lcd.print(n);
  lcd.print(" cm");
  delay(2000);
  
}


void loop() {
  client.loop();
  
  readbutton1();
  if(counter1 != 0 and counter1 % 2 == 1){
    ChangeDistance1();
    counter1 = 0;
  }

  readbutton2();
  if(counter2 != 0 and counter2 % 2 == 1){
    ChangeDistance2();
    counter2 = 0;
  }
  
  hc04();
  lcd.clear();
  lcd.print("Distance: ");
  lcd.print(distance);

  if(distance <= n){
    client.publish("distance", "DANGER");
    digitalWrite(relay, HIGH);
    lcd.setCursor(0,1);
    lcd.print("State: ");
    lcd.print("DANGER");
  } else {
    client.publish("distance", "SAFE");
    digitalWrite(relay, LOW);
    lcd.setCursor(0,1);
    lcd.print("State: ");
    lcd.print("SAFE");
  }
  delay(500);

}

void mqttconnect(){
   while(!client.connected()){
    delay(500);
    //Serial.println("MQTT connecting");
    if(client.connect("ESP8266client", "jcxuzvsa", "n-ruWz9Xkpc2")){
      //Serial.println("Connected");
      client.publish("state", "MQTT Connected");
      client.subscribe("state");
      client.subscribe("distance");
    }
    else{
      //Serial.print("failed with state: ");
      //Serial.println(client.state());
      delay(2000);
    }
   }
}


void callback(char* topic, byte* payload, long int length){
  //Serial.print("Message arrive: [ ");
  //Serial.print(topic);
  //Serial.print(" ]: ");
  //for(int i=0; i<length; i++){
    //Serial.print((char)payload[i]);
  //}
  //Serial.println();

}

void setWiFi(){
  delay(10);
  //Serial.println();
  //Serial.println("Dang ket noi");
  WiFi.begin(ssid, pass);
  timer = millis();
  while((unsigned long) (millis() - timer) < 60000 and WiFi.status() != WL_CONNECTED){
    delay(500);
    //Serial.print(".");
  }
  if(WiFi.status() == WL_CONNECTED){
    lcd.clear();
    lcd.print("WiFi Connected");
    //Serial.println();
    //Serial.println("Da ket noi");
    //Serial.print("Dia chi IP: ");
    //Serial.println(WiFi.localIP());
  } else{
    lcd.clear();
    lcd.print("WiFi Disconnected");
    lcd.setCursor(0,1);
    lcd.print("MQTT Disconnected");
  }
  
}

void hc04(){
  unsigned long tgian;
  
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW);

  tgian=pulseIn(echo, HIGH);
  distance=tgian/2/29.412;
  
}

void ChangeDistance1(){
  n = n + 5;
  lcd.clear();
  lcd.print("Safe distance: ");
  lcd.setCursor(4,1);
  lcd.print(n);
  lcd.print(" cm");
  delay(1000);
  
}

void ChangeDistance2(){
  n = n - 5;
  lcd.clear();
  lcd.print("Safe distance: ");
  lcd.setCursor(4,1);
  lcd.print(n);
  lcd.print(" cm");
  delay(1000);
  
}

void readbutton1(){
  buttonstatus1 = digitalRead(buttonpin1);
  if (buttonstatus1 != lastbuttonstatus1){
    if (buttonstatus1 == HIGH){
      counter1++;
    }
  }
  lastbuttonstatus1 = buttonstatus1;
}

void readbutton2(){
  buttonstatus2 = digitalRead(buttonpin2);
  if (buttonstatus2 != lastbuttonstatus2){
    if (buttonstatus2 == HIGH){
      counter2++;
    }
  }
  lastbuttonstatus2 = buttonstatus2;
}
