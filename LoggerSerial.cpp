#include "Logger.h"
#include "Arduino.h"
#include <SoftwareSerial.h>
bool Logger::getDebug()
{
  return _debug;
}
void Logger::setDebug(bool value)
{
  _debug = value;
}

Logger::Logger() {
}

void Logger::Initialize() {
  Serial.begin(9600);

  Serial.println("Initializing...");
  Serial.println("Remember to to set Both NL & CR in the serial monitor.");
  Serial.println("Ready");
  Serial.println("");
}

void Logger::Log(LogType logType, char* objectSource, char *fmt, ...) {
  va_list ap;
  char secondaryBuffer[200];

  if (logType != Debug || logType == Debug && getDebug()) {
    snprintf(secondaryBuffer, 200 - 1, "%s - ", LogTypeString[(int)logType]);
    Serial.print(secondaryBuffer);
    snprintf(secondaryBuffer, 200 - 1, "%s - ", objectSource);
    Serial.print(secondaryBuffer);

    va_start(ap, fmt);
    vsnprintf (secondaryBuffer, 200 - 1, fmt, ap);
    Serial.println(secondaryBuffer);
    va_end(ap);
  }
}
