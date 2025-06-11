#define BLYNK_TEMPLATE_ID "TMPL6efuZEiHQ"
#define BLYNK_TEMPLATE_NAME "Sistem Monitoring Kelembaban Tanah"
#define BLYNK_AUTH_TOKEN "2NjnbgLKRXDRu-rrB8XHhLUKsimkidVW"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

const int SensorPin = 34; 

LiquidCrystal_I2C lcd(0x27, 16, 2);

WidgetLCD Blynklcd(V0);

char auth[] = BLYNK_AUTH_TOKEN; 
char ssid[] = "MNC_plus";     
char pass[] = "Karbala2013";     

WebServer server(80);

void sendData(){
  int SensorValue = analogRead(SensorPin) / 4;
  float SensorVolts = SensorValue * 0.0048828125;

  Blynk.virtualWrite(V1, SensorValue);
  Blynk.virtualWrite(V2, SensorVolts);

  if (SensorValue < 850){
    Blynklcd.print(0,0, "Soil Level: WET");
  } else {
    Blynklcd.print(0,0, "Soil Level: DRY");
  }
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected..");
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

  Blynk.begin(auth, ssid, pass);
}

void loop() {
  server.handleClient();
  int SensorValue = analogRead(SensorPin) / 4;
  float SensorVolts = SensorValue * 0.0048828125;


  lcd.setCursor(0,0);
  lcd.print("Soil Level: ");
  if (SensorValue < 850){
    lcd.print("WET");
  } else {
    lcd.print("DRY");
  }

  lcd.setCursor(0,1);
  lcd.print(SensorValue);
  lcd.setCursor(7,1);
  lcd.print(SensorVolts);
  lcd.setCursor(11,1);
  lcd.println(" V ");

  delay(1000); 

  Blynk.run();
  delay(1000);
  sendData();
}

void handle_OnConnect() {
  int SensorValue = analogRead(SensorPin) / 4;
  float SensorVolts = SensorValue * 0.0048828125;
  String soilLevel = "DRY";
  if (SensorValue < 850){
    soilLevel = "WET";
  }
  server.send(200, "text/html", SendHTML(SensorValue, SensorVolts, soilLevel));
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float SensorValuestat, float SensorVoltsstat, String soilLevel) {
  String ptr = "<!DOCTYPE html><html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.7.2/css/all.css\" integrity=\"sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9S\" crossorigin=\"anonymous\">\n";
  ptr += "<title>Monitoring Kondisi Tanah, ADC, dan Tegangan</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444; margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px; color: #444444; margin-bottom: 10px;} units {font-size: 1.2rem;} \n";
  ptr += "dht-labels {font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;} \n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>Monitoring Kondisi Tanah, ADC, dan Tegangan dengan ESP32</h1>\n";
  ptr += "<p>\n";
  ptr += "ADC: ";
  ptr += SensorValuestat;
  ptr += "</p>\n";
  ptr += "<p>\n";
  ptr += "Tegangan: ";
  ptr += SensorVoltsstat;
  ptr += "</p>\n";
  ptr += "<p>\n";
  ptr += "Status Tanah: ";
  ptr += soilLevel;
  ptr += "</p>\n";
  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}