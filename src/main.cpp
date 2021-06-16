#include <avr/pgmspace.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <SparkFun_ADXL345.h>
//#include <MemoryFree.h>; // use this lib and freeMemory() to check for free memory

// comment out DEBUG_PRINTS to remove print statements from final build
//#define DEBUG_PRINTS

const int NODEPIN = 7;  // pin 7 is nodes line
const int NODELEDS = 168; //168; //total node pixels 
const int CONNPIN = 13;  // pin 13 is connections line
const int CONNLEDS = 188; //total connection pixels
const int ORPHANPIN = 5;  // pin 5 is orphan line
const int ORPHANLEDS = 78; //total orphan pixels
const int STATUSPIN = MISO;  // pin 14/miso is STATUS line output
const int STATUSLEDS = 8; // 3 OPTO sensors and one ADXL status indicatior, adjust for full size table

const byte fadeTable[]PROGMEM = {200, 100, 50, 15, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // need at least as many bytes as longest connection or orphans

int rampFillOne = 255; //conn fadeup controller
int rampFillTwo = 255;
int rampFillThree = 255;
int rampFillFour = 255;
int rampFillFive = 255;
int rampFillSix = 255;
int rampFillSeven = 255;
int rampFillEight = 255;
int rampFillNine = 255;
int rampFillTen = 255;
int rampFillEleven = 255;
int rampFillTwelve = 255;

int rampFillOneWhite = 255;
int rampFillTwoWhite = 255;
int rampFillThreeWhite = 255;
int rampFillFourWhite = 255;
int rampFillFiveWhite = 255;
int rampFillSixWhite = 255;
int rampFillSevenWhite = 255;
int rampFillEightWhite = 255;
int rampFillNineWhite = 255;
int rampFillTenWhite = 255;
int rampFillElevenWhite = 255;
int rampFillTwelveWhite = 255;

//----------------------------FUNCTION PROTOTYPES FOR COMPILER----------------------------------
void setAllNodes(byte red, byte green, byte blue, byte white);
void setAllConns(byte red, byte green, byte blue, byte white);
void connTwinkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay);
void orphanTwinkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay);
void setAllOrphans(byte red, byte green, byte blue, byte white);
void winAnimation();
void pickANode(bool node1, bool node2, bool node3, bool node4, bool node5, bool node6, bool node7);  //add more nodes and dupe full function
void pickAConn(bool conn1, bool conn2, bool conn3, bool conn4, bool conn5, bool conn6, bool conn7);  //control for connection lighting
void pickAOrphan(bool node1, bool node2, bool node3, bool node4, bool node5, bool node6, bool node7); //orphan control when node is lit
void lowPulseNodes(); //Low node fade control
void highPulseNodes(); //active connection with two nodes fade control
void setStatus(bool opto1, bool opto2, bool opto3, bool opto4, bool opto5, bool opto6, bool opto7, bool adxl1); //Status output indicator
void resetNodeTimers();

//-----------------------------TILT SENSOR SETUP-------------------------------------------------
// The sensor is ADXL343, but it works with the ADXL345 library
ADXL345 adxl = ADXL345();            // USE FOR I2C COMMUNICATION
//const float ADXLTriggerValue = 265; //ADXL z axis trigger value.
int changeInAccelZ = 0;
int currentAccelZ = 0;
int accelZTriggerValue = 14; // was 20, 7 was having false triggers and staying active indefinitely.
int previousAccelZ = 0;
// FIX IDEA: use the proximity sensors to trigger a start

//---------------------------AUDIO OUT PINS FOR TRIGGERS--------------------------------------
const int Audio_1 = 8; // Attractor Song - hold level high to loop
const int Audio_2 = 12; // Start sound on 10 - edge triggered 
const int Audio_3 = 11; // Connection / Node sound on 9 - edge triggered
const int Audio_4 = 10; // Win sound on 8 - edge triggered


//--------------------------ANALOG SETUP FOR OPTOs---------------------------------------

const int node1Sensor = A6; //pin D4 is also A6
const int node2Sensor = A5;
const int node3Sensor = A3;
const int node4Sensor = A2;
const int node5Sensor = A1;
const int node6Sensor = A0;
const int node7Sensor = A4;

const int optoTriggerValue = 350;  //OPTO input level trigger values - adjust as needed
const int opto1TriggerValue = 350; //Individual OPTO values
const int opto2TriggerValue = 350; 
const int opto3TriggerValue = 350;
const int opto4TriggerValue = 350; 
const int opto5TriggerValue = 350; 
const int opto6TriggerValue = 350;
const int opto7TriggerValue = 350; 



int node1SensorValue = 0;
int node2SensorValue = 0;
int node3SensorValue = 0;
int node4SensorValue = 0;
int node5SensorValue = 0;
int node6SensorValue = 0;
int node7SensorValue = 0;

int node1OutputValue = 0;
int node2OutputValue = 0;
int node3OutputValue = 0;
int node4OutputValue = 0;
int node5OutputValue = 0;
int node6OutputValue = 0;
int node7OutputValue = 0;

//------------------------------------------SET UP NEO STRAND SEGMENTS PER GROUP---------------------------------------
const int node1LedCount = 24; // adjust if you use different sized neo rings
const int node2LedCount = 24;
const int node3LedCount = 24;
const int node4LedCount = 24;
const int node5LedCount = 24;
const int node6LedCount = 24;
const int node7LedCount = 24;

bool node1on = LOW; //use node states for game logic
bool node2on = LOW;
bool node3on = LOW;
bool node4on = LOW;
bool node5on = LOW;
bool node6on = LOW;
bool node7on = LOW;

const int conn1LedCount = 17; // adjust if you use different counts per connection groups
const int conn2LedCount = 8;
const int conn3LedCount = 23;
const int conn4LedCount = 23;
const int conn5LedCount = 8;
const int conn6LedCount = 17;
const int conn7LedCount = 17;
const int conn8LedCount = 10;
const int conn9LedCount = 16;
const int conn10LedCount = 21;
const int conn11LedCount = 16;
const int conn12LedCount = 10;


const int orphan1LedCount = 15;  // USE Longest orphan per node since they are in parallel groups
const int orphan2LedCount = 12;
const int orphan3LedCount = 10;
const int orphan4LedCount = 19;
const int orphan5LedCount = 10;
const int orphan6LedCount = 12;


Adafruit_NeoPixel node = Adafruit_NeoPixel(NODELEDS, NODEPIN, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel conn = Adafruit_NeoPixel(CONNLEDS, CONNPIN, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel orphans = Adafruit_NeoPixel(ORPHANLEDS, ORPHANPIN, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel sensorStatus = Adafruit_NeoPixel(STATUSLEDS, STATUSPIN, NEO_GRB + NEO_KHZ800);

//----------------------STATE MACHINE SETUP--------------------------------
enum State_enum {IDLEMODE, STARTMODE, GAMEMODE, WINMODE};
uint8_t state = WINMODE;

enum Fade_enum {FADEUP, FADEDOWN};
uint8_t fadeState = FADEUP;

enum HighFade_enum {FADEUPNODE, FADEDOWNNODE};
uint8_t highfadeState = FADEDOWNNODE;

unsigned long previousMillis = 0; // restart machine every 49 days or this might overflow!!!!!!!
const long interval = 45000; // Milliseconds before auto reset if no children are 'using' machine

const long nodeInterval = 10000; // 10 sec times for node repeat
unsigned long node1Millis = 1; // 0.001 second for start of game
unsigned long node2Millis = 1;
unsigned long node3Millis = 1;
unsigned long node4Millis = 1;
unsigned long node5Millis = 1;
unsigned long node6Millis = 1;
unsigned long node7Millis = 1;


int lowFade = 0;
int highFadeNode = 250;

bool s1, s2, s3, s4, s5, s6, s7, sADXL; //temps for sensor states
int previousS1, previousS2, previousS3, previousS4, previousS5, previousS6, previousS7;
int currentS1, currentS2, currentS3, currentS4, currentS5, currentS6, currentS7;
//float previousADXL;
//float currentADXL;

//-------------------------SETUP------------------------------------------------
void setup() {

  Serial.begin(115200); // UNCOMMENT for testing OPTO732s if needed
  /* Initialise the sensor */
  //while (!Serial) ;

  adxl.powerOn();
  adxl.setRangeSetting(2);  // Set it for 2G. 2G 4G 8G and 16G available
  Serial.print(F("ADXL started"));
  pinMode(Audio_1, OUTPUT);
  pinMode(Audio_2, OUTPUT);
  pinMode(Audio_3, OUTPUT);
  pinMode(Audio_4, OUTPUT); //not really used due to same SFX for node/conn

  node.begin(); // Initialize all pixels to 'off' states
  node.show();
  conn.begin();
  conn.show();
  orphans.begin();
  orphans.show();
  sensorStatus.begin();
  sensorStatus.show();
  delay(2000);

  //Serial.println(F("Free RAM = ")); //F function does the same and is now a built in library, in IDE > 1.0.0
  //Serial.println(freeMemory(), DEC);  // print how much RAM is available.
}

//------------------------------MAIN LOOP---------------------------------------
void loop() {
  Serial.print(F("looping"));

  unsigned long currentMillis = millis();//for 30 second reset timer function and node sound repeating

  int accelX, accelY, accelZ;
  // Read the accelerometer values and store them in variables declared above x,y,z
  adxl.readAccel(&accelX, &accelY, &accelZ);
  
  changeInAccelZ = abs(accelZ - previousAccelZ);
  currentAccelZ = accelZ;
  previousAccelZ = accelZ; // wont use previousAccelZ the rest of this loop
  
  

  // read OPTO analog values
  node1SensorValue = analogRead(node1Sensor); //Add additional sensors as needed
  node2SensorValue = analogRead(node2Sensor);
  node3SensorValue = analogRead(node3Sensor);
  node4SensorValue = analogRead(node4Sensor);
  node5SensorValue = analogRead(node5Sensor);
  node6SensorValue = analogRead(node6Sensor);
  node7SensorValue = analogRead(node7Sensor);

  node1OutputValue = node1SensorValue;
  node2OutputValue = node2SensorValue;
  node3OutputValue = node3SensorValue;
  node4OutputValue = node4SensorValue;
  node5OutputValue = node5SensorValue;
  node6OutputValue = node6SensorValue;
  node7OutputValue = node7SensorValue;


  //----------------------------------------------------SENSOR STATUS TESTING-----------------------------------
  currentS1 = node1SensorValue; //copy sensor data for input testing
  currentS2 = node2SensorValue;
  currentS3 = node3SensorValue;
  currentS4 = node4SensorValue;
  currentS5 = node5SensorValue;
  currentS6 = node6SensorValue;
  currentS7 = node7SensorValue;
  
  
  //Serial.println("after accelerometer read");
  if (abs(currentS1 - previousS1) > 70 || currentS1 < 45) s1 = 0; // test for OPEN noise or pull to GND
  else s1 = 1;

  if (abs(currentS2 - previousS2) > 70 || currentS2 < 45) s2 = 0;
  else s2 = 1;

  if (abs(currentS3 - previousS3) > 70 || currentS3 < 45) s3 = 0;
  else s3 = 1;

  if (abs(currentS4 - previousS4) > 70 || currentS4 < 45) s4 = 0;
  else s4 = 1;

  if (abs(currentS5 - previousS5) > 70 || currentS5 < 45) s5 = 0;
  else s5 = 1;

  if (abs(currentS6 - previousS6) > 70 || currentS6 < 45) s6 = 0;
  else s6 = 1;

  if (abs(currentS7 - previousS7) > 70 || currentS7 < 45) s7 = 0;
  else s7 = 1;
  
  if (changeInAccelZ > accelZTriggerValue) sADXL = 0; // Test for out of range AXIS Sensor
  else sADXL = 1;
  setStatus(s1, s2, s3, s4, s5, s6, s7, sADXL); // Update Sensor STATUS LEDs

  previousS1 = currentS1;  //store last sensor values for next test
  previousS2 = currentS2;
  previousS3 = currentS3;
  previousS4 = currentS4;
  previousS5 = currentS5;
  previousS6 = currentS6;
  previousS7 = currentS7;
  //previousADXL = currentADXL;

  

  //-------------------------------------------------END SENSOR STATUS TESTING-----------------------------------


  //--------------------------TESTING sensor event values, add new units enable if needed-----------------------------------------

  //Serial.print(F("X: ")); Serial.print(accelX); Serial.print(F("  "));
  //Serial.print(F("Y: ")); Serial.print(accelY); Serial.print(F("  "));
  //Serial.print(F("Z: ")); Serial.print(accelZ); Serial.print(F("  ")); Serial.println(F(""));

  #ifdef DEBUG_PRINTS
    Serial.print(F("accel X Y Z - changeInZ :: "));
    Serial.print(accelX);
    Serial.print(" . ");
    Serial.print(accelY);
    Serial.print(" . ");
    Serial.print(accelZ);
    Serial.print(" . ");
    Serial.print(changeInAccelZ);
    Serial.println(F(" . "));
    Serial.print(F("OPTO1: ")); Serial.print(node1OutputValue); Serial.print(F("  "));
    Serial.print(F("OPTO2: ")); Serial.print(node2OutputValue); Serial.print(F("  "));
    Serial.print(F("OPTO3: ")); Serial.print(node3OutputValue); Serial.print(F("  "));
    Serial.print(F("OPTO4: ")); Serial.print(node4OutputValue); Serial.print(F("  "));
    Serial.print(F("OPTO5: ")); Serial.print(node5OutputValue); Serial.print(F("  "));
    Serial.print(F("OPTO6: ")); Serial.print(node6OutputValue); Serial.print(F("  "));
    Serial.print(F("OPTO7: ")); Serial.print(node7OutputValue); Serial.print(F("  ")); 
    Serial.println(F(""));
  #endif

  
  //---------------------------------STATE MACHINE CONTROL LOOP-------------------------------------------
  //Serial.println(F("About to enter state switch "));
  switch (state)
  {

    case IDLEMODE:
      Serial.println(F("IdleMode"));

      if (changeInAccelZ > accelZTriggerValue) {
        digitalWrite(Audio_1, LOW); //Shut off Idle song  loop

        state = STARTMODE;
      } else {
        connTwinkle(0x00, 0x00, 0x60, 40, random(20, 100)); // twinkle connections and orphans
        lowPulseNodes(); setAllNodes(0, 0, lowFade, 0); node.show();
        orphanTwinkle(0x00, 0x00, 0x60, 40, random(20, 100)); // twinkle orphans
        lowPulseNodes(); setAllNodes(0, 0, lowFade, 0); node.show();    //run it twice due to random delays in twinkle patterns

        digitalWrite(Audio_1, HIGH);   //play idle sound (hold state)
      }
      break;

    case STARTMODE:
      Serial.println(F("StartMode"));
      digitalWrite(Audio_2, HIGH); //Trigger start sound
      delay(10);
      digitalWrite(Audio_2, LOW);
      previousMillis = currentMillis;// start 30 sec counter for bored kids
      setAllConns(0, 0, 0, 0); //reset all LEDs to off
      lowPulseNodes(); setAllNodes(0, 0, lowFade, 0); node.show();
      setAllOrphans(0, 0, 0, 0);
      state = GAMEMODE;
      highFadeNode = 250; // reset
      rampFillOne = 255; // Connection ramp controller reset
      rampFillTwo = 255; // Connection ramp controller reset
      rampFillThree = 255;
      rampFillFour = 255;
      rampFillFive = 255;
      rampFillSix = 255;
      rampFillSeven = 255;
      rampFillEight = 255;
      rampFillNine = 255;
      rampFillTen = 255;
      rampFillEleven = 255;
      rampFillTwelve = 255;

      rampFillOneWhite = 255;
      rampFillTwoWhite = 255;
      rampFillThreeWhite = 255;
      rampFillFourWhite = 255;
      rampFillFiveWhite = 255;
      rampFillSixWhite = 255;
      rampFillSevenWhite = 255;
      rampFillEightWhite = 255;
      rampFillNineWhite = 255;
      rampFillTenWhite = 255;
      rampFillElevenWhite = 255;
      rampFillTwelveWhite = 255;


      break;

    case GAMEMODE:
      Serial.println(F("GAMEMODE"));

      if (changeInAccelZ > accelZTriggerValue) previousMillis = currentMillis;// reset inactivity timeout if movement
      if (node1OutputValue > opto1TriggerValue & currentMillis - node1Millis >= nodeInterval) { //Node 1 interaction test
        digitalWrite(Audio_3, HIGH);
        delay(10);
        digitalWrite(Audio_3, LOW);
        node1on = 1;
        node1Millis = currentMillis;
        previousMillis = currentMillis; // reset inactivity timeout
      }
      if (node2OutputValue > opto2TriggerValue & currentMillis - node2Millis >= nodeInterval) { //Node 2 interaction test
        digitalWrite(Audio_3, HIGH);
        delay(10);
        digitalWrite(Audio_3, LOW);
        node2on = 1;
        node2Millis = currentMillis;
        previousMillis = currentMillis; // reset inactivity timeout
      }
      if (node3OutputValue > opto3TriggerValue & currentMillis - node3Millis >= nodeInterval) { //Node 3 interaction test
        digitalWrite(Audio_3, HIGH);
        delay(10);
        digitalWrite(Audio_3, LOW);
        node3on = 1;
        node3Millis = currentMillis;
        previousMillis = currentMillis; // reset inactivity timeout
      }
      if (node4OutputValue > opto4TriggerValue & currentMillis - node4Millis >= nodeInterval) { //Node 4 interaction test
        digitalWrite(Audio_3, HIGH);
        delay(10);
        digitalWrite(Audio_3, LOW);
        node4on = 1;
        node4Millis = currentMillis;
        previousMillis = currentMillis; // reset inactivity timeout
      }
      if (node5OutputValue > opto5TriggerValue & currentMillis - node5Millis >= nodeInterval) { //Node 5 interaction test
        digitalWrite(Audio_3, HIGH);
        delay(10);
        digitalWrite(Audio_3, LOW);
        node5on = 1;
        node5Millis = currentMillis;
        previousMillis = currentMillis; // reset inactivity timeout
      }
      if (node6OutputValue > opto6TriggerValue & currentMillis - node6Millis >= nodeInterval) { //Node 6 interaction test
        digitalWrite(Audio_3, HIGH);
        delay(10);
        digitalWrite(Audio_3, LOW);
        node6on = 1;
        node6Millis = currentMillis;
        previousMillis = currentMillis; // reset inactivity timeout
      }
      if (node7OutputValue > opto7TriggerValue & currentMillis - node7Millis >= nodeInterval) { //Node 7 interaction test
        digitalWrite(Audio_3, HIGH);
        delay(10);
        digitalWrite(Audio_3, LOW);
        node7on = 1;
        node7Millis = currentMillis;
        previousMillis = currentMillis; // reset inactivity timeout
      }

//----------------------------------------------------------------------------------------------
//--------------------------------Serial Input for Testing -------------------------------------
//----------------------------------------------------------------------------------------------
// Uncomment to test node on states from Serial input
/*
      while(Serial.available() > 0) {

        int num = Serial.parseInt();
        switch(num) {
          case 1:
          node1on = 1;
          break;
          case 2:
          node2on = 1;
          break;
          case 3:
          node3on = 1;
          break;
          case 4:
          node4on = 1;
          break;
          case 5:
          node5on = 1;
          break;
          case 6:
          node6on = 1;
          break;
          case 7:
          node7on = 1;
          break;
          default:
          Serial.println(F("node num out of bounds"));
        }
      }
      Serial.print(F("Node1on: ")); Serial.print(node1on); Serial.print(F(" ,Node2on")); Serial.print(node2on); Serial.print(F(" ,Node3on: ")); Serial.print(node3on);
      Serial.print(F(" ,Node4on: ")); Serial.print(node4on); Serial.print(F(" ,Node5on: ")); Serial.print(node5on); Serial.print(F(" ,Node6on: ")); Serial.print(node6on);
      Serial.print(F(" ,Node7on: ")); Serial.print(node7on); Serial.println(F(""));
*/
      pickANode(node1on, node2on, node3on, node4on, node5on, node6on, node7on); // Light up nodes
      pickAConn(node1on, node2on, node3on, node4on, node5on, node6on, node7on); // Light up connections
      pickAOrphan(node1on, node2on, node3on, node4on, node5on, node6on, node7on); // Light up orphans

      if (currentMillis - previousMillis >= interval) {
        setAllConns(0, 0, 0, 0); setAllNodes(0, 0, 0, 0); setAllOrphans(0, 0, 0, 0); // reset playfield
        node1on = 0, node2on = 0, node3on = 0, node4on = 0, node5on = 0, node6on = 0, node7on = 0; // reset node state,  expand for full project
        state = IDLEMODE; //Back to Idle if no movement
      }
      if (node1on == 1 & node2on == 1 & node3on == 1 & node4on == 1 & node5on == 1 & node6on == 1 & node7on == 1) state = WINMODE;

      
      break;

    case WINMODE: 
      Serial.println(F("winmode!"));
      digitalWrite(Audio_4, HIGH);
      delay(10);
      digitalWrite(Audio_4, LOW);
      previousMillis = currentMillis; // reset for win animation
      resetNodeTimers();
      setStatus(1, 1, 1, 1, 1, 1, 1, 1); // green on all STATUS indicators, test the indicator LED test....
      //Serial.println(F("Free RAM  from winmode= ")); 
      //Serial.println(freeMemory(), DEC);  // print how much RAM is available.
      /*
      int loopCount = 0;
      do {
        pickANode(node1on, node2on, node3on, node4on, node5on, node6on, node7on); // Light up nodes while end animation
        pickAConn(node1on, node2on, node3on, node4on, node5on, node6on, node7on); // Light up connections
        pickAOrphan(node1on, node2on, node3on, node4on, node5on, node6on, node7on); // Light up orphans
        loopCount++;
      } while (loopCount <= 16); // do more sparkle before fade
      */
      setAllConns(0, 0, 0, 0); setAllNodes(0, 0, 0, 0); setAllOrphans(0, 0, 0, 0); // reset playfield
      node1on = 0, node2on = 0, node3on = 0, node4on = 0, node5on = 0, node6on = 0, node7on = 0; // reset node state
      
      winAnimation();
      setAllConns(0, 0, 0, 0); setAllNodes(0, 0, 0, 0); setAllOrphans(0, 0, 0, 0); // reset playfield
      delay(1000); // pause befor reset to idle
      state = IDLEMODE;
    
      break;
  }

}

//----------------------------------------------------------------------------------------------
//---------------------------------LIGHTING FUNCTIONS BELOW-------------------------------------
//----------------------------------------------------------------------------------------------


//---------------------------------CONNECTION-TWINKLE FUNCTION----------------------------------

void connTwinkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
  setAllConns(red, green, blue, 0);

  int Pixel = random(CONNLEDS);
  conn.setPixelColor(Pixel, conn.Color(0x00, 0x00, 0xff, 0xff)); //full blue and white for star effect
  conn.show();
  delay(SparkleDelay);
  conn.setPixelColor(Pixel, conn.Color(red, green, blue, 0x00)); //add other colors
  conn.show();
  delay(SpeedDelay);

  //Serial.println(F("Free RAM from conn twinkler = ")); 
  //Serial.println(freeMemory(), DEC);  // print how much RAM is available.
}

//---------------------------------ORPHAN-TWINKLE FUNCTION-------------------------------------

void orphanTwinkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
  setAllOrphans(red, green, blue, 0);
  int Pixel = random(ORPHANLEDS);
  orphans.setPixelColor(Pixel, orphans.Color(0x00, 0x00, 0xff, 0xff));
  orphans.show();
  delay(SparkleDelay);
  orphans.setPixelColor(Pixel, orphans.Color(red, green, blue, 0x00));
  orphans.show();
  delay(SpeedDelay);

}

//-------------------------------CONNECTION LEDS FILL FUNCTION----------------------------------

void setAllConns(byte red, byte green, byte blue, byte white) {
  for (int i = 0; i < CONNLEDS; i++ ) {
    conn.setPixelColor(i, red, green, blue, white);
  }
  conn.show();
}

//--------------------------------NODE LEDS FILL FUNCTION-------------------------------------

void setAllNodes(byte red, byte green, byte blue, byte white) {
  for (int i = 0; i < NODELEDS; i++ ) {
    node.setPixelColor(i, red, green, blue, white);
  }
  node.show();
}


//--------------------------------ORPHAN LEDS FILL FUNCTION-------------------------------------

void setAllOrphans(byte red, byte green, byte blue, byte white) {
  for (int i = 0; i < ORPHANLEDS; i++ ) {
    orphans.setPixelColor(i, red, green, blue, white);
  }
  orphans.show();
}

//-------------------------------NODES LOW FADE IDLE FUNCTION--------------------------------------

void lowPulseNodes() {
  switch (fadeState)
  {
    case FADEUP:
      if (lowFade > 20) fadeState = FADEDOWN;  // Fade from 0 to 20 only
      lowFade++;
      break;

    case FADEDOWN:
      if (lowFade <= 1) {
        fadeState = FADEUP;
        break;
      }
      lowFade--;
      break;
  }

}
//----------------------------------------------PAIRED NODES CONNECTION FADER----------------------------------------
void highPulseNodes() {
  switch (highfadeState)
  {
    case FADEUPNODE:
      if (highFadeNode > 200) highfadeState = FADEDOWNNODE; // fade from 200 to 3 at double inc
      highFadeNode += 2;
      break;

    case FADEDOWNNODE:
      if (highFadeNode <= 3) {
        highfadeState = FADEUPNODE;
        break;
      }
      highFadeNode -= 2;
      break;
  }

}
//-------------------------------NODES FADE IN AND OUT FOR WIN FUNCTION--------------------------------

void nodeFade() {
  for (int loop = 1; loop < 30; loop++) {
    connTwinkle(0x00, 0x00, 0x80, 60, 20); // twinkle connections and orphans
    //orphanTwinkle(0x00, 0x00, 0x80, 60, random(20,100)); // twinkle orphans
    for (int i = 200; i > 1; i -= 20) { //fade down
      for (int j = 0; j < NODELEDS; j++) {
        node.setPixelColor(j, 0, 0, i, i);
      }
      node.show();
    }
    connTwinkle(0x00, 0x00, 0x80, 60, 20); // twinkle connections and orphans
    //orphanTwinkle(0x00, 0x00, 0x80, 60, random(20,100)); // twinkle orphans
    for (int i = 1; i < 200; i += 20) {
      for (int j = 0; j < NODELEDS; j++) {
        node.setPixelColor(j, 0, 0, i, 0);
      }
      node.show();
    }
  }
}

void winAnimation() {
  for (int loop = 1; loop < 8; loop++) {
    
    if(random(2) == 1) {
      connTwinkle(0x00, 0x00, 0x80, 60, 20); // twinkle connections and orphans
    } else {
      orphanTwinkle(0x00, 0x00, 0x80, 60, 20); // twinkle orphans
    }
    for (int i = 200; i > 1; i -= 20) { //fade down
      for (int j = 0; j < NODELEDS; j++) {
        node.setPixelColor(j, 0, 0, i, i);
      }
      node.show();
    }
    
    if(random(2) == 1) {
      connTwinkle(0x00, 0x00, 0x80, 60, 20); // twinkle connections and orphans
    } else {
      orphanTwinkle(0x00, 0x00, 0x80, 60, 20); // twinkle orphans
    }
    for (int i = 1; i < 200; i += 20) {
      for (int j = 0; j < NODELEDS; j++) {
        node.setPixelColor(j, 0, 0, i, 0);
      }
      node.show();
    }
  }
}

//--------------------------------INDEPENDENT NODE LIGHTING FUNCTION---------------------------------------

void pickANode(bool node1, bool node2, bool node3, bool node4, bool node5, bool node6, bool node7) {

  for (int i = 0; i < node1LedCount; i++) {
    if (node1 == HIGH && node2 == HIGH || node1 == HIGH && node7 == HIGH || node1 == HIGH && node6 == HIGH) {
      node.setPixelColor(i, 0, 0, 200, rampFillOne); //for new Node white fadeout
    } else if (node1 == HIGH) {
      node.setPixelColor(i, 0, 0, 80, 0); // active node light value
    } else {
      node.setPixelColor(i, 0, 0, lowFade, 0); // pulsing dimly
    }
  }

  for (int i = node1LedCount; i < node1LedCount + node2LedCount; i++) {
    if (node2 == HIGH && node1 == HIGH || node2 == HIGH && node3 == HIGH || node2 == HIGH && node7 == HIGH) {
      node.setPixelColor(i, 0, 0, 200, rampFillTwo); //for new Node white fadeout
    } else if (node2 == HIGH) {
      node.setPixelColor(i, 0, 0, 80, 0);
    } else {
      node.setPixelColor(i, 0, 0, lowFade, 0);
    }
  }
  for (int i = node1LedCount + node2LedCount; i < node1LedCount + node2LedCount + node3LedCount; i++) {
    if (node3 == HIGH && node2 == HIGH || node3 == HIGH && node7 == HIGH || node3 == HIGH && node4 == HIGH) {
      node.setPixelColor(i, 0, 0, 200, rampFillThree); //for new Node white fadeout
    } else if (node3 == HIGH) {
      node.setPixelColor(i, 0, 0, 80, 0);
    } else {
      node.setPixelColor(i, 0, 0, lowFade, 0);
    }
  }

  for (int i = node1LedCount + node2LedCount + node3LedCount; i < node1LedCount + node2LedCount + node3LedCount + node4LedCount; i++) {
    if (node4 == HIGH && node3 == HIGH || node4 == HIGH && node7 == HIGH || node4 == HIGH && node5 == HIGH) {
      node.setPixelColor(i, 0, 0, 200, rampFillFour); //for new Node white fadeout
    } else if (node4 == HIGH) {
      node.setPixelColor(i, 0, 0, 80, 0);
    } else {
      node.setPixelColor(i, 0, 0, lowFade, 0);
    }
  }
  for (int i = node1LedCount + node2LedCount + node3LedCount + node4LedCount; i < node1LedCount + node2LedCount + node3LedCount + node4LedCount + node5LedCount; i++) {
    if (node5 == HIGH && node4 == HIGH || node5 == HIGH && node7 == HIGH || node5 == HIGH && node6 == HIGH) {
      node.setPixelColor(i, 0, 0, 200, rampFillFive); //for new Node white fadeout
    } else if (node5 == HIGH) {
      node.setPixelColor(i, 0, 0, 80, 0);
    } else {
      node.setPixelColor(i, 0, 0, lowFade, 0);
    }
  }
  for (int i = node1LedCount + node2LedCount + node3LedCount + node4LedCount + node5LedCount; i < node1LedCount + node2LedCount + node3LedCount + node4LedCount + node5LedCount + node6LedCount; i++) {
    if (node6 == HIGH && node5 == HIGH || node6 == HIGH && node7 == HIGH || node6 == HIGH && node1 == HIGH) {
      node.setPixelColor(i, 0, 0, 200, rampFillSix); //for new Node white fadeout
    } else if (node6 == HIGH) {
      node.setPixelColor(i, 0, 0, 80, 0);
    } else {
      node.setPixelColor(i, 0, 0, lowFade, 0);
    }
  }
  for (int i = node1LedCount + node2LedCount + node3LedCount + node4LedCount + node5LedCount + node6LedCount; i < node1LedCount + node2LedCount + node3LedCount + node4LedCount + node5LedCount + node6LedCount + node7LedCount; i++) {
    if (node7 == HIGH && node1 == HIGH || node7 == HIGH && node2 == HIGH || node7 == HIGH && node3 == HIGH || node7 == HIGH && node4 == HIGH || node7 == HIGH && node5 == HIGH || node7 == HIGH && node6 == HIGH ) {
      node.setPixelColor(i, 0, 0, 200, rampFillSeven); //for new Node white fadeout
    } else if (node7 == HIGH) {
      node.setPixelColor(i, 0, 0, 80, 0);
    } else {
      node.setPixelColor(i, 0, 0, lowFade, 0);
    }
  }

  if (node1 == HIGH && node2 == HIGH || node1 == HIGH && node7 == HIGH || node1 == HIGH && node6 == HIGH) rampFillOne -= 4; //for new Node white fadeout
  if (node2 == HIGH && node1 == HIGH || node2 == HIGH && node7 == HIGH || node2 == HIGH && node3 == HIGH) rampFillTwo -= 4;
  if (node3 == HIGH && node2 == HIGH || node3 == HIGH && node7 == HIGH || node3 == HIGH && node4 == HIGH) rampFillThree -= 4;
  if (node4 == HIGH && node3 == HIGH || node4 == HIGH && node7 == HIGH || node4 == HIGH && node5 == HIGH) rampFillFour -= 4;
  if (node5 == HIGH && node4 == HIGH || node5 == HIGH && node7 == HIGH || node5 == HIGH && node6 == HIGH) rampFillFive -= 4;
  if (node6 == HIGH && node5 == HIGH || node6 == HIGH && node7 == HIGH || node6 == HIGH && node1 == HIGH) rampFillSix -= 4;
  if ((node7 == HIGH && node1 == HIGH ) || (node7 == HIGH && node2 == HIGH ) || (node7 == HIGH && node3 == HIGH )|| (node7 == HIGH && node4 == HIGH  )|| (node7 == HIGH && node5 == HIGH ) || (node7 == HIGH && node6 == HIGH) ) rampFillSeven -= 4;
  if (rampFillOne <= 0) rampFillOne = 0;
  if (rampFillTwo <= 0) rampFillTwo = 0;
  if (rampFillThree <= 0) rampFillThree = 0;
  if (rampFillFour <= 0) rampFillFour = 0;
  if (rampFillFive <= 0) rampFillFive = 0;
  if (rampFillSix <= 0) rampFillSix = 0;
  if (rampFillSeven <= 0) rampFillSeven = 0;
  node.show();
  delay(20);
  lowPulseNodes(); //adjust fade counter
}
//---------------------------------------------------------------------------------------------------------------
//--------------------------------INDEPENDENT CONNECTION LIGHTING FUNCTION---------------------------------------
//-----------------------------------MOST EFFECTS AND LOGIC HERE-------------------------------------------------


void pickAConn(bool node1, bool node2, bool node3, bool node4, bool node5, bool node6, bool node7) {
  int fadeFill = 0; // reset each loop
  for (int i = 0; i < conn1LedCount; i++) {
    if (node1 == HIGH && node2 == HIGH) {
      conn.setPixelColor(i, 0, 0, highFadeNode, rampFillOneWhite); // fade down white
    } else if (node1 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade conn out from node 1
      fadeFill++;
    } else if (node2 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[conn1LedCount - fadeFill - 1]), 0); // fade conn out from node 2
      fadeFill++;
    } else {
      conn.setPixelColor(i, 0, 0, 0, 0); // no color if no active nodes attached
    }
  }
  fadeFill = 0;
  for (int i = conn1LedCount; i < conn1LedCount + conn2LedCount; i++) {
    if (node2 == HIGH && node3 == HIGH) {
      conn.setPixelColor(i, 0, 0, highFadeNode, rampFillTwoWhite);
    } else if (node2 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade conn out from node 2
      fadeFill++;
    } else if (node3 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[conn2LedCount - fadeFill - 1]), 0); // fade conn out from node 3
      fadeFill++;
    } else {
      conn.setPixelColor(i, 0, 0, 0, 0);
    }
  }
  fadeFill = 0;
  for (int i = conn1LedCount + conn2LedCount; i < conn1LedCount + conn2LedCount + conn3LedCount; i++) {
    if (node3 == HIGH && node4 == HIGH) {
      conn.setPixelColor(i, 0, 0, highFadeNode, rampFillThreeWhite); //fade out white and pulse blue
    } else if (node3 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade conn out from node 3
      fadeFill++;
    } else if (node4 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[conn3LedCount - fadeFill - 1]), 0); // fade conn out from node 4
      fadeFill++;
    } else {
      conn.setPixelColor(i, 0, 0, 0, 0);
    }
  }
  fadeFill = 0;
  for (int i = conn1LedCount + conn2LedCount + conn3LedCount; i < conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount; i++) {
    if (node4 == HIGH && node5 == HIGH) {
      conn.setPixelColor(i, 0, 0, highFadeNode, rampFillFourWhite); //fade out white and pulse blue
    } else if (node4 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade conn out from node 4
      fadeFill++;
    } else if (node5 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[conn4LedCount - fadeFill - 1]), 0); // fade conn out from node 5
      fadeFill++;
    } else {
      conn.setPixelColor(i, 0, 0, 0, 0);
    }
  }
  fadeFill = 0;
  for (int i = conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount; i < conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount + conn5LedCount; i++) {
    if (node5 == HIGH && node6 == HIGH) {
      conn.setPixelColor(i, 0, 0, highFadeNode, rampFillFiveWhite); //fade out white and pulse blue
    } else if (node5 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade conn out from node 5
      fadeFill++;
    } else if (node6 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[conn5LedCount - fadeFill - 1]), 0); // fade conn out from node 6
      fadeFill++;
    } else {
      conn.setPixelColor(i, 0, 0, 0, 0);
    }
  }
  fadeFill = 0;
  for (int i = conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount + conn5LedCount; i < conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount + conn5LedCount + conn6LedCount; i++) {
    if (node6 == HIGH && node1 == HIGH) {
      conn.setPixelColor(i, 0, 0, highFadeNode, rampFillSixWhite); //fade out white and pulse blue
    } else if (node6 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade conn out from node 6
      fadeFill++;
    } else if (node1 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[conn6LedCount - fadeFill - 1]), 0); // fade conn out from node 1
      fadeFill++;
    } else {
      conn.setPixelColor(i, 0, 0, 0, 0);
    }
  }
  fadeFill = 0;
  for (int i = conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount + conn5LedCount + conn6LedCount; i < conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount + conn5LedCount + conn6LedCount + conn7LedCount; i++) {
    if (node7 == HIGH && node1 == HIGH) {
      conn.setPixelColor(i, 0, 0, highFadeNode, rampFillSevenWhite); //fade out white and pulse blue
    } else if (node7 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade conn out from node 7
      fadeFill++;
    } else if (node1 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[conn7LedCount - fadeFill - 1]), 0); // fade conn out from node 1
      fadeFill++;
    } else {
      conn.setPixelColor(i, 0, 0, 0, 0);
    }
  }
  fadeFill = 0;
  for (int i = conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount + conn5LedCount + conn6LedCount + conn7LedCount; i < conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount + conn5LedCount + conn6LedCount  + conn7LedCount + conn8LedCount; i++) {
    if (node7 == HIGH && node2 == HIGH) {
      conn.setPixelColor(i, 0, 0, highFadeNode, rampFillEightWhite); //fade out white and pulse blue
    } else if (node7 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade conn out from node 7
      fadeFill++;
    } else if (node2 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[conn8LedCount - fadeFill - 1]), 0); // fade conn out from node 2
      fadeFill++;
    } else {
      conn.setPixelColor(i, 0, 0, 0, 0);
    }
  }
  fadeFill = 0;
  for (int i = conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount + conn5LedCount + conn6LedCount + conn7LedCount + conn8LedCount; i < conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount + conn5LedCount + conn6LedCount + conn7LedCount + conn8LedCount + conn9LedCount; i++) {
    if (node7 == HIGH && node3 == HIGH) {
      conn.setPixelColor(i, 0, 0, highFadeNode, rampFillNineWhite); //fade out white and pulse blue
    } else if (node7 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade conn out from node 7
      fadeFill++;
    } else if (node3 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[conn9LedCount - fadeFill - 1]), 0); // fade conn out from node 3
      fadeFill++;
    } else {
      conn.setPixelColor(i, 0, 0, 0, 0);
    }
  }
  fadeFill = 0;
  for (int i = conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount + conn5LedCount + conn6LedCount + conn7LedCount + conn8LedCount + conn9LedCount; i < conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount + conn5LedCount + conn6LedCount + conn7LedCount + conn8LedCount + conn9LedCount + conn10LedCount; i++) {
    if (node7 == HIGH && node4 == HIGH) {
      conn.setPixelColor(i, 0, 0, highFadeNode, rampFillTenWhite); //fade out white and pulse blue
    } else if (node7 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade conn out from node 7
      fadeFill++;
    } else if (node4 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[conn10LedCount - fadeFill - 1]), 0); // fade conn out from node 4
      fadeFill++;
    } else {
      conn.setPixelColor(i, 0, 0, 0, 0);
    }
  }
  fadeFill = 0;
  for (int i = conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount + conn5LedCount + conn6LedCount + conn7LedCount + conn8LedCount + conn9LedCount + conn10LedCount; i < conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount + conn5LedCount + conn6LedCount + conn7LedCount + conn8LedCount + conn9LedCount + conn10LedCount + conn11LedCount; i++) {
    if (node7 == HIGH && node5 == HIGH) {
      conn.setPixelColor(i, 0, 0, highFadeNode, rampFillElevenWhite); //fade out white and pulse blue
    } else if (node7 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade conn out from node 7
      fadeFill++;
    } else if (node5 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[conn11LedCount - fadeFill - 1]), 0); // fade conn out from node 5
      fadeFill++;
    } else {
      conn.setPixelColor(i, 0, 0, 0, 0);
    }
  }
  fadeFill = 0;
  for (int i = conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount + conn5LedCount + conn6LedCount + conn7LedCount + conn8LedCount + conn9LedCount + conn10LedCount + conn11LedCount; i < conn1LedCount + conn2LedCount + conn3LedCount + conn4LedCount + conn5LedCount + conn6LedCount + conn7LedCount + conn8LedCount + conn9LedCount + conn10LedCount + conn11LedCount + conn12LedCount; i++) {
    if (node7 == HIGH && node6 == HIGH) {
      conn.setPixelColor(i, 0, 0, highFadeNode, rampFillTwelveWhite); //fade out white and pulse blue
    } else if (node7 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade conn out from node 7
      fadeFill++;
    } else if (node6 == HIGH) {
      conn.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[conn12LedCount - fadeFill - 1]), 0); // fade conn out from node 6
      fadeFill++;
    } else {
      conn.setPixelColor(i, 0, 0, 0, 0);
    }
  }

  if (node1 == HIGH && node2 == HIGH)rampFillOneWhite -= 4;
  if (node2 == HIGH && node3 == HIGH)rampFillTwoWhite -= 4;
  if (node3 == HIGH && node4 == HIGH)rampFillThreeWhite -= 4;
  if (node4 == HIGH && node5 == HIGH)rampFillFourWhite -= 4;
  if (node5 == HIGH && node6 == HIGH)rampFillFiveWhite -= 4;
  if (node6 == HIGH && node1 == HIGH)rampFillSixWhite -= 4;
  if (node7 == HIGH && node1 == HIGH)rampFillSevenWhite -= 4;
  if (node7 == HIGH && node2 == HIGH)rampFillEightWhite -= 4;
  if (node7 == HIGH && node3 == HIGH)rampFillNineWhite -= 4;
  if (node7 == HIGH && node4 == HIGH)rampFillTenWhite -= 4;
  if (node7 == HIGH && node5 == HIGH)rampFillElevenWhite -= 4;
  if (node7 == HIGH && node6 == HIGH)rampFillTwelveWhite -= 4;
  if (rampFillOneWhite <= 0) rampFillOneWhite = 0;
  if (rampFillTwoWhite <= 0) rampFillTwoWhite = 0;
  if (rampFillThreeWhite <= 0) rampFillThreeWhite = 0;
  if (rampFillFourWhite <= 0) rampFillFourWhite = 0;
  if (rampFillFiveWhite <= 0) rampFillFiveWhite = 0;
  if (rampFillSixWhite <= 0) rampFillSixWhite = 0;
  if (rampFillSevenWhite <= 0) rampFillSevenWhite = 0;
  if (rampFillEightWhite <= 0) rampFillEightWhite = 0;
  if (rampFillNineWhite <= 0) rampFillNineWhite = 0;
  if (rampFillTenWhite <= 0) rampFillTenWhite = 0;
  if (rampFillElevenWhite <= 0) rampFillElevenWhite = 0;
  if (rampFillTwelveWhite <= 0) rampFillTwelveWhite = 0;

  conn.show();
  highPulseNodes();
  //delay(10);
}

//--------------------------------INDEPENDENT ORPHAN LIGHTING FUNCTION---------------------------------------

void pickAOrphan(bool node1, bool node2, bool node3, bool node4, bool node5, bool node6, bool node7) {
  int fadeFill = 0;
  for (int i = 0; i < orphan1LedCount; i++) {
    if (node1 == HIGH) {
      orphans.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade orphan out from node 1
    } else {
      orphans.setPixelColor(i, 0, 0, 0, 0);
    }
    fadeFill++;
  }
  fadeFill = 0;
  for (int i = orphan1LedCount; i < orphan1LedCount + orphan2LedCount; i++) {
    if (node2 == HIGH) {
      orphans.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade orphan out from node 2
    } else {
      orphans.setPixelColor(i, 0, 0, 0, 0);
    }
    fadeFill++;
  }
  fadeFill = 0;
  for (int i = orphan1LedCount + orphan2LedCount; i < orphan1LedCount + orphan2LedCount + orphan3LedCount; i++) {
    if (node3 == HIGH) {
      orphans.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade orphan out from node 3
    } else {
      orphans.setPixelColor(i, 0, 0, 0, 0);
    }
    fadeFill++;
  }
  fadeFill = 0;
  for (int i = orphan1LedCount + orphan2LedCount + orphan3LedCount; i < orphan1LedCount + orphan2LedCount + orphan3LedCount + orphan4LedCount; i++) {
    if (node4 == HIGH) {
      orphans.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade orphan out from node 4
    } else {
      orphans.setPixelColor(i, 0, 0, 0, 0);
    }
    fadeFill++;
  }
  fadeFill = 0;
  for (int i = orphan1LedCount + orphan2LedCount + orphan3LedCount + orphan4LedCount; i < orphan1LedCount + orphan2LedCount + orphan3LedCount + orphan4LedCount + orphan5LedCount; i++) {
    if (node5 == HIGH) {
      orphans.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade orphan out from node 5
    } else {
      orphans.setPixelColor(i, 0, 0, 0, 0);
    }
    fadeFill++;
  }
  fadeFill = 0;
  for (int i = orphan1LedCount + orphan2LedCount + orphan3LedCount + orphan4LedCount + orphan5LedCount; i < orphan1LedCount + orphan2LedCount + orphan3LedCount + orphan4LedCount + orphan5LedCount + orphan6LedCount; i++) {
    if (node6 == HIGH) {
      orphans.setPixelColor(i, 0, 0, pgm_read_byte(&fadeTable[fadeFill]), 0); // fade orphan out from node 6
    } else {
      orphans.setPixelColor(i, 0, 0, 0, 0);
    }
    fadeFill++;
  }
  fadeFill = 0;
  orphans.show();

}

//-------------------------------Reset Node trigger Timers----------------------------------
void resetNodeTimers() {
node1Millis = 1; // 0.001 second for start of game
node2Millis = 1;
node3Millis = 1;
node4Millis = 1;
node5Millis = 1;
node6Millis = 1;
node7Millis = 1;
}

//-------------------------------STATUS LEDS FILL FUNCTION----------------------------------

void setStatus(bool opto1, bool opto2, bool opto3, bool opto4, bool opto5, bool opto6, bool opto7, bool adxl1) {

  if (opto1) sensorStatus.setPixelColor(0, 0, 20, 0, 0);  // Red if sensor is stable
  else sensorStatus.setPixelColor(0, 100, 0, 0, 0);    // Green if sensor is bad or active

  if (opto2) sensorStatus.setPixelColor(1, 0, 20, 0, 0);
  else sensorStatus.setPixelColor(1, 100, 0, 0, 0);

  if (opto3) sensorStatus.setPixelColor(2, 0, 20, 0, 0);
  else sensorStatus.setPixelColor(2, 100, 0, 0, 0);

  if (opto4) sensorStatus.setPixelColor(3, 0, 20, 0, 0);
  else sensorStatus.setPixelColor(3, 100, 0, 0, 0);

  if (opto5) sensorStatus.setPixelColor(4, 0, 20, 0, 0);
  else sensorStatus.setPixelColor(4, 100, 0, 0, 0);

  if (opto6) sensorStatus.setPixelColor(5, 0, 20, 0, 0);
  else sensorStatus.setPixelColor(5, 100, 0, 0, 0);

  if (opto7) sensorStatus.setPixelColor(6, 0, 20, 0, 0);
  else sensorStatus.setPixelColor(6, 100, 0, 0, 0);

  if (adxl1) sensorStatus.setPixelColor(7, 0, 20, 0, 0);
  else sensorStatus.setPixelColor(7, 100, 0, 0, 0);
  
  sensorStatus.show();
}