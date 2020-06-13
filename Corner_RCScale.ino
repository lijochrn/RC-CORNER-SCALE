#include <LiquidCrystal_I2C.h>
#include <HX711_ADC.h>

LiquidCrystal_I2C lcd(0x27,20,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display


char Telecom[32] = {'P','R','E','S','S',' ','T','A','R','E',' ','B','U','T','T','O','N',' ','T','O',' ','C','O','N','T','I','N','U','E','!','!',' '};

int ModeButton = 11;             // Button connected to digital pin 11 acts as mode/tare selection
int WtButton = 8;                // Button connected to digital pin 8 acts as Weight/Total Weight display

int L1DOUT = A0;              // LOADCELL connected to ANALOG INPUT 0  //Front Right  DOUT
int L1SCK = 2;              // LOADCELL connected to DIGITAL INPUT 2  //Front Right  SCK

int L2DOUT = A1;              // LOADCELL connected to ANALOG INPUT 1  //Front Left  DOUT
int L2SCK = 3;              // LOADCELL connected to DIGITAL INPUT 3  //Front Left  SCK

int L3DOUT = A2;              // LOADCELL connected to ANALOG INPUT 2  //Back Right  DOUT
int L3SCK = 4;              // LOADCELL connected to DIGITAL INPUT 4  //Back Right  SCK

int L4DOUT = A3;              // LOADCELL connected to ANALOG INPUT 3//Back Left  DOUT
int L4SCK = 5;              // LOADCELL connected to DIGITAL INPUT 5  //Back Left  SCK

int presscount = 0;              // Counting how many Wtbutton press has performed
int buttonState = 0;             // current state of the Wtbutton
int lastButtonState = 0;         // previous state of the Wtbutton
int modestate = 0;

//HX711 constructor (dout pin, sck pin)
HX711_ADC Loadcell_1(L1DOUT, L1SCK); //Load Cell 1 Front Right
HX711_ADC Loadcell_2(L2DOUT, L2SCK); //Load Cell 2 Front Left
HX711_ADC Loadcell_3(L3DOUT, L3SCK); //Load Cell 3 Back Right
HX711_ADC Loadcell_4(L4DOUT, L4SCK); //Load Cell 4 Back Left

//const int calVal_eepromAdress_1 = 0; // eeprom adress for calibration value load cell 1 (4 bytes)
//const int calVal_eepromAdress_2 = 4; // eeprom adress for calibration value load cell 2 (4 bytes)
//const int calVal_eepromAdress_3 = 8; // eeprom adress for calibration value load cell 3 (4 bytes)
//const int calVal_eepromAdress_4 = 12; // eeprom adress for calibration value load cell 4 (4 bytes)
long t;
float weight_FR=0.0;
float weight_FL=0.0;
float weight_BR=0.0;
float weight_BL=0.0;
float weight_F=0.0;
float weight_B=0.0;
float totalwt=0.0;
float weight_FRBL=0.0;
float weight_FLBR= 0.0;
float percnt_F = 0.0;
float percnt_B = 0.0;
void setup()
{
  pinMode(ModeButton, INPUT);      // sets the digital pin as input
  pinMode(WtButton, INPUT);       // sets the digital pin as input
  pinMode(6, OUTPUT); 

  lcd.init();                     // initialize the lcd   
  lcd.backlight(); 
  lcd.setCursor(2,0);
  lcd.print("<<WELCOME>>"); 
  lcd.setCursor(1,1);
  lcd.print("CORNER WEIGHT"); 
  delay(2000);
  lcd.setCursor(2,0);
  lcd.print("Initiating..");   
  float calibrationValue_1; // calibration value load cell 1 Front Right
  float calibrationValue_2; // calibration value load cell 2 Front Left
  float calibrationValue_3; // calibration value load cell 3 Back Right
  float calibrationValue_4; // calibration value load cell 4 Back Left
  calibrationValue_1 = 1070.13; 
  calibrationValue_2 = 1145.97; 
  calibrationValue_3 = 1154.0; 
  calibrationValue_4 = 1056.95; //1132.28

#if defined(ESP8266) || defined(ESP32)
  //EEPROM.begin(512); // uncomment this if you use ESP8266 and want to fetch the value from eeprom
#endif
  //EEPROM.get(calVal_eepromAdress_1, calibrationValue_1); // uncomment this if you want to fetch the value from eeprom
  //EEPROM.get(calVal_eepromAdress_2, calibrationValue_2); // uncomment this if you want to fetch the value from eeprom 
  //EEPROM.get(calVal_eepromAdress_3, calibrationValue_3); // uncomment this if you want to fetch the value from eeprom
  //EEPROM.get(calVal_eepromAdress_4, calibrationValue_4); // uncomment this if you want to fetch the value from eeprom 

  
  Loadcell_1.begin(); // start connection to HX711 1 Front Right
  Loadcell_2.begin(); // start connection to HX711 2 Front Left
  Loadcell_3.begin(); // start connection to HX711 3 Back Right
  Loadcell_4.begin(); // start connection to HX711 4 Back Left
  
  long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  byte loadcell_1_rdy = 0;
  byte loadcell_2_rdy = 0;
  byte loadcell_3_rdy = 0;
  byte loadcell_4_rdy = 0;


  lcd.clear(); 
  lcd.setCursor(1,0);
  lcd.print("LOADCELL CHECK");  
  lcd.setCursor(3,1);
  lcd.print("CONNECTION");  

  while ((loadcell_1_rdy + loadcell_2_rdy + loadcell_3_rdy + loadcell_4_rdy) < 4) { //run startup, stabilization and tare, both modules simultaniously
    if (!loadcell_1_rdy) loadcell_1_rdy = Loadcell_1.startMultiple(stabilizingtime, _tare);
    if (!loadcell_2_rdy) loadcell_2_rdy = Loadcell_2.startMultiple(stabilizingtime, _tare);
    if (!loadcell_3_rdy) loadcell_3_rdy = Loadcell_3.startMultiple(stabilizingtime, _tare);
    if (!loadcell_4_rdy) loadcell_4_rdy = Loadcell_4.startMultiple(stabilizingtime, _tare);    
  }

  lcd.clear(); 
  
  if (Loadcell_1.getTareTimeoutFlag()) {
    lcd.setCursor(0,0);
    lcd.print("CHECK WIRING OF"); 
    lcd.setCursor(0,1);
    lcd.print("LOADCELL 1 FR"); 
     delay(1000);    
  }
  if (Loadcell_2.getTareTimeoutFlag()) {
    lcd.setCursor(0,0);
    lcd.print("CHECK WIRING OF"); 
    lcd.setCursor(0,1);
    lcd.print("LOADCELL 2 FL"); 
     delay(1000);  
  }
   if (Loadcell_3.getTareTimeoutFlag()) {
    lcd.setCursor(0,0);
    lcd.print("CHECK WIRING OF"); 
    lcd.setCursor(0,1);
    lcd.print("LOADCELL 3 BR"); 
     delay(1000);    
  }
  if (Loadcell_4.getTareTimeoutFlag()) {
    lcd.setCursor(0,0);
    lcd.print("CHECK WIRING OF"); 
    lcd.setCursor(0,1);
    lcd.print("LOADCELL 4 FL"); 
     delay(2000);  
  }
  
  Loadcell_1.setCalFactor(calibrationValue_1); // user set calibration value (float)
  Loadcell_2.setCalFactor(calibrationValue_2); // user set calibration value (float)
  Loadcell_3.setCalFactor(calibrationValue_3); // user set calibration value (float)
  Loadcell_4.setCalFactor(calibrationValue_4); // user set calibration value (float)

  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("STARTUP COMPLETE");     
  delay(2000);  
  lcd.clear(); 
}


void loop()
{

  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (Loadcell_1.update()) newDataReady = true;
  Loadcell_2.update();
  Loadcell_3.update();
  Loadcell_4.update();

  //get smoothed value from data set
  if ((newDataReady)) {
    if (millis() > t + serialPrintInterval) {
      float FR = Loadcell_1.getData();           // Read Analog Value from loadcell 1 to FR internal register.
      weight_FR= FR;
      
      float FL = Loadcell_2.getData(); 
      weight_FL= FL;
      
      float BR = Loadcell_3.getData(); 
      weight_BR= BR;

      float BL = Loadcell_4.getData(); 
      weight_BL= BL;
      
      weight_F= weight_FR+weight_FL;        // Calculating total weight of FRONT in grams
      weight_B= weight_BR+weight_BL;        // Calculating total weight of BACK in grams
      totalwt= weight_F+weight_B;           // Calculating total weight of chassis

      weight_FRBL= weight_FR+weight_BL;        // Calculating total weight of FRONT RIGHT AND BACK LEFT in grams
      weight_FLBR= weight_BR+weight_FL;        // Calculating total weight of FRONT LEFT AND BACK RIGHT in grams
      float totalCRNRwt= weight_FRBL+weight_FLBR;    // Calculating CORNER EACH weight of chassis

      percnt_F = weight_F*(100.0/totalwt);  // Calculating total weight % of Front area
      percnt_B = weight_B*(100.0/totalwt);  // Calculating total weight % of Back area

      newDataReady = 0;
      t = millis();
    }
  }

buttonState = digitalRead(WtButton); // read the pushbutton input pin:
modestate = digitalRead(ModeButton); // read the pushbutton input pin:

//------Calling TARE Function if TARE button is pressed
if (modestate == HIGH) {
  tare ();
}
//--------------------------------------   


      
  if (buttonState != lastButtonState) {   // compare the buttonState to its previous state
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button
      // went from off to on:
        presscount++;
        lcd.clear(); 
        }
          
    delay(50);  // Delay a little bit to avoid bouncing
  }    

lastButtonState = buttonState; // save the current state as the last state,for next time through the loop

switch (presscount) {
    case 0:    
      lcd.setCursor(2,0);
      lcd.print("PRESS BUTTON");  
      lcd.setCursor(4,1);
      lcd.print("TOGGLE");  
      
      break;
    case 1:    // WEIGHT OF EACH TIRE
      lcd.setCursor(0,0);
      lcd.print(String("FR ") + String(weight_FR));  
      lcd.setCursor(8,0);
      lcd.print(String("BR ") + String(weight_BR));
            /////
      lcd.setCursor(0,1);
      lcd.print(String("FL ") + String(weight_FL)); 
      lcd.setCursor(8,1);
      lcd.print(String("BL ") + String(weight_BL));
      
      break;
    case 2:   // WEIGHT OF FRONT AND REAR & TOTAL WEIGHT
      lcd.setCursor(0,0);
      lcd.print(String("F=") + String(weight_F));  
      lcd.setCursor(8,0);
      lcd.print(String("B=") + String(weight_B));
            /////
      lcd.setCursor(2,1);
      lcd.print(String("TOTAL=") + String(totalwt) + String("g"));   
         
      break;     
    case 3:    // TWO CORNER WEIGHTS
      lcd.setCursor(2,0);
      lcd.print(String("FR+BL=") + String(weight_FRBL));  
      lcd.setCursor(2,1);
      lcd.print(String("FL+BR=") + String(weight_FLBR));
      break;
    case 4:    // WEIGHT DISTRIBUTION %  OF FRONT AND REAR 
      
      lcd.setCursor(2,0);
      lcd.print(String("FRONT=") + String(percnt_F) + String("%"));  
      lcd.setCursor(2,1);
      lcd.print(String("BACK =") + String(percnt_B) + String("%"));                   
      break;
    case 5:    // GO BACK 
      presscount = 1;
    break;
}

}

void tare(){
//Function for TARE operation   
      lcd.clear(); 
      lcd.setCursor(1,0);
      lcd.print("TARE OPERATION");  
      lcd.setCursor(4,1);
      lcd.print("STARTING"); 
      delay(2000); 
      lcd.setCursor(0,0);
      lcd.print("REMOVE ALL LOAD");  
      lcd.setCursor(3,1);
      lcd.print("FROM TRAYS"); 
      delay(2000); 
      modestate = digitalRead(ModeButton); // read the pushbutton input pin:

// Delay for REMOVING load from all trays, system conitue only on TARE button ptess.
// Below code for LCD scrolling[Message types in chracter Array[Telecom] of 32 length  from right to left
      while (modestate == LOW) {
      lcd.setCursor(0,1);  
      for (int j=0;j<=31;j++)
      {
      lcd.print(Telecom[j]); 
      }
      lcd.setCursor(0,1);
      delay(250);
      int swap = Telecom[0];
      for (int i=0;i<31;i++)
      {
      Telecom[i] = Telecom[i+1]; 
      }
      Telecom[31] = swap; 
      modestate = digitalRead(ModeButton); // read the pushbutton input pin:    
      }
/////////////////////////////// LCD scrolling code ends.

      Loadcell_1.tareNoDelay();
      Loadcell_2.tareNoDelay();
      Loadcell_3.tareNoDelay();
      Loadcell_4.tareNoDelay(); 
            
      presscount = 0;     
   
  //check if last tare operation is complete
  if (Loadcell_1.getTareStatus() == true) {
       lcd.clear();
       lcd.setCursor(1,0);
       lcd.print("TARE FRLOADCELL");  
       lcd.setCursor(4,2);
       lcd.print("COMPLETE");
        delay(1000);      
  }
  if (Loadcell_2.getTareStatus() == true) {
       lcd.setCursor(1,0);
       lcd.print("TARE FLLOADCELL");  
       lcd.setCursor(4,2);
       lcd.print("COMPLETE");
       delay(1000);  
  }
  if (Loadcell_3.getTareStatus() == true) {
       lcd.setCursor(1,0);
       lcd.print("TARE BRLOADCELL");  
       lcd.setCursor(4,2);
       lcd.print("COMPLETE");
       delay(1000);  
  }    
  if (Loadcell_4.getTareStatus() == true) {
       lcd.setCursor(1,0);
       lcd.print("TARE BLLOADCELL");  
       lcd.setCursor(4,2);
       lcd.print("COMPLETE");
       delay(1000);  
  }  
   lcd.clear();
}
