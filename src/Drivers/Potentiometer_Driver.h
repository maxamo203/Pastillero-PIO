short readPotentiometer();

short readPotentiometer(int pin)
{
 int value = analogRead(pin);        // Lee el valor del potenciómetro
 return map(value, 0, 1023, 0, 100); // Mapea el valor a un rango de 0 a 100
}