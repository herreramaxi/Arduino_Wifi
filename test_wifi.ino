#include <SoftwareSerial.h>
#include "Logger.h"

SoftwareSerial ESPserial(2, 3); // RX | TX
Logger logger = Logger();

static int DEBUG = 1;

void setup()
{
  //Serial.begin(9600);     // communication with the host computer

  logger.Initialize();
  ESPserial.begin(115200);
  //ESPserial.setTimeout(5000);

  logger.setDebug(true);
  //logger.Log(Debug, "test_wifi", "setup: isDebug: %d (%s)", logger.getDebug(), logger.getDebug() ? "true" : "false");
  logger.Log(Info, "test_wifi", "setup: ok");
}
bool _manual = false;

void loop()
{
  if (!_manual) {
    if ( Serial.available() ) {
      String cmd = Serial.readString();

      logger.Log(Debug, "loop", "buf: %s, %d", cmd.c_str(), cmd.length());
      //free(buf);
      if (cmd.indexOf("init") >= 0) Initialize();
      else if (cmd.indexOf("manual") >= 0) SetManual(cmd);
      else if (cmd.indexOf("connect") >= 0) Connect();
      else if (cmd.indexOf("timeout") >= 0) SetTimeout(cmd);
      else if (cmd.indexOf("debug") >= 0) SetDebug(cmd);
      else if (cmd.indexOf("send") >= 0) SendData(cmd);
      else if (cmd.indexOf("tcpto") >= 0) TcpTo(cmd);
      else {
        bool res = SendCommand(cmd);
        logger.Log(Info, "loop", "res: %d", (int)res);
      }
    }

    if (ESPserial.available()) {
      Serial.print(ESPserial.readString());
    }
  }
  else {

    if ( ESPserial.available() )   {
      String espData = ESPserial.readString();
      
      Serial.write( espData.c_str() );     
    }

    // listen for user input and send it to the ESP8266
    if ( Serial.available() ) {
      String cmd = Serial.readString();
      if (cmd.indexOf("manual") >= 0) {
        SetManual(cmd);
      }
      else {
        ESPserial.write( cmd.c_str() );
      }
    }
  }
}

bool Initialize( ) {
  bool result = SendCommand("AT") && SendCommand("AT+CWMODE=3");

  logger.Log(Info, "Initialize", "result= %d", (int)result);

  return result;
}

bool Connect() {
  bool result = SendCommand("AT+CWJAP=\"VMA1732E8\",\"mJsrzss2nzuV\"");
  logger.Log(Info, "Connect", "result= %d", (int)result);

  return result;
}


void SetManual(String command) {
  String paramString = command.substring(sizeof("manual")) ;
  paramString.trim();

  if (isDigit(paramString[0])) {
    int manual = paramString.toInt();

    if (manual == 1) {

      _manual = 1;
    }
    else {
      _manual = 0;
    }

    logger.Log(Info, "SetManual", "Setting manual to -> %d", manual);
  }


}

bool SetTimeout(String command) {
  String paramString = command.substring(sizeof("timeout")) ;
  paramString.trim();

  String timeoutStr = "";

  for (int i = 0 ; i < paramString.length(); ++i) {
    if ( isDigit(paramString[i])) {
      timeoutStr += paramString[i];
    }
  }

  if (timeoutStr.length() > 0) {
    int timeout = timeoutStr.toInt();
    logger.Log(Info, "SetTimeout", "Setting timeout to -> %d" , timeout);
    ESPserial.setTimeout(timeout);

    return timeout > 0;
  }

  return false;
}

bool SetDebug(String command) {
  String paramString = command.substring(sizeof("debug")) ;
  paramString.trim();

  String timeoutStr = "";

  for (int i = 0 ; i < paramString.length(); ++i) {
    if ( isDigit(paramString[i])) {
      timeoutStr += paramString[i];
    }
  }

  if (timeoutStr.length() > 0) {
    int timeout = timeoutStr.toInt();
    logger.Log(Info, "SetDebug", "Setting debug to -> %d", timeout);
    logger.setDebug((bool)timeout) ;

    return timeout > 0;
  }

  return false;
}

bool SendCommand(String command) {
  bool success = false;

  if (WriteCommand(command)) {
    //    while (ESPserial.available()) {
    //      if (ESPserial.find("OK")) {
    //        success = true;
    //        break;
    //      }
    //    }
    while (ESPserial.available()) {
      //TODO: Test Serial.find
      String dataRead = ESPserial.readString();

      logger.Log(Debug, "SendCommand", "ESPserial.readString(): %s", dataRead.c_str());

      dataRead.toUpperCase();

      if (dataRead.indexOf("OK") >= 0) {
        success = true;
        break;
      }
    }
  }

  logger.Log(Debug, "SendCommand" , "success= %d", (int)success);
  return success;
}

bool WriteCommand(String command) {
  //TODO: I need to write and wait for the timeout because if not I lose information
  command += "\r\n";

  int length = command.length();
  int bytesSent = 0;
  int ret = 0;
  bool success = false;

  while (bytesSent < length ) {
    String buffer = command.substring(bytesSent);
    ret = ESPserial.print(buffer);

    if (ret < 0) {
      logger.Log(Error, "WriteCommand", "ESPserial.write, ret<0: %d", (int)(ret < 0) ); //Handle error
      break;
    }

    bytesSent += ret;
    logger.Log(Debug, "WriteCommand", "ESPserial.write, command.length: %d, ret: %d, bytesSent: %d, command: %s, buffer: %s", length,  (int)ret, bytesSent, command.c_str(), buffer.c_str());
  }

  success = bytesSent == length;
  logger.Log(Debug, "WriteCommand", "success: %d, bytesSent: %d, length: %d, command: %s", (int)success, bytesSent, length, command.c_str());

  return success;
}

bool SendData(String command) {
  String paramString = command.substring(sizeof("send")) ;
  paramString.trim();
  int len = paramString.length();
  String commandLenght = "AT+CIPSEND=" + (String)len;
  logger.Log(Debug, "SendData", "%s", commandLenght.c_str());

  bool result = SendCommand(commandLenght);
  logger.Log(Debug, "SendData", "SendCommand result: %d", result);

  logger.Log(Debug, "SendData", "data: %s", paramString.c_str());

  result = SendCommand(paramString);
  logger.Log(Debug, "SendData", "SendCommand result: %d", result);

  return result;
}

bool TcpTo(String command) {
  //AT+CIPSTART="TCP","192.168.0.73",8081
  String paramString = command.substring(sizeof("tcpto")) ;
  int index = paramString.indexOf(" ");
  bool res = false;
  String reason = "";
  String ip = "";
  String port = "";

  if (index > 0) {
    ip = paramString.substring(0, index);
    port = paramString.substring(index + 1);

    if (ip.length() > 0 && port.length() > 0) {
      res = SendCommand("AT+CIPSTART=\"TCP\",\"" + ip + "\"," + port);
    } else {
      reason = "wrong ip or port";
    }
  } else {
    reason = "There is not space between ip and port. Example: tcpto 192.168.0.73 8081";
  }

  if (!res) {
    logger.Log(Error, "TcpTo", "There was an error on stablishing a tcp connection. Reason: %s, ip: %s, port: %s", reason.c_str(), ip.c_str(), port.c_str());
  }
  else {
    logger.Log(Info,  "TcpTo", "Connection stablished to: %s %s", ip.c_str(), port.c_str());
  }

  return res;
}
