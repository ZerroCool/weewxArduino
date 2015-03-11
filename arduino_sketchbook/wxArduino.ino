#include <stdlib.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimerOne.h>

// Using Adafruit library - is there something lighter specific to BMP085?
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

// 1-wire data wire is plugged into port 10 on the Arduino
#define ONE_WIRE_BUS 10

// Setup a oneWire instance
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

String weatherData;
String timeString = "2015/02/28 12:34:56"; 
char   valueAsChar[8];
String inputLine;

// outsideThermometer
DeviceAddress outsideThermometer = { 0x28, 0x30, 0x22, 0x67, 0x03, 0x00, 0x00, 0x7A };
//equipmentThermometer
DeviceAddress equipmentThermometer = { 0x28, 0x1B, 0x64, 0x67, 0x03, 0x00, 0x00, 0x41 };

float temperature;
unsigned long ts = 1425000000; // default start time. weewx driver immediately sets time

void setup(void) {
  Serial.begin(9600);
  /* Initialise the sensor */
  if(!bmp.begin()) //
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  // Start up the DS library
  sensors.begin();
  sensors.setResolution(outsideThermometer, 12);
  
  pinMode(13, OUTPUT);    
  Timer1.initialize(1000000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
}

void loop(void) {
    /* Get a new sensor event */
    sensors_event_t event;
    bmp.getEvent(&event);
    weatherData = String(ts);                   // TIMESTAMP

    /* Display the results (barometric pressure is measured in Pa) */
    if (event.pressure) {
      float pressure;
      bmp.getPressure(&pressure);
      // convert from Pascal to millibar and then to a String
      dtostrf(0.01 * pressure, 7, 2, valueAsChar);
      weatherData += "," + String(valueAsChar); // PRESSURE
      
      bmp.getTemperature(&temperature);
      dtostrf(temperature, 5, 2, valueAsChar);
      weatherData += "," + String(valueAsChar); // TEMP IN - onboard BMP180
      
      sensors.requestTemperatures();
      
      temperature = sensors.getTempC(outsideThermometer);
      dtostrf(temperature, 5, 2, valueAsChar);
      weatherData += "," + String(valueAsChar); // TEMP OUT - 1 wire
      
      temperature = sensors.getTempC(equipmentThermometer);
      dtostrf(temperature, 4, 1, valueAsChar);
      weatherData += "," + String(valueAsChar); // Equipment - 1 wire
      
      float humidity = random(50, 76); 
      dtostrf(humidity, 4, 1, valueAsChar);
      weatherData += "," + String(valueAsChar); // HUMIDITY - Hfx average March
      
      float windDir = 0.0;
      windDir += random(-10, 20); 
      if (windDir > 360.0){ windDir -= 360.0;}
      else if (windDir < 0.0){ windDir += 360.0;}
      dtostrf(windDir, 4, 1, valueAsChar);
      weatherData += "," + String(valueAsChar); // WIND DIRECTION
      
      float windSpeed = 0.0;
      windSpeed = random(0, 25); 
      dtostrf(windSpeed, 4, 1, valueAsChar);
      weatherData += "," + String(valueAsChar); // WIND SPEED
      
      float windGust = windSpeed + random(0, 10); 
      dtostrf(windGust, 4, 1, valueAsChar);
      weatherData += "," + String(valueAsChar); // WIND GUST
      // debugging Serial.println(weatherData);
    }
    else{
      Serial.println("Sensor error");
    }
    
    while (Serial.available() > 0) {
      //delay(3);
      if (Serial.available() >0) {
        char c = Serial.read();
        inputLine += c;
      } 
    }
    //Serial.print("Got this: ");
    //Serial.println(inputLine);
    if (inputLine.length() > 0) {
      if(inputLine == "HEADER") {
        Serial.println("HDR,TIMESTAMP,PRESSURE,TEMP IN,TEMP OUT,EQUIPMENT,HUMIDITY,WIND DIRECTION,WIND SPEED,WIND GUST");       
      }
      else if(inputLine == "TIME?") {
        Serial.println(ts);
      }
      else if(inputLine.startsWith("TIME=")) {
        timeString = inputLine.substring(5,24); // consume string part
        timeString = inputLine.substring(25,35);// read long int part
        ts = timeString.toInt();
        Serial.println("OK");
      } 
      else if(inputLine.startsWith("NOW")) {
        Serial.println(weatherData);
      }
      else if(inputLine.startsWith("DOWNLOAD")) {
        Serial.println(weatherData);
      }
      else if(inputLine.startsWith("LOGINT?")) {
        Serial.println("60");
      } 
      inputLine = "";
    } 
    delay(1000);
}

void timerIsr()
{
    // Toggle LED
    digitalWrite( 13, digitalRead( 13 ) ^ 1 );
    // increment seconds counter
    ts += 1;
}
