#pragma once
#include "setup_utils.h"

typedef void (*action)();

void showHourTimerLCD(void *)
{
  short displayingCurrentTime = 1;
  while (1)
  {
    if (uxSemaphoreGetCount(showTimerSemaphore) > 0)
    {
      getLocalTime(&timeinfo, 10); // Actualiza la hora
      // Semáforo disponible (no lo toma)
      char mensaje[32]; // Ajustá el tamaño según el contenido esperado

      if (displayingCurrentTime == 1)
      {
        snprintf(mensaje, sizeof(mensaje), "Hora: %02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
      }
      else
      {
        snprintf(mensaje, sizeof(mensaje), "Proxima toma: %02d:%02d", schedule[nextPeriod].tm_hour, schedule[nextPeriod].tm_min);
      }

      writeLCD(mensaje);
      displayingCurrentTime *= -1; // Cambia el estado de la variable
      Serial.println("Show hour timer LCD");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    else
    {
      // Semáforo ocupado: tomar con bloqueo
      xSemaphoreTake(showTimerSemaphore, portMAX_DELAY);
    }
  }
}
// Actions
void initialize()
{

  int scheduleSetup[MAX_DAYS][MAX_PILLS_PER_DAY] = {
      {6, 14, 21}, // Domingo Mañana, Domingo Tarde, Domingo Noche
      {6, 0, 0},
      {6, 0, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0}};

  // Lee archivo con horarios y dias
  // Calcula el proximo horario y dia
  // Pasa al estado awaiting reminder time
  fisicalSetup();
  attachInterrupt(LIMIT_SWITCH_MOVIL, detectMovingLimitSwitch, RISING);
  setupWifi();
  setupTime();

  setupSchedule(scheduleSetup);

  createNewScheduledTimer();
  timeEventsQueue = xQueueCreate(MAX_EVENTS_QUEUE, sizeof(events));
  showTimerSemaphore = xSemaphoreCreateMutex();
  xTaskCreate(showHourTimerLCD, "showHourTimerLCD", 2048, NULL, 1, NULL);
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

void returnToHome()
{
  startMotorLeft();
};
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
}
void stopReturning()
{
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
  setDayAndPeriod();
  startMotorRight();
}
void scanning()
{
  Serial.println("Scanning...");
}
void pillDetected()
{
  Serial.println("Pill detected...");
}
void noPillDetected()
{
  Serial.println("No pill detected...");
}
void doseSkipped()
{
  Serial.println("Dose skipped...");
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