#define INITIAL_STATE 0
#define ARMING_STATE 1
#define ARMED_STATE 2
#define TRIGGERED_STATE 3
#define PASSWORD_STATE 4
#define PASSFAIL_STATE 5
#define FAILED_STATE 6
#define RESET_STATE 7

#define BUTTON 2
#define PIR 3
#define LED 4
#define BUZZER 5

#define A 7
#define B 8
#define C 9
#define D 10
#define E 11
#define FLED 12
#define G 6

#define PASSWORD "senha"

int b_press = 0;
int state = INITIAL_STATE;
int previous_ldr = 0;
int correct = 0;

bool sete_segmentos[10][7] = { 
{ 1,1,1,1,1,1,0 }, // = Digito 0
{ 0,1,1,0,0,0,0 }, // = Digito 1
{ 1,1,0,1,1,0,1 }, // = Digito 2
{ 1,1,1,1,0,0,1 }, // = Digito 3
{ 0,1,1,0,0,1,1 }, // = Digito 4
{ 1,0,1,1,0,1,1 }, // = Digito 5
{ 1,0,1,1,1,1,1 }, // = Digito 6
{ 1,1,1,0,0,0,0 }, // = Digito 7
{ 1,1,1,1,1,1,1 }, // = Digito 8
{ 1,1,1,1,0,1,1 }, // = Digito 9
};


void setup() {
  pinMode(BUTTON, INPUT); //button
  pinMode(A0, INPUT); //LDR
  pinMode(PIR, INPUT); //PIR
  
  pinMode(LED, OUTPUT); //led
  pinMode(BUZZER, OUTPUT); //buzzer
  
  Serial.begin(9600);

  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(FLED, OUTPUT);
  pinMode(G, OUTPUT);
  
  Serial.println("Press the button if you want to initialize the security system");
}

void loop() {
  switch(state) {
  case INITIAL_STATE:
    readButton();
  break;
  case ARMING_STATE:
    Serial.println("Initializing System...");
    initializeSystem();
  break;
  case ARMED_STATE:
    Serial.println("Sensors are now active.");
    activateSensor();
   break;
  case TRIGGERED_STATE:
    Serial.println("Unknown suspected activity. Triggering Security System!");
    triggerBuzzer();
  break;
  case PASSWORD_STATE:
    Serial.println("Please enter the password:");
    enterPassword();
  break;
  case PASSFAIL_STATE:
    Serial.println("Please try again:");
    retryPassword();
  break;
  case FAILED_STATE:
    Serial.println("Activating invasion alert!");
    alertInvasion();
  break;
  case RESET_STATE:
    Serial.println("Ok. Deactivating System...");
    Serial.println("Press the button if you want to re-initialize the security system");
    reset();
  default:
    break;
  }
  delay(10);
}

void readButton() {
  b_press = digitalRead(BUTTON);
  if (b_press == HIGH) {
    state = ARMING_STATE;
  }
}

void initializeSystem() {
  for (int i = 0; i < 10; i++) {
    delay(500);
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
  }
  previous_ldr = analogRead(A0);
  state = ARMED_STATE;
}

void activateSensor() {
  int pir_read = digitalRead(PIR);
  int ldr_read = analogRead(A0);
  Serial.println(pir_read);
  if (pir_read or (ldr_read - previous_ldr > 300)) {
    state = TRIGGERED_STATE;
  }
  previous_ldr = ldr_read;
  delay(2000);  
}

void triggerBuzzer() {
  tone(BUZZER, 500);
  delay(1000);
  noTone(BUZZER);
  delay(1500);
  tone(BUZZER, 500);
  delay(1000);
  noTone(BUZZER);
  state = PASSWORD_STATE;  
}

void enterPassword() {
  for (int i = 9; i >= 0; i--) {
    writeDisplay(i);
    delay(1000);
    if (Serial.available() > 0) {
      String paswd = Serial.readString();
      Serial.println(paswd);
      if (paswd == PASSWORD) {
        state = RESET_STATE;
        break;
      } else {
        state = PASSFAIL_STATE;
        Serial.println("Wrong password...");
        break;
      }
    }
  }
  if (state == PASSWORD_STATE) {
    Serial.println("Timeout...");
    state = PASSFAIL_STATE;
  }
}

void retryPassword() {
  for (int i = 9; i >= 0; i--) {
    writeDisplay(i);
    delay(1000);
    if (Serial.available() > 0) {
      String paswd = Serial.readString();
      Serial.println(paswd);
      if (paswd == PASSWORD) {
        state = RESET_STATE;
        break;
      } else {
        state = FAILED_STATE;
        Serial.println("Wrong password...");
        break;
      }
    }
  }
  if (state == PASSFAIL_STATE) {
    state = FAILED_STATE;
    Serial.println("Timeout...");
  }
}

void alertInvasion() {
  tone(BUZZER, 500);
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(500);
}

void reset() {
  digitalWrite(A, 0);
  digitalWrite(B, 0);
  digitalWrite(C, 0);
  digitalWrite(D, 0);
  digitalWrite(E, 0);
  digitalWrite(FLED, 0);
  digitalWrite(G, 0);
  state = INITIAL_STATE;
}

void writeDisplay(int number) {
  bool* display = sete_segmentos[number];
  digitalWrite(A, display[0]);
  digitalWrite(B, display[1]);
  digitalWrite(C, display[2]);
  digitalWrite(D, display[3]);
  digitalWrite(E, display[4]);
  digitalWrite(FLED, display[5]);
  digitalWrite(G, display[6]);
}  
