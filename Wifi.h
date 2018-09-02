#ifndef Wifi_h
#define Wifi_h

#include "Arduino.h"
#include "Connection.h"

class Wifi
{
  public:
    Wifi(String ssid, String password);

    bool Initialize();
    bool Join();
    bool Disconnect();

    Connection CreateConnection(Protocol protocol, String ip, int port);

  private:
    String _ssid;
    String _password;
    Connection _connection1;
    Connection _connection2;
};

#endif
