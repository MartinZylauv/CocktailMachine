/*     Air Pump testing with pos

    by Martin Zylauv

*/








int maximumLength =  1000;
float currentLength = 0;

const int endSwitch = 13;
const int stepPin = 11; //slæde step
const int dirPin = 12; //slæde dir

const int pumpDir = 7;

int pumpSteps[] = {30, 32, 34, 36, 38, 40, 42, 44, 46, 48,2,3,4,5};
int pumpEnable[] = {31, 33, 35, 37, 39, 41, 43, 45, 47, 49};


double bottlesPos[] = {1000,940,880,820,760,700,640,580,520,460,400,340,280,220}; //these are in order from left to right




int right = 0;
int left = 1;

int fastestDelay = 35; //minimum 750 i fart
int slowestDelay = 10000;

int airPumps[] = {2,3,4,5}; //the pins for the airpumps relays.

int airPumpDelays[13]; //init the array which holds the airpumps delays. Make this larger or less compared to the number of bottles you totally have.


double airPumpMsPrMl[13];  //same as above, but inits the array which holds the calibrated amount

bool tested = false;





void setup() {

//measure out how long it takes for the pump to start, to the liquid to start pouring for each pump. Then enter the time in miliseconds here, for each pump.
airPumpDelays[10] = 1000;
airPumpDelays[11] = 1000;
airPumpDelays[12] = 1000;
airPumpDelays[13] = 1000;


//measure out how many miliseconds it takes to pour out 1 ml. then enter the amount under here. This can be done by pouring out some water and measuring it.
airPumpMsPrMl[10] = 50;
airPumpMsPrMl[11] = 50;
airPumpMsPrMl[12] = 40;
airPumpMsPrMl[13] = 50;




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


  move(right, 1100, false); //resets machine



  
}
void loop() {





//tests the air pump which is the 12. bottle.
  if(!tested){
    int pos = bottlesPos[12]; //
    moveToPos(pos);
    pump(20,12);
    tested = true;
  }
 



 


}


bool pump(float amount, int pumpNumber) {
  //const int pumpDir = 5;
  //const pump1Step = 31;
  //const pumpt1Enable = 32;

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
  } else{ //for the air pumps.
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


  //Serial.println("er hernedde");
  if (buttonRead == 0) {
    printToLcd(3, "reset state: pressed", true);
  } else {

    // printToLcd(3, "reset state: notPressed", true);
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
  //right er 0
  //Serial.println("flytter sig");
  //Serial.println(currentLength);
  if (pos < currentLength / (3.935 * 16)) {
    //Serial.println("moving right");

    float wantedLength = (currentLength / (3.935 * 16)) - pos;
    //Serial.println(wantedLength);
    move(0, wantedLength, false);
    //printToLcd(0,"Moving right "+String(wantedLength),true);
  } else if (pos > currentLength / (3.935 * 16)) {
    float wantedLength = pos - (currentLength / (3.935 * 16));

    //Serial.println(wantedLength);
    //printToLcd(0,"Moving left "+String(wantedLength),true);
    move(1, wantedLength, false);
  }
}
