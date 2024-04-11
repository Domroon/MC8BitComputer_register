class Timer{
  private:
    unsigned long _startMillis = 0;
    unsigned int _timerDuration;
  public:
    Timer(unsigned int duration){
      _timerDuration = duration;
    }
    void start(){ // start the timer             
      _startMillis = millis();
    }
    void restart(){ // restart the timer
      _startMillis = millis();
    }
    void changeDuration(unsigned int newTimerDuration){
        _startMillis = millis();
        _timerDuration = newTimerDuration;
    }
    unsigned int getDuration(){ // return the duration of the timer
      return _timerDuration;
    }
    bool checkTimeOver() {; // check if the timer is over and return a bool value
      if (millis() - _startMillis >= _timerDuration){
        return true;
      } else {
        return false;
      }
    }
};

#define LATCH_PIN_REG_A     A0     // Pin connected to ST_CP of 74HC595
#define LATCH_PIN_REG_B     A1     // Pin connected to ST_CP of 74HC595
#define CLOCK_PIN           12     // Pin connected to SH_CP of 74HC595
#define DATA_PIN            11     // Pin connected to DS of 74HC595

#define REGISTER_A          0
#define REGISTER_B          1

Timer LED_buildin_timer(500);
bool led_state = LOW;

Timer infoTimer(10);
int analogA7value = 0;
bool clock_high = HIGH;
bool clock_high_before = false;

// test variables
byte numberToDisplay = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A7, INPUT);

  // shift register
  pinMode(LATCH_PIN_REG_A, OUTPUT);
  pinMode(LATCH_PIN_REG_B, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  // init registers
  writeToShiftRegister(0, REGISTER_A);
  writeToShiftRegister(0, REGISTER_B);
}

void loop() {
    change_led_builtin_state();
    check_clock();

    if(clock_high){
      clock_high_before = true;
    }

    if(!clock_high && clock_high_before){
      clock_high_before = false;
      Serial.println("Clock changes from HIGH to LOW");
      writeToShiftRegister(numberToDisplay, REGISTER_A);
      writeToShiftRegister(numberToDisplay, REGISTER_B);
      if(numberToDisplay > 256) {
        numberToDisplay = 0;
      }
      numberToDisplay++;
    }   
}

void check_clock(){
  analogA7value = analogRead(A7);
  if(analogA7value > 800){
    clock_high = 1;
  } else if(analogA7value < 250) {
    clock_high = 0;
  }
}

void change_led_builtin_state(){
    if(LED_buildin_timer.checkTimeOver()){
        led_state = !led_state;
        digitalWrite(LED_BUILTIN, led_state);
        LED_buildin_timer.restart();
    }
}

void writeToShiftRegister(byte data, byte shiftregister){
  int latch_pin;
  if(shiftregister == REGISTER_A){
    latch_pin = LATCH_PIN_REG_A;
  } else if(shiftregister == REGISTER_B){
    latch_pin = LATCH_PIN_REG_B;
  }  

  digitalWrite(latch_pin, LOW);
  if(shiftregister == REGISTER_A){
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0);
  }

  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, numberToDisplay);

  if(shiftregister == REGISTER_B){
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0);
  }
  digitalWrite(latch_pin, HIGH);
}
