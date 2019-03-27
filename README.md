# ADC-calibration-ESP32
This code contains an example of a Calibration for the ESP32 ADC using the recommended libraries provided by Espressif in the documentation.
https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/adc.html

The code uses a moving average implementation to smooth the values, also it was designed to transmit information through WiFi that's the reason
it uses the WiFi server and publishes an HTML code.
