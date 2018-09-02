#ifndef Logger_h
#define Logger_h

#include "Arduino.h"

enum LogType {Debug, Info, Warning, Error};
static const char LogTypeString[4][10] = {"Debug", "Info", "Warning", "Error"};
     
class Logger
{
  public:
    Logger();

    bool getDebug();
    void setDebug(bool value);
    void Initialize();
    //void Log(int logType, char* source, char* message);
    void Log(LogType logType, char* objectSource, char *fmt, ...);
  private: 
    //static const char _logTypeString[4][10] = {"Debug", "Info", "Warning", "Error"};
  
    bool _debug;
};

#endif
