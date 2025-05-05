#pragma once
#include "setup_utils.h"
#include "config.h"
#include "fisical.h"
#include "event_types.h"

#define LCD_BLINK_TIME 5000
#define NOTIFICATION_FRECUENCY_ALERT 1000
typedef void (*action)();
void (*setLeds[MAX_PILLS_PER_DAY])(short) = {setLedPresence_TM, setLedPresence_TT, setLedPresence_TN};

void showHourTimerLCDCallback(void *)
{
 getLocalTime(&timeinfo, 10); // Actualiza la hora
 char mensaje[32];            // Ajustá el tamaño según el contenido esperado

 if (millis() % (LCD_BLINK_TIME * 2) < LCD_BLINK_TIME)
 {
  snprintf(mensaje, sizeof(mensaje), "Hora: %02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
 }
 else
 {
  snprintf(mensaje, sizeof(mensaje), "Proxima toma: \n%02d:%02d %s", schedule[nextPeriod].tm_hour, schedule[nextPeriod].tm_min, weekDays[schedule[nextPeriod].tm_wday]);
 }

 writeLCD(mensaje);
 Serial.println("Show hour timer LCD");
 vTaskDelay(LCD_BLINK_TIME);
}

void notifyDoseAvailableCallback(void *)
{
 if (millis() % (NOTIFICATION_FRECUENCY_ALERT * 2) < NOTIFICATION_FRECUENCY_ALERT)
 {
  setLeds[objetivePeriod](HIGH);
  writeLCD("Dose available");
  stopBuzzer();
 }
 else
 {
  setLeds[objetivePeriod](LOW);
  writeLCD("Take it now!");
  startBuzzer();
 }
 vTaskDelay(NOTIFICATION_FRECUENCY_ALERT);
}
void notifyDoseAvailable(void *)
{
 while (1)
 {
  waitForSemaphore(notifyDoseAvailableCallback, NULL, notificationSemaphore); // Espera a que el semáforo esté disponible
 }
}

void showHourTimerLCD(void *)
{
 while (1)
 {
  waitForSemaphore(showHourTimerLCDCallback, NULL, showTimerSemaphore); // Espera a que el semáforo esté disponible
 }
}
// Actions
void initialize()
{
 /*
 int scheduleSetup[MAX_DAYS][MAX_PILLS_PER_DAY] = {
     {6, 14, 21}, // Domingo Mañana, Domingo Tarde, Domingo Noche
     {6, 0, 0},
     {6, 0, 0},
     {0, 0, 0},
     {0, 0, 0},
     {0, 0, 0},
     {0, 0, 0}};
     */
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
 timeEventsQueue = xQueueCreate(MAX_EVENTS_QUEUE, sizeof(events));
 buttonEventsQueue = xQueueCreate(MAX_EVENTS_QUEUE, sizeof(short));
 showTimerSemaphore = xSemaphoreCreateMutex();
 lcdMutex = xSemaphoreCreateMutex();
 notificationSemaphore = xSemaphoreCreateMutex();
 xSemaphoreTake(notificationSemaphore, 0);
 xTaskCreate(showHourTimerLCD, "showHourTimerLCD", 2048, NULL, 1, NULL);
 xTaskCreate(notifyDoseAvailable, "notifyDoseAvailable", 2048, NULL, 1, NULL);
}

void noScheduleSet();
void settingSchedule();
void awaitingTimer();
void scanning()
{
 stopMotor();
 DebugPrint("Scanning");
};
void pillDetected();
void noPillDetected();
void doseTaken();
void stopReturning();
void doseSkipped();

void error();
void none();

void error()
{
}
void none()
{
}
void doseTaken()
{
 xSemaphoreTake(notificationSemaphore, 0);
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
 Serial.println(uxSemaphoreGetCount(showTimerSemaphore)); // No lo toma
 writeLCD("Moving...");
 setDayAndPeriod();
 startMotorRight();
}
void pillDetected()
{
 writeLCD("Pill detected");
 xSemaphoreGive(notificationSemaphore);
 Serial.println("Pill detected...");
}
void noPillDetected()
{
 writeLCD("No pill detected");
 Serial.println("No pill detected...");
}
void doseSkipped()
{
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