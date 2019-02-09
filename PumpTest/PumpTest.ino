/*     Pump test

    By Zylauv

*/




const int pumpDir = 7; //all pumps have a shared dir.

int pumpSteps[] = {30, 32, 34, 36, 38, 40, 42, 44, 46, 48,2,3,4,5}; //defined the pins that the pump steps are in here, with pump #1 first and so on
int pumpEnable[] = {31, 33, 35, 37, 39, 41, 43, 45, 47, 49}; //same here, just the enable pins, as the will use full power if we dont disable them between pumps













void setup() {




    
 //init the pumps
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





  
}
void loop() {





int bottleNum = 1;
 pump(20, bottleNum-1); 
 delay(2000);


//Serial.println((bool)testing);

//testPumpAndPos();

  




}


bool pump(float amount, int pumpNumber) {
 /*
  * HOW TO CALIBRATE THE PUMPS:
  * Call this pump function with 20 in amount, and pumpnumber as the #pump
  * measure how many ml comes out
  * adjust "caliAmount" up or down based on that. A small error margin is fine. The one that is currently set, works fine for the setup in my video.
  * 
  */
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
    digitalWrite(pumpSteps[pumpNumber], HIGH);
    delayMicroseconds(currentSpeed); //skift måske denne delay.
    digitalWrite(pumpSteps[pumpNumber], LOW);
    delayMicroseconds(currentSpeed);
  }

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
