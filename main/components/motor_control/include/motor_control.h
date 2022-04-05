#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H





void setupMotors(void);
void A_CW(int speed);
void A_CCW(int speed);
void A_Stop(void);
void B_CW(int speed);
void B_CCW(int speed);
void B_Stop(void);
void forward(int speed);
void reverse(int speed);
void stop(void);
void turn_left(int speed);
void turn_right(int speed);
void turn_left_reverse(int speed);
void turn_right_reverse(int speed);
void rotate_left(int speed);
void rotate_right(int speed);




 
#endif