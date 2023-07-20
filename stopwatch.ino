#include "funshield.h"

// Constants
constexpr int numberOfDigits = 4;
constexpr int maxValueToDisplay = 10000;
constexpr unsigned long myDelay = 100;

// Button pins
constexpr int buttons[] = { button1_pin, button2_pin, button3_pin };
constexpr int numberOfButtons = sizeof(buttons) / sizeof(buttons[0]);

// IMPLEMENTATION OF THE DISPLAY CLASS
class Display {
private:
  int displayedValue = 0;
  int capPos = 0;
  int dot = 0x7f;

public:
  // Set the value to be displayed on the 7-segment display
  void set_value(int number) {
    displayedValue = number;
  }

  // Initialize the pins and display a default value on the 7-segment display
  void setup() {
    pinMode(latch_pin, OUTPUT);
    pinMode(clock_pin, OUTPUT);
    pinMode(data_pin, OUTPUT);
    displayFinalNumber(0);
  }

  // Get the value of a digit at a specific position
  int getDigitValue(int value, int currentDigit);

  // Display a digit at a specific position on the 7-segment display
  void displayDigit(int digit, int position);

  // Display the final number on the 7-segment display
  void displayFinalNumber(int value);
};

// Get the value of a digit at a specific position
int Display::getDigitValue(int value, int currentDigit) {
  return (value / currentDigit) % 10;
}

// Display a digit at a specific position on the 7-segment display
void Display::displayDigit(int digit, int position) {
  constexpr int placeOfSeconds = 2;
  int realPos = numberOfDigits - position - 1;
  int num = (realPos == placeOfSeconds) ? digits[digit] & dot : digits[digit];
  digitalWrite(latch_pin, ON);
  shiftOut(data_pin, clock_pin, MSBFIRST, num);
  shiftOut(data_pin, clock_pin, MSBFIRST, digit_muxpos[realPos]);
  digitalWrite(latch_pin, OFF);
}

// Display the final number on the 7-segment display
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
// END OF DISPLAY CLASS IMPLEMENTATION

// IMPLEMENTATION OF THE BUTTON CLASS
class Button {
private:
  int pin;
  int change_value;
  bool stateNow = true;
  bool stateBefore = true;

public:
  // Constructor to initialize the button object with the pin and change value
  Button(int pin1, int change_val) { 
    pin = pin1;
    change_value = change_val;
  }

  // Setup the button pin as INPUT and read its initial state
  void setup() { 
    pinMode(pin, INPUT);
    stateNow = digitalRead(pin);
  }

  // Update the state of the button (current and previous)
  void updateState();

  // Check if the button is pressed (based on the state change)
  bool isPressed();
};

// Update the state of the button (current and previous)
void Button::updateState() {
  stateBefore = stateNow;
  stateNow = digitalRead(pin);
}

// Check if the button is pressed (based on the state change)
bool Button::isPressed() {
  return (stateBefore && !stateNow);
}
// END OF BUTTON CLASS IMPLEMENTATION

// IMPLEMENTATION OF THE STOPWATCH CLASS
class Stopwatch {
private:
  unsigned long stopWatchTimeValue = 0;
  int lappedTimeValue = 0;
  bool lappedTimeRecorded = false;

  // States of the stopwatch
  enum possible_states { stopped, running, lapped };
  possible_states currentState = stopped;
  possible_states previousState = stopped;

  // Timing variables
  unsigned long startButtonPress = 0;
  unsigned long realStopwatchTime = 0;

  // Pointers to active buttons and display
  Button* activeButtons[numberOfButtons];
  Display& activeDisplay;

public:
  // Constructor to set the active buttons and display for the stopwatch
  Stopwatch(Button* b[], Display& d) : activeDisplay(d) { 
    for (int i = 0; i < numberOfButtons; i++) {
      activeButtons[i] = b[i];
    }
  }

  // Function to increment the stopwatch time
  int incrementFunction(int, int);

  // Update the state of the stopwatch based on button presses and time
  void updateStateStopwatch(Button*[], unsigned long);

  // Loop function to handle stopwatch operations
  void loop(unsigned long time) {
    updateStateStopwatch(activeButtons, time);
    switch (currentState) {
      case running:
        previousState = currentState;
        realStopwatchTime += time - startButtonPress - realStopwatchTime;
        lappedTimeRecorded = false;
        break;

      case lapped:
        realStopwatchTime += time - startButtonPress - realStopwatchTime;
        if (!lappedTimeRecorded) {
          lappedTimeValue = stopWatchTimeValue;
        }
        lappedTimeRecorded = true;
        break;

      case stopped:
        if (previousState == stopped) {
          stopWatchTimeValue = 0;
          realStopwatchTime = 0;
        }
        break;

      default:
        break;
    }

    if (realStopwatchTime - stopWatchTimeValue * myDelay >= myDelay) {
      stopWatchTimeValue = incrementFunction(stopWatchTimeValue, 1);
    }

    if (lappedTimeRecorded)
      activeDisplay.displayFinalNumber(lappedTimeValue);
    else
      activeDisplay.displayFinalNumber(stopWatchTimeValue);
  }
};

// Function to increment the stopwatch time with a specific increment value
int Stopwatch::incrementFunction(int value, int increment) {
  value += increment;
  return (value % maxValueToDisplay + maxValueToDisplay) % maxValueToDisplay;
}

// Update the state of the stopwatch based on button presses and time
void Stopwatch::updateStateStopwatch(Button* b[], unsigned long time) {
  for (int i = 0; i < numberOfButtons; i++) {
    b[i]->updateState();
  }

  switch (currentState) {
    case running:
      if (b[0]->isPressed()) {
        realStopwatchTime += time - startButtonPress - realStopwatchTime;
        previousState = currentState;
        currentState = stopped;
      }
      else if (b[1]->isPressed()) {
        previousState = currentState;
        currentState = lapped;
      }
      break;

    case stopped:
      if (b[0]->isPressed()) {
        startButtonPress = time - realStopwatchTime;
        previousState = currentState;
        currentState = running;
      }
      if (b[2]->isPressed()) {
        previousState = currentState;
        currentState = stopped;
      }
      break;

    case lapped:
      if (b[1]->isPressed()) {
        previousState = currentState;
        currentState = running;
      }
      break;
    
    default:
      break;
  }
}
// END OF STOPWATCH CLASS IMPLEMENTATION

// Create Button objects
Button firstButton(buttons[0], 1);
Button secondButton(buttons[1], 2);
Button thirdButton(buttons[2], 3);
Button* usedButtons[numberOfButtons] = { &firstButton, &secondButton, &thirdButton };

// Create Display object
Display integratedDisplay;

// Create Stopwatch object with the buttons and display
Stopwatch stopwatch(usedButtons, integratedDisplay);

void setup() {
  // Display setup
  integratedDisplay.setup();

  // Buttons setup
  for (int i = 0; i < numberOfButtons; i++) {
    usedButtons[i]->setup();
  }
}

void loop() {
  unsigned long currentTime = millis();
  stopwatch.loop(currentTime);
}