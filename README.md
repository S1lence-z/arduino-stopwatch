# Arduino FunShield Stopwatch 

The FunShield Stopwatch is an Arduino-based stopwatch application that utilizes the FunShield hardware, including a 4-digit 7-segment display and three buttons. This application allows users to measure time, lap time, and reset the stopwatch. It is a simple and interactive project suitable for beginners and hobbyists.

## Features

- Stopwatch functionality: Start, stop, and reset the stopwatch.
- Lap time: Record lap times while the stopwatch is running.
- 4-digit 7-segment display: Display the elapsed time or the lap time on the FunShield hardware.
- Easy-to-use buttons: Three buttons for controlling the stopwatch and navigating through different modes.

## Prerequisites

To use this code, you need the following hardware components:

- Arduino board (tested with Arduino UNO)
- FunShield hardware, including a 4-digit 7-segment display and three buttons.

## Getting Started

1. Connect the FunShield hardware to your Arduino board.

2. Download the "funshield.h" library from the "FunShield" repository (provide a link to the repository).

3. Include the "funshield.h" library in your Arduino sketch.

4. Copy and paste the code from "stopwatch.ino" into your Arduino sketch.

5. Upload the sketch to your Arduino board.

6. Once uploaded, start using the stopwatch and enjoy!

## How to Use

1. Press the first button (Button1) to start the stopwatch. The display will show the elapsed time in seconds.

2. Press the first button (Button1) again to stop the stopwatch. The display will freeze, showing the last recorded time.

3. While the stopwatch is running, press the second button (Button2) to record a lap time. The display will show the lap time while keeping the stopwatch running.

4. To return to the stopwatch display, press the second button (Button2) again.

5. To reset the stopwatch, press the third button (Button3). This will set the elapsed time to zero.

## Authors

- [Jiří Zelena] - Initial implementation