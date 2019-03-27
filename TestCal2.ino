#include "esp_adc_cal.h"
#include "esp_err.h"
#include <WiFi.h>

//---------------Conn--------------------

WiFiServer server(80);

const char *ssid = "MyESP32AP";
const char *password = "testpassword";

// Variable to store the HTTP request
String header;

//---------------------------------------

//Sense----------------------------------------

int answer;
esp_adc_cal_characteristics_t ESPTONO;
//esp_adc_cal_characteristics_t *ESPTONOdir;
//ESPTONOdir = &ESPTONO;
//#define ANALOG_PIN_0 34
unsigned int corriente_6;
unsigned int virtualGND_7;
unsigned int tension_4;
//unsigned int promval;

const int numReadings=1000;

int readIndex = 0;              // the index of the current reading

int readings[numReadings];      // the readings from the analog input
int total = 0;                  // the running total
int average = 0;                // the average

int readings1[numReadings];      // the readings from the analog input
int total1 = 0;                  // the running total
int average1 = 0;                // the average

int readings2[numReadings];      // the readings from the analog input
int total2 = 0;                  // the running total
int average2 = 0;

String corriente,GND,tension1,tension10,tension100 ;

//------------------------------------------------------------

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  adc_power_on();
  adc_gpio_init(ADC_UNIT_1,ADC_CHANNEL_6);
  adc_gpio_init(ADC_UNIT_1,ADC_CHANNEL_7);
  adc_gpio_init(ADC_UNIT_1,ADC_CHANNEL_4);
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc_set_data_inv(ADC_UNIT_1,1);  
  adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC1_CHANNEL_7,ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_DB_11);
  answer=esp_adc_cal_characterize(ADC_UNIT_1,ADC_ATTEN_DB_11,
                        ADC_WIDTH_BIT_12, 1100, &ESPTONO);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
    readings1[thisReading] = 0;
    readings2[thisReading] = 0;
  }

  //------------WiFi Config-------------------------

  Serial.print("Connecting to ");
  //Serial.println(ssid);
  WiFi.softAP(ssid, password);
  //while (WiFi.status() != WL_CONNECTED) {
  //  delay(500);
  //  Serial.print(".");
  //}
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi AP Created");
  Serial.println("IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();
  //----------------------------------------------
}

void loop() {
  // put your main code here, to run repeatedly:
 
//Serial.printf(("Answer : %i\n"),answer);//print Low 4bytes.
//Serial.printf(("Vref : %d\n"),ESPTONO.vref);
esp_adc_cal_get_voltage(ADC_CHANNEL_6,&ESPTONO,&corriente_6);
esp_adc_cal_get_voltage(ADC_CHANNEL_7,&ESPTONO,&virtualGND_7);
esp_adc_cal_get_voltage(ADC_CHANNEL_4,&ESPTONO,&tension_4);
//promval=(analog_value1+analog_value2)/2;
//analog_value2=analog_value1;
//Serial.printf (("Valor detectado: %d mV \n"),promval);

total = total - readings[readIndex];
total1 = total1 - readings1[readIndex];
total2 = total2 - readings2[readIndex];
  // read from the sensor:
readings[readIndex] = corriente_6;
readings1[readIndex] = virtualGND_7;
readings2[readIndex] = tension_4;
  // add the reading to the total:
total = total + readings[readIndex];
total1 = total1 + readings1[readIndex];
total2 = total2 + readings2[readIndex];
  // advance to the next position in the array:
readIndex = readIndex + 1;

  // if we're at the end of the array...
if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
}

  // calculate the average:
average = total / numReadings;
average1 = total1 / numReadings;
average2 = total2 / numReadings;
  // send it to the computer as ASCII digits
  //print analog_value2;analog_value1;average
corriente=String(average-average1);
GND=String(average1);
tension1=String((average2-average1)/1.5);
tension10=String((average2-average1)/8);
tension100=String((average2-average1)/80);
//total=String(analog_value2+";"+analog_value1+";"+average+";")
//Serial.println(average);
//Serial.println(average1);

//----------- Conn -----------------------------------
WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");


            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            client.println("<p>t1" + tension1 + "</p>");
            client.println("<p>t2" + tension10 + "</p>");
            client.println("<p>t3" + tension100 + "</p>");
            client.println("<p>c" + corriente + "</p>");
                                 
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

  Serial.print("Valor Corriente: ");
  Serial.println(average);
  Serial.print("Valor GND: ");
  Serial.println(average1);
  Serial.print("Valor Tension: ");
  Serial.println(average2);
  //-----------------------------------------------------------------------

delay(1);

}
