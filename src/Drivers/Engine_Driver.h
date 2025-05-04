void startMotorLeft();
void startMotorRight();
void stopMotor();

void startEngineLeft(int pinPWM, int pinEngine1, int pinEngine2)
{
    digitalWrite(pinPWM, HIGH);
    digitalWrite(pinEngine1, HIGH);
    digitalWrite(pinEngine2, LOW);
}
void startEngineRight(int pinPWM, int pinEngine1, int pinEngine2)
{
    digitalWrite(pinPWM, HIGH);
    digitalWrite(pinEngine1, LOW);
    digitalWrite(pinEngine2, HIGH);
}
void stopEngine(int pinPWM, int pinEngine1, int pinEngine2)
{
    digitalWrite(pinPWM, LOW);
}