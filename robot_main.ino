#include <AccelStepper.h>

const uint8_t FR_STEP_PIN = 39;
const uint8_t FR_DIR_PIN = 41;

const uint8_t FL_STEP_PIN = 47;
const uint8_t FL_DIR_PIN = 49;

const uint8_t BR_STEP_PIN = 43;
const uint8_t BR_DIR_PIN = 48;

const uint8_t BL_STEP_PIN = 53;
const uint8_t BL_DIR_PIN = 51;

AccelStepper stepperFR(AccelStepper::DRIVER, FR_STEP_PIN, FR_DIR_PIN);
AccelStepper stepperBL(AccelStepper::DRIVER, BL_STEP_PIN, BL_DIR_PIN);
AccelStepper stepperFL(AccelStepper::DRIVER, FL_STEP_PIN, FL_DIR_PIN);
AccelStepper stepperBR(AccelStepper::DRIVER, BR_STEP_PIN, BR_DIR_PIN);

const float STEP_SPEED = 600.0f;
const float STEP_ACCEL = 8000.0f;
const long STEP_MOVE_STEPS = 50;

int motor_right1 = 8;
int motor_right2 = 9;
int motor_left1 = 5;
int motor_left2 = 4;

int button_top = 28;
int button_btm = 26;

int motor_podnytie1 = 36;
int motor_podnytie2 = 38;

int y = 0;
int x = 0;
int r = 0;
int l = 0;

int motor_sensor1 = 31;
int motor_sensor2 = 33;
int sensor_niz = 22;
int sensor_verh = 24;

int v = 0;
int time1;
int time2;
int q = 0;
int s = 0;
int z = 0;

int btn_byr1 = 30;
int btn_byr2 = 32;
int motor_byr1 = 35;
int motor_byr2 = 37;
int byr_podnytie1 = 40;
int byr_podnytie2 = 42;

int vibra1 = 6;
int vibra2 = 7;

int cam_podnytie1 = 2;
int cam_podnytie2 = 3;
int cam_povorot1 = 44;
int cam_povorot2 = 46;
int cam_naklon1 = 27;
int cam_naklon2 = 29;
int e = 0;
int o = 0;

#define CH1 10
#define CH2 11
#define CH3 12
#define CH4 13
#define CH5 50
#define CH6 45
int ch1Value; int ch2Value; int ch3Value; int ch4Value; bool ch5Value; bool ch6Value;



enum CycleState : uint8_t {
  CYCLE_IDLE,                      
  CYCLE_DOWN_BUTTON,        
  CYCLE_DOWN_SENSOR,        
  CYCLE_VIBRA_ON,           
  CYCLE_VIBRA_WAIT,         
  CYCLE_UP_BUTTON,          
  CYCLE_UP_BYR,     
  CYCLE_BUR_DOWNSPIN, 
  CYCLE_BUR_SPINFINISH,     
  CYCLE_UP_SENSOR           
};

enum AutoUpState : uint8_t {
  AUTO_UP_SENSOR,
  AUTO_UP_BYR,    
  AUTO_UP_BUTTON
};

CycleState cycleState = CYCLE_IDLE;
AutoUpState autoUpState = AUTO_UP_SENSOR;
unsigned long stateStartMs = 0;


const unsigned long VIBRA_ON_MS = 1500;
const unsigned long POST_VIBRA_DELAY_MS = 4000;
const unsigned long BYR_SPIN_MS = 2500;


int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue) {
  int ch = pulseIn(channelInput, HIGH, 30000);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

bool readSwitch(byte channelInput, bool defaultValue) {
  int intDefaultValue = (defaultValue) ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}

static inline int ch1StateFromValue(int value) {
  if (value > 50) return 1;
  if (value < -50) return -1;
  return 0;
}

static inline void runAllSteppers() {
  stepperFR.run();
  stepperFL.run();
  stepperBL.run();
  stepperBR.run();
}

static inline bool anySteppersMoving() {
  return (stepperFR.distanceToGo() != 0 ||
          stepperFL.distanceToGo() != 0 ||
          stepperBL.distanceToGo() != 0 ||
          stepperBR.distanceToGo() != 0);
}

static void moveSteppersBlocking(long frDelta, long flDelta, long blDelta, long brDelta) {
  stepperFR.move(frDelta);
  stepperFL.move(flDelta);
  stepperBL.move(blDelta);
  stepperBR.move(brDelta);
  while (anySteppersMoving()) {
    runAllSteppers();
  }
}



void setup() {
  Serial.begin(115200);
  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);
  pinMode(CH5, INPUT);
  pinMode(CH6, INPUT);
  pinMode(motor_right1, OUTPUT);
  pinMode(motor_right2, OUTPUT);
  pinMode(motor_left1, OUTPUT);
  pinMode(motor_left2, OUTPUT);

  pinMode(cam_podnytie1, OUTPUT);
  pinMode(cam_podnytie2, OUTPUT);
  pinMode(cam_povorot1, OUTPUT);
  pinMode(cam_povorot2, OUTPUT);
  pinMode(cam_naklon1, OUTPUT);
  pinMode(cam_naklon2, OUTPUT);

  pinMode(button_top, INPUT);
  digitalWrite(button_top, HIGH);
  pinMode(button_btm, INPUT);
  digitalWrite(button_btm, HIGH);

  pinMode(sensor_niz, INPUT);
  digitalWrite(sensor_niz, HIGH);
  pinMode(sensor_verh, INPUT);
  digitalWrite(sensor_verh, HIGH);
  pinMode(motor_sensor1, OUTPUT);
  pinMode(motor_sensor2, OUTPUT);

  pinMode(vibra1, OUTPUT);
  pinMode(vibra2, OUTPUT);

  pinMode(btn_byr1, INPUT);
  digitalWrite(btn_byr1, HIGH);
  pinMode(btn_byr2, INPUT);
  digitalWrite(btn_byr2, HIGH);
  pinMode(byr_podnytie1, OUTPUT);
  pinMode(byr_podnytie2, OUTPUT);
  pinMode(motor_byr1, OUTPUT);
  pinMode(motor_byr2, OUTPUT);

  stepperFR.setMaxSpeed(STEP_SPEED);
  stepperFR.setAcceleration(STEP_ACCEL);
  stepperFL.setMaxSpeed(STEP_SPEED);
  stepperFL.setAcceleration(STEP_ACCEL);
  stepperBL.setMaxSpeed(STEP_SPEED);
  stepperBL.setAcceleration(STEP_ACCEL);
  stepperBR.setMaxSpeed(STEP_SPEED);
  stepperBR.setAcceleration(STEP_ACCEL);
}



void loop() {
  unsigned long now = millis();

  ch1Value = readChannel(CH1, -100, 100, 0);
  ch2Value = readChannel(CH2, -100, 100, 0);
  ch5Value = readSwitch(CH5, false);
  ch6Value = readSwitch(CH6, false);
  ch3Value = readChannel(CH3, -100, 100, 0);
  ch4Value = readChannel(CH4, -100, 100, 0);


  if (ch3Value > 20) {

    analogWrite(cam_podnytie1, 0);
    analogWrite(cam_podnytie2, 250);
  } else if (ch3Value < -20) {
    
    analogWrite(cam_podnytie1, 250);
    analogWrite(cam_podnytie2, 0);
  } else {
    analogWrite(cam_podnytie1, 0);
    analogWrite(cam_podnytie2, 0);
  }


  if (ch4Value > 20) {
   
    analogWrite(cam_povorot1, 0);
    analogWrite(cam_povorot2, 100);
  } else if (ch4Value < -20) {
    
    analogWrite(cam_povorot1, 100);
    analogWrite(cam_povorot2, 0);
  } else {
    analogWrite(cam_povorot1, 0);
    analogWrite(cam_povorot2, 0);
  }


  if (ch2Value > 20) {
    y = map(ch2Value, 20, 100, 50, 255);
    analogWrite(motor_right1, 0);
    analogWrite(motor_right2, y);
    analogWrite(motor_left1, 0);
    analogWrite(motor_left2, y);
  } else if (ch2Value < -20) {
    y = map(ch2Value, -20, -100, 50, 255);
    analogWrite(motor_right1, y);
    analogWrite(motor_right2, 0);
    analogWrite(motor_left1, y);
    analogWrite(motor_left2, 0);
  } else {
    analogWrite(motor_right1, 0);
    analogWrite(motor_right2, 0);
    analogWrite(motor_left1, 0);
    analogWrite(motor_left2, 0);
  }


  int ch1State = ch1StateFromValue(ch1Value);
  if (ch1State != 0) {
    long frDelta = 0, flDelta = 0, blDelta = 0, brDelta = 0;

    if (ch6Value == 0) {
      if (ch1State > 0) {
        frDelta = -STEP_MOVE_STEPS; flDelta = -STEP_MOVE_STEPS;
        blDelta = -STEP_MOVE_STEPS; brDelta = -STEP_MOVE_STEPS;
      } else {
        frDelta = STEP_MOVE_STEPS; flDelta = STEP_MOVE_STEPS;
        blDelta = STEP_MOVE_STEPS; brDelta = STEP_MOVE_STEPS;
      }
    } else {
      if (ch1State > 0) {
        frDelta = STEP_MOVE_STEPS;  flDelta = STEP_MOVE_STEPS;
        blDelta = -STEP_MOVE_STEPS; brDelta = -STEP_MOVE_STEPS;
      } else {
        frDelta = -STEP_MOVE_STEPS; flDelta = -STEP_MOVE_STEPS;
        blDelta = STEP_MOVE_STEPS;  brDelta = STEP_MOVE_STEPS;
      }
    }

    moveSteppersBlocking(frDelta, flDelta, blDelta, brDelta);
  }

  bool cycleRequested = (ch5Value == 1);

  if (cycleState != CYCLE_IDLE || cycleRequested) {
    if (cycleState == CYCLE_IDLE) {
      cycleState = CYCLE_BUR_DOWNSPIN;
    }

    switch (cycleState) {

      case CYCLE_BUR_DOWNSPIN:
        analogWrite(motor_podnytie1, 0);
        analogWrite(motor_podnytie2, 0);
        analogWrite(motor_sensor1, 0);
        analogWrite(motor_sensor2, 0);
        analogWrite(vibra1, 0);
        analogWrite(vibra2, 0);
        analogWrite(motor_byr1, 250);
        analogWrite(motor_byr2, 0);

        if (digitalRead(btn_byr2) == 0) {
          
          analogWrite(byr_podnytie2, 0);
          analogWrite(byr_podnytie1, 200);
          
        } else {
          
          analogWrite(byr_podnytie1, 0);
          analogWrite(byr_podnytie2, 0);

          if (now - stateStartMs < BYR_SPIN_MS) {
            cycleState = CYCLE_BUR_SPINFINISH;
            
          } else {
            
            analogWrite(motor_byr1, 0);
            analogWrite(motor_byr2, 0);
            cycleState = CYCLE_DOWN_BUTTON;
          
          }
        }
        break;


      case CYCLE_BUR_SPINFINISH:
        analogWrite(byr_podnytie1, 0);
        analogWrite(byr_podnytie2, 0);
        analogWrite(motor_byr1, 250);
        analogWrite(motor_byr2, 0);
        if (now - stateStartMs >= BYR_SPIN_MS) {
          analogWrite(motor_byr1, 0);
          analogWrite(motor_byr2, 0);
          cycleState = CYCLE_DOWN_BUTTON;
        }
        break;

        


   
      case CYCLE_DOWN_BUTTON:
        if (digitalRead(button_btm) == 0) {
          analogWrite(motor_podnytie1, 0);
          analogWrite(motor_podnytie2, 240);
        } else {
          analogWrite(motor_podnytie1, 0);
          analogWrite(motor_podnytie2, 0);
          cycleState = CYCLE_DOWN_SENSOR;
        }
        analogWrite(motor_sensor1, 0);
        analogWrite(motor_sensor2, 0);
        analogWrite(vibra1, 0);
        analogWrite(vibra2, 0);
        analogWrite(motor_byr1, 0);
        analogWrite(motor_byr2, 0);
        analogWrite(byr_podnytie1, 0);
        analogWrite(byr_podnytie2, 0);
        break;

      
      case CYCLE_DOWN_SENSOR:
        analogWrite(motor_podnytie1, 0);
        analogWrite(motor_podnytie2, 0);
        if (digitalRead(sensor_niz) == 0) {
          analogWrite(motor_sensor1, 150);
          analogWrite(motor_sensor2, 0);
        } else {
          analogWrite(motor_sensor1, 0);
          analogWrite(motor_sensor2, 0);
          cycleState = CYCLE_VIBRA_ON;
          stateStartMs = now;
        }
        analogWrite(vibra1, 0);
        analogWrite(vibra2, 0);
        analogWrite(motor_byr1, 0);
        analogWrite(motor_byr2, 0);
        analogWrite(byr_podnytie1, 0);
        analogWrite(byr_podnytie2, 0);
        break;

     
      case CYCLE_VIBRA_ON:
        analogWrite(motor_podnytie1, 0);
        analogWrite(motor_podnytie2, 0);
        analogWrite(motor_sensor1, 0);
        analogWrite(motor_sensor2, 0);
        analogWrite(vibra1, 100);
        analogWrite(vibra2, 0);
        analogWrite(motor_byr1, 0);
        analogWrite(motor_byr2, 0);
        analogWrite(byr_podnytie1, 0);
        analogWrite(byr_podnytie2, 0);
        if (now - stateStartMs >= VIBRA_ON_MS) {
          analogWrite(vibra1, 0);
          analogWrite(vibra2, 0);
          cycleState = CYCLE_VIBRA_WAIT;
          stateStartMs = now;
        }
        break;

      case CYCLE_VIBRA_WAIT:
        analogWrite(motor_podnytie1, 0);
        analogWrite(motor_podnytie2, 0);
        analogWrite(motor_sensor1, 0);
        analogWrite(motor_sensor2, 0);
        analogWrite(vibra1, 0);
        analogWrite(vibra2, 0);
        analogWrite(motor_byr1, 0);
        analogWrite(motor_byr2, 0);
        analogWrite(byr_podnytie1, 0);
        analogWrite(byr_podnytie2, 0);
        if (now - stateStartMs >= POST_VIBRA_DELAY_MS) {
          cycleState = CYCLE_UP_BUTTON;
        }
        break;

      
      case CYCLE_UP_BUTTON:
        analogWrite(motor_sensor2, 0);
        analogWrite(motor_sensor1, 0);
        analogWrite(vibra1, 0);
        analogWrite(vibra2, 0);
        analogWrite(motor_byr1, 0);
        analogWrite(motor_byr2, 0);
        analogWrite(byr_podnytie1, 0);
        analogWrite(byr_podnytie2, 0);
        if (digitalRead(button_top) == 0) {
          analogWrite(motor_podnytie1, 240);
          analogWrite(motor_podnytie2, 0);
        } else {
          analogWrite(motor_podnytie1, 0);
          analogWrite(motor_podnytie2, 0);
          cycleState = CYCLE_UP_BYR; 
        }
        break;

      
      case CYCLE_UP_BYR:
        analogWrite(motor_podnytie1, 0);
        analogWrite(motor_podnytie2, 0);
        analogWrite(motor_sensor1, 0);
        analogWrite(motor_sensor2, 0);
        analogWrite(vibra1, 0);
        analogWrite(vibra2, 0);
        analogWrite(motor_byr1, 0);
        analogWrite(motor_byr2, 0);
        if (digitalRead(btn_byr1) == 0) {
          analogWrite(byr_podnytie2, 200);
          analogWrite(byr_podnytie1, 0);
         
        } else {
          analogWrite(byr_podnytie1, 0);
          analogWrite(byr_podnytie2, 0);
          cycleState = CYCLE_UP_SENSOR;
          
        }
        break;

    ///

     
      case CYCLE_UP_SENSOR:
        analogWrite(motor_podnytie1, 0);
        analogWrite(motor_podnytie2, 0);
        analogWrite(byr_podnytie1, 0);
        analogWrite(byr_podnytie2, 0);
        analogWrite(vibra1, 0);
        analogWrite(vibra2, 0);
        analogWrite(motor_byr1, 0);
        analogWrite(motor_byr2, 0);
        if (digitalRead(sensor_verh) == 0) {
          analogWrite(motor_sensor2, 200);
          analogWrite(motor_sensor1, 0);
        } else {
          analogWrite(motor_sensor2, 0);
          analogWrite(motor_sensor1, 0);
          cycleState = CYCLE_IDLE;  
        }
        break;

      case CYCLE_IDLE:
      default:
        break;
    }

  } else {
    switch (autoUpState) {
      case AUTO_UP_SENSOR:
        if (digitalRead(sensor_verh) == 0) {
          analogWrite(motor_sensor2, 200);
          analogWrite(motor_sensor1, 0);
        } else {
          analogWrite(motor_sensor2, 0);
          analogWrite(motor_sensor1, 0);
          autoUpState = AUTO_UP_BYR;
        }
        analogWrite(motor_podnytie1, 0);
        analogWrite(motor_podnytie2, 0);
        analogWrite(byr_podnytie1, 0);
        analogWrite(byr_podnytie2, 0);
        break;

      case AUTO_UP_BYR:
        if (digitalRead(btn_byr1) == 0) {
          analogWrite(byr_podnytie2, 200);
          analogWrite(byr_podnytie1, 0);
        } else {
          analogWrite(byr_podnytie1, 0);
          analogWrite(byr_podnytie2, 0);
          autoUpState = AUTO_UP_BUTTON;
        }
        analogWrite(motor_sensor1, 0);
        analogWrite(motor_sensor2, 0);
        analogWrite(motor_podnytie1, 0);
        analogWrite(motor_podnytie2, 0);
        break;

      case AUTO_UP_BUTTON:
        if (digitalRead(button_top) == 0) {
          analogWrite(motor_podnytie1, 240);
          analogWrite(motor_podnytie2, 0);
        } else {
          analogWrite(motor_podnytie1, 0);
          analogWrite(motor_podnytie2, 0);
          autoUpState = AUTO_UP_SENSOR;
        }
        analogWrite(motor_sensor2, 0);
        analogWrite(motor_sensor1, 0);
        analogWrite(byr_podnytie1, 0);
        analogWrite(byr_podnytie2, 0);
        break;

      default:
        break;
    }
    analogWrite(vibra1, 0);
    analogWrite(vibra2, 0);
  }
}
