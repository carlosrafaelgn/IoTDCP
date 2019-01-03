
//**************************************
// If the device cannot be renamed
#define IoTNameReadOnly
//**************************************
// If the device can be renamed
// #define IoTMaxNameLength 32
//**************************************

//**************************************
// If the device requires no password
//#define IoTNoPassword
//**************************************
// If the password cannot be changed
#define IoTPasswordReadOnly
//**************************************
// If the password can be changed
//#define IoTMaxPasswordLength 32
//**************************************

#define IoTInterfaceCount 1
#define IoTMaxPayloadLength 256

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <IoTDCP.h>

// Just to make it easier to reference the interfaces and properties
#define Interface0 0
#define PropState 0
#define PropColor 1
#define PropSampleEnum 2

const IoTPropertyDescriptor IoTInterface0Properties[] = {
  { "State", IoTProperty.ModeReadOnly, IoTProperty.DataTypeU8, 1, IoTProperty.UnitEnum, IoTProperty.UnitOne, 0 },
  { "Color", IoTProperty.ModeReadWrite, IoTProperty.DataTypeRGBTriplet, 1, IoTProperty.UnitRGB, IoTProperty.UnitOne, 0 },
  { "Sample Enum", IoTProperty.ModeReadWrite, IoTProperty.DataTypeS16, 1, IoTProperty.UnitEnum, IoTProperty.UnitOne, 0 }
};

const IoTInterfaceDescriptor IoTInterfaces[IoTInterfaceCount] = {
  { "Sample Interface", IoTInterface.TypeOnOff, countof(IoTInterface0Properties), IoTInterface0Properties }
};

const IoTEnumDescriptor16 IoTInterface0SampleEnum[] = {
  { "Value 0", 0 },
  { "Value 1", 1 },
  { "Value 2", 2 },
  { "Value 255", 255 }
};

const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

uint8_t wifiConnected, wifiConnecting, onOff, color[3];
uint8_t receivedBuffer[256];
uint16_t enumValue;
uint32_t wifiConnectionStartTime;
WiFiUDP udpServer;

void describeEnum(IoTMessageDescribeEnum* msg) {
  if (msg->interfaceIndex) {
    IoTServer.buildResponse(IoTServer.ResponseInvalidInterface);
    return;
  }

  if (msg->propertyIndex != 2) {
    // Since describing state's enum is not necessary and we do not have any other enums...
    IoTServer.buildResponse(IoTServer.ResponseInvalidInterfaceProperty);
  } else {
    IoTServer.buildResponseEnumDescriptor16(0, 2, IoTInterface0SampleEnum, countof(IoTInterface0SampleEnum));
  }
}

void executeCommand(IoTMessageExecute* msg) {
  if (msg->interfaceIndex) {
    IoTServer.buildResponse(IoTServer.ResponseInvalidInterface);
    return;
  }
  switch (msg->interfaceCommand) {
  case IoTInterfaceOnOff.CommandOff:
    if (!IoTServer.isMessageRepeated()) {
      onOff = IoTInterfaceOnOff.StateOff;
      // Any other commands should go here
    }
    IoTServer.writeResponseProperty8(Interface0, PropState, onOff);
    IoTServer.buildResponse(IoTServer.ResponseOK);
    break;
  case IoTInterfaceOnOff.CommandOn:
    if (!IoTServer.isMessageRepeated()) {
      onOff = IoTInterfaceOnOff.StateOn;
      // Any other commands should go here
    }
    IoTServer.writeResponseProperty8(Interface0, PropState, onOff);
    IoTServer.buildResponse(IoTServer.ResponseOK);
    break;
  default:
    IoTServer.buildResponse(IoTServer.ResponseInvalidInterfaceCommand);
    break;
  }
}

void getProperty(IoTMessageGetProperty* msg) {
  if (msg->interfaceIndex) {
    IoTServer.buildResponse(IoTServer.ResponseInvalidInterface);
    return;
  }
  switch (msg->propertyIndex) {
  case PropState:
    IoTServer.writeResponseProperty8(Interface0, PropState, onOff);
    IoTServer.buildResponse(IoTServer.ResponseOK);
    break;
  case PropColor:
    IoTServer.writeResponsePropertyRGB(Interface0, PropColor, color);
    IoTServer.buildResponse(IoTServer.ResponseOK);
    break;
  case PropSampleEnum:
    IoTServer.writeResponseProperty16(Interface0, PropSampleEnum, enumValue);
    IoTServer.buildResponse(IoTServer.ResponseOK);
    break;
  default:
    IoTServer.buildResponse(IoTServer.ResponseInvalidInterfaceProperty);
    return;
  }
}

void setProperty(IoTMessageSetProperty* msg, uint16_t payloadLength) {
  if (msg->interfaceIndex) {
    IoTServer.buildResponse(IoTServer.ResponseInvalidInterface);
    return;
  }

  switch (msg->propertyIndex) {
  case PropState:
    IoTServer.buildResponse(IoTServer.ResponseInterfacePropertyReadOnly);
    break;
  case PropColor:
    if (msg->propertyValueLength != 3) {
      IoTServer.buildResponse(IoTServer.ResponseInvalidInterfacePropertyValue);
    } else {
      color[0] = msg->propertyValue[0];
      color[1] = msg->propertyValue[1];
      color[2] = msg->propertyValue[2];
      // Any other commands should go here
      IoTServer.writeResponsePropertyRGB(Interface0, PropColor, color);
      IoTServer.buildResponse(IoTServer.ResponseOK);
    }
    break;
  case PropSampleEnum:
    if (msg->propertyValueLength != 2) {
      IoTServer.buildResponse(IoTServer.ResponseInvalidInterfacePropertyValue);
    } else {
      switch (*((uint16_t*)msg->propertyValue)) {
      case 0:
      case 1:
      case 2:
      case 255:
        enumValue = *((uint16_t*)msg->propertyValue);
        // Any other commands should go here
        IoTServer.writeResponseProperty16(Interface0, PropSampleEnum, enumValue);
        IoTServer.buildResponse(IoTServer.ResponseOK);
        break;
      default:
        IoTServer.buildResponse(IoTServer.ResponseInvalidInterfacePropertyValue);
        break;
      }
    }
    break;
  default:
    IoTServer.buildResponse(IoTServer.ResponseInvalidInterfaceProperty);
    return;
  }
}

void handleMessage() {
  switch (IoTServer.message()) {
  case IoTServer.MessageDescribeEnum:
    describeEnum((IoTMessageDescribeEnum*)IoTServer.payloadBuffer());
    break;
  case IoTServer.MessageExecute:
    executeCommand((IoTMessageExecute*)IoTServer.payloadBuffer());
    break;
  case IoTServer.MessageGetProperty:
    getProperty((IoTMessageGetProperty*)IoTServer.payloadBuffer());
    break;
  case IoTServer.MessageSetProperty:
    setProperty((IoTMessageSetProperty*)IoTServer.payloadBuffer(), IoTServer.payloadLength());
    break;
  default:
    IoTServer.buildResponse(IoTServer.ResponseUnsupportedMessage);
    break;
  }
}

void WiFiEvent(WiFiEvent_t event) {
  switch(event) {
  case WIFI_EVENT_STAMODE_GOT_IP:
    if (!wifiConnecting)
      break;
    wifiConnected = 1;
    wifiConnecting = 0;
    udpServer.begin(IoTPort);
    break;
  case WIFI_EVENT_STAMODE_DISCONNECTED:
    wifiConnected = 0;
    wifiConnecting = 0;
    udpServer.stop();
    udpServer.stopAll();
    break;
  }
}

void setup() {
  IoTServer.begin();

  IoTServer.storedName("Sample Device");

  //**************************************
  // Set the initial password, if the
  // device is password protected
  IoTServer.storedPassword("Password", 8);
  //**************************************

  wifiConnected = 0;
  wifiConnecting = 0;

  onOff = IoTInterfaceOnOff.StateOff;
  color[0] = 0;
  color[1] = 0;
  color[2] = 0;
  enumValue = 0;
}

void loop() {
  if (!wifiConnected) {
    if (!wifiConnecting) {
      wifiConnecting = 1;
      WiFi.disconnect(true);
      delay(2000);
      wifiConnectionStartTime = millis();
      WiFi.onEvent(WiFiEvent);
      WiFi.begin(ssid, password);
      return;
    }

    uint32_t now = millis();
    if ((now - wifiConnectionStartTime) >= 30000) {
      wifiConnected = 0;
      wifiConnecting = 0;
      return;
    }

    delay(500);
    return;
  }

  uint16_t bytesInPacket = udpServer.parsePacket();
  if (!bytesInPacket)
    return;

  uint16_t bytesRead = udpServer.read(receivedBuffer, bytesInPacket < sizeof(receivedBuffer) ? bytesInPacket : sizeof(receivedBuffer));
  if (!bytesRead || bytesRead > sizeof(receivedBuffer)) {
    udpServer.flush();
    return;
  }

  IPAddress ip = udpServer.remoteIP();
  const uint16_t port = udpServer.remotePort();

  IoTServer.currentClientIP = (uint32_t)ip;
  IoTServer.currentClientPort = port;

  uint8_t flushed = false;
  if (IoTServer.process(receivedBuffer, bytesRead)) {
    yield();

    udpServer.flush();
    flushed = true;
    if (!IoTServer.responseReady())
      handleMessage();

    udpServer.beginPacket(ip, port);
    udpServer.write(IoTServer.responseBuffer(), IoTServer.responseLength());
    udpServer.endPacket();
  }

  if (!flushed)
    udpServer.flush();
}
