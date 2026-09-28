
const int RED_PIN = 13;      
const int YELLOW_PIN = 12;    
const int GREEN_PIN = 11;     

const int BTN_PED_PIN = 2;    
const int BTN_EMERG_PIN = 3;  


enum TrafficState {
  STATE_GREEN,   
  STATE_YELLOW,  
  STATE_RED,     
  STATE_WARNING  
};


TrafficState currentState = STATE_GREEN;
unsigned long stateTimer = 0;      
unsigned long stateDuration = 0;   

bool pedestrianRequested = false;  
bool isEmergencyMode = false;      


const unsigned long DEBOUNCE_MS = 50; 

bool lastPedState = LOW;
bool pedBtnPressed = false;
unsigned long lastPedDebounce = 0;

bool lastEmergState = LOW;
bool emergBtnPressed = false;
unsigned long lastEmergDebounce = 0;


unsigned long blinkTimer = 0;
bool yellowBlinkState = LOW;


void changeState(TrafficState newState, unsigned long duration, const char* reason) {
  currentState = newState;
  stateTimer = millis();
  stateDuration = duration;

  
  digitalWrite(RED_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);

  
  switch (newState) {
    case STATE_GREEN:
      digitalWrite(GREEN_PIN, HIGH);
      break;
    case STATE_YELLOW:
      digitalWrite(YELLOW_PIN, HIGH);
      break;
    case STATE_RED:
      digitalWrite(RED_PIN, HIGH);
      break;
    case STATE_WARNING:
      
      break;
  }

  
  Serial.print(F("[FSM] Time: "));
  Serial.print(stateTimer);
  Serial.print(F(" ms | State: "));
  
  if (newState == STATE_GREEN) Serial.print(F("GREEN"));
  else if (newState == STATE_YELLOW) Serial.print(F("YELLOW"));
  else if (newState == STATE_RED) Serial.print(F("RED"));
  else if (newState == STATE_WARNING) Serial.print(F("WARNING"));

  Serial.print(F(" | Reason: "));
  Serial.println(reason);
}


void processInputs() {
  unsigned long now = millis();

  
  bool pedReading = digitalRead(BTN_PED_PIN);
  if (pedReading != lastPedState) {
    lastPedDebounce = now;
    lastPedState = pedReading;
  }
  
  if ((now - lastPedDebounce) > DEBOUNCE_MS) {
    if (pedReading == HIGH && !pedBtnPressed) {
      pedBtnPressed = true;
      pedestrianRequested = true; 
      
      
      Serial.print(F("[INPUT] Pedestrian button pressed! Current state: "));
      if (currentState == STATE_GREEN) Serial.println(F("GREEN (Will extend next RED)"));
      else if (currentState == STATE_YELLOW) Serial.println(F("YELLOW (Will extend next RED)"));
      else if (currentState == STATE_RED) Serial.println(F("RED (Already active)"));
    } else if (pedReading == LOW) {
      pedBtnPressed = false;
    }
  }

  
  bool emergReading = digitalRead(BTN_EMERG_PIN);
  if (emergReading != lastEmergState) {
    lastEmergDebounce = now;
    lastEmergState = emergReading;
  }

  if ((now - lastEmergDebounce) > DEBOUNCE_MS) {
    if (emergReading == HIGH && !emergBtnPressed) {
      emergBtnPressed = true;
      isEmergencyMode = !isEmergencyMode; // Переключение режима
      Serial.print(F("[INPUT] Emergency mode toggled to: "));
      Serial.println(isEmergencyMode ? F("ACTIVE") : F("INACTIVE"));
    } else if (emergReading == LOW) {
      emergBtnPressed = false;
    }
  }
}


void setup() {
  Serial.begin(9600);

  
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  
  pinMode(BTN_PED_PIN, INPUT);
  pinMode(BTN_EMERG_PIN, INPUT);

  Serial.println(F("=================================================="));
  Serial.println(F(" Traffic Light FSM - Method 1: switch-case"));
  Serial.println(F("=================================================="));

  changeState(STATE_GREEN, 10000, "System start");
}

void loop() {
  
  processInputs();

  
  if (isEmergencyMode && currentState != STATE_WARNING) {
    changeState(STATE_WARNING, 500, "Emergency button pressed");
  }

  
  switch (currentState) {

    case STATE_GREEN:
      
      if (millis() - stateTimer >= stateDuration) {
        changeState(STATE_YELLOW, 3000, "Green time expired");
      }
      break;

    case STATE_YELLOW:
      
      if (millis() - stateTimer >= stateDuration) {
        if (pedestrianRequested) {
          pedestrianRequested = false; 
          
          changeState(STATE_RED, 15000, "Yellow expired (Extra time for Pedestrian)");
        } else {
          changeState(STATE_RED, 10000, "Yellow expired");
        }
      }
      break;

    case STATE_RED:
      
      if (millis() - stateTimer >= stateDuration) {
        changeState(STATE_GREEN, 10000, "Red expired");
      }
      break;

    case STATE_WARNING:
      
      if (millis() - blinkTimer >= 500) {
        blinkTimer = millis();
        yellowBlinkState = !yellowBlinkState;
        digitalWrite(YELLOW_PIN, yellowBlinkState);
      }

      
      if (!isEmergencyMode) {
        changeState(STATE_GREEN, 10000, "Emergency cleared");
      }
      break;
  }
}