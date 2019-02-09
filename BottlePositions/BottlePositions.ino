/*     Bottle Positions Example

    by Martin Zylauv Kristensen

*/







// defines pins numbers
//4 er 1.
int maximumLength =  1000; //the max length of our machine
float currentLength = 0; //float to keep the currentlength, so we can set a certain position in the code.

const int endSwitch = 13;
const int stepPin = 11; //sleigh step
const int dirPin = 12; //sleigh dir

const int pumpDir = 7;

int pumpSteps[] = {30, 32, 34, 36, 38, 40, 42, 44, 46, 48,2,3,4,5}; //the step pins for the pumps
int pumpEnable[] = {31, 33, 35, 37, 39, 41, 43, 45, 47, 49}; //the enable pins for the pumps


double bottlesPos[] = {1000,940,880,820,760,700,640,580,520,460,400,340,280,220}; //The position where each bottle will be. bottle 1 is the first, and so on. These are the distance from the right start position of the sleigh, measured in milimeters.
bool pumpTested = false;






int right = 0;
int left = 1;


int fastestDelay = 35; //minimum 750 in speed
int slowestDelay = 10000;









void setup() {



 
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




  Serial.begin(9600);




  move(right, 1100, false); //resets machine



  
}
void loop() {


//do this test 1 time.
if(!pumpTested){
  int desiredBottle = 1; //set this to the desired bottle, based on the entered values below
 int pos = bottlesPos[desiredBottle]; 
    moveToPos(pos); //move to the position
    pump(20,desiredBottle); //pumps 20 ml at the desired bottle
  pumpTested = true;
}

/*
 * Basically, this test just moves the pump to a pos and pumps. This is actually the main heart of the main code which is later on in the examples. BE sure to include the  right calibration values based on the former examples.
 * If you have any questions, feel free to comment on my youtube video/facebook page! I'm always ready to help!
 * 
 */





  


}


bool pump(float amount, int pumpNumber) {
  //const int pumpDir = 5;
  //const pump1Step = 31;
  //const pumpt1Enable = 32;

  //just doing the first for a second
  float caliAmount = 4706;
  int maxPumpSpeed = 50;
  int slowPumpSpeed = 200;
  
  
  
 
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


//this is my method for moving the sleigh easier. Just enter a desired position (eg 400mm which is a certain pump position, and it will do it for you.)
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














void testPumpAndPos(){
    int pos = bottlesPos[8];
    moveToPos(pos);
    pump(20,9);
  
}
