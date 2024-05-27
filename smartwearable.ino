#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Bluetooth
SoftwareSerial BTSerial(2, 3); // RX, TX

// Temperature Sensor
#define ONE_WIRE_BUS A0
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float temp;

// OLED Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pulse Sensor
int pulsePin = A1;                 // Pulse Sensor purple wire connected to analog pin A1
int blinkPin = 13;                 // Pin to blink LED at each beat
int fadePin = 8;                   // Pin to do fancy fading blink at each beat
int fadeRate = 0;                  // Used to fade LED on with PWM on fadePin

// Volatile Variables, used in the interrupt service routine
volatile int BPM;                  // Holds raw Analog in A1, updated every 2mS
volatile int Signal;               // Holds the incoming raw data
volatile int IBI = 600;            // Holds the time interval between beats, seeded
volatile boolean Pulse = false;    // "True" when User's live heartbeat is detected
volatile boolean QS = false;       // Becomes true when Arduino finds a beat

// Serial Output
static boolean serialVisual = true; // Set to 'false' by default. Reset to 'true' to see Arduino Serial Monitor ASCII Visual Pulse

volatile int rate[10];                      // Array to hold last ten IBI values
volatile unsigned long sampleCounter = 0;   // Used to determine pulse timing
volatile unsigned long lastBeatTime = 0;    // Used to find IBI
volatile int P = 512;                       // Used to find peak in pulse wave, seeded
volatile int T = 512;                       // Used to find trough in pulse wave, seeded
volatile int thresh = 525;                  // Used to find instant moment of heart beat, seeded
volatile int amp = 100;                     // Used to hold amplitude of pulse waveform, seeded
volatile boolean firstBeat = true;          // Used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = false;        // Used to seed rate array so we startup with reasonable BPM

String data_to_send = "";

void setup() {
  BTSerial.begin(9600);
  pinMode(blinkPin, OUTPUT);      // Pin that will blink to your heartbeat
  pinMode(fadePin, OUTPUT);       // Pin that will fade to your heartbeat
  Serial.begin(115200);           // Initialize Serial communication
  interruptSetup();               // Sets up to read Pulse Sensor signal every 2mS

  sensors.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) {} // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(" SMART WEARABLE ");
  display.display();
  delay(2000); // Pause for 2 seconds
}

void loop() {
  display.clearDisplay();
  delay(200);

  display.setCursor(0, 0);
  display.print(" SMART WEARABLE ");
  display.display();
  delay(200);

  data_to_send = "";

  // DS18B20 Temperature Sensor
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);

  display.setCursor(0, 20);
  display.print("Temp: ");
  display.print(temp);
  display.print(" degC:");
  display.display();

  // Pulse Sensor
  serialOutput();

  if (QS == true) { // A Heartbeat Was Found
    fadeRate = 255; // Makes the LED Fade Effect Happen
    serialOutputWhenBeatHappens(); // A Beat Happened, Output that to serial
    QS = false; // Reset the Quantified Self flag for next time
  } else {
    BPM = 0;
  }

  ledFadeToBeat(); // Makes the LED Fade Effect Happen
  delay(20); // Take a break

  data_to_send += String(BPM);
  data_to_send += ",";
  data_to_send += String(temp);
  data_to_send += "#";
  Serial.println(data_to_send);

  BTSerial.println(data_to_send);

  delay(5000);
}

void ledFadeToBeat() {
  fadeRate -= 15;                         // Set LED fade value
  fadeRate = constrain(fadeRate, 0, 255); // Keep LED fade value from going into negative numbers
  analogWrite(fadePin, fadeRate);         // Fade LED
}

void interruptSetup() {
  // Initializes Timer2 to throw an interrupt every 2mS
  TCCR2A = 0x02;    // DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
  TCCR2B = 0x06;    // DON'T FORCE COMPARE, 256 PRESCALER
  OCR2A = 0X7C;     // SET THE TOP OF THE COUNT TO 124 FOR 500Hz SAMPLE RATE
  TIMSK2 = 0x02;    // ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
  sei();            // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED
}

void serialOutput() {
  // Decide How To Output Serial
  if (serialVisual == true) {
    arduinoSerialMonitorVisual('-', Signal); // Goes to function that makes Serial Monitor Visualizer
  } else {
    sendDataToSerial('S', Signal); // Goes to sendDataToSerial function
  }
}

void serialOutputWhenBeatHappens() {
  if (serialVisual == true) { // Code to Make the Serial Monitor Visualizer Work
    display.setCursor(0, 10);
    display.print("Pulse: ");
    display.print(BPM);
    display.display();
  } else {
    sendDataToSerial('B', BPM); // Send heart rate with a 'B' prefix
    sendDataToSerial('Q', IBI); // Send time between beats with a 'Q' prefix
  }
}

void arduinoSerialMonitorVisual(char symbol, int data) {
  const int sensorMin = 0;      // Sensor minimum, discovered through experiment
  const int sensorMax = 1024;   // Sensor maximum, discovered through experiment
  int sensorReading = data;     // Map the sensor range to a range of 12 options
  int range = map(sensorReading, sensorMin, sensorMax, 0, 11);

  // Do something different depending on the range value
  switch (range) {
    case 0: break;
    case 1: break;
    case 2: break;
    case 3: break;
    case 4: break;
    case 5: break;
    case 6: break;
    case 7: break;
    case 8: break;
    case 9: break;
    case 10: break;
    case 11: break;
  }
}

void sendDataToSerial(char symbol, int data) {
  Serial.print(symbol);
  Serial.println(data);
}

ISR(TIMER2_COMPA_vect) { // Triggered when Timer2 counts to 124
  cli(); // Disable interrupts while we do this
  Signal = analogRead(pulsePin); // Read the Pulse Sensor
  sampleCounter += 2; // Keep track of the time in mS with this variable
  int N = sampleCounter - lastBeatTime; // Monitor the time since the last beat to avoid noise

  // Find the peak and trough of the pulse wave
  if (Signal < thresh && N > (IBI / 5) * 3) { // Avoid dichrotic noise by waiting 3/5 of last IBI
    if (Signal < T) { // T is the trough
      T = Signal; // Keep track of lowest point in pulse wave
    }
  }

  if (Signal > thresh && Signal > P) { // Thresh condition helps avoid noise
    P = Signal; // P is the peak, keep track of highest point in pulse wave
  }

  // Now it's time to look for the heartbeat
  if (N > 250) { // Avoid high frequency noise
    if ((Signal > thresh) && (Pulse == false) && (N > (IBI / 5) * 3)) {
      Pulse = true; // Set the Pulse flag when we think there is a pulse
      digitalWrite(blinkPin, HIGH); // Turn on pin 13 LED
      IBI = sampleCounter - lastBeatTime; // Measure time between beats in mS
      lastBeatTime = sampleCounter; // Keep track of time for next pulse

      if (secondBeat) { // If this is the second beat
        secondBeat = false; // Clear secondBeat flag
        for (int i = 0; i <= 9; i++) { // Seed the running total to get a realistic BPM at startup
          rate[i] = IBI;
        }
      }

      if (firstBeat) { // If it's the first time we found a beat
        firstBeat = false; // Clear firstBeat flag
        secondBeat = true; // Set the second beat flag
        sei(); // Enable interrupts again
        return; // IBI value is unreliable so discard it
      }

      // Keep a running total of the last 10 IBI values
      word runningTotal = 0; // Clear the runningTotal variable

      for (int i = 0; i <= 8; i++) { // Shift data in the rate array
        rate[i] = rate[i + 1]; // Drop the oldest IBI value
        runningTotal += rate[i]; // Add up the 9 oldest IBI values
      }

      rate[9] = IBI; // Add the latest IBI to the rate array
      runningTotal += rate[9]; // Add the latest IBI to runningTotal
      runningTotal /= 10; // Average the last 10 IBI values
      BPM = 60000 / runningTotal; // Calculate BPM
      QS = true; // Set Quantified Self flag
      // QS flag is not cleared inside this ISR
    }
  }

  if (Signal < thresh && Pulse == true) { // When the values are going down, the beat is over
    digitalWrite(blinkPin, LOW); // Turn off pin 13 LED
    Pulse = false; // Reset the Pulse flag so we can do it again
    amp = P - T; // Get amplitude of the pulse wave
    thresh = amp / 2 + T; // Set thresh at 50% of the amplitude
    P = thresh; // Reset these for next time
    T = thresh;
  }

  if (N > 2500) { // If 2.5 seconds go by without a beat
    thresh = 512; // Set thresh default
    P = 512; // Set P default
    T = 512; // Set T default
    lastBeatTime = sampleCounter; // Bring the lastBeatTime up to date
    firstBeat = true; // Set these to avoid noise
    secondBeat = false; // When we get the heartbeat back
  }

  sei(); // Enable interrupts when you're done
}
