#pragma once
#include "setup_utils.h"
#include "config.h"
#include "fisical.h"
#include "event_types.h"

#define LCD_BLINK_TIME 5000
#define NOTIFICATION_FRECUENCY_ALERT 1000
#define NOTIFICATION_UNNAVAILABLE_ALERT 500
typedef void (*action)();
void (*setLeds[MAX_PILLS_PER_DAY])(short) = {setLedPresence_TM, setLedPresence_TT, setLedPresence_TN};

bool isBelowTime(int frecuency)
{
 return (millis() % (frecuency * 2)) < frecuency;
}

void showHourTimerLCDCallback(void *)
{
 getLocalTime(&timeinfo, 10); // Actualiza la hora
 char mensaje[32];            // Ajustá el tamaño según el contenido esperado

 if (isBelowTime(LCD_BLINK_TIME))
 {
  snprintf(mensaje, sizeof(mensaje), "Hour: %02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
 }
 else
 {
  snprintf(mensaje, sizeof(mensaje), "Next dose: \n%02d:%02d %s", schedule[nextPeriod].tm_hour, schedule[nextPeriod].tm_min, weekDays[schedule[nextPeriod].tm_wday]);
 }

 writeLCD(mensaje);
 vTaskDelay(LCD_BLINK_TIME);
}

void notifyDoseAvailableCallback(void *)
{
 if (isBelowTime(NOTIFICATION_FRECUENCY_ALERT))
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
void notifyDoseUnavailableCallback(void *)
{
 if (isBelowTime(NOTIFICATION_UNNAVAILABLE_ALERT))
 {
  setLeds[objetivePeriod](HIGH);
  writeLCD("No pill detected...");
  stopBuzzer();
 }
 else
 {
  setLeds[objetivePeriod](LOW);
  writeLCD("Fill the dispenser");
  startBuzzer();
 }
 vTaskDelay(NOTIFICATION_UNNAVAILABLE_ALERT);
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

void notifyDoseUnnavailable(void *)
{
 while (1)
 {
  waitForSemaphore(notifyDoseUnavailableCallback, NULL, noPillNotificationSemaphore);
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
 noPillNotificationSemaphore = xSemaphoreCreateMutex();

 xSemaphoreTake(notificationSemaphore, 0);
 xSemaphoreTake(noPillNotificationSemaphore, 0);

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