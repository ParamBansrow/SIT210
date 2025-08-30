

#include <Arduino.h>

// ===== CONFIG =====
const uint8_t BUTTON_PIN = 2;   // panel mount push button
const uint8_t PIR_PIN    = 3;   // HC-SR501 output
const uint8_t LED1_PIN   = 8;   // toggled by button
const uint8_t LED2_PIN   = 9;   // toggled by PIR

const unsigned long DEBOUNCE_MS = 50;       // button debounce window
const unsigned long PIR_AUTO_OFF_MS = 0;    

// ===== volatile flags (set in ISRs) =====
volatile bool buttonFlag = false;
volatile bool pirFlag = false;

// ===== runtime state =====
unsigned long lastButtonMillis = 0;
bool led1State = false;
bool led2State = false;
unsigned long pirActivatedAt = 0;

// ===== ISRs - keep tiny! =====
void buttonISR() {
  buttonFlag = true;
}

void pirISR() {
  pirFlag = true;
}

void setup() {
  // pins
  pinMode(BUTTON_PIN, INPUT_PULLUP); // button to GND
  pinMode(PIR_PIN, INPUT);           // HC-SR501 output (3.3V)
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);


  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), pirISR, RISING);

  Serial.begin(115200);
  
  while (!Serial) { delay(10); }

  Serial.println("Task4.2C - Nano 33 IoT: Multiple Interrupts READY");
  Serial.print("Button pin: "); Serial.println(BUTTON_PIN);
  Serial.print("PIR pin:    "); Serial.println(PIR_PIN);
  Serial.print("LED1 pin:   "); Serial.println(LED1_PIN);
  Serial.print("LED2 pin:   "); Serial.println(LED2_PIN);
}

void loop() {
  unsigned long now = millis();

  if (buttonFlag) {
    // consume flag atomically
    noInterrupts();
    buttonFlag = false;
    interrupts();

    if (now - lastButtonMillis > DEBOUNCE_MS) {
      led1State = !led1State;
      digitalWrite(LED1_PIN, led1State ? HIGH : LOW);
      Serial.print(now); Serial.print(" ms - Button pressed -> LED1 ");
      Serial.println(led1State ? "ON" : "OFF");
      lastButtonMillis = now;
    } else {
      Serial.print(now); Serial.println(" ms - Button press ignored (debounce)");
    }
  }

  // ---------- PIR event handling ----------
  if (pirFlag) {
    noInterrupts();
    pirFlag = false;
    interrupts();

    // toggle LED2 (customize behavior if you prefer 'turn-on-while-motion')
    led2State = !led2State;
    digitalWrite(LED2_PIN, led2State ? HIGH : LOW);
    Serial.print(now); Serial.print(" ms - Motion detected -> LED2 ");
    Serial.println(led2State ? "ON" : "OFF");

    if (led2State) pirActivatedAt = now;
  }

  if (PIR_AUTO_OFF_MS > 0 && led2State) {
    if (now - pirActivatedAt >= PIR_AUTO_OFF_MS) {
      led2State = false;
      digitalWrite(LED2_PIN, LOW);
      Serial.print(now); Serial.println(" ms - PIR auto-off -> LED2 OFF");;[]
    }
  }

  // small yield to prevent a tight busy loop
  delay(10);
}
