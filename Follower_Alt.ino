#include <Servo.h>

Servo servo;

const int trigPin = 13;
const int echoPin = 12;

const int servoPin = 11;

const int enAPin = 6;
const int in1Pin = 7;
const int in2Pin = 5;
const int in3Pin = 4;
const int in4Pin = 2;
const int enBPin = 3;

const int thresholdDistance = 400; // Distance threshold for following in centimeters
const int stopDistance = 50; // Distance threshold for stopping in centimeters

unsigned long lastSweepTime = 0; // Timestamp of the last sweep
const unsigned long sweepInterval = 50; // Interval between servo sweeps

int currentLeftSpeed = 0; // Current speed of the left motor
int currentRightSpeed = 0; // Current speed of the right motor
const int maxSpeedChange = 10; // Maximum speed change per interval

enum Motor { LEFT, RIGHT };

void setMotorSpeed(enum Motor m, int targetSpeed)
{
    int* currentSpeed = (m == LEFT) ? &currentLeftSpeed : &currentRightSpeed;
    
    if (*currentSpeed < targetSpeed)
    {
        *currentSpeed += maxSpeedChange;
        if (*currentSpeed > targetSpeed)
            *currentSpeed = targetSpeed;
    }
    else if (*currentSpeed > targetSpeed)
    {
        *currentSpeed -= maxSpeedChange;
        if (*currentSpeed < targetSpeed)
            *currentSpeed = targetSpeed;
    }

    if (m == LEFT)
    {
        digitalWrite(in1Pin, *currentSpeed > 0 ? HIGH : LOW);
        digitalWrite(in2Pin, *currentSpeed <= 0 ? HIGH : LOW);
        analogWrite(enAPin, abs(*currentSpeed));
    }
    else
    {
        digitalWrite(in3Pin, *currentSpeed > 0 ? HIGH : LOW); 
        digitalWrite(in4Pin, *currentSpeed <= 0 ? HIGH : LOW);
        analogWrite(enBPin, abs(*currentSpeed));
    }
}

unsigned int readDistance()
{
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    unsigned long period = pulseIn(echoPin, HIGH);
    return period * 343 / 2000;
}

#define NUM_ANGLES 7
unsigned char sensorAngle[NUM_ANGLES] = {60, 70, 80, 90, 100, 110, 120};
unsigned int distance[NUM_ANGLES];

void readNextDistance()
{
    static unsigned char angleIndex = 0;
    static signed char step = 1;
    distance[angleIndex] = readDistance();
    angleIndex += step;
    if (angleIndex == NUM_ANGLES - 1)
        step = -1;
    else if (angleIndex == 0)
        step = 1;
    servo.write(sensorAngle[angleIndex]);
}

void setup()
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);

    pinMode(enAPin, OUTPUT);
    pinMode(in1Pin, OUTPUT);
    pinMode(in2Pin, OUTPUT);
    pinMode(in3Pin, OUTPUT);
    pinMode(in4Pin, OUTPUT);
    pinMode(enBPin, OUTPUT);

    servo.attach(servoPin);
    servo.write(90);

    setMotorSpeed(LEFT, 0);
    setMotorSpeed(RIGHT, 0);

    servo.write(sensorAngle[0]);
    delay(250);
    for (unsigned char i = 0; i < NUM_ANGLES; i++) {
        readNextDistance();
        delay(250);
    }
}

void loop()
{
    if (millis() - lastSweepTime >= sweepInterval) {
        lastSweepTime = millis();
        readNextDistance();
    }

    int minDistance = 9999;
    int minIndex = -1;
    
    for (unsigned char i = 0; i < NUM_ANGLES; i++)
    {
        if (distance[i] < minDistance)
        {
            minDistance = distance[i];
            minIndex = i;
        }
    }

    if (minDistance < stopDistance) // Stop if object is too close
    {
        setMotorSpeed(LEFT, 0);
        setMotorSpeed(RIGHT, 0);
    }
    else if (minDistance < thresholdDistance) // Follow object if within threshold distance
    {
        if (minIndex < NUM_ANGLES / 2) // Object is to the left
        {
            setMotorSpeed(LEFT, 140);
            setMotorSpeed(RIGHT, 255);
        }
        else if (minIndex > NUM_ANGLES / 2) // Object is to the right
        {
            setMotorSpeed(LEFT, 255);
            setMotorSpeed(RIGHT, 140);
        }
        else // Object is straight ahead
        {
            setMotorSpeed(LEFT, 255);
            setMotorSpeed(RIGHT, 255);
        }
    }
    else // No object detected within threshold distance
    {
        setMotorSpeed(LEFT, z);  // Move forward slowly
        setMotorSpeed(RIGHT, 210); // Move forward slowly
    }

    delay(50); // Adjust delay for smoother operation
}



