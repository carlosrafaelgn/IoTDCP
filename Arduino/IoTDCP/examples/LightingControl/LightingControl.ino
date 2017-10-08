
#define IoTNameUTF8 "Sample Device"
// IoTCategoryUuid should be the same for all devices of the same category (i.e. same product)
#define IoTCategoryUuid {0xB0, 0x2C, 0xB8, 0x8E, 0xBC, 0x6A, 0xC1, 0xB6, 0xEE, 0x49, 0xBC, 0x6F, 0xA4, 0x36, 0x41, 0x77} // 774136A4-6FBC-49EE-B6C1-6ABC8EB82CB0
#define IoTUuid {0x19, 0xB4, 0x77, 0xF1, 0x7F, 0x54, 0xD2, 0x94, 0x22, 0x40, 0x9B, 0x68, 0xED, 0xA6, 0xD9, 0x7B} // 7BD9A6ED-689B-4022-94D2-547FF177B419
//**************************************
// If the device requires no password
//#define IoTNoPassword
//**************************************
// If the device is password protected
#define IoTPasswordReadOnly
#define IoTMaxPasswordLength 32
//**************************************
#define IoTInterfaceCount 1
#define IoTMaxPayloadLength 256

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <IoTDCP.h>

// Just to make it easier to reference the properties
#define PropState 0
#define PropColor 1
#define PropSampleEnum 2

const IoTPropertyDescriptor IoTInterface0Properties[] = {
  { "State", IoTProperty.ModeReadOnly, IoTProperty.DataTypeU8, 1, IoTProperty.UnitEnum, IoTProperty.UnitOne, 0 },
  { "Color", IoTProperty.ModeReadWrite, IoTProperty.DataTypeU8, 3, IoTProperty.UnitRGB, IoTProperty.UnitOne, 0 },
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

uint16_t responseLength;

void describeEnum(IoTMessageDescribeEnum* msg) {
  if (msg->interfaceIndex) {
    responseLength = IoTServer.buildResponse(IoTServer.ResponseInvalidInterface);
    return;
  }

  if (msg->propertyIndex != 2) {
    // Since describing state's enum is not necessary and we do not have any other enums...
    responseLength = IoTServer.buildResponse(IoTServer.ResponseInvalidInterfaceProperty);
  } else {
    responseLength = IoTServer.buildResponseEnumDescriptor16(0, 2, IoTInterface0SampleEnum, countof(IoTInterface0SampleEnum));
  }
}

void executeCommand(IoTMessageExecute* msg) {
  if (msg->interfaceIndex) {
    responseLength = IoTServer.buildResponse(IoTServer.ResponseInvalidInterface);
    return;
  }
  switch (msg->interfaceCommand) {
  case IoTInterfaceOnOff.CommandOff:
    if (!IoTServer.isMessageRepeated()) {
      onOff = IoTInterfaceOnOff.StateOff;
      // Any other commands should go here
    }
    responseLength = IoTServer.buildResponse8(IoTServer.ResponseOK, onOff);
    break;
  case IoTInterfaceOnOff.CommandOn:
    if (!IoTServer.isMessageRepeated()) {
      onOff = IoTInterfaceOnOff.StateOn;
      // Any other commands should go here
    }
    responseLength = IoTServer.buildResponse8(IoTServer.ResponseOK, onOff);
    break;
  default:
    responseLength = IoTServer.buildResponse(IoTServer.ResponseInvalidInterfaceCommand);
    break;
  }
}

void getProperty(IoTMessageGetProperty* msg) {
  if (msg->interfaceIndex) {
    responseLength = IoTServer.buildResponse(IoTServer.ResponseInvalidInterface);
    return;
  }
  switch (msg->propertyIndex) {
  case PropState:
    responseLength = IoTServer.buildResponse8(IoTServer.ResponseOK, onOff);
    break;
  case PropColor:
    responseLength = IoTServer.buildResponseBuffer(IoTServer.ResponseOK, color, 3);
    break;
  case PropSampleEnum:
    responseLength = IoTServer.buildResponse16(IoTServer.ResponseOK, enumValue);
    break;
  default:
    responseLength = IoTServer.buildResponse(IoTServer.ResponseInvalidInterfaceProperty);
    return;
  }
}

void setProperty(IoTMessageSetProperty* msg, uint16_t payloadLength) {
  if (msg->interfaceIndex) {
    responseLength = IoTServer.buildResponse(IoTServer.ResponseInvalidInterface);
    return;
  }

  // Ignore the first two bytes (IoTMessageSetProperty.interfaceIndex and IoTMessageSetProperty.propertyIndex)
  payloadLength -= 2;

  switch (msg->propertyIndex) {
  case PropState:
    responseLength = IoTServer.buildResponse(IoTServer.ResponseInterfacePropertyReadOnly);
    break;
  case PropColor:
    if (payloadLength != 3) {
      responseLength = IoTServer.buildResponse(IoTServer.ResponseInvalidInterfacePropertyValue);
    } else {
      color[0] = msg->propertyValue[0];
      color[1] = msg->propertyValue[1];
      color[2] = msg->propertyValue[2];
      // Any other commands should go here
      responseLength = IoTServer.buildResponseBuffer(IoTServer.ResponseOK, color, 3);
    }
    break;
  case PropSampleEnum:
    if (payloadLength != 2) {
      responseLength = IoTServer.buildResponse(IoTServer.ResponseInvalidInterfacePropertyValue);
    } else {
      switch (*((uint16_t*)msg->propertyValue)) {
      case 0:
      case 1:
      case 2:
      case 255:
        enumValue = *((uint16_t*)msg->propertyValue);
        // Any other commands should go here
        responseLength = IoTServer.buildResponse16(IoTServer.ResponseOK, enumValue);
        break;
      default:
        responseLength = IoTServer.buildResponse(IoTServer.ResponseInvalidInterfacePropertyValue);
        break;
      }
    }
    break;
  default:
    responseLength = IoTServer.buildResponse(IoTServer.ResponseInvalidInterfaceProperty);
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
    responseLength = IoTServer.buildResponse(IoTServer.ResponseUnsupportedMessage);
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
  if (!bytesInPacket) {
    delay(10);
    return;
  }

  uint8_t flushed = false;
  while (bytesInPacket) {
    uint16_t bytesRead = udpServer.read(receivedBuffer, bytesInPacket < sizeof(receivedBuffer) ? bytesInPacket : sizeof(receivedBuffer));
    if (!bytesRead)
      break;
    uint16_t usedBytes;
    if (IoTServer.process(receivedBuffer, bytesRead, &usedBytes)) {
      yield();

      IPAddress ip = udpServer.remoteIP();
      const uint16_t port = udpServer.remotePort();

      IoTServer.currentClientIP = (uint32_t)ip;
      IoTServer.currentClientPort = port;

      udpServer.flush();
      flushed = true;
      if (!(responseLength = IoTServer.buildResponseIfAlreadyPossible()))
        handleMessage();

      udpServer.beginPacket(ip, port);
      udpServer.write(IoTServer.responseBuffer(), responseLength);
      udpServer.endPacket();
      break;
    }
    bytesInPacket -= usedBytes;
  }

  if (!flushed)
    udpServer.flush();
}
