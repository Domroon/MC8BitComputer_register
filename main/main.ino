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

Timer LED_buildin_timer(500);
bool led_state = LOW;

Timer infoTimer(10);
int analogA7value = 0;
bool clock_high = HIGH;
bool clock_high_before = false;

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(A7, INPUT);
}

void loop() {
    change_led_builtin_state();
    check_clock();

    if(clock_high){
      clock_high_before = true;
    }

    if(!clock_high && clock_high_before){
      Serial.println("Clock changes from HIGH to LOW");
      clock_high_before = false;
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
