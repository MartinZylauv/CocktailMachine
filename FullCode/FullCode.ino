/*     Cocktail Machine arduino

    by Martin Zylauv.

    I'm sorry for the messy code, it is currently what is running on the machine, and it kinda is a work in progress :) I know what clean code looks like, but sometimes things get messy when you're trying new things

    A more updated code will come later!

*/


#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

int maximumLength =  1000;
int glassLength = 20;
float currentLength = 0;
const int endSwitch = 13;
const int stepPin = 11; //sleigh step
const int dirPin = 12; //sleigh dir
const int pumpDir = 7;

int pumpSteps[] = {30, 32, 34, 36, 38, 40, 42, 44, 46, 48,2,3,4,5};
int pumpEnable[] = {31, 33, 35, 37, 39, 41, 43, 45, 47, 49};
double bottlesPos[] = {1000,940,880,820,760,700,640,580,520,460,400,340,280,220}; //these are in order from left to right
const int pump5Step = 36;
const int pumpt5Enable = 37;
const int trigPin = 9;
const int echoPin = 10;
int right = 0;
int left = 1;
double ar[] = {400, 450, 550, 450, 400, 550, 500, 450};
int i = 0;
int maxi = 8;
int fastestDelay = 35; //minimum 750 i fart
int slowestDelay = 10000;
bool hasBeenStopped = false;
bool lefts = false;
int lcdTesti = 0;
char state;
char c = ' ';
int airPumps[] = {2,3,4,5};
int sodas[] = {11,12,13,14};
bool glassPlaced = false;
int airPumpDelays[13];
double airPumpMsPrMl[13]; 

struct BottleAmount {
  int bottleNum;
  float amount;
};

struct Command {
  BottleAmount bottleAmounts[20];
  String commandName;
  String userName;
  int bas = 0 ;
};

Command cmdmsg; //the current command
 bool commandInProgress = false;
bool pumpTested = false;
bool testBottlePosDone = false;
bool testing = false;


SoftwareSerial UNO(50, 52); // //the uno is coded for lights, more info about this will come in a video later!:)
int currentLight = 0;

void setup() {

  airPumpDelays[10] = 1000;
airPumpDelays[11] = 1000;
airPumpDelays[12] = 1000;
airPumpDelays[13] = 1000;


airPumpMsPrMl[10] = 50;
airPumpMsPrMl[11] = 50;
airPumpMsPrMl[12] = 40;
airPumpMsPrMl[13] = 50;

  lcd.init();
  lcd.backlight();
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);


    
 
  for (int i = 0; i < 14; i++) {

    if(i < 10){
    pinMode(pumpSteps[i], OUTPUT);
    pinMode(pumpEnable[i], OUTPUT);
    
    digitalWrite(pumpEnable[i], HIGH);
    } else{
      pinMode(pumpSteps[i], OUTPUT);
      digitalWrite(pumpSteps[i], HIGH);
    }
  }



  pinMode(pumpDir, OUTPUT);



  pinMode(endSwitch, INPUT_PULLUP);



  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  Serial.begin(9600);
  Serial1.begin(9600);
  Serial3.begin(9600);
  UNO.begin(9600);


  printToLcd(0, "Resetting machine..", false);
  // digitalWrite(enablePin, LOW);
  move(right, 1100, false); //resets machine



  printToLcd(0, "Hey! Want a drink?", true);
  printToLcd(1, "", false);
  printToLcd(2, "Select drink in app", false);
  lightCommand(1);

  
}
void loop() {


  if (!commandInProgress ) {
    cmdmsg = getBluetoothCmd();
  }

 
    
  

if ( cmdmsg.commandName != "" && cmdmsg.bas != 0   ) {

    
    commandInProgress = true;
    printToLcd(0, "Please place glass", true);
    lightCommand(2);
  } 

  if(cmdmsg.bas != 0){
  int dist = getDistance();
  if (dist < glassLength && !glassPlaced && !lefts) {
     lightCommand(3);
    printToLcd(0, "Please stand back", true);
    glassPlaced = true;
    printToLcd(1, "starting in 5", false);
    delay(1000);
    printToLcd(1, "starting in 4", false);
    delay(1000);
    printToLcd(1, "starting in 3", false);
    delay(1000);
    printToLcd(1, "starting in 2", false);
    delay(1000);
    printToLcd(1, "starting in 1", false);
    delay(1000);

    printToLcd(0, "Making drink..", true);

  lightCommand(4);
    for (int i = 0; i < cmdmsg.bas; i++) {
      int pos = bottlesPos[cmdmsg.bottleAmounts[i].bottleNum-1];
      
      moveToPos(pos);
      pump(cmdmsg.bottleAmounts[i].amount, cmdmsg.bottleAmounts[i].bottleNum-1);

    }
    printToLcd(0, "drink done, resetting..", true);
   moveToPos(0);
   lightCommand(1);
    printToLcd(0, "Enjoy your drink!", true);
    printToLcd(1, "Dont drink and drive", false);
    printToLcd(2, "Please Take glass", false);
    delay(1000);
    delay(4000);
    glassPlaced = false;
    commandInProgress = false;
  printToLcd(0, "Hey! Want a drink?", true);
  printToLcd(1, "", false);
  printToLcd(2, "Select drink in app", false);
  }
  }


}


bool pump(float amount, int pumpNumber) {

  //just doing the first for a second
  float caliAmount = 4706;
  int maxPumpSpeed = 50;
  int slowPumpSpeed = 200;
  
  
  
  if(pumpNumber < 10){ // for the perastaic pumpts
  amount = amount*caliAmount; //CALIBRATION FOR AMOUNT IN MILILETERS.
  digitalWrite(pumpEnable[pumpNumber], LOW);
  Serial.println(pumpEnable[pumpNumber]);
   Serial.println(pumpSteps[pumpNumber]);
    Serial.println(amount);
  digitalWrite(pumpDir, HIGH);

  for (float x = 0; x < amount; x++) {
  int currentSpeed = maxPumpSpeed;

  if(x < caliAmount){
    currentSpeed = slowPumpSpeed;
  }
   //Serial.println("yes");
    digitalWrite(pumpSteps[pumpNumber], HIGH);
    delayMicroseconds(currentSpeed); //skift måske denne delay.
    digitalWrite(pumpSteps[pumpNumber], LOW);
    delayMicroseconds(currentSpeed);
  }
Serial.println("rolling back");
  //roll back whats in the tube
   digitalWrite(pumpDir, LOW);

    for (float x = 0; x < caliAmount; x++) {


    digitalWrite(pumpSteps[pumpNumber], HIGH);
    delayMicroseconds(slowPumpSpeed); //skift måske denne delay.
    digitalWrite(pumpSteps[pumpNumber], LOW);
    delayMicroseconds(slowPumpSpeed);
  }

  digitalWrite(pumpEnable[pumpNumber], HIGH);
  } else{
      int airCaliAmount = airPumpMsPrMl[pumpNumber];
      int waitMs = airCaliAmount*amount+airPumpDelays[pumpNumber];
      int pumpPin = pumpSteps[pumpNumber];
    Serial.println(waitMs);
     digitalWrite(pumpPin, LOW);
     delay(waitMs);
     digitalWrite(pumpPin, HIGH);
     delay(3000);
  }

}



void lightCommand(int cmd){

  if(cmd != currentLight){
    Serial.println("sender data");
  UNO.write(cmd);
  currentLight = cmd;
  }
}
bool move(int dir, float pulses, bool force) { //returns if the operation has been stopped. that is false. returns success

  pulses = pulses * 3.935; //this is done so that it can be measured in millimeters.
  pulses = pulses * 16;

  
   
  int startDelay = slowestDelay;

  int startDecSpeed = 500;
  int stopDecSpeed = 100;

  double slowSpeed = 400;
  double slowestSpeed = 100;

  bool slowestSpeedSet = false;
  bool slowSpeedSet = false;

  bool endSlowestSpeedSet = false;
  bool endSlowSpeed = false;
  bool normalSpeedSet = false;
  if (dir ==  0) {
    digitalWrite(dirPin, HIGH);
  } else if (dir == 1) {
    digitalWrite(dirPin, LOW);

  }
  startDelay = startDecSpeed;
  for (long x = 0; x < pulses; x++) {

    if (!force && isEndSwitch()) {
      return false;
      break;
    }



    if (x < 500 && startDelay > stopDecSpeed) {

      startDelay = startDelay - 1;
      //slowestSpeedSet = true;
    }



    if (x > 500 && !normalSpeedSet) {
      startDelay = fastestDelay;
      normalSpeedSet = true;
    }

    if ( pulses - x < 500 ) {



      startDelay = startDelay + 1;
    }



    digitalWrite(stepPin, HIGH);
    delayMicroseconds(startDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(startDelay);


  }


  if (dir == 1) {
    currentLength += pulses;
  } else {
    currentLength -= pulses;

  }
  return true;
}

bool isEndSwitch() {
  int buttonRead = digitalRead(endSwitch) ; //0 is pressed

  if (buttonRead == 0) {
    printToLcd(3, "reset state: pressed", true);
  } else {

  }




  if (buttonRead == 0) {

    digitalWrite(stepPin, LOW);
    delay(500);


    move(left, 10, true);
    currentLength = 10 * 3.935 * 16;
    return true;
  } else {

    return false;
  }
}

bool moveToPos(double pos) {

  pos = pos+11; //CALIBRATION

  if (pos < currentLength / (3.935 * 16)) {

    float wantedLength = (currentLength / (3.935 * 16)) - pos;
    move(0, wantedLength, false);
  } else if (pos > currentLength / (3.935 * 16)) {
    float wantedLength = pos - (currentLength / (3.935 * 16));

    move(1, wantedLength, false);
  }
}


void printToLcd(int line, String message, bool shouldClear) {

  if (shouldClear)
    lcd.clear();
  lcd.setCursor(0, line);
  lcd.print(message);

}


int getDistance() {
  long duration;
  int distance;


  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  return distance;

}






Command getBluetoothCmd() { // see the app for how this is done, but it just reads bluetooth all the time
  //this call blocks the entire chain. Do something about it if it becomes a problem.
delay(200);

  char charArr[512];
  String currentMessage = "";
  
  //Command cmd;
  //BottleAmount bas[20]; //maximum of 20 bottleamounts;
  int currentBAIndex = 0;
  bool hasStarted = false;
  bool endReached = false;

  while(!endReached){
    
  while (  Serial3.available() ) {
    char inChar = (char)Serial3.read(); //read the input
   
    
      if(inChar == '$'){

        if(!hasStarted){
        hasStarted = true;
        } else{
          endReached = true;
        }
      }
    
    currentMessage += inChar;
   // Serial.println(currentMessage);
    //Serial.println(inChar == ';');

  }
  
  }
if(currentMessage !=""){
  currentMessage += "&";
strcpy(charArr, currentMessage.c_str());
Serial.println(currentMessage + "yes");
Command cmd = handleCharArr(charArr);
return cmd;
} else{
  Serial.println(currentMessage + "yes");
  Command cmd;
  
  return cmd;
}
 
}

Command handleCharArr(char charArr[512]){
Serial.println("handler");
Command cmd;
 BottleAmount bas[20]; //maximum of 20 bottleamounts;
  int currentBAIndex = 0;

  String currentMessage = "";
  
int sizes = 512;
  for(int i = 0; i < sizes; i++){
  if(charArr[i] == '&'){
    break;
  }
    char inChar = charArr[i];
    //Serial.println(inChar);
    currentMessage.concat(inChar);
    if (inChar == ';') {
      
      if (currentMessage.indexOf("cmd:") >= 0) { //if the command is the commandname
        currentMessage.replace("$cmd:", "");
        currentMessage.replace(";", "");
        cmd.commandName = currentMessage;

      }
     // Serial.println(currentMessage);
      if (currentMessage.indexOf("btl:") >= 0) { //if the command is the commandname
        
        //Serial.println("bottle");
        currentMessage.replace("btl:", "");
        currentMessage.replace(";", "");
        Serial.println(currentMessage);
        int commaIndex = currentMessage.indexOf(',');


        //ONLY CURRENTLY HOLDS 2 VALUES, BOTTLENUMBER AND BOTTLEABOUMT.
        String firstValue = currentMessage.substring(0, commaIndex);
        //Serial.println(firstValue);
        String secondValue = currentMessage.substring(commaIndex + 1);

        BottleAmount ba;
        ba.bottleNum = firstValue.toInt();
        
        ba.amount = secondValue.toInt();

        cmd.bottleAmounts[currentBAIndex] = ba;
        currentBAIndex++;
        cmd.bas = currentBAIndex;

      }
      //do something with the current data.
      currentMessage = "";
    }
  }



  return cmd;
}
