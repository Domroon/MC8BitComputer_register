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

#define CLK                 A7

#define LATCH_PIN_REG_A     A0     // Pin connected to ST_CP of 74HC595
#define LATCH_PIN_REG_B     A1     // Pin connected to ST_CP of 74HC595
#define CLOCK_PIN           12     // Pin connected to SH_CP of 74HC595
#define DATA_PIN            11     // Pin connected to DS of 74HC595

#define REGISTER_A          0
#define REGISTER_B          1

#define BIT0                2
#define BIT1                3
#define BIT2                4
#define BIT3                5
#define BIT4                6
#define BIT5                7
#define BIT6                8
#define BIT7                9

#define AI                  A2      // enable Register A INPUT
#define AO                  A3      // enable Register A OUTPUT
#define BI                  A4      // enable Register B INPUT

Timer LED_buildin_timer(500);
bool led_state = LOW;

Timer infoTimer(10);
int analogA7value = 0;
bool clock_high = HIGH;
bool clock_high_before = false;

// test variables
byte numberToDisplay = 0;

byte bus_data = 0;

// control signals
bool input_A_enabled = false;
bool output_A_enabled = false;
bool input_B_enabled = false;


void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(CLK, INPUT);

  // shift register
  pinMode(LATCH_PIN_REG_A, OUTPUT);
  pinMode(LATCH_PIN_REG_B, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  // init registers
  writeToShiftRegister(0, REGISTER_A);
  writeToShiftRegister(0, REGISTER_B);

  // bus input
  pinMode(BIT0, INPUT);
  pinMode(BIT1, INPUT);
  pinMode(BIT2, INPUT);
  pinMode(BIT3, INPUT);
  pinMode(BIT4, INPUT);
  pinMode(BIT5, INPUT);
  pinMode(BIT6, INPUT);
  pinMode(BIT7, INPUT);
}

void loop() {
    change_led_builtin_state();
    check_clock();
    read_control_signals();

    // read bus
    if(input_A_enabled && !clock_high && clock_high_before){
      read_bus();
      Serial.println("READ BUS DATA into Register A");
      Serial.print("BUS DATA: ");
      Serial.println(bus_data);
      writeToShiftRegister(bus_data, REGISTER_A);
    }
    if(input_B_enabled && !clock_high && clock_high_before){
      read_bus();
      Serial.println("READ BUS DATA into Register B");
      Serial.print("BUS DATA: ");
      Serial.println(bus_data);
      writeToShiftRegister(bus_data, REGISTER_B);
    }

    if(clock_high){
      clock_high_before = true;
    }

    if(!clock_high && clock_high_before){
      clock_high_before = false;
      Serial.println("Clock changes from HIGH to LOW");
    }   
}

void check_clock(){
  analogA7value = analogRead(CLK);
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

  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, data);

  if(shiftregister == REGISTER_B){
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0);
  }
  digitalWrite(latch_pin, HIGH);
}

void read_bus(){
  pinMode(BIT0, INPUT);
  pinMode(BIT1, INPUT);
  pinMode(BIT2, INPUT);
  pinMode(BIT3, INPUT);
  pinMode(BIT4, INPUT);
  pinMode(BIT5, INPUT);
  pinMode(BIT6, INPUT);
  pinMode(BIT7, INPUT);

  bus_data = 0;

  if(digitalRead(BIT0)){
    bus_data = bus_data + 1;
  }
  if(digitalRead(BIT1)){
    bus_data = bus_data + 2;
  }
  if(digitalRead(BIT2)){
    bus_data = bus_data + 4;
  }
  if(digitalRead(BIT3)){
    bus_data = bus_data + 8;
  }
  if(digitalRead(BIT4)){
    bus_data = bus_data + 16;
  }
  if(digitalRead(BIT5)){
    bus_data = bus_data + 32;
  }
  if(digitalRead(BIT6)){
    bus_data = bus_data + 64;
  }
  if(digitalRead(BIT7)){
    bus_data = bus_data + 128;
  }
}

void read_control_signals(){
  if(digitalRead(AI)){
    input_A_enabled = true;
  } else {
    input_A_enabled = false;
  }
  if(digitalRead(AO)){
    output_A_enabled = true;
  } else {
    output_A_enabled = false;
  }
  if(digitalRead(BI)){
    input_B_enabled = true;
  } else {
    input_B_enabled = false;
  }
}