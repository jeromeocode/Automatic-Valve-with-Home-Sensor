//Jerome Villapando

//Valve used is classified due to NDA
//Stepper https://www.pololu.com/product/1478
//Big Easy Driver (https://www.sparkfun.com/products/12859)

//Declare pin functions on Arduino

//motor driver pins
#define stp 2
#define dir 3
#define MS1 4
#define MS2 5
#define MS3 6
#define EN  7

//optical switch sensor pins
#define negV 8
#define posV 9

//Declare variables for functions
char user_input;
int sens = 0;
int sens2 = 0;

//the motor is 200 steps/revolution, and since we are running at microstepping 1/16, we need to send signals for 16*200 steps for one revolution
int spinAmnt = 16*200;
//you can choose the delay between movements
int delayAmnt = 100;

void setup() {
  pinMode(negV, INPUT);
  pinMode(posV,INPUT);
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  pinMode(EN, OUTPUT);

  resetBEDPins(); //Set step, direction, microstep and enable pins to default states
  Serial.begin(9600); //Open Serial connection for debugging
  
}

//Main loop
void loop() {
  while(Serial.available()){
      Serial.println("Press 0-6 for a position");
      
      user_input = Serial.read(); //Read user input and trigger appropriate function
      
      digitalWrite(EN, LOW); //Pull enable pin low to set FETs active and allow motor control

      //before any movement to any position, the valve needs to be positioned at the origin
      //the movements are split into 60 degree increments 200*16*(60/360) = 533.333
      
      if (user_input == '1'){
        homePos();
        delay(delayAmnt);
        SmallStepMode(0, 533, 0);
      }
      else if (user_input == '2'){
        homePos();
        delay(delayAmnt);
        SmallStepMode(0, 533, 0);
        delay(delayAmnt);
        SmallStepMode(0, 533, 0);
      }
      else if(user_input =='3')
      {
        homePos();
        delay(delayAmnt);
        SmallStepMode(0, 533, 0);
        delay(delayAmnt);
        SmallStepMode(0, 533, 0);
        delay(delayAmnt);
        SmallStepMode(0, 533, 0);
      }
      else if (user_input == '5'){
        homePos();
        delay(delayAmnt);
        SmallStepMode(1, 533, 0);
      }
      else if(user_input =='4')
      {
        homePos();
        delay(delayAmnt);
        SmallStepMode(1, 533, 0);
        delay(delayAmnt);
        SmallStepMode(1, 533, 0);
      }
      else if(user_input == '6' || user_input == '0'){
        homePos();
      }
      else
      {
        Serial.println("Invalid option entered.");
      }
      
      resetBEDPins();
  }
}

//Reset Big Easy Driver pins to default states
void resetBEDPins()
{
  digitalWrite(stp, LOW);
  digitalWrite(dir, LOW);
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(MS3, LOW);
  digitalWrite(EN, HIGH);
}

void SmallStepMode(bool rotDir, int stepAmnt, bool homeYes)
{
  //these are essentialy logic storage
  sens = 0;
  sens2 = 0;

  Serial.println("Moving");
  
  digitalWrite(dir, rotDir); //Pull direction pin low to move "forward"
  
  digitalWrite(MS1, HIGH); //Pull MS1,MS2, and MS3 high to set logic to 1/16th microstep resolution
  digitalWrite(MS2, HIGH);
  digitalWrite(MS3, HIGH);
  
  for(int x = 0; x<stepAmnt; x++)  //Loop the forward stepping enough times for motion to be visible
  {
    //the readout difference is 1 when the optical sensor is interrupted by the tab
    sens2 = digitalRead(posV)-digitalRead(negV);

    //the motor stops when there are two consecutive highs to counterract stopping at any random fluctuations that were occuring time to time
    if(sens2 == 1 && sens ==1 && homeYes){
      x=stepAmnt + 1;
      Serial.println("break");
    }
    
    sens = sens2;
    
    digitalWrite(stp,HIGH); //Trigger one step forward
    delay(1);
    digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
    delay(1);
  }
  Serial.println("Enter new option");
  Serial.println();
}

//this could be a redundant function
void homePos(){
  SmallStepMode(1, spinAmnt, 1);
}



