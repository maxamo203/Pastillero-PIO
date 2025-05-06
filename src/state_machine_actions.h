#pragma once
#include "setup_utils.h"
#include "config.h"
#include "fisical.h"
#include "event_types.h"
#include "freeRTOS_Tasks.h"

typedef void (*action)();

// Actions
void initialize()
{

 configSetup();

 // Lee archivo con horarios y dias
 // Calcula el proximo horario y dia
 // Pasa al estado awaiting reminder time
 fisicalSetup();
 attachInterrupt(LIMIT_SWITCH_PIN, detectMovingLimitSwitch, RISING);
 attachInterrupt(BUTTON_PIN, detectButtonPress, RISING); // Configura la interrupción para el botón
 setupWifi();
 setupTime();

 createNewScheduledTimer();
  
 queueSetup();
 semaphoreSetup();
  
 xTaskCreate(showHourTimerLCD, "showHourTimerLCD", 2048, NULL, 1, NULL);
 xTaskCreate(notifyDoseAvailable, "notifyDoseAvailable", 2048, NULL, 1, NULL);
 xTaskCreate(notifyDoseUnnavailable, "notifyDoseUnnavailable", 2048, NULL, 1, NULL);

}

void noScheduleSet();
void settingSchedule();
void awaitingTimer();
void scanning();
void pillDetected();
void noPillDetected();
void doseTaken();
void stopReturning();
void doseSkipped();
void reanudeCycle();
void pauseCycle();

void error();
void none();

void error()
{
}
void none()
{
}

void modifyVolume()
{
 setVolumeBuzzer(potentiometerLastValue);

 Serial.print("New volume: ");
 Serial.println(potentiometerLastValue);
}

void doseTaken()
{
 xSemaphoreTake(noPillNotificationSemaphore, 0);
 xSemaphoreTake(notificationSemaphore, 0);
 setLeds[objetivePeriod](LOW);
 stopBuzzer();
 writeLCD("Dose taken\nReturning...");
 startMotorLeft();
}
void stopReturning()
{
 stopMotor();
 xSemaphoreGive(showTimerSemaphore);
 objetiveDay = NO_PILL_TOOKING;
 objetivePeriod = NO_PILL_TOOKING;
 Serial.println(uxSemaphoreGetCount(showTimerSemaphore));
 Serial.println("Stop returning...");
}
void awaitingTimer()
{
 Serial.println("Awaiting timer...");
 xSemaphoreGive(showTimerSemaphore);
}
void moving()
{
 Serial.println("Moving...");
 xSemaphoreTake(showTimerSemaphore, 0);
 writeLCD("Moving...");
 setDayAndPeriod();
 startMotorRight();
}
void scanning()
{
 setLeds[objetivePeriod](LOW);
 stopBuzzer();
 stopMotor();
 DebugPrint("Scanning");
}
void pillDetected()
{
 writeLCD("Pill detected");
 xSemaphoreGive(notificationSemaphore);
 Serial.println("Pill detected...");
}
void noPillDetected()
{
 Serial.println("No pill detected...");
 xSemaphoreGive(noPillNotificationSemaphore);
}
void doseSkipped()
{
 xSemaphoreTake(notificationSemaphore, 0);
 xSemaphoreTake(noPillNotificationSemaphore, 0);
 setLeds[objetivePeriod](LOW);
 stopBuzzer();
 Serial.println("Dose skipped...");
 writeLCD("Dose skipped\nReturning...");
 startMotorLeft();
}
void settingSchedule()
{
 Serial.println("Setting schedule...");
 xSemaphoreGive(showTimerSemaphore);
}
void noScheduleSet()
{
 Serial.println("No schedule set...");
 xSemaphoreGive(showTimerSemaphore);
}
void reanudeCycle()
{
 xSemaphoreGive(showTimerSemaphore);
}
void pauseCycle()
{
 xSemaphoreTake(showTimerSemaphore, 0);
 writeLCD("Cycle paused\nPress button to resume...");
}