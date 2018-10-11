# ESE519_Lab3_Part3

In this part, a Pong Game is implemented.
 
The paddles, gridlines, scores and the ball is created using basic drawing functions.

To achieve paddles moving when touched on screen, the x coordinate of the touching point is checked, if it is less than 31, the left paddle is controlled. Otherwise the right paddle is controlled. Then the y coordinate of the touching point is checked, if it is above the paddle, clear the original paddle and draw a new one 2 pixels above, if it is below the paddle, clear the original paddle and draw a new one 2 pixels below.

To achieve ball movement and random speed/direction in every round. Two variables mov and coef are assigned to control the speed in x-axis and direction slope. In every iteration, clear the original ball and draw a new ball at (x+mov, y+mov*coef). At the beginning of each round, we assign different values to mov and coef according to the values of timer register to get the random speed/direction. Whenever the ball hits the horizontal gridline, we invert the coef and whenever it hits the paddles, invert mov.

To trigger the buzzer, the CTC mode of Timer1 is applied. Use different output compare values to generate pulses of different frequencies on PORTB1. PORTB1 is connected to a transistor to switch on and off the buffer. After a delay of 1000ms, PORTB1 is set to input to disable the buzzer.
