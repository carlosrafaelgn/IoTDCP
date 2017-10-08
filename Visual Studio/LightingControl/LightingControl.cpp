//
// IoTDCP is distributed under the FreeBSD License
//
// Copyright (c) 2017, Carlos Rafael Gimenes das Neves
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// https://github.com/carlosrafaelgn/IoTDCP
//
#include "stdafx.h"

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

#include "IoTDCP.h"

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

uint8_t onOff, color[3];
uint8_t receivedBuffer[32 * 1024];
uint16_t enumValue;

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

int main() {
	IoTServer.begin();
	//**************************************
	// Set the initial password, if the
	// device is password protected
	IoTServer.storedPassword("Password", 8);
	//**************************************

	onOff = IoTInterfaceOnOff.StateOff;
	color[0] = 0;
	color[1] = 0;
	color[2] = 0;
	enumValue = 0;

	WSAData data;
	WSAStartup(MAKEWORD(2, 2), &data);
	sockaddr_in local;

	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(IoTPort);

	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (bind(s, (sockaddr*)&local, sizeof(local)) < 0) {
		printf("bind error: %d", WSAGetLastError());
		system("pause");
		closesocket(s);
		WSACleanup();
		return 0;
	}

	int ok = 1;
	if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*)&ok, sizeof(ok)) < 0) {
		printf("setsockopt error SO_BROADCAST: %d", WSAGetLastError());
		system("pause");
		closesocket(s);
		WSACleanup();
		return 0;
	}

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 500000;
	if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv)) < 0) {
		printf("setsockopt error SO_RCVTIMEO: %d", WSAGetLastError());
		system("pause");
		closesocket(s);
		WSACleanup();
		return 0;
	}

	tv.tv_sec = 0;
	tv.tv_usec = 500000;
	if (setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(tv)) < 0) {
		printf("setsockopt error SO_SNDTIMEO: %d", WSAGetLastError());
		system("pause");
		closesocket(s);
		WSACleanup();
		return 0;
	}

	volatile bool alive = true;

	std::thread t([s, &alive]() {
		sockaddr_in remote;
		while (alive) {
			memset(&remote, 0, sizeof(remote));
			int remoteLen = sizeof(remote);
			int bytesInPacket = recvfrom(s, (char*)receivedBuffer, sizeof(receivedBuffer), 0, (sockaddr*)&remote, &remoteLen);
			printf("*** Received bytes: %d\n", bytesInPacket);
			if (bytesInPacket > 0) {
				IoTServer.currentClientIP = remote.sin_addr.S_un.S_addr;
				IoTServer.currentClientPort = remote.sin_port;
				
				uint8_t* usedBuffer = receivedBuffer;
				while (bytesInPacket > 0) {
					uint16_t usedBytes;
					if (IoTServer.process(usedBuffer, (uint16_t)bytesInPacket, &usedBytes)) {
						if (!(responseLength = IoTServer.buildResponseIfAlreadyPossible()))
							handleMessage();

						printf("Sent bytes: %d\n", responseLength);
						sendto(s, (char*)IoTServer.responseBuffer(), responseLength, 0, (sockaddr*)&remote, remoteLen);
					}
					usedBuffer += usedBytes;
					bytesInPacket -= usedBytes;
				}
			}
		}
	});

	printf("Server running on port %d...\n", IoTPort);

	system("pause");

	alive = false;

	shutdown(s, SD_BOTH);
	closesocket(s);
	WSACleanup();

	t.join();

	return 0;
}
