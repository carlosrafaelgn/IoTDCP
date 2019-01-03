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
#ifndef IoTDCP_h
#define IoTDCP_h

#include <inttypes.h>

// Client message format (Request)
// - StartOfPacket
// - Message type
// - Client Id
// - Client Sequence Number (Low byte)
// - Client Sequence Number (High byte)
// - Password length
// - Password (UTF-8 encoded)
// - Payload length (Low byte)
// - Payload length (High byte)
// - Payload bytes
// - EndOfPacket

// Server message format (Response)
// - StartOfPacket
// - Message type (this is the same message type sent by the client in case of a regular response, or ServerMessagePropertyChange)
// - Client Id
// - Client Sequence Number (Low byte)
// - Client Sequence Number (High byte)
// - Response code
// - Payload length (Low byte)
// - Payload length (High byte)
// - Payload bytes
// - EndOfPacket

#ifndef countof
#define countof(X) (sizeof(X) / sizeof((X)[0]))
#endif

#ifndef IoTCategoryUuid
#error("IoTCategoryUuid not defined")
#endif

#ifndef IoTUuid
#error("IoTUuid not defined")
#endif

#ifndef IoTInterfaceCount
#error("IoTInterfaceCount not defined")
#endif

#if (IoTInterfaceCount <= 0)
#error("IoTInterfaceCount <= 0")
#endif

#if (IoTInterfaceCount > 128)
#error("IoTInterfaceCount > 128")
#endif

#ifndef IoTClientCount
#define IoTClientCount 8
#endif

#if (IoTClientCount <= 0)
#error("IoTClientCount <= 0")
#endif

#if (IoTClientCount > 255)
#error("IoTClientCount > 255")
#endif

#ifndef IoTMaxPayloadLength
#define IoTMaxPayloadLength 512
#endif

#if (IoTMaxPayloadLength < 64)
#error("IoTMaxPayloadLength < 64")
#endif

#if (IoTMaxPayloadLength > 32768)
#error("IoTMaxPayloadLength > 32768")
#endif

#ifndef IoTMaxNameLength
#define IoTMaxNameLength 32
#endif

#if (IoTMaxNameLength < 0)
#error("IoTMaxNameLength < 0")
#endif

#if (IoTMaxNameLength > 64)
#error("IoTMaxNameLength > 64")
#endif

#if (IoTMaxPayloadLength <= IoTMaxNameLength)
#error("IoTMaxPayloadLength <= IoTMaxNameLength")
#endif

#ifndef IoTMaxPasswordLength
#define IoTMaxPasswordLength 32
#endif

#if (IoTMaxPasswordLength < 0)
#error("IoTMaxPasswordLength < 0")
#endif

#if (IoTMaxPasswordLength > 64)
#error("IoTMaxPasswordLength > 64")
#endif

#if (IoTMaxPayloadLength <= IoTMaxPasswordLength)
#error("IoTMaxPayloadLength <= IoTMaxPasswordLength")
#endif

#ifdef IoTNoPassword
#undef IoTMaxPasswordLength
#define IoTMaxPasswordLength 0
#define passwordLength 0
#ifndef IoTPasswordReadOnly
#define IoTPasswordReadOnly
#endif
#elif (IoTMaxPasswordLength == 0)
#error("IoTMaxPasswordLength == 0")
#endif

// 2570 = 0x0A0A (at the present date it is not assigned to any services)
#define IoTPort 2570

#pragma pack(push, 1)

struct _IoTInterface {
public:
	enum _Types {
		TypeSensor = 0x00,
		TypeOnOff = 0x01,
		TypeOnOffSimple = 0x02,
		TypeOpenClose = 0x03,
		TypeOpenCloseStop = 0x04
	};
};

struct _IoTProperty {
public:
	enum _Modes {
		ModeReadOnly = 0x00,
		ModeWriteOnly = 0x01,
		ModeReadWrite = 0x02
	};

	enum _DataTypes {
		DataTypeS8 = 0x00, // Signed 8 bits integer (two's complement)
		DataTypeS16 = 0x01, // Signed 16 bits integer (two's complement / little endian) 
		DataTypeS32 = 0x02, // Signed 32 bits integer (two's complement / little endian)
		DataTypeS64 = 0x03, // Signed 64 bits integer (two's complement / little endian)
		DataTypeU8 = 0x04, // Unsigned 8 bits integer
		DataTypeU16 = 0x05, // Unsigned 16 bits integer (little endian)
		DataTypeU32 = 0x06, // Unsigned 32 bits integer (little endian)
		DataTypeU64 = 0x07, // Unsigned 64 bits integer (little endian)
		DataTypeFloat32 = 0x08, // 32 bits float point (IEEE 754 / little endian)
		DataTypeFloat64 = 0x09, // 64 bits float point (IEEE 754 / little endian)
		DataTypeRGBTriplet = 0x0A // Unsigned 24 bits integer (little endian / to be used only with UnitRGB)
	};

	enum _Units {
		UnitOne = 0x00, // Used in composed units, or as the default unit when a unit is not necessary
		UnitBit = 0x01,
		UnitByte = 0x02,
		UnitRadian = 0x03,          
		UnitDegree = 0x04,
		UnitSecond = 0x05,
		UnitSecond2 = 0x06, // s²
		UnitSecond3 = 0x07, // s³
		UnitMeter = 0x08,
		UnitMeter2 = 0x09, // m²
		UnitMeter3 = 0x0A, // m³
		UnitGram = 0x0B,
		UnitOhm = 0x0C,
		UnitSiemens = 0x0D,
		UnitVolt = 0x0E,
		UnitCoulomb = 0x0F,
		UnitAmpere = 0x10,
		UnitAmpere2 = 0x11,
		UnitWatt = 0x12,
		UnitFarad = 0x13,
		UnitHenry = 0x14,
		UnitWeber = 0x15,
		UnitTesla = 0x16,
		UnitNewton = 0x17,
		UnitPascal = 0x18,
		UnitJoule = 0x19,
		UnitKelvin = 0x1A,
		UnitDegreeCelsius = 0x1B,
		UnitBel = 0x1C,
		UnitMole = 0x1D,
		UnitCandela = 0x1E,
		UnitLumen = 0x1F,
		UnitLux = 0x20,
		UnitBecquerel = 0x21,
		UnitGray = 0x22,
		UnitSievert = 0x23,
		UnitKatal = 0x24,

		UnitBool = 0xFB, // dataType must be DataTypeU8, where false == 0, true != 0 (unitDen and exponent must be ignored)
		UnitRGB = 0xFC, // dataType must be DataTypeRGBTriplet, unitDen and exponent must be ignored (first byte must be R, followed by G, then B)
		UnitRGBA = 0xFD, // dataType must be DataTypeU32, unitDen and exponent must be ignored (first byte must be R, followed by G, then B and finally A)
		UnitUTF8Text = 0xFE, // dataType must be DataTypeU8, elementCount contains the maximum length (including the null char), unitDen and exponent must be ignored (text must be null terminated and must be encoded in UTF-8)
		UnitEnum = 0xFF // dataType must be DataTypeS8/DataTypeU8/DataTypeS16/DataTypeU16/DataTypeS32/DataTypeU32, unitDen and exponent must be ignored
	};

	enum _IECMultipliers {
		IECKibi = 0x7F, // 2^10
		IECMebi = 0x7E, // 2^20
		IECGibi = 0x7D, // 2^30
		IECTebi = 0x7C, // 2^40
		IECPebi = 0x7B, // 2^50
		IECExbi = 0x7A, // 2^60
		IECZebi = 0x79, // 2^70
		IECYobi = 0x78  // 2^80
	};
};

struct IoTEnumDescriptor8 {
public:
	const char* name; // UTF-8 encoded
	uint8_t value;
};

struct IoTEnumDescriptor16 {
public:
	const char* name; // UTF-8 encoded
	uint16_t value;
};

struct IoTEnumDescriptor32 {
public:
	const char* name; // UTF-8 encoded
	uint32_t value;
};

struct IoTPropertyDescriptor {
public:
	const char* name; // UTF-8 encoded
	uint8_t mode;
	uint8_t dataType;
	uint8_t elementCount; // Used with arrays, such as RGB components and so on
	uint8_t unitNum, unitDen; // Used to describe composite units, such as m/s^2
	// If -30 <= exponent <= 30, then the value must be multiplied by 10^exponent
	// If exponent is one of the _IECMultipliers values, then the value must be multiplied by the corresponding value (such as 2^10, 2^20 and so on)
	int8_t exponent;
};

struct IoTInterfaceDescriptor {
public:
	const char* name; // UTF-8
	uint8_t type;
	uint8_t propertyCount;
	const IoTPropertyDescriptor* propertyDescriptors;
};

struct _IoTInterfaceSensor {
public:
	enum _Properties {
		PropertyValue = 0x00 // Could have any data type and any unit
	};
};

struct _IoTInterfaceOnOff {
public:
	enum _Commands {
		CommandOff = 0x00,
		CommandOn = 0x01
	};
	enum _Properties {
		PropertyState = 0x00 // Must be { "<Name>", IoTProperty.ModeReadOnly, IoTProperty.DataTypeU8, 1, IoTProperty.UnitEnum, IoTProperty.UnitOne, 0 }
	};
	enum _States {
		StateUnknown = 0x00,
		StateOff = 0x01,
		StateOn = 0x02,
		StateTurningOff = 0x03,
		StateTurningOn = 0x04
	};
};

struct _IoTInterfaceOnOffSimple {
public:
	enum _Commands {
		CommandOnOff = 0x00
	};
	enum _Properties {
		PropertyState = 0x00 // Must be { "<Name>", IoTProperty.ModeReadOnly, IoTProperty.DataTypeU8, 1, IoTProperty.UnitEnum, IoTProperty.UnitOne, 0 }
	};
	enum _States {
		StateUnknown = 0x00,
		StateOff = 0x01,
		StateOn = 0x02,
		StateTurningOff = 0x03,
		StateTurningOn = 0x04
	};
};

struct _IoTInterfaceOpenClose {
public:
	enum _Commands {
		CommandClose = 0x00,
		CommandOpen = 0x01
	};
	enum _Properties {
		PropertyState = 0x00 // Must be { "<Name>", IoTProperty.ModeReadOnly, IoTProperty.DataTypeU8, 1, IoTProperty.UnitEnum, IoTProperty.UnitOne, 0 }
	};
	enum _States {
		StateUnknown = 0x00,
		StateClosed = 0x01,
		StateOpen = 0x02,
		StateClosing = 0x03,
		StateOpening = 0x04
	};
};

struct _IoTInterfaceOpenCloseStop {
public:
	enum _Commands {
		CommandClose = 0x00,
		CommandOpen = 0x01,
		CommandStop = 0x02
	};
	enum _Properties {
		PropertyState = 0x00 // Must be { "<Name>", IoTProperty.ModeReadOnly, IoTProperty.DataTypeU8, 1, IoTProperty.UnitEnum, IoTProperty.UnitOne, 0 }
	};
	enum _States {
		StateUnknown = 0x00,
		StateClosed = 0x01,
		StateOpen = 0x02,
		StateClosing = 0x03,
		StateOpening = 0x04,
		StatePartiallyClosed = 0x05,
		StatePartiallyOpen = 0x06
	};
};

_IoTInterface IoTInterface;
_IoTProperty IoTProperty;
_IoTInterfaceSensor IoTInterfaceSensor;
_IoTInterfaceOnOff IoTInterfaceOnOff;
_IoTInterfaceOnOffSimple IoTInterfaceOnOffSimple;
_IoTInterfaceOpenClose IoTInterfaceOpenClose;
_IoTInterfaceOpenCloseStop IoTInterfaceOpenCloseStop;

struct IoTMessageDescribeEnum {
public:
	uint8_t interfaceIndex;
	uint8_t propertyIndex;
};

struct IoTMessageExecute {
public:
	uint8_t interfaceIndex;
	uint8_t interfaceCommand;
};

struct IoTMessageGetProperty {
public:
	uint8_t interfaceIndex;
	uint8_t propertyIndex;
};

struct IoTMessageSetProperty {
public:
	uint8_t interfaceIndex;
	uint8_t propertyIndex;
	uint16_t propertyValueLength;
	uint8_t propertyValue[1];
};

#define StartOfPacket 0x55
#define EndOfPacket 0x33
#define ResponseHeaderLength 8
#define RequestHeaderLength 8
#define EndOfPacketLength 1

const uint8_t IoTServerCategoryUuid[] = IoTCategoryUuid; // Element 0 must be the least significant, whereas element 15 must be the most significant
const uint8_t IoTServerUuid[] = IoTUuid; // Element 0 must be the least significant, whereas element 15 must be the most significant
extern const IoTInterfaceDescriptor IoTInterfaces[IoTInterfaceCount];

class _IoTServer {
public:
	enum _CliendIds {
		InvalidClientId = 0xFF
	};

	enum _SequenceNumbers {
		MaximumSequenceNumber = 0xFFFF
	};

	enum _Messages {
		MessageQueryDevice = 0x00,
		MessageDescribeInterface = 0x01,
		MessageDescribeEnum = 0x02,
		MessageChangeName = 0x03,
		MessageChangePassword = 0x04,
		MessageHandshake = 0x05,
		MessagePing = 0x06,
		MessageReset = 0x07,
		MessageGoodBye = 0x08,
		MessageExecute = 0x09,
		MessageGetProperty = 0x0A,
		MessageSetProperty = 0x0B,
		MessageMax = 0x0B
	};

	enum _ServerMessages {
		ServerMessagePropertyChange = 0x80
	};

	enum _Replies {
		ResponseOK = 0x00,
		ResponseDeviceError = 0x01,
		ResponseUnknownClient = 0x02,
		ResponseUnsupportedMessage = 0x03,
		ResponsePayloadTooLarge = 0x04,
		ResponseInvalidPayload = 0x05,
		ResponseEndOfPacketNotFound = 0x06,
		ResponseWrongPassword = 0x07,
		ResponseNameReadOnly = 0x08,
		ResponsePasswordReadOnly = 0x09,
		ResponseCannotChangeNameNow = 0x0A,
		ResponseCannotChangePasswordNow = 0x0B,
		ResponseInvalidInterface = 0x0C,
		ResponseInvalidInterfaceCommand = 0x0D,
		ResponseInvalidInterfaceProperty = 0x0E,
		ResponseInterfacePropertyReadOnly = 0x0F,
		ResponseInterfacePropertyWriteOnly = 0x10,
		ResponseInvalidInterfacePropertyValue = 0x11,
		ResponseTryAgainLater = 0x12,
		ResponseMax = 0x20
	};

	enum _Flags {
		FlagNameReadOnly = 0x01,
		FlagPasswordProtected = 0x02,
		FlagPasswordReadOnly = 0x04,
		FlagResetSupported = 0x08,
		FlagEncryptionRequired = 0x10
	};

private:
	struct _IoTClient {
	public:
		uint16_t sequenceNumber;
		uint16_t port;
		uint32_t ip;
	};

	static _IoTClient clients[IoTClientCount];

	static uint8_t nameLength;
#ifdef IoTNameReadOnly
	static const uint8_t* name;
#else
	static const uint8_t name[IoTMaxNameLength];
#endif

#ifndef IoTNoPassword
	static uint8_t passwordLength;
#ifdef IoTPasswordReadOnly
	static const uint8_t* password;
#else
	static uint8_t password[IoTMaxPasswordLength];
#endif
#endif

	static uint8_t clientId;
	static uint16_t clientSequenceNumber;
	static uint8_t clientMessageRepeated;
	static uint8_t clientMessage;
	static const uint8_t* clientPayloadBuffer;
	static uint16_t clientPayloadLength;
	static uint8_t clientResponseReady;

	static uint16_t bufferOffset;
	static uint8_t buffer[ResponseHeaderLength + IoTMaxPayloadLength + EndOfPacketLength];

	static void buildQueryDeviceResponse() {
		uint8_t flags = 0;
#ifndef IoTNoPassword
		flags |= FlagPasswordProtected;
#endif
#ifdef IoTPasswordReadOnly
		flags |= FlagPasswordReadOnly;
#endif
#ifdef IoTResetSupported
		flags |= FlagResetSupported;
#endif
#ifdef IoTNameReadOnly
		flags |= FlagNameReadOnly;
#endif
#ifdef IoTEncryptionRequired
		flags |= FlagEncryptionRequired;
#endif
		writeResponse(flags);

		writeResponse(IoTServerCategoryUuid, 16);

		writeResponse(IoTServerUuid, 16);

		writeResponse(IoTInterfaceCount);

		for (uint8_t i = 0; i < IoTInterfaceCount; i++)
			writeResponse(IoTInterfaces[i].type);

		if (!nameLength || !name) {
			writeResponse(3);
			writeResponse('I');
			writeResponse('o');
			writeResponse('T');
		} else {
			writeResponse(nameLength);
			writeResponse(name, nameLength);
		}

		buildResponse(ResponseOK);
	}

	static void buildDescribeInterfaceResponse(uint8_t interfaceIndex) {
		if (interfaceIndex >= IoTInterfaceCount)
			return buildResponse(ResponseInvalidInterface);

		const IoTInterfaceDescriptor* const interfaceDescriptor = &(IoTInterfaces[interfaceIndex]);

		writeResponse(interfaceIndex);

		uint8_t nameLen = (uint8_t)strlen(interfaceDescriptor->name);
		writeResponse(nameLen);
		writeResponse(interfaceDescriptor->name, nameLen);

		writeResponse(interfaceDescriptor->type);

		const uint8_t propertyCount = interfaceDescriptor->propertyCount;
		writeResponse(propertyCount);

		for (uint8_t i = 0; i < propertyCount; i++) {
			const IoTPropertyDescriptor* const propertyDescriptor = &(interfaceDescriptor->propertyDescriptors[i]);

			nameLen = (uint8_t)strlen(propertyDescriptor->name);
			writeResponse(nameLen);
			writeResponse(propertyDescriptor->name, nameLen);
			writeResponse(&(propertyDescriptor->mode), 6);
		}

		buildResponse(ResponseOK);
	}

	static void buildHandshakeResponse(uint16_t sequenceNumber) {
		uint8_t i;
		// First, try to find the client itself
		for (i = 0; i < IoTClientCount; i++) {
			if (clients[i].ip == currentClientIP &&
				clients[i].port == currentClientPort)
				break;
		}
		if (i >= IoTClientCount) {
			// If not found, try to find an empty client slot
			for (i = 0; i < IoTClientCount; i++) {
				if (!clients[i].ip)
					break;
			}
			if (i >= IoTClientCount) {
				// Since there were no empty slots, we will have to overwrite someone...
				// TODO: create a RLU list of client id's
				i = 0;
			}
		}
		clients[i].sequenceNumber = sequenceNumber;
		clients[i].ip = currentClientIP;
		clients[i].port = currentClientPort;

		writeResponse(i);

		buildResponse(ResponseOK);
	}

	static void buildGoodByeResponse() {
		buildResponse(ResponseOK);
		clients[clientId].sequenceNumber = MaximumSequenceNumber;
		clients[clientId].ip = 0;
		clients[clientId].port = 0;
	}

	static void buildResponseEnumDescriptor(uint8_t interfaceIndex, uint8_t propertyIndex, const uint8_t* enumDescriptors, uint8_t count, uint8_t valueSize) {
		writeResponse(interfaceIndex);

		writeResponse(propertyIndex);

		writeResponse(count);

		for (uint8_t i = 0; i < count; i++) {
			// interfaceDescriptor->name
			uint8_t nameLen = (uint8_t)strlen(*((char**)enumDescriptors));
			writeResponse(nameLen);
			writeResponse(*((char**)enumDescriptors), nameLen);
			enumDescriptors += sizeof(char*);
			// interfaceDescriptor->value
			writeResponse(enumDescriptors, valueSize);
			enumDescriptors += valueSize;
		}
		
		buildResponse(ResponseOK);
	}

public:
	static uint32_t currentClientIP;
	static uint16_t currentClientPort;

	static void begin() {
		uint8_t i;
		for (i = 0; i < IoTClientCount; i++) {
			clients[i].sequenceNumber = MaximumSequenceNumber;
			clients[i].ip = 0;
			clients[i].port = 0;
		}
		nameLength = 0;
#ifdef IoTNameReadOnly
		name = 0;
#else
		for (i = 0; i < IoTMaxNameLength; i++)
			name[i] = 0;
#endif
#ifndef IoTNoPassword
		passwordLength = 0;
#ifdef IoTPasswordReadOnly
		password = 0;
#else
		for (i = 0; i < IoTMaxPasswordLength; i++)
			password[i] = 0;
#endif
#endif
		clientId = InvalidClientId;
		clientSequenceNumber = 0;
		clientMessage = 0;
		clientMessageRepeated = false;
		clientPayloadBuffer = 0;
		clientPayloadLength = 0;
		clientResponseReady = false;

		bufferOffset = ResponseHeaderLength;
	}

	inline static uint8_t isBigEndian() {
		const uint32_t x = 0x03020100;
		return ((uint8_t*)&x)[0];
	}

	static uint8_t process(const uint8_t* srcBuffer, uint16_t length) {
		if (length < (RequestHeaderLength + EndOfPacketLength) ||
			srcBuffer[0] != StartOfPacket ||
			srcBuffer[length - 1] != EndOfPacket)
			return false;

		srcBuffer++;
		clientMessage = *srcBuffer++;
		clientId = *srcBuffer++;
		clientSequenceNumber = ((uint16_t)srcBuffer[0]) | (((uint16_t)srcBuffer[1]) << 8);
		srcBuffer += 2;

		uint16_t clientPasswordLength = *srcBuffer++;
		if (clientPasswordLength > length - (RequestHeaderLength + EndOfPacketLength))
			return false;
		const uint8_t* clientPassword = srcBuffer;
		srcBuffer += clientPasswordLength;

		clientPayloadLength = ((uint16_t)srcBuffer[0]) | (((uint16_t)srcBuffer[1]) << 8);
		if (clientPayloadLength != length - clientPasswordLength - (RequestHeaderLength + EndOfPacketLength))
			return false;
		srcBuffer += 2;
		clientPayloadBuffer = srcBuffer;

		clientResponseReady = false;
		bufferOffset = ResponseHeaderLength;

		switch (clientMessage) {
		case MessageQueryDevice:
			clientResponseReady = true;
			if (clientPayloadLength ||
				clientId != InvalidClientId ||
				clientSequenceNumber != MaximumSequenceNumber)
				buildResponse(ResponseInvalidPayload);
			else if (clientPasswordLength)
				buildResponse(ResponseWrongPassword);
			else
				buildQueryDeviceResponse();
			break;
		case MessageDescribeInterface:
			clientResponseReady = true;
			if (clientPayloadLength != 1 ||
				clientId != InvalidClientId ||
				clientSequenceNumber != MaximumSequenceNumber)
				buildResponse(ResponseInvalidPayload);
			else if (clientPasswordLength)
				buildResponse(ResponseWrongPassword);
			else
				buildDescribeInterfaceResponse(*clientPayloadBuffer);
			break;
		case MessageDescribeEnum:
			if (clientPayloadLength != 2 ||
				clientId != InvalidClientId ||
				clientSequenceNumber != MaximumSequenceNumber) {
				clientResponseReady = true;
				buildResponse(ResponseInvalidPayload);
			} else if (clientPasswordLength) {
				clientResponseReady = true;
				buildResponse(ResponseWrongPassword);
			}

			// This message must be handled by the user
			break;
		case MessageChangeName:
			if (clientPayloadLength ||
				clientId != InvalidClientId ||
				clientSequenceNumber != MaximumSequenceNumber) {
				clientResponseReady = true;
				buildResponse(ResponseInvalidPayload);
				break;
			}

#ifdef IoTNameReadOnly
			clientResponseReady = true;
			buildResponse(ResponseNameReadOnly);
#else
			if (clientPayloadLength > IoTMaxNameLength) {
				clientResponseReady = true;
				buildResponse(ResponsePayloadTooLarge);
			}
#endif

			// This message must be handled by the user
			break;
		case MessageChangePassword:
			if (clientPayloadLength ||
				clientId != InvalidClientId ||
				clientSequenceNumber != MaximumSequenceNumber) {
				clientResponseReady = true;
				buildResponse(ResponseInvalidPayload);
				break;
			}

#ifndef IoTNoPassword
#ifdef IoTPasswordReadOnly
			clientResponseReady = true;
			buildResponse(ResponsePasswordReadOnly);
#else
			clientPayloadLength = clientPasswordLength;
			if (clientPayloadLength > IoTMaxPasswordLength) {
				clientResponseReady = true;
				buildResponse(ResponsePayloadTooLarge);
			} else {
				clientPayloadBuffer = clientPassword;
			}
#endif
#else
			clientResponseReady = true;
			buildResponse(ResponsePasswordReadOnly);
#endif

			// This message must be handled by the user
			break;
		default:
			// Validate the message and the password
			if (clientPasswordLength != passwordLength) {
				clientResponseReady = true;
				buildResponse(ResponseWrongPassword);
				break;
#ifndef IoTNoPassword
			} else {
				const uint8_t* passwordBuffer = password;
				while (clientPasswordLength--) {
					if (*clientPassword++ != *passwordBuffer++) {
						clientResponseReady = true;
						buildResponse(ResponseWrongPassword);
						break;
					}
				}

				if (clientResponseReady)
					break;
#endif
			}

			if (clientMessage == MessageHandshake) {
				clientResponseReady = true;
				if (clientPayloadLength ||
					clientId != InvalidClientId)
					buildResponse(ResponseInvalidPayload);
				else
					buildHandshakeResponse(clientSequenceNumber);
				break;
			}

			// Try to find the client
			if (clientId >= IoTClientCount ||
				clients[clientId].ip != currentClientIP ||
				clients[clientId].port != currentClientPort) {
				clientResponseReady = true;
				buildResponse(ResponseUnknownClient);
				break;
			}

			if (clientSequenceNumber == clients[clientId].sequenceNumber) {
				clientMessageRepeated = true;
			} else {
				if ((uint16_t)(clientSequenceNumber - clients[clientId].sequenceNumber) > 0x7FFF) {
					// Old message arriving too late (we will just ignore it)
					return false;
				} else {
					clientMessageRepeated = false;
					clients[clientId].sequenceNumber = clientSequenceNumber;

					if (clientMessage == MessageGoodBye) {
						clientResponseReady = true;
						buildGoodByeResponse();
					}
				}
			}
			break;
		}

		return true;
	}

	inline static uint8_t storedNameLength() {
		return nameLength;
	}

	inline static const uint8_t* storedName() {
		return name;
	}

	inline static uint8_t storedName(const char* newName, uint8_t newNameLength = 255) {
		return storedName((uint8_t*)newName, newNameLength);
	}

	static uint8_t storedName(const uint8_t* newName, uint8_t newNameLength = 255) {
		if (newNameLength && !newName)
			return false;
		nameLength = (newNameLength == 255 ? (uint8_t)strlen((const char*)newName) : newNameLength);
#ifdef IoTNameReadOnly
		name = newName;
#else
		if (newNameLength > IoTMaxNameLength)
			return false;
		for (newNameLength = 0; newNameLength < nameLength; newNameLength++)
			name[newNameLength] = newName[newNameLength];
		for (; newNameLength < IoTMaxNameLength; newNameLength++)
			name[newNameLength] = 0;
#endif
		return true;
	}

	inline static uint8_t storedPasswordLength() {
		return passwordLength;
	}

	inline static const uint8_t* storedPassword() {
#ifndef IoTNoPassword
		return password;
#else
		return 0;
#endif
	}

	inline static uint8_t storedPassword(const char* newPassword, uint8_t newPasswordLength = 255) {
		return storedPassword((uint8_t*)newPassword, newPasswordLength);
	}

	static uint8_t storedPassword(const uint8_t* newPassword, uint8_t newPasswordLength = 255) {
#ifndef IoTNoPassword
		if (newPasswordLength && !newPassword)
			return false;
		passwordLength = (newPasswordLength == 255 ? (uint8_t)strlen((const char*)newPassword) : newPasswordLength);
#ifdef IoTPasswordReadOnly
		password = newPassword;
#else
		if (newPasswordLength > IoTMaxPasswordLength)
			return false;
		for (newPasswordLength = 0; newPasswordLength < passwordLength; newPasswordLength++)
			password[newPasswordLength] = newPassword[newPasswordLength];
		for (; newPasswordLength < IoTMaxPasswordLength; newPasswordLength++)
			password[newPasswordLength] = 0;
#endif
		return true;
#else
		return false;
#endif
	}

	inline static uint8_t message() {
		return clientMessage;
	}

	inline static uint8_t isMessageRepeated() {
		return clientMessageRepeated;
	}

	inline static uint16_t responseLength() {
		return bufferOffset;
	}

	inline static const uint8_t* responseBuffer() {
		return buffer;
	}

	inline static uint16_t payloadLength() {
		return clientPayloadLength;
	}

	inline static const uint8_t* payloadBuffer() {
		return clientPayloadBuffer;
	}

	inline static uint8_t responseReady() {
		return clientResponseReady;
	}

	inline static void writeResponse(uint8_t value) {
		buffer[bufferOffset++] = value;
	}

	static void writeResponse(const void* srcBuffer, uint16_t length) {
		const uint8_t* srcBuffer8 = (const uint8_t*)srcBuffer;
		uint8_t* dstBuffer = buffer + bufferOffset;
		bufferOffset += length;
		while (length--)
			*dstBuffer++ = *srcBuffer8++;
	}

	static void writeResponseProperty8(uint8_t interfaceIndex, uint8_t propertyIndex, uint8_t value) {
		uint8_t* dstBuffer = buffer + bufferOffset;
		bufferOffset += 5;
		*dstBuffer++ = interfaceIndex;
		*dstBuffer++ = propertyIndex;
		*dstBuffer++ = 1;
		*dstBuffer++ = 0;
		*dstBuffer++ = value;
	}

	static void writeResponseProperty16(uint8_t interfaceIndex, uint8_t propertyIndex, uint16_t value) {
		uint8_t* dstBuffer = buffer + bufferOffset;
		bufferOffset += 6;
		*dstBuffer++ = interfaceIndex;
		*dstBuffer++ = propertyIndex;
		*dstBuffer++ = 2;
		*dstBuffer++ = 0;
		*dstBuffer++ = (uint8_t)value;
		*dstBuffer++ = (uint8_t)(value >> 8);
	}

	static void writeResponseProperty32(uint8_t interfaceIndex, uint8_t propertyIndex, uint32_t value) {
		uint8_t* dstBuffer = buffer + bufferOffset;
		bufferOffset += 8;
		*dstBuffer++ = interfaceIndex;
		*dstBuffer++ = propertyIndex;
		*dstBuffer++ = 4;
		*dstBuffer++ = 0;
		*dstBuffer++ = (uint8_t)value;
		*dstBuffer++ = (uint8_t)(value >> 8);
		*dstBuffer++ = (uint8_t)(value >> 16);
		*dstBuffer++ = (uint8_t)(value >> 24);
	}

	static void writeResponsePropertyFloat(uint8_t interfaceIndex, uint8_t propertyIndex, float value) {
		uint8_t* dstBuffer = buffer + bufferOffset;
		bufferOffset += 8;
		const uint32_t v = *((uint32_t*)&value);
		*dstBuffer++ = interfaceIndex;
		*dstBuffer++ = propertyIndex;
		*dstBuffer++ = 4;
		*dstBuffer++ = 0;
		*dstBuffer++ = (uint8_t)v;
		*dstBuffer++ = (uint8_t)(v >> 8);
		*dstBuffer++ = (uint8_t)(v >> 16);
		*dstBuffer++ = (uint8_t)(v >> 24);
	}

	static void writeResponsePropertyRGB(uint8_t interfaceIndex, uint8_t propertyIndex, uint8_t r, uint8_t g, uint8_t b) {
		uint8_t* dstBuffer = buffer + bufferOffset;
		bufferOffset += 7;
		*dstBuffer++ = interfaceIndex;
		*dstBuffer++ = propertyIndex;
		*dstBuffer++ = 3;
		*dstBuffer++ = 0;
		*dstBuffer++ = r;
		*dstBuffer++ = g;
		*dstBuffer++ = b;
	}

	static void writeResponsePropertyRGB(uint8_t interfaceIndex, uint8_t propertyIndex, const uint8_t* rgb) {
		uint8_t* dstBuffer = buffer + bufferOffset;
		bufferOffset += 7;
		*dstBuffer++ = interfaceIndex;
		*dstBuffer++ = propertyIndex;
		*dstBuffer++ = 3;
		*dstBuffer++ = 0;
		*dstBuffer++ = rgb[0];
		*dstBuffer++ = rgb[1];
		*dstBuffer++ = rgb[2];
	}

	static void writeResponsePropertyBuffer(uint8_t interfaceIndex, uint8_t propertyIndex, const void* srcBuffer, uint16_t length) {
		uint8_t* dstBuffer = buffer + bufferOffset;
		bufferOffset += length + 4;
		*dstBuffer++ = interfaceIndex;
		*dstBuffer++ = propertyIndex;
		*dstBuffer++ = (uint8_t)length;
		*dstBuffer++ = (uint8_t)(length >> 8);
		const uint8_t* srcBuffer8 = (const uint8_t*)srcBuffer;
		while (length--)
			*dstBuffer++ = *srcBuffer8++;
	}

	static void buildResponse(uint8_t responseCode) {
		const uint16_t payloadLength = bufferOffset - ResponseHeaderLength;
		buffer[0] = StartOfPacket;
		buffer[1] = clientMessage;
		buffer[2] = clientId;
		buffer[3] = (uint8_t)clientSequenceNumber;
		buffer[4] = (uint8_t)(clientSequenceNumber >> 8);
		buffer[5] = responseCode;
		buffer[6] = (uint8_t)payloadLength;
		buffer[7] = (uint8_t)(payloadLength >> 8);
		buffer[ResponseHeaderLength + payloadLength] = EndOfPacket;
		bufferOffset += EndOfPacketLength;
	}

	inline static void buildResponseEnumDescriptor8(uint8_t interfaceIndex, uint8_t propertyIndex, const IoTEnumDescriptor8* enumDescriptors, uint8_t count) {
		buildResponseEnumDescriptor(interfaceIndex, propertyIndex, (const uint8_t*)enumDescriptors, count, 1);
	}

	inline static void buildResponseEnumDescriptor16(uint8_t interfaceIndex, uint8_t propertyIndex, const IoTEnumDescriptor16* enumDescriptors, uint8_t count) {
		buildResponseEnumDescriptor(interfaceIndex, propertyIndex, (const uint8_t*)enumDescriptors, count, 2);
	}

	inline static void buildResponseEnumDescriptor32(uint8_t interfaceIndex, uint8_t propertyIndex, const IoTEnumDescriptor32* enumDescriptors, uint8_t count) {
		buildResponseEnumDescriptor(interfaceIndex, propertyIndex, (const uint8_t*)enumDescriptors, count, 4);
	}
};

_IoTServer::_IoTClient _IoTServer::clients[IoTClientCount];


uint8_t _IoTServer::nameLength;
#ifdef IoTNameReadOnly
const uint8_t* _IoTServer::name;
#else
const uint8_t _IoTServer::name[IoTMaxNameLength];
#endif

#ifndef IoTNoPassword
uint8_t _IoTServer::passwordLength;
#ifdef IoTPasswordReadOnly
const uint8_t* _IoTServer::password;
#else
uint8_t _IoTServer::password[IoTMaxPasswordLength];
#endif
#else
#undef passwordLength
#endif

uint8_t _IoTServer::clientId;
uint16_t _IoTServer::clientSequenceNumber;
uint8_t _IoTServer::clientMessage;
uint8_t _IoTServer::clientMessageRepeated;
const uint8_t* _IoTServer::clientPayloadBuffer;
uint16_t _IoTServer::clientPayloadLength;
uint8_t _IoTServer::clientResponseReady;
uint32_t _IoTServer::currentClientIP;
uint16_t _IoTServer::currentClientPort;

uint16_t _IoTServer::bufferOffset;
uint8_t _IoTServer::buffer[ResponseHeaderLength + IoTMaxPayloadLength + EndOfPacketLength];

_IoTServer IoTServer;

#undef StartOfPacket
#undef Escape
#undef EndOfPacket
#undef ResponseHeaderLength
#undef RequestHeaderLength
#undef EndOfPacketLength

#pragma pack(pop)

#endif
