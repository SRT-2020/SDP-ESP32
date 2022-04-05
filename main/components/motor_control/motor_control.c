#include "motor_control.h"
#include "analogWrite.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "constants.h"
#include "pin_control.h"
#include "driver/gpio.h"
#include "delay.h"





int MA_IN1 = 33;
int MA_IN2 = 27;  
int MB_IN3 = 32;
int MB_IN4 = 14;
int MA_PWM = 15;
int MB_PWM = 13;


int MAX_SPEED = 255;

void setupMotors(void)
{
    pinMode(MA_IN1, GPIO_MODE_DEF_OUTPUT);
    pinMode(MA_IN2, GPIO_MODE_DEF_OUTPUT);
    pinMode(MA_PWM, GPIO_MODE_DEF_OUTPUT);
    pinMode(MB_IN3, GPIO_MODE_DEF_OUTPUT);
    pinMode(MB_IN4, GPIO_MODE_DEF_OUTPUT);
    pinMode(MB_PWM, GPIO_MODE_DEF_OUTPUT);
}

void A_CW(int speed)
{
    digitalWrite(MA_IN1, 1);
    digitalWrite(MA_IN2, 0);
    analogWrite(MA_PWM, speed, MAX_SPEED);
}

void A_CCW(int speed)
{
    digitalWrite(MA_IN1, 0);
    digitalWrite(MA_IN2, 1);
    analogWrite(MA_PWM, speed, MAX_SPEED);
}

void A_Stop(void)
{
    digitalWrite(MA_IN1, 0);
    digitalWrite(MA_IN2, 0);
    analogWrite(MA_PWM, 0, MAX_SPEED);
}

void B_CW(int speed)
{
    digitalWrite(MB_IN3, 1);
    digitalWrite(MB_IN4, 0);
    analogWrite(MB_PWM, speed, MAX_SPEED);
}

void B_CCW(int speed)
{
    digitalWrite(MB_IN3, 0);
    digitalWrite(MB_IN4, 1);
    analogWrite(MB_PWM, speed, MAX_SPEED);

}

void B_Stop(void)
{
    digitalWrite(MB_IN3, 0);
    digitalWrite(MB_IN4, 0);
    analogWrite(MB_PWM, 0, MAX_SPEED);
}

void forward(int speed)
{
    A_CW(speed);
    B_CCW(speed);
}

void reverse(int speed)
{
    A_CCW(speed);
    B_CW(speed);
}

void stop(void)
{
    A_Stop();
    B_Stop();
}

void turn_left(int speed)
{
    A_CW(speed-20);
    B_CCW(speed);
}

void turn_right(int speed)
{
    A_CW(speed);
    B_CCW(speed-20);
}

void turn_left_reverse(int speed)
{
    A_CCW(speed);
    B_CW(speed-20);
}

void turn_right_reverse(int speed)
{
    A_CCW(speed-20);
    B_CW(speed);
}

void rotate_right(int speed)
{
    A_CCW(speed);
    B_CCW(speed);
}

void rotate_left(int speed)
{
    A_CW(speed);
    B_CW(speed);
}





