#include <OneWire.h>
#include <DallasTemperature.h>
#include <dht.h>
#include "FastRunningMedian.h"


#define ONE_WIRE_BUS 2



const int inritPin = 6;    // the number of the pushbutton pin
const int tuinPin = 7;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin

const int dht22Pin = 5;

const int relay0Pin = 4;
const int relay1Pin = 3;
const int relay2Pin = A5;

const int tempPin = 2;
OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

#define DHT22_PIN 5
#define DHTTYPE DHT22

DHT dht(DHT22_PIN,DHTTYPE);




FastRunningMedian<unsigned int,32, 0> myA0Median;
int A0counter=0;
int lastA0Value=LOW;
int A0isLow= LOW;
int A0isHigh = LOW;



int inritValue;
int lastInritValue = HIGH;
int inritWatcher = HIGH;
int lastInritEntry;
int lastInritLeft;

int tuinValue;
int lastTuinValue = HIGH;
int tuinWatcher = HIGH;
int lastTuinEntry;
int lastTuinLeft;

int relay0value = LOW;
long lastRelay0active;
int relay1value = LOW;
long lastRelay1active;
int relay2value = LOW;
long lastRelay2active;

char incomingSerialChar;



void setup() {
  dht.begin();
  pinMode(inritPin, INPUT_PULLUP);
  pinMode(tuinPin, INPUT_PULLUP);
  pinMode(A0, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(relay0Pin, OUTPUT);
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  lastRelay0active = millis();
  lastRelay1active = millis();
  lastRelay2active = millis();
  digitalWrite(relay0Pin, LOW);
  digitalWrite(relay1Pin, LOW);
  digitalWrite(relay2Pin, LOW);
  // set initial LED state
  digitalWrite(ledPin, LOW);
  sensors.begin();
  //enable the serial line
  Serial.begin(9600);
}

void printDouble( double val, byte precision){
 // prints val with number of decimal places determine by precision
 // precision is a number from 0 to 6 indicating the desired decimial places
 // example: lcdPrintDouble( 3.1415, 2); // prints 3.14 (two decimal places)

 if(val < 0.0){
   Serial.print('-');
   val = -val;
 }

 Serial.print (int(val));  //prints the int part
 if( precision > 0) {
   Serial.print("."); // print the decimal point
   unsigned long frac;
   unsigned long mult = 1;
   byte padding = precision -1;
   while(precision--)
 mult *=10;

   if(val >= 0)
frac = (val - int(val)) * mult;
   else
frac = (int(val)- val ) * mult;
   unsigned long frac1 = frac;
   while( frac1 /= 10 )
padding--;
   while(  padding--)
Serial.print("0");
   Serial.print(frac,DEC) ;
 }
}

void printAddress(DeviceAddress addr) {
  byte i;
  for( i=0; i < 8; i++) {                         // prefix the printout with 0x
      Serial.print("0x");
      if (addr[i] < 16) {
        Serial.print('0');                        // add a leading '0' if required.
      }
      Serial.print(addr[i], HEX);                 // print the actual value in HEX
      if (i < 7) {
        Serial.print(", ");
      }
    }
}


void loop() {
   inritValue = digitalRead(inritPin);
   if(lastInritValue == HIGH && inritValue == HIGH){
   // nothing is there
     if((millis() - lastInritLeft)>300 && inritWatcher == LOW){
       inritWatcher = HIGH;
       Serial.println("inrit gone");
     }
   }
   else if(lastInritValue == LOW && inritValue == LOW){
   // it is still there
     if((millis() - lastInritEntry)>300 && inritWatcher == HIGH){
       // already up for 300ms, lets do something
       inritWatcher = LOW;
       Serial.println("inrit seen");
       }
   }
   else if(lastInritValue == HIGH && inritValue == LOW){
   // it has appeared
     lastInritEntry = millis();
     lastInritLeft = millis();
   }
   else if(lastInritValue == LOW && inritValue == HIGH){
   // it has disappeared
     lastInritEntry = millis();
     lastInritLeft = millis();
   }
   lastInritValue = inritValue;
//////////////////////////////////////////////////////////////////////////////////////
   tuinValue = digitalRead(tuinPin);
   if(lastTuinValue == HIGH && tuinValue == HIGH){
   // nothing is there
     if((millis() - lastTuinLeft)>300 && tuinWatcher == LOW){
       tuinWatcher = HIGH;
       Serial.println("tuin gone");
     }
   }
   else if(lastTuinValue == LOW && tuinValue == LOW){
   // it is still there
     if((millis() - lastTuinEntry)>300 && tuinWatcher == HIGH){
       // already up for 300ms, lets do something
       tuinWatcher = LOW;
       Serial.println("tuin seen");
       }
   }
   else if(lastTuinValue == HIGH && tuinValue == LOW){
   // it has appeared
     lastTuinEntry = millis();
     lastTuinLeft = millis();
   }
   else if(lastTuinValue == LOW && tuinValue == HIGH){
   // it has disappeared
     lastTuinEntry = millis();
     lastTuinLeft = millis();
   }
   lastTuinValue = tuinValue;
//////////////////////////////////////////////////////////////////////
    // lets put relays down if they weren't active recently
    if(millis() - lastRelay0active > 60000){
        if(relay0value == HIGH){
            Serial.println("set relay 0 down");
            digitalWrite(relay0Pin, LOW);
            relay0value = LOW;
        }
    }
    if(millis() - lastRelay1active > 60000){
        if(relay1value == HIGH){
            Serial.println("set relay 1 down");
            digitalWrite(relay1Pin, LOW);
            relay1value = LOW;
        }    
    }
    if(millis() - lastRelay2active > 4000){
        if(relay2value == HIGH){
            Serial.println("set relay 2 down");
            digitalWrite(relay2Pin, LOW);
            relay2value = LOW;
        }  
     }  
//////////////////////////////////////////////////////////////////////    
/*
    int A0Value = analogRead(A0);
    myA0Median.addValue(A0Value);
    int m = myA0Median.getMedian();
    
   if(m+15 > A0Value && lastA0Value == HIGH){
    //still high
   }
   else if(lastA0Value == LOW && m-15 < A0Value){
   // still low
   }
   else if(lastA0Value == LOW && m+15 > A0Value){
   // it has appeared
     lastA0Value = HIGH;
   }
   else if(lastA0Value == HIGH && m-15 < A0Value){
   // it has disappeared
     lastA0Value = LOW;
     A0counter++;
   }
   if (A0counter >= 1){
     Serial.print("A0 hit ");
     Serial.println(A0Value);
     A0counter = 0;
   }
*/
 int A0Value = digitalRead(A0); // Read the state of the switch
  if (A0Value == LOW && lastA0Value == HIGH) // If the pin reads low, the switch is closed.
  {
    lastA0Value = LOW;
    Serial.println("A0 hit low");
  }
  else if (A0Value == HIGH && lastA0Value == LOW)
  {
    lastA0Value = HIGH;
    //Serial.println("A0 hit high");
  }
  else
  {
    // nothing happened
  }       
    
//////////////////////////////////////////////////////////////////////
    // lets read our possible commands
    if(Serial.available() > 0){
        incomingSerialChar = (char) Serial.read();
        if (incomingSerialChar == '4'){
            Serial.println("activated relay 0");
            digitalWrite(relay0Pin,HIGH);
            lastRelay0active = millis();
            relay0value = HIGH;
        }
        if (incomingSerialChar == '3'){
            Serial.println("activated relay 1");
            digitalWrite(relay1Pin,HIGH);
            lastRelay1active = millis();
            relay1value = HIGH;            
        }
        if (incomingSerialChar == '6'){
            Serial.println("activated relay 2");
            digitalWrite(relay2Pin,HIGH);
            lastRelay2active = millis();
            relay2value = HIGH;
        }
        if (incomingSerialChar == '2'){            
            Serial.println("will check temp");
            sensors.requestTemperatures();
            for (int i=0;i<sensors.getDeviceCount();i++){
                float tempC = sensors.getTempCByIndex(i);
                DeviceAddress sensor;
                sensors.getAddress(sensor,i);
                printAddress(sensor);
                Serial.print(":");
                printDouble(tempC,1);
                Serial.print("\r\n");
            }
            Serial.println("done temps");
            
        }
        if (incomingSerialChar == '5'){
            Serial.println("will check dht");
            // Reading temperature or humidity takes about 250 milliseconds!
            // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
            float h = dht.readHumidity();
            // Read temperature as Celsius (the default)
            float t = dht.readTemperature();
            // Check if any reads failed and exit early (to try again).
            if (isnan(h) || isnan(t)) {
              Serial.println("Failed to read from DHT sensor!");
            }
            else {
            /*
            int chk = DHT.read22(DHT22_PIN);
            switch (chk)
            {
            case DHTLIB_OK:  
                Serial.print("OK:"); 
                break;
            case DHTLIB_ERROR_CHECKSUM: 
                Serial.print("Checksum error,\t"); 
                break;
            case DHTLIB_ERROR_TIMEOUT: 
                Serial.print("Time out error,\t"); 
                break;
            default: 
                Serial.print("Unknown error,\t"); 
                break;
            }
            */
            
              // DISPLAY DATA
              Serial.print(h, 1);
              Serial.print(":");
              Serial.println(t, 1);
            }
        }
    }

}
