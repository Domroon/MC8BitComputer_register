// for use in the program
#define REGISTER_A          0
#define REGISTER_B          1

// Shift Register Pins
#define LATCH_PIN_REG_A     A0     // Pin connected to ST_CP of 74HC595
#define LATCH_PIN_REG_B     A1     // Pin connected to ST_CP of 74HC595
#define CLOCK_PIN           12     // Pin connected to SH_CP of 74HC595
#define DATA_PIN            11     // Pin connected to DS of 74HC595

// Bus Pins
#define BIT0                2
#define BIT1                3
#define BIT2                4
#define BIT3                5
#define BIT4                6
#define BIT5                7
#define BIT6                8
#define BIT7                9

// Control Signal Pins
#define AI                  A2      // enable Register A INPUT
#define AO                  A3      // enable Register A OUTPUT
#define BI                  A4      // enable Register B INPUT

#define CLK                 A7

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

// Timer
Timer LED_buildin_timer(500);
bool led_state = LOW;

// Clock
int analogA7value = 0;
bool clock_high = HIGH;
bool clock_high_before = false;
bool clock_is_falling = false;

// Bus and Register
byte bus_data = 0;
byte register_a_data = 0;
byte register_b_data = 0;

// Control Signals
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

  // init bus connection
  set_bus_connection_as(INPUT);
}

void loop() {
    change_led_builtin_state();
    clock_high = check_clock();
    clock_is_falling = check_falling_clock();
    read_control_signals();

    // read from bus
    if(input_A_enabled && clock_is_falling){
      bus_data = read_bus();
      write_to_register(bus_data, REGISTER_A);
    }
    if(input_B_enabled && clock_is_falling){
      bus_data = read_bus();
      write_to_register(bus_data, REGISTER_B);
    }

    // special case: register a writes and register b reads at the same time
    if(output_A_enabled && input_B_enabled && clock_is_falling){
      register_b_data = register_a_data;
      writeToShiftRegister(register_b_data, REGISTER_B);
    }

    // write to bus
    if(output_A_enabled && clock_is_falling){
      enable_bus_output(register_a_data);
      bus_data = register_a_data;
    }
    if(!output_A_enabled && clock_is_falling){
      disable_bus_output();
    }
}

void set_bus_connection_as(bool input_or_output){
  for(int i=BIT0; i <= BIT7; i++){
    pinMode(i, input_or_output);
  }
}

bool check_clock(){
  analogA7value = analogRead(CLK);
  if(analogA7value > 800){
    return HIGH;
  } else if(analogA7value < 250) {
    return LOW;
  }
}

bool check_falling_clock(){
  if(clock_high){
      clock_high_before = true;
  }

  if(!clock_high && clock_high_before){
    clock_high_before = false;
    Serial.println("Clock changes from HIGH to LOW");
    return true;
  } else {
    return false;
  }
}

void write_to_register(byte data, byte reg){
  if(reg == REGISTER_A){
    Serial.println("Read BUS DATA into Register A");
    register_a_data = data;
  } else if(reg == REGISTER_B){
    Serial.println("Read BUS DATA into Register B");
    register_b_data = data;
  }
  Serial.print("BUS DATA: ");
  Serial.println(data);
  Serial.println();
  writeToShiftRegister(data, reg);
  
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

byte read_bus(){
  set_bus_connection_as(INPUT);

  byte data = 0;

  if(digitalRead(BIT0)){
    data = data + 1;
  }
  if(digitalRead(BIT1)){
    data = data + 2;
  }
  if(digitalRead(BIT2)){
    data = data + 4;
  }
  if(digitalRead(BIT3)){
    data = data + 8;
  }
  if(digitalRead(BIT4)){
    data = data + 16;
  }
  if(digitalRead(BIT5)){
    data = data + 32;
  }
  if(digitalRead(BIT6)){
    data = data + 64;
  }
  if(digitalRead(BIT7)){
    data = data + 128;
  }

  return data;
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

void enable_bus_output(byte data){
  set_bus_connection_as(OUTPUT);

  Serial.print("outputData in Decimal: ");
  Serial.println(data);
  bool outputData[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  for(int i=0; i < sizeof(outputData); i++){
    outputData[i] = bitRead(data, i);
  }

  Serial.print("outputData in Binary: ");
  for(int i=0; i < sizeof(outputData); i++){
    Serial.print(outputData[i]);
  }
  Serial.println();
  Serial.println();

  for(int i=0; i < sizeof(outputData); i++){
    if(outputData[i] == HIGH){
      digitalWrite(i+2, HIGH);
    } else {
      digitalWrite(i+2, LOW);
    }
  }
}

void disable_bus_output(){
  set_bus_connection_as(OUTPUT);
  Serial.println("Disable Output");
  Serial.println();
  for(int i=0; i < 8; i++){
    digitalWrite(i+2, LOW);
  }
  set_bus_connection_as(INPUT);
}