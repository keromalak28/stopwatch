Project Title:  
Stopwatch with Dual Mode (Increment and Countdown) Using ATmega32 and Seven-Segment Display

Objective:  
To design and implement a digital stopwatch with two modes:  
1. Increment Mode (counting up from zero).  
2. Countdown Mode (counting down from a user-defined time).

Hardware Components:  
1. Microcontroller: ATmega32  
2. Display: Six multiplexed seven-segment displays (common anode), controlled by a 7447 BCD to seven-segment decoder.  
3.Inputs: 10 push buttons for various functions such as reset, pause, resume, mode toggle, and time adjustments.  
4. Alarm/Buzzer: Triggered when the countdown reaches zero.  
5. LED Indicators:  
   - Red LED in Increment Mode.  
   - Yellow LED in Countdown Mode.

Features:
1. Reset: Resets the stopwatch to zero in both modes.
2. Pause/Resume: Pauses and resumes time counting.
3. Mode Toggle: Switches between increment and countdown modes.
4. Time Adjustment: Allows the user to adjust hours, minutes, and seconds.
5. Buzzer: Activated when the countdown reaches zero.

Operation:
1. Increment Mode: The stopwatch counts upwards from zero by default. The time is displayed across six seven-segment displays showing hours, minutes, and seconds.
2. Countdown Mode: In this mode, the user can set a specific countdown time. The countdown begins once resumed, and the buzzer activates when the time reaches zero.

System Requirements:
- Microcontroller: ATmega32 with 16 MHz system frequency.
- Timer1: Configured in CTC mode for time counting.
- Multiplexed 7-segment Displays: Controlled using a 7447 decoder and NPN transistors.
- Interrupts:  
  - INT0 (PD2): Reset (falling edge).
  - INT1 (PD3): Pause (rising edge).
  - INT2 (PB2): Resume (falling edge).

Countdown Mode Setup:
- Pause the timer, toggle to countdown mode, adjust the countdown time, and resume.
- The buzzer is triggered when the countdown reaches zero.
