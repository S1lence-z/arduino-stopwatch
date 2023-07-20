#include "funshield.h"

// constants
constexpr int numberOfDigits = 4;
constexpr int maxValueToDisplay = 10000;
constexpr unsigned long myDelay = 100;

// button constants
constexpr int buttons[] = { button1_pin, button2_pin, button3_pin };
constexpr int numberOfButtons = sizeof(buttons)/sizeof(buttons[0]);

// IMPLEMENTACE TŘÍDY NA DISPLEJ
class Display {
private:
  int displayedValue = 0;
  int capPos = 0;
  int dot = 0x7f;

public:
  void set_value(int number) {
    displayedValue = number;
  }
  void setup() {
    pinMode(latch_pin, OUTPUT);
    pinMode(clock_pin, OUTPUT);
    pinMode(data_pin, OUTPUT);
    displayFinalNumber(0);
  }
  int getDigitValue(int, int);
  void displayDigit(int, int);
  void displayFinalNumber(int);

};
int Display::getDigitValue(int value, int currentDigit) {
  return (value / currentDigit) % 10;
}

void Display::displayDigit(int digit, int position) {
  constexpr int placeOfSeconds = 2;
  int realPos = numberOfDigits - position - 1;
  int num = (realPos == placeOfSeconds) ? digits[digit] & dot : digits[digit];
  digitalWrite(latch_pin, ON);
  shiftOut(data_pin, clock_pin, MSBFIRST, num);
  shiftOut(data_pin, clock_pin, MSBFIRST, digit_muxpos[realPos]);
  digitalWrite(latch_pin, OFF);
}

void Display::displayFinalNumber(int valueToDisplay) {
  constexpr int allDigits[numberOfDigits] = { 1, 10, 100, 1000 };
  set_value(valueToDisplay);
  int temp = getDigitValue(displayedValue, allDigits[capPos]);
  if (capPos > 1) 
  {
    if (temp == 0 && (displayedValue / allDigits[capPos] <= 0))
      temp = 10;
  }
  displayDigit(temp, capPos);
  capPos = (capPos + 1) % numberOfDigits;
}
// KONEC IMPLEMENTACE TŘÍDY NA DISPLEJ

// IMPLEMENTACE TŘÍDY NA TLAČÍTKA
class Button {
private:
  int pin;
  int change_value;
  bool stateNow = true;
  bool stateBefore = true;

public:
  Button(int pin1, int change_val)
  { 
    pin = pin1;
    change_value = change_val;
  }
  void setup() { 
    pinMode(pin, INPUT);
    stateNow = digitalRead(pin);
  }
  void updateState();
  bool isPressed();
};
void Button::updateState() {
  stateBefore = stateNow;
  stateNow = digitalRead(pin);
}
bool Button::isPressed() {
  return (stateBefore && !stateNow);
}
// KONEC IMPLEMENTACE TŘÍDY NA TLAČÍTKA

// IMPLEMENTACE TŘÍDY NA STOPKY

class Stopwatch {
private:
  unsigned long stopWatchTimeValue = 0;
  int lappedTimeValue = 0;
  bool lappedTimeRecorded = false;

  // states
  enum possible_states { stopped, running, lapped };
  possible_states currentState = stopped;
  possible_states previousState = stopped;

  // timing
  unsigned long startButtonPress = 0;
  unsigned long realStopwatchTime = 0;

  // required objects
  Button* activeButtons[numberOfButtons];
  Display& activeDisplay;

public:
  Stopwatch(Button* b[], Display& d) : activeDisplay(d)
  { 
    for (int i = 0; i < numberOfButtons; i++)
    {
      activeButtons[i] = b[i];
    }
  }
  int incrementFunction(int, int);
  void updateStateStopwatch(Button*[], unsigned long);

  void loop(unsigned long time) 
  {
    updateStateStopwatch(activeButtons, time);
    switch (currentState)
    {
      case running:
        previousState = currentState;
        realStopwatchTime += time - startButtonPress - realStopwatchTime;
        lappedTimeRecorded = false;
        break;

      case lapped:
        realStopwatchTime += time - startButtonPress - realStopwatchTime;
        if (!lappedTimeRecorded) 
        {
          lappedTimeValue = stopWatchTimeValue;
        }
        lappedTimeRecorded = true;
        break;

      case stopped:
        if (previousState == stopped)
        {
          stopWatchTimeValue = 0;
          realStopwatchTime = 0;
        }
        break;

      default:
        break;
    }

    if (realStopwatchTime - stopWatchTimeValue * myDelay >= myDelay)
    {
      stopWatchTimeValue = incrementFunction(stopWatchTimeValue, 1);
    }

    if (lappedTimeRecorded)
      activeDisplay.displayFinalNumber(lappedTimeValue);
    else
      activeDisplay.displayFinalNumber(stopWatchTimeValue);
  }
};

void Stopwatch::updateStateStopwatch(Button* b[], unsigned long time) 
{
  for (int i = 0; i < numberOfButtons; i++) 
  {
    b[i]->updateState();
  }

  switch(currentState)
  {
    case running:
      if (b[0]->isPressed())
      {
        realStopwatchTime += time - startButtonPress - realStopwatchTime;
        previousState = currentState;
        currentState = stopped;
      }
      else if (b[1]->isPressed())
      {
        previousState = currentState;
        currentState = lapped;
      }
      break;

    case stopped:
      if (b[0]->isPressed())
      {
        startButtonPress = time - realStopwatchTime;
        previousState = currentState;
        currentState = running;
      }
      if (b[2]->isPressed())
      {
        previousState = currentState;
        currentState = stopped;
      }
      break;

    case lapped:
      if (b[1]->isPressed())
      {
        previousState = currentState;
        currentState = running;
      }
      break;
    
    default:
      break;
  }
}

int Stopwatch::incrementFunction(int value, int increment) {
  value += increment;
  return (value % maxValueToDisplay + maxValueToDisplay) % maxValueToDisplay;
}
// KONEC IMPLEMENTACE TŘÍDY NA STOPKY

Button firstButton(buttons[0], 1);
Button secondButton(buttons[1], 2);
Button thirdButton(buttons[2], 3);
Button* usedButtons[numberOfButtons] = { &firstButton, &secondButton, &thirdButton };
Display integratedDisplay;
Stopwatch stopwatch(usedButtons, integratedDisplay);

void setup() {
  // display set up
  integratedDisplay.setup();
  // buttons set up
  for (int i = 0; i < numberOfButtons; i++)
  {
    usedButtons[i]->setup();
  }
}

void loop() {
  unsigned long currentTime = millis();
  stopwatch.loop(currentTime);
}