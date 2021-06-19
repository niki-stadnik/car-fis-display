//#include <Nextion.h>
#include <Wire.h>
#include "RTClib.h"
#include <math.h>


class SensorReading
{
  float Minimum = 0;
  float Maximum = 1.1;
  float ARef = 0.0;
  
  unsigned int counter = 0;
  float Sum = 0.0;
  float Average = 0.0;


  public:
  SensorReading(float Min, float Max, float ref)   //podavane na  granici za proverka, defalt e 0-1.1v
  {
    Minimum = Min;
    Maximum = Max;
    ARef = ref;
  }

  void SetData(float  RAWIN)
  {
    float Voltage = (RAWIN * ARef)/1024.00;
    
    if(Voltage <= Maximum && Voltage >= Minimum){
      noInterrupts();
      Sum = Sum + Voltage;
      counter++;
      interrupts();  
    }
  }

  float GetData()
  {
    Average = Sum / counter;
    Sum = 0.0;
    counter = 0;
    return Average;
  }

  float GetResistance(float Vout, float R1, float Vin)
  {
    float Resistance = (Vout * R1)/(Vin - Vout);
    return Resistance;
  }
};



  RTC_DS3231 rtc;


  //Internal Analog Reference
  float ARef = 1.08; //samo na regulator 1.090 // na usb i regulator 1.095 // 1.13 za to4en i na 2te // 1.125 za toqen samo na zahranvane?

  //voltage
  int analogInputExternal = A0;
  int analogInputInternal = A1;
  float IntVout = 0.0;
  float IntVin = 0.0;
  float ExtVout = 0.0;
  float ExtVin = 0.0;
  
  float intR1 = 29430.0; //29730.0 real //28690
  float intR2 = 7470.0; //7470.0 real //7600
  float extR1 = 90000.0; //29760.0    da sloja 100k   //to 3.3V //89000
  float extR2 = 5158.0; //to ground 7440  da skoja 4.9k   //100k + 4.9k mi dava ot4et ot 0 do 22V i bezopasen do 105V //5195

  //ohm pres  99.8 test
  int PinOilPressure = A2;
  float OilPressure = 0.0;
  float Resistor = 400.0; //463 real   // 460 mi dava ot4et do 215ohm

  //ohm temp oil  7.47test
  int PinOilTemp = A3;
  float OilTemp = 0.0;
  float ResistorOilTemp = 46240.0; //46100 real //47k mi dava otqet ot 0 do 22kohm - 150-45gradusa

  //ohm temp water
  int PinWaterTemp = A4;
  float WaterTemp = 0.0;
  float ResistorWaterTemp = 46130.0;  //46000 real

  //send data timer
  unsigned int Inter = 500;   //500 default
  unsigned long PreviousUp = 0;

  SensorReading VoltageExt(0.0, 1.1, ARef);
  SensorReading VoltageInt(0.0, 1.1, ARef);
  SensorReading OIL(0.0, 1.1, ARef); 
  SensorReading OILTemp(0.0, 1.1, ARef);
  SensorReading WATERTemp(0.0, 1.1, ARef);


  //test
   float OilResistance = 0.0;
   float OilTempResistance = 0.0;
   float WaterResistance = 0.0;
   float WaterTempResistance = 0.0;


   //Warning lights
   const int NUM_lights = 6;
   const int warPin[NUM_lights] = {9, 8, 7, 6, 5, 4};
   boolean warFlag[NUM_lights];
   int warState[NUM_lights];
   int lightsON = 0;

   //Dimmer
   int DimmerPin = 14;
   unsigned long PWMduration;


   int burzodeistvie = 0;

  
void setup()
{

  //voltage
  analogReference(INTERNAL1V1);
  
  pinMode(analogInputExternal, INPUT);
  pinMode(analogInputInternal, INPUT);
  pinMode(PinOilPressure, INPUT);
  pinMode(PinOilTemp, INPUT);
  pinMode(PinWaterTemp, INPUT);

  //Warning lights
  for (int i = 0; i < NUM_lights; i++) {
    pinMode(warPin[i], INPUT_PULLUP);
    warFlag[i] = false;
  }

  //Dimmer
  pinMode(DimmerPin, INPUT);

  
  Serial.begin(9600);
  Serial1.begin(9600);

  //clock
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");  
  // Comment out below lines once you set the date & time.
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
}

void loop()
{
  burzodeistvie++;
  
  //Voltage
  VoltageExt.SetData(analogRead(analogInputExternal));
  VoltageInt.SetData(analogRead(analogInputInternal));


   //pressure
  OIL.SetData(analogRead(PinOilPressure));

  //temp
  OILTemp.SetData(analogRead(PinOilTemp));
  WATERTemp.SetData(analogRead(PinWaterTemp));
  

  if(millis() >= (PreviousUp + Inter))
  {
    PreviousUp = millis();

    //voltage meter
    // real voltage going in the arduino (car voltage)
    ExtVout = VoltageExt.GetData();
    ExtVin = ExtVout / (extR2/(extR1+extR2));
    // real voltage at the arduino regulator
    IntVout = VoltageInt.GetData();
    IntVin = IntVout / (intR2/(intR1+intR2));

    OilPressure = OIL.GetData();
    //OilResistance = OIL.GetResistance(OilPressure, Resistor, IntVin);
    if(OilPressure <= 0.06) OilPressure = 0;
    else if (OilPressure > 0.06 && OilPressure <= 0.2)  OilPressure = 41506 * pow(OilPressure, 5.0) - 30628 * pow(OilPressure, 4.0) + 8868.7 * pow(OilPressure, 3.0) - 1257.3 * pow(OilPressure, 2.0) + 92.081 * OilPressure - 2.1502;
    else if (OilPressure > 0.2 && OilPressure <= 0.38)  OilPressure = 98690 * pow(OilPressure, 5.0) - 141546 * pow(OilPressure, 4.0) + 80064 * pow(OilPressure, 3.0) - 22305 * pow(OilPressure, 2.0) + 3065.4 * OilPressure - 165.29;
    else if (OilPressure > 0.38 && OilPressure <= 0.52) OilPressure = 318436 * pow(OilPressure, 5.0) - 724624 * pow(OilPressure, 4.0) + 657792 * pow(OilPressure, 3.0) - 297737 * pow(OilPressure, 2.0) + 67200 * OilPressure - 6048.08;
    else if (OilPressure > 0.52 && OilPressure <= 0.67) OilPressure = -878.47 * pow(OilPressure, 4.0) + 1354.7 * pow(OilPressure, 3.0) - 529.86 * pow(OilPressure, 2.0) - 53.47 * OilPressure + 48.834;
    else if (OilPressure > 0.67 && OilPressure <= 0.93) OilPressure = -44345 * pow(OilPressure, 5.0) + 176441 * pow(OilPressure, 4.0) - 279398 * pow(OilPressure, 3.0) + 220044 * pow(OilPressure, 2.0) - 86155 * OilPressure + 13416;
    else OilPressure = 10;
    

    OilTemp = OILTemp.GetData();
    //OilTempResistance = OILTemp.GetResistance(OilTemp, ResistorOilTemp, IntVin);
    OilTemp = 2150.6 * pow(OilTemp, 6.0) - 8656.7 * pow(OilTemp, 5.0) + 13925 * pow(OilTemp, 4.0) - 11497 * pow(OilTemp, 3.0) + 5211.2 * pow(OilTemp, 2.0) - 1307.6 * OilTemp + 236.22;
    
    WaterTemp = WATERTemp.GetData();
    //WaterTempResistance = WATERTemp.GetResistance(WaterTemp, ResistorWaterTemp, IntVin);
    WaterTemp = 2150.6 * pow(WaterTemp, 6.0) - 8656.7 * pow(WaterTemp, 5.0) + 13925 * pow(WaterTemp, 4.0) - 11497 * pow(WaterTemp, 3.0) + 5211.2 * pow(WaterTemp, 2.0) - 1307.6 * WaterTemp + 236.22;
    

    //Dimmer
    PWMduration = pulseIn(DimmerPin, HIGH, 150000);     //da go svalq na minimuma vyzmoxen (nai golemiq interval za dimura)
    //if the value is HIGH, pulseIn() waits for the pin to go from LOW to HIGH, starts timing then waits for the pin to go LOW and stops timing.
    //Returns the length of the pulse in microseconds or gives up and returns 0 if no complete pulse was received within the timeout
    
    //SendToPC();
    SendToNextion();
    Warnings();

    burzodeistvie = 0;
  }


  
}


void SendToNextion()
  {
    //clock
    DateTime now = rtc.now();
    Serial1.print("time");
    Serial1.print(F(".txt=\""));
    if(now.hour()<10)Serial1.print('0');
    Serial1.print(now.hour());
    Serial1.print(':');
    if(now.minute()<10)Serial1.print('0');
    Serial1.println(now.minute());
    Serial1.print(F("\""));
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);


//da mahna!!!
    //Nextxt("time", burzodeistvie);

    
    //nextion dimmer
    int brightness = map(PWMduration, 0, 11000, 0, 100);
    brightness = constrain(brightness, 45, 100);
    String dim = "dim=" + String(brightness);
    Serial1.print(dim);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    //dimura 6te se durji na 7k /2500-13000
    //String printedString = String("brightness") + String(": ") + String(brightness) + String(" %") + String("\n");
    //Serial.write(printedString.c_str());
  

    //sensors
    Nextxt("cv", ExtVin);
    if(OilPressure > 0.42)Nextxt("op", OilPressure);
    else Nextxt("op", 0.0);
    if(OilTemp > 57.6)    Nextxt("ot", OilTemp);
    else Nextcold("ot");
    if(WaterTemp > 57.6)  Nextxt("wt", WaterTemp);
    else Nextcold("wt");
  }
void Nextxt(String to, float se)  //name of nextion var, name of arduino var
  {
    Serial1.print(to);
    Serial1.print(F(".txt=\""));
    Serial1.print(se);
    Serial1.print(F("\""));
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
  }

 void Nextcold(String to)
  {
    Serial1.print(to);
    Serial1.print(F(".txt=\"Cold\""));
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
  }

void Warnings()    //минава лампите 1 по 1 и ако е пусната и няма флаг праща 1, ако е пусната и има флаг не прави нищо, ако е изключена и няма флаг праща 0, ако е изключена и има флаг не прави нищо
 {                 //флаг определя дали лампата вече свети за да не праща еднакви сигнали; LightsON брой пуснатите лампи и когато последната изгасне спира надписа
   for (int i = 0; i < NUM_lights; i++) {
     String lightNum = String("p") + String(i);
     warState[i] = digitalRead(warPin[i]);
     if (warState[i] == LOW) {   //dali logikata ne e obyrnata zaradi pullup??
       if (warFlag[i] == false){
         Light(lightNum, 1);
         Light("war", 1);
         warFlag[i] = true;
         lightsON++;
       }
     } else if(warFlag[i] == true){
       Light(lightNum, 0);
       warFlag[i] = false;
       lightsON--;
       if(lightsON == 0){
         Light("war", 0);
       }
     }
   }
 }
void Light(String to, int state)   //name of nextion var, state on/off 1/0
 {
   Serial1.print("vis ");
   Serial1.print(to);
   Serial1.print(",");
   Serial1.print(state);
   Serial1.write(0xff);
   Serial1.write(0xff);
   Serial1.write(0xff);
 }


void SendToPC()
  {
    DateTime now = rtc.now();
    Serial.print("Current Time: ");
    if(now.hour()<10)Serial.print('0');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    if(now.minute()<10)Serial.print('0');
    Serial.println(now.minute(), DEC);
    
    String printedString = String("ExtVin") + String(": ") + String(ExtVin) + String(" mV") + String("\n");
    Serial.write(printedString.c_str());
    printedString = String("IntVin") + String(": ") + String(IntVin) + String(" mV") + String("\n");
    Serial.write(printedString.c_str());
    printedString = String("OilPressure") + String(": ") + String(OilPressure) + String(" bar") + String("\n");
    Serial.write(printedString.c_str());
    printedString = String("OilResistance") + String(": ") + String(OilResistance) + String(" ohm") + String("\n");
    Serial.write(printedString.c_str());
    printedString = String("OilTemp") + String(": ") + String(OilTemp) + String(" mV") + String("\n");
    Serial.write(printedString.c_str());
    printedString = String("OilTempResistance") + String(": ") + String(OilTempResistance) + String(" ohm") + String("\n");
    Serial.write(printedString.c_str());
    printedString = String("WaterTemp") + String(": ") + String(WaterTemp) + String(" mV") + String("\n");
    Serial.write(printedString.c_str());
    printedString = String("WaterTempResistance") + String(": ") + String(WaterTempResistance) + String(" ohm") + String("\n");
    Serial.write(printedString.c_str());
    printedString = String("Dimmer PWM") + String(": ") + String(PWMduration) + String(" microseconds") + String("\n");
    Serial.write(printedString.c_str());
    printedString = String("burzodeistvie") + String(": ") + String(burzodeistvie) + String(" loops/print") + String("\n");
    Serial.write(printedString.c_str());
    

    Serial.write("\n");
  }
