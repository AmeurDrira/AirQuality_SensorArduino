#include <dht.h>
#define dht_apin A15
#define delai 500
#define  delaiOfSend 20000
#define dht_apin A15
#define PinHumiditSol A0
#define PinLight A2
#define airqual A1
#define PinSound A3
#define dustpin 8

dht DHT;
unsigned int moisture;
unsigned int light;
unsigned int sound;
float tempAir;
float dust;
unsigned int humiditeAir;
unsigned int airQuality;

unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 500;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
String frameHeader = "{";
String frameSound = "\'sound\':";
String comma = ",";
String frameLight = "\'light\':";
String frameTemp = "\'temp\':";
String frameAirHumidity = "\'airHumidity\':";
String frameMoisture = "\'moisture\':";
String frameAirQuality = "\'airQuality\':";
String frameDust = "\'dust\':";
String frameTailer = "}";

void setup() {
  starttime = millis();
  pinMode(dustpin, INPUT);
  Serial.begin(9600);
  /*initialisation transmission Lora*/
  Serial1.begin(57600);
  Serial.println("Startup");
  Serial1.println("mac set dr 5");
  delay(2000);
  Serial1.println("mac join abp");
  delay(2000);
  Serial1.println("mac save");
  delay(2000);
}

void loop() {
  /*appel des méthodes des capteurs instialisation des attributs*/
  soundSensor();
  lightSensor();
  humiditeAirSensor();
  temperatureAirSensor();
  moistureSensor();
  airQualitySensor() ;
  dustAnalysis();

  Serial1.print("mac tx uncnf 3 ");
  String toSend = frameHeader + frameSound + sound + comma + frameLight + light + comma + frameTemp + tempAir;
  toSend = toSend + comma + frameAirHumidity + humiditeAir + comma + frameMoisture + moisture + comma + frameAirQuality + airQuality + comma + frameDust + dust + frameTailer;
  Serial.print("about to send ");
  Serial.println(toSend.length());
  writeString(toSend);
  Serial1.println();
  delay(delaiOfSend);
}

void serialEvent1() {
  while (Serial1.available()) {
    Serial.println(Serial1.readStringUntil('\n'));
  }
}

void temperatureAirSensor() {
  DHT.read11(dht_apin);
  tempAir = DHT.temperature;
}

void humiditeAirSensor() {
  DHT.read11(dht_apin);
  humiditeAir = DHT.humidity;

}

void moistureSensor() {
  moisture = analogRead(PinHumiditSol);

}

void airQualitySensor() {
  airQuality = analogRead (airqual);

}

void lightSensor() {
  light = analogRead(PinLight);
}

void soundSensor() {
  sound = analogRead(PinSound);
}
void dustAnalysis() {
  duration = pulseIn(dustpin, LOW);
  lowpulseoccupancy = lowpulseoccupancy + duration;

  if ((millis() - starttime) >= sampletime_ms) //if the sampel time = = 30s
  {
    ratio = lowpulseoccupancy / (sampletime_ms * 10.0); // Integer percentage 0=&gt;100
    dust = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62; // using spec sheet curve
    Serial.print("Dust concentration = ");

    Serial.print(dust);
    Serial.println(" pcs/0.01cf");
    Serial.print("\n");
    delay(500);
    lowpulseoccupancy = 0;
    starttime = millis();

  }

}

void writeString(String stringData) {
  for (int i = 0; i < stringData.length(); i++) {
    Serial1.print((int)stringData[i], HEX);
  }
}
