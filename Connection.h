#ifndef Connection_h
#define Connection_h

#include "Arduino.h"

enum Protocol { TCP, UDP};

class Connection
{
  public:
    Connection(int id, Protocol protocol, String ip, int port );
    bool Connect();
    bool Disconnect();
    void Status();
  private:
    int _id;
    String _ip;
    int _port;
    Protocol _protocol;
};

#endif
