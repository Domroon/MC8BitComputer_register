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

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    change_led_builtin_state();
}

void change_led_builtin_state(){
    if(LED_buildin_timer.checkTimeOver()){
        led_state = !led_state;
        digitalWrite(LED_BUILTIN, led_state);
        LED_buildin_timer.restart();
    }
}
