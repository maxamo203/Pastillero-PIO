#pragma once // Avoid multiple redefinitions of the same file
// This file encapsulates the physical layer of the system, including the buttons, presence sensors, limit switches, and buzzer.
//  It provides functions to read the state of these components and to write to the buzzer.
// #include <Wire.h> // Librería para comunicación I2C
#include "debug.h"
#include "Drivers/LCD_Driver.h"           // Librería para manejar el LCD
#include "Drivers/SD_Driver.h"            // Librería para manejar la SD
#include "Drivers/CNY70_Driver.h"         // Librería para manejar el sensor de presencia CNY70
#include "Drivers/Buzzer_Driver.h"        // Librería para manejar el zumbador
#include "Drivers/Potentiometer_Driver.h" // Librería para manejar el potenciómetro
#include "Drivers/LimitSwitch_Driver.h"   // Librería para manejar el fin de carrera
#include "Drivers/Button_Driver.h"        // Librería para manejar los botones
#include "Drivers/Engine_Driver.h"        // Librería para manejar el motor

// Variables y valores
#define BUTTON_PIN 33
#define PRESENCE_PIN_1 34
#define PRESENCE_PIN_2 26
#define PRESENCE_PIN_3 25

#define PRESENCE_LED_1 27
#define PRESENCE_LED_2 14
#define PRESENCE_LED_3 12

// TODO: Agregar pines necesarios para el girarMotor

// Fines de carrera
#define LIMIT_SWITCH_PIN 35

#define BUZZER_PIN 13
#define POTENTIOMETER_PIN 32

#define LCD_SDA_PIN 15
#define LCD_SCL_PIN 16

#define IN1_PIN_PUENTE_H 4
#define IN2_PIN_PUENTE_H 2
#define EN_PIN_PUENTE_H 15

#define LCD_ADDRESS 0x27 // Dirección I2C del LCD (ajustar según el modelo)
#define LCD_COLUMNS 16   // Número de columnas del LCD
#define LCD_ROWS 2       // Número de filas del LCD

void fisicalSetup();
short readPresenceSensor_TM();
short readPresenceSensor_TT();
short readPresenceSensor_TN();
short readLimitSwitch();
short readButton();
void startBuzzer();
void stopBuzzer();
short readPotentiometer();

short buzzerVolume = 255;

short readPresenceSensor_TM()
{
 return readCNY70(PRESENCE_PIN_1); // Lee el valor del sensor de presencia
}
short readPresenceSensor_TT()
{
 return readCNY70(PRESENCE_PIN_2); // Lee el valor del sensor de presencia
}
short readPresenceSensor_TN()
{
 return readCNY70(PRESENCE_PIN_3); // Lee el valor del sensor de presencia
}

short readLimitSwitch()
{
 return readLimitSwitch(LIMIT_SWITCH_PIN); // Lee el valor del fin de carrera
}

short readButton()
{
 return readButton(BUTTON_PIN); // Lee el valor del botón
}

void startBuzzer()
{
 writeBuzzer(BUZZER_PIN, buzzerVolume); // Enciende el zumbador con el volumen especificado
}

void stopBuzzer()
{
 writeBuzzer(BUZZER_PIN, 0); // Enciende el zumbador con el volumen especificado
}

short readPotentiometer()
{
 return readPotentiometer(POTENTIOMETER_PIN); // Lee el valor del potenciómetro
}

void setVolumeBuzzer(short volume)
{
 if (volume < 0 || volume > 255)
 {
  DebugPrint("Error: Volumen fuera de rango (0-255).");
  return; // Retorna si el volumen está fuera de rango
 }
 buzzerVolume = volume; // Asigna el nuevo volumen al zumbador
}

void startMotorLeft()
{
 startMotorLeft(IN1_PIN_PUENTE_H, IN2_PIN_PUENTE_H, EN_PIN_PUENTE_H); // Inicia el motor izquierdo
}

void startMotorRight()
{
 startMotorRight(IN1_PIN_PUENTE_H, IN2_PIN_PUENTE_H, EN_PIN_PUENTE_H); // Inicia el motor derecho
}
void stopMotor()
{
 stopMotor(IN1_PIN_PUENTE_H, IN2_PIN_PUENTE_H, EN_PIN_PUENTE_H); // Detiene el motor
}

void fisicalSetup()
{
 pinMode(BUTTON_PIN, INPUT_PULLUP); // Configura el botón 1 como entrada con resistencia pull-up

 pinMode(PRESENCE_PIN_1, INPUT); // Configura el sensor de presencia 1 como entrada
 pinMode(PRESENCE_PIN_2, INPUT); // Configura el sensor de presencia 2 como entrada
 pinMode(PRESENCE_PIN_3, INPUT); // Configura el sensor de presencia 3 como entrada

 pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP); // Configura el fin de carrera 1 como entrada con resistencia pull-up

 pinMode(BUZZER_PIN, OUTPUT);       // Configura el zumbador como salida
 pinMode(POTENTIOMETER_PIN, INPUT); // Configura el potenciómetro como entrada

 pinMode(IN1_PIN_PUENTE_H, OUTPUT);
 pinMode(IN2_PIN_PUENTE_H, OUTPUT);
 pinMode(EN_PIN_PUENTE_H, OUTPUT);
 setupLCD(); // Inicializa la pantalla LCD
}