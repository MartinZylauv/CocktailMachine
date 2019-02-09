/*     Left and right example code Cocktail Machine

    by Martin Zylauv Kristensen //ByZylauv www.zylauv,dk

*/




const int endSwitch = 13; //endswitch digital pin
const int stepPin = 11; //step pin for sleigh
const int dirPin = 12; //dir for sleigh






//defines left and right for the dir in the sleigh, switch these if not correct.
int right = 0;
int left = 1;

//defines the speeds, tweak these if the stepper skips steps or is too slow/fast.
int fastestDelay = 35; //minimum 750 in speed
int slowestDelay = 10000;










void setup() {




  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(endSwitch, INPUT_PULLUP);
  Serial.begin(9600);

  //for testing....
  bool isRight = false;
  
}
void loop() {



//calls the move command back and forth 100 mm
  if(!isRight){
    move(left,100);
    isRight = true;
  } else{
    move(right,100);
    isRight = false;
  }



}


bool move(int dir, float pulses, bool force) { //returns if the operation has been stopped. that is false. returns success

  pulses = pulses * 3.935; //this is done so that it can be measured in millimeters. You can calibrate this by calling this function with "100" in pulses. This should result in a movement in 10 cm the way specified. multiply or divide based on the deviance and try again!
  pulses = pulses * 16; //If you are using microstepping you need this. If not, then remove this.

  
   
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

    //if endswitch is touched, then reset and stop all.

    if (!force && isEndSwitch()) {
      return false;
      break;
    }


//acceleration + decalleration
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

//holds the currentLength in a global variable.
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

    digitalWrite(stepPin, LOW);
    delay(500);


    move(left, 10, true);
    currentLength = 10 * 3.935 * 16;
    return true;
  } else {

    return false;
  }
}
