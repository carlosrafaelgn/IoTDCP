//
// IoTDCP is distributed under the FreeBSD License
//
// Copyright (c) 2016, Carlos Rafael Gimenes das Neves
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// The views and conclusions contained in the software and documentation are those
// of the authors and should not be interpreted as representing official policies,
// either expressed or implied, of the FreeBSD Project.
//
// https://github.com/carlosrafaelgn/IoTDCP
//

#ifndef IoTDCP_h
#define IoTDCP_h

#include <inttypes.h>

// Client message format (Request)
// - StartOfPacket
// - Client Id
// - Client Sequence Number (Low byte)
// - Client Sequence Number (High byte)
// - Password length
// - Password (UTF-8 encoded)
// - Message
// - Payload length (Low byte)
// - Payload length (High byte)
// - Payload bytes
// - EndOfPacket

// Server message format (Response)
// - StartOfPacket
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

#ifndef IoTNameUTF8
#error("IoTNameUTF8 not defined")
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

#if (IoTClientCount > 128)
#error("IoTClientCount > 128")
#endif

#ifndef IoTMaxPayloadLength
#define IoTMaxPayloadLength 512
#endif

#if (IoTMaxPayloadLength < 64)
#error("IoTMaxPayloadLength < 64")
#endif

#if (IoTMaxPayloadLength > 4096)
#error("IoTMaxPayloadLength > 4096")
#endif

#ifndef IoTMaxPasswordLength
#define IoTMaxPasswordLength 32
#endif

#if (IoTMaxPasswordLength < 0)
#error("IoTMaxPasswordLength < 0")
#endif

#if (IoTMaxPasswordLength > 32)
#error("IoTMaxPasswordLength > 32")
#endif

#define IoTMaxPasswordLengthEscaped (IoTMaxPasswordLength * 2)

#if (IoTMaxPayloadLength < IoTMaxPasswordLengthEscaped)
#error("IoTMaxPayloadLength < IoTMaxPasswordLengthEscaped")
#endif

#ifdef IoTNoPassword
#undef IoTMaxPasswordLength
#define IoTMaxPasswordLength 0
#define passwordLength 0
#define clientPasswordLength 0
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
		DataTypeFloat64 = 0x09 // 64 bits float point (IEEE 754 / little endian)
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

		UnitBool = 0xFC, // dataType must be DataTypeU8, where false == 0, true != 0 (unitDen and exponent must be ignored)
		UnitRGB = 0xFD, // dataType must be DataTypeU8, elementCount must be 3 or 4, unitDen and exponent must be ignored (first byte must be R, followed by G, then B and finally A, if elementCount is 4)
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
	uint8_t propertyValue[1];
};

#define StartOfPacket 0x55
#define Escape 0x1B
#define EndOfPacket 0x33
#define HeaderLength 7
#define EndOfPacketLength 1
#define ServerStateMask 0x0F
#define ServerFlagEscape 0x10

const uint8_t IoTServerNameUTF8[] = IoTNameUTF8;
const uint8_t IoTServerUuid[] = IoTUuid; // Element 0 must be the least significant, whereas element 15 must be the most significant
extern const IoTInterfaceDescriptor IoTInterfaces[IoTInterfaceCount];

class _IoTServer {
public:
	enum _CliendIds {
		ClientIdQueryDevice = 0xFF,
		ClientIdDescribeInterface = 0xFD,
		ClientIdDescribeEnum = 0xFB,
		ClientIdChangePassword = 0xF9,
		ClientIdHandshake = 0xF7
	};

	enum _SequenceNumbers {
		MaximumSequenceNumber = 0xFFFC // 0xFFFC is 0x3FFF shifted left by two bits
	};

	enum _Messages {
		MessageQueryDevice = 0x00,
		MessageDescribeInterface = 0x02,
		MessageDescribeEnum = 0x04,
		MessageChangePassword = 0x06,
		MessageHandshake = 0x08,
		MessagePing = 0x0A,
		MessageReset = 0x0C,
		MessageGoodBye = 0x0E,
		MessageExecute = 0x10,
		MessageGetProperty = 0x12,
		MessageSetProperty = 0x14,
		MessageMax = 0x14
	};

	enum _Replies {
		ResponseOK = 0x00,
		ResponseDeviceError = 0x02,
		ResponseUnknownClient = 0x04,
		ResponseUnsupportedMessage = 0x06,
		ResponsePayloadTooLarge = 0x08,
		ResponseInvalidPayload = 0x0A,
		ResponseEndOfPacketNotFound = 0x0C,
		ResponseWrongPassword = 0x0E,
		ResponsePasswordReadOnly = 0x10,
		ResponseCannotChangePasswordNow = 0x12,
		ResponseInvalidInterface = 0x14,
		ResponseInvalidInterfaceCommand = 0x16,
		ResponseInvalidInterfaceProperty = 0x18,
		ResponseInterfacePropertyReadOnly = 0x1A,
		ResponseInterfacePropertyWriteOnly = 0x1C,
		ResponseInvalidInterfacePropertyValue = 0x1E,
		ResponseTryAgainLater = 0x20,
		ResponseMax = 0x20
	};

	enum _Flags {
		FlagPasswordProtected = 0x01,
		FlagPasswordReadOnly = 0x02,
		FlagResetSupported = 0x04,
		FlagEncryptionRequired = 0x08
	};

private:
	struct _IoTClient {
	public:
		uint16_t sequenceNumber;
		uint8_t id;
		uint32_t ip;
		uint16_t port;
	};

	static uint8_t state;

	static _IoTClient clients[IoTClientCount];
	static uint8_t nextClientIndex, nextClientId;

#ifndef IoTNoPassword
	static uint8_t passwordLength;
	static uint8_t password[IoTMaxPasswordLength];
	static uint8_t clientPasswordLength;
	static uint8_t clientPassword[IoTMaxPasswordLength];
#endif
	static uint8_t clientPasswordLengthEscaped;
	static uint8_t clientIndex;
	static uint8_t clientId;
	static uint16_t clientSequenceNumber;
	static uint8_t clientMessageRepeated;
	static uint8_t clientMessage;
	static uint16_t clientPayloadIndex;
	static uint16_t clientPayloadLength;
	static uint16_t clientPayloadReceivedLength;
	static uint8_t clientError;
	static uint16_t clientResponseReady;

	static uint8_t buffer[HeaderLength + IoTMaxPayloadLength + EndOfPacketLength];

	static uint16_t escapeBuffer(uint16_t dstOffset, const void* srcBuffer, uint16_t length) {
		while (length--) {
			const uint8_t v = *((const uint8_t*)srcBuffer);
			srcBuffer = ((const uint8_t*)srcBuffer) + 1;
			if (v == StartOfPacket || v == Escape) {
				buffer[dstOffset++] = Escape;
				buffer[dstOffset++] = v ^ 1;
			} else {
				buffer[dstOffset++] = v;
			}
		}
		return dstOffset;
	}

	static uint16_t buildQueryDeviceResponse() {
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
#ifdef IoTEncryptionRequired
		flags |= FlagEncryptionRequired;
#endif
		uint16_t dstOffset = escapeBuffer(HeaderLength, &flags, 1);

		dstOffset = escapeBuffer(dstOffset, IoTServerUuid, 16);

		const uint8_t interfaceCount = IoTInterfaceCount;
		dstOffset = escapeBuffer(dstOffset, &interfaceCount, 1);

		for (uint8_t i = 0; i < IoTInterfaceCount; i++)
			dstOffset = escapeBuffer(dstOffset, &(IoTInterfaces[i].type), 1);

		const uint8_t nameLen = sizeof(IoTServerNameUTF8) - 1; // Remove the null char the end
		dstOffset = escapeBuffer(dstOffset, &nameLen, 1);
		dstOffset = escapeBuffer(dstOffset, IoTServerNameUTF8, nameLen);

		return buildResponse(ResponseOK, dstOffset - HeaderLength);
	}

	static uint16_t buildDescribeInterfaceResponse(uint8_t interfaceIndex) {
		if (interfaceIndex >= IoTInterfaceCount)
			return buildResponse(ResponseInvalidInterface);

		const IoTInterfaceDescriptor* const interfaceDescriptor = &(IoTInterfaces[interfaceIndex]);

		uint16_t dstOffset = escapeBuffer(HeaderLength, &interfaceIndex, 1);

		uint8_t nameLen = (uint8_t)strlen(interfaceDescriptor->name);
		dstOffset = escapeBuffer(dstOffset, &nameLen, 1);
		dstOffset = escapeBuffer(dstOffset, interfaceDescriptor->name, nameLen);

		dstOffset = escapeBuffer(dstOffset, &(interfaceDescriptor->type), 1);

		const uint8_t propertyCount = interfaceDescriptor->propertyCount;
		dstOffset = escapeBuffer(dstOffset, &propertyCount, 1);

		for (uint8_t i = 0; i < propertyCount; i++) {
			const IoTPropertyDescriptor* const propertyDescriptor = &(interfaceDescriptor->propertyDescriptors[i]);

			nameLen = (uint8_t)strlen(propertyDescriptor->name);
			dstOffset = escapeBuffer(dstOffset, &nameLen, 1);
			dstOffset = escapeBuffer(dstOffset, propertyDescriptor->name, nameLen);
			dstOffset = escapeBuffer(dstOffset, &(propertyDescriptor->mode), 6);
		}

		return buildResponse(ResponseOK, dstOffset - HeaderLength);
	}

	static uint16_t buildHandshakeResponse() {
		uint8_t i;
		for (i = 0; i < IoTClientCount; i++) {
			// First, try to find an empty client slot
			if (!clients[i].ip)
				break;
		}
		if (i >= IoTClientCount) {
			// Since there were no empty slots, we will have to overwrite someone...
			nextClientIndex++;
			if (nextClientIndex >= IoTClientCount)
				nextClientIndex = 0;
			i = nextClientIndex;
		}
		clients[i].id = nextClientId;
		clients[i].sequenceNumber = 0;
		clients[i].ip = currentClientIP;
		clients[i].port = currentClientPort;
		const uint16_t ret = buildResponse8(ResponseOK, nextClientId);
		nextClientId += 2;
		return ret;
	}

	static uint16_t buildGoodByeResponse() {
		const uint16_t ret = buildResponse(ResponseOK);
		clients[clientIndex].sequenceNumber = 0xFFFF;
		clients[clientIndex].id = 0xFF;
		clients[clientIndex].ip = 0;
		clients[clientIndex].port = 0;
		return ret;
	}

	static uint16_t buildResponseEnumDescriptor(uint8_t interfaceIndex, uint8_t propertyIndex, const uint8_t* enumDescriptors, uint8_t count, uint8_t valueSize) {
		uint16_t dstOffset = escapeBuffer(HeaderLength, &interfaceIndex, 1);

		dstOffset = escapeBuffer(dstOffset, &propertyIndex, 1);

		dstOffset = escapeBuffer(dstOffset, &count, 1);

		for (uint8_t i = 0; i < count; i++) {
			// interfaceDescriptor->name
			uint8_t nameLen = (uint8_t)strlen(*((char**)enumDescriptors));
			dstOffset = escapeBuffer(dstOffset, &nameLen, 1);
			dstOffset = escapeBuffer(dstOffset, *((char**)enumDescriptors), nameLen);
			enumDescriptors += sizeof(char*);
			// interfaceDescriptor->value
			dstOffset = escapeBuffer(dstOffset, enumDescriptors, valueSize);
			enumDescriptors += valueSize;
		}
		
		return buildResponse(ResponseOK, dstOffset - HeaderLength);
	}

public:
	static uint32_t currentClientIP;
	static uint16_t currentClientPort;

	static void begin() {
		uint8_t i;
		state = 0;
		for (i = 0; i < IoTClientCount; i++) {
			clients[i].sequenceNumber = 0xFFFF;
			clients[i].id = 0xFF;
			clients[i].ip = 0;
			clients[i].port = 0;
		}
		nextClientIndex = IoTClientCount;
		nextClientId = 0;
#ifndef IoTNoPassword
		passwordLength = 0;
		clientPasswordLength = 0;
		for (i = 0; i < IoTMaxPasswordLength; i++) {
			password[i] = 0;
			clientPassword[i] = 0;
		}
#endif
		clientPasswordLengthEscaped = 0;
		clientIndex = 0;
		clientId = 0;
		clientSequenceNumber = 0;
		clientMessage = 0;
		clientMessageRepeated = false;
		clientPayloadIndex = HeaderLength;
		clientPayloadLength = 0;
		clientPayloadReceivedLength = 0;
		clientError = 0;
		clientResponseReady = 0;
	}

	inline static uint8_t isBigEndian() {
		const uint32_t x = 0x03020100;
		return ((uint8_t*)&x)[0];
	}

	static uint8_t process(const uint8_t* receivedBuffer, uint16_t length, uint16_t* usedBytes = 0) {
		uint8_t i;
		uint16_t used = 0;

		while (length--) {
			uint8_t b = *receivedBuffer++;
			used++;

			if (b == StartOfPacket) {
				state = 1;
				clientMessageRepeated = false;
				clientPayloadIndex = HeaderLength;
				clientPayloadLength = 0;
				clientPayloadReceivedLength = 0;
				clientError = 0;
				clientResponseReady = 0;
				continue;
			}

			switch ((state & ServerStateMask)) {
			case 9:
				if ((state & ServerFlagEscape)) {
					b ^= 1;
					state &= ~ServerFlagEscape;
				} else if (b == Escape) {
					clientPayloadReceivedLength++;
					state |= ServerFlagEscape;
					break;
				}

				if (clientPayloadReceivedLength == clientPayloadLength) {
					state = 0;
					if (b != EndOfPacket) {
						clientError = ResponseEndOfPacketNotFound;
					} else {
						switch (clientMessage) {
						case MessageQueryDevice:
							clientResponseReady = buildQueryDeviceResponse();
							break;
						case MessageDescribeInterface:
							clientResponseReady = buildDescribeInterfaceResponse(buffer[HeaderLength]);
							break;
						case MessageDescribeEnum:
							// This one must be handled by the user
							break;
						case MessageChangePassword:
#ifndef IoTNoPassword
							// clientPayloadLength now contains the actual amount of valid bytes received (after escaping)
							clientPayloadLength = clientPayloadIndex - HeaderLength;
							if (clientPayloadLength > IoTMaxPasswordLength)
								clientError = ResponsePayloadTooLarge;
#else
							clientError = ResponsePasswordReadOnly;
#endif
							break;
						default:
#ifndef IoTNoPassword
							// If there is a password, and we are not handling one of the three messages above, we must validate the password
							if (clientPasswordLength != passwordLength) {
								clientError = ResponseWrongPassword;
								break;
							}
							for (i = 0; i < passwordLength; i++) {
								if (clientPassword[i] != password[i]) {
									clientError = ResponseWrongPassword;
									goto ReturnTrue;
								}
							}
#endif

							if (clientMessage == MessageHandshake) {
								clientResponseReady = buildHandshakeResponse();
								break;
							} else if (clientMessage == MessageGoodBye) {
								clientResponseReady = buildGoodByeResponse();
								break;
							}

							// clientPayloadLength now contains the actual amount of valid bytes received (after escaping)
							clientPayloadLength = clientPayloadIndex - HeaderLength;
							break;
						}
					}
					goto ReturnTrue;
				}

				clientPayloadReceivedLength++;

				buffer[clientPayloadIndex] = b;
				clientPayloadIndex++;
				break;
			case 1:
				clientId = b;
				state++;
				break;
			case 2:
				clientSequenceNumber = (((uint16_t)b) << 1);
				state++;
				break;
			case 3:
				clientSequenceNumber |= (((uint16_t)b) << 8);
				state++;
				break;
			case 4:
				clientPasswordLengthEscaped = b >> 1;
				if (clientPasswordLengthEscaped > IoTMaxPasswordLengthEscaped || (b & 1)) {
					state = 0;
					clientError = ResponseWrongPassword;
					goto ReturnTrue;
				}
#ifndef IoTNoPassword
				clientPasswordLength = 0;
				if (!clientPasswordLengthEscaped)
					state += 2;
				else
					state++;
#else
				state += 2;
#endif
				break;
#ifndef IoTNoPassword
			case 5:
				clientPasswordLengthEscaped--;
				if ((state & ServerFlagEscape)) {
					b ^= 1;
					state &= ~ServerFlagEscape;
				} else if (b == Escape) {
					if (!clientPasswordLengthEscaped)
						state++;
					else
						state |= ServerFlagEscape;
					break;
				}
				clientPassword[clientPasswordLength++] = b;
				if (!clientPasswordLengthEscaped)
					state++;
				break;
#endif
			case 6:
				clientMessage = b;
				state++;
				break;
			case 7:
				clientPayloadLength = (uint16_t)(b >> 1);
				state++;
				break;
			case 8:
				clientPayloadLength |= (((uint16_t)b) << 6);

				state++;

				// Time to check for errors

				if ((clientMessage & 1) ||
					(clientMessage > MessageMax) ||
					(clientMessage <= MessageHandshake && (!(clientId & 1) || clientSequenceNumber != MaximumSequenceNumber))) {
					state = 0;
					goto ReturnFalse;
				}

				switch (clientMessage) {
				case MessageQueryDevice:
					if (clientPayloadLength || clientPasswordLength || clientId != ClientIdQueryDevice) {
						state = 0;
						goto ReturnFalse;
					}
					break;
				case MessageDescribeInterface:
					if (clientId != ClientIdDescribeInterface) {
						state = 0;
						goto ReturnFalse;
					}
					if (clientPasswordLength) {
						state = 0;
						clientError = ResponseWrongPassword;
						goto ReturnTrue;
					}
					if (clientPayloadLength != 1) {
						state = 0;
						clientError = ResponseInvalidPayload;
						goto ReturnTrue;
					}
					break;
				case MessageDescribeEnum:
					if (clientId != ClientIdDescribeEnum) {
						state = 0;
						goto ReturnFalse;
					}
					if (clientPasswordLength) {
						state = 0;
						clientError = ResponseWrongPassword;
						goto ReturnTrue;
					}
					if (clientPayloadLength != 2) {
						state = 0;
						clientError = ResponseInvalidPayload;
						goto ReturnTrue;
					}
					break;
				case MessageChangePassword:
					if (clientId != ClientIdChangePassword) {
						state = 0;
						goto ReturnFalse;
					}
#ifdef IoTPasswordReadOnly
					state = 0;
					clientError = ResponsePasswordReadOnly;
					goto ReturnTrue;
#else
					// The password must be empty when changing the password
					if (clientPasswordLength) {
						state = 0;
						clientError = ResponseWrongPassword;
						goto ReturnTrue;
					}
					if (clientPayloadLength > IoTMaxPayloadLength) {
						state = 0;
						clientError = ResponsePayloadTooLarge;
						goto ReturnTrue;
					}
					break;
#endif
				case MessageHandshake:
					if (clientId != ClientIdHandshake) {
						state = 0;
						goto ReturnFalse;
					}
					if (clientPayloadLength) {
						state = 0;
						clientError = ResponseInvalidPayload;
						goto ReturnTrue;
					}
					break;
				default:
					if ((clientId & 1)) {
						state = 0;
						clientError = ResponseUnknownClient;
						goto ReturnTrue;
					}
					for (i = 0; i < IoTClientCount; i++) {
						if (clients[i].id == clientId && clients[i].ip == currentClientIP && clients[i].port == currentClientPort) {
							clientIndex = i;

							if (clientSequenceNumber == clients[i].sequenceNumber) {
								clientMessageRepeated = true;
							} else {
								if ((uint16_t)(clientSequenceNumber - clients[i].sequenceNumber) > 0x7FFF) {
									// Old message arriving too late (we will just ignore the remaining bytes)
									state = 0;
									break;
								} else {
									clientMessageRepeated = false;
									clients[i].sequenceNumber = clientSequenceNumber;
								}
							}
							break;
						}
					}
					if (i == IoTClientCount) {
						state = 0;
						clientError = ResponseUnknownClient;
						goto ReturnTrue;
					}
					if (clientPayloadLength > IoTMaxPayloadLength) {
						state = 0;
						clientError = ResponsePayloadTooLarge;
						goto ReturnTrue;
					}
					break;
				}
				break;
			}
		}

	ReturnFalse: // The default return value
		if (usedBytes)
			*usedBytes = used;
		return false;

	ReturnTrue:
		if (usedBytes)
			*usedBytes = used;
		return true;
	}

	inline static uint8_t storedPasswordLength() {
		return passwordLength;
	}

	inline static uint8_t* storedPassword() {
#ifndef IoTNoPassword
		return password;
#else
		return 0;
#endif
	}

	inline static uint8_t storedPassword(const char* newPassword, uint8_t newPasswordLength) {
		return storedPassword((uint8_t*)newPassword, newPasswordLength);
	}

	inline static uint8_t storedPassword(const uint8_t* newPassword, uint8_t newPasswordLength) {
#ifndef IoTNoPassword
		if (newPasswordLength > IoTMaxPasswordLength || newPasswordLength && !newPassword)
			return false;
		passwordLength = newPasswordLength;
		for (newPasswordLength = 0; newPasswordLength < passwordLength; newPasswordLength++)
			password[newPasswordLength] = newPassword[newPasswordLength];
		for (; newPasswordLength < IoTMaxPasswordLength; newPasswordLength++)
			password[newPasswordLength] = 0;
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

	inline static const uint8_t* responseBuffer() {
		return buffer;
	}

	inline static uint16_t payloadLength() {
		return clientPayloadLength;
	}

	inline static uint8_t* payloadBuffer() {
		return (buffer + HeaderLength);
	}

	inline static uint16_t buildResponseIfAlreadyPossible() {
		return (clientError ? buildResponse(clientError, 0) : clientResponseReady);
	}

	static uint16_t buildResponse(uint8_t responseCode, uint16_t payloadLength = 0) {
		buffer[0] = StartOfPacket;
		buffer[1] = clientId;
		buffer[2] = (uint8_t)(clientSequenceNumber >> 1);
		buffer[3] = ((uint8_t)(clientSequenceNumber >> 8)) & 0xFE;
		buffer[4] = responseCode;
		buffer[5] = ((uint8_t)payloadLength) << 1;
		buffer[6] = ((uint8_t)(payloadLength >> 7)) << 1;
		buffer[7 + payloadLength] = EndOfPacket;
		return payloadLength + HeaderLength + EndOfPacketLength;
	}

	static uint16_t buildResponse8(uint8_t responseCode, uint8_t value) {
		if (value == StartOfPacket || value == Escape) {
			buffer[HeaderLength] = Escape;
			buffer[HeaderLength + 1] = value ^ 1;
			return buildResponse(responseCode, 2);
		}
		buffer[HeaderLength] = value;
		return buildResponse(responseCode, 1);
	}

	static uint16_t buildResponse16(uint8_t responseCode, uint16_t value) {
		uint8_t dst = HeaderLength;
		uint8_t v = (uint8_t)value;
		if (v == StartOfPacket || v == Escape) {
			buffer[dst++] = Escape;
			buffer[dst++] = v ^ 1;
		} else {
			buffer[dst++] = v;
		}
		v = (uint8_t)(value >> 8);
		if (v == StartOfPacket || v == Escape) {
			buffer[dst++] = Escape;
			buffer[dst++] = v ^ 1;
		} else {
			buffer[dst++] = v;
		}
		return buildResponse(responseCode, dst - HeaderLength);
	}

	static uint16_t buildResponse32(uint8_t responseCode, uint32_t value) {
		uint8_t dst = HeaderLength;
		for (uint8_t i = 0; i < 4; i++) {
			const uint8_t v = (uint8_t)value;
			value >>= 8;
			if (v == StartOfPacket || v == Escape) {
				buffer[dst++] = Escape;
				buffer[dst++] = v ^ 1;
			} else {
				buffer[dst++] = v;
			}
		}
		return buildResponse(responseCode, dst - HeaderLength);
	}

	static uint16_t buildResponseFloat(uint8_t responseCode, float value) {
		uint8_t dst = HeaderLength;
		for (uint8_t i = 0; i < 4; i++) {
			const uint8_t v = (uint8_t)(*((uint32_t*)&value));
			*((uint32_t*)&value) >>= 8;
			if (v == StartOfPacket || v == Escape) {
				buffer[dst++] = Escape;
				buffer[dst++] = v ^ 1;
			} else {
				buffer[dst++] = v;
			}
		}
		return buildResponse(responseCode, dst - HeaderLength);
	}

	inline static uint16_t buildResponseBuffer(uint8_t responseCode, const void* srcBuffer, uint16_t length) {
		return buildResponse(responseCode, escapeBuffer(HeaderLength, srcBuffer, length) - HeaderLength);
	}

	inline static uint16_t buildResponseEnumDescriptor8(uint8_t interfaceIndex, uint8_t propertyIndex, const IoTEnumDescriptor8* enumDescriptors, uint8_t count) {
		return buildResponseEnumDescriptor(interfaceIndex, propertyIndex, (const uint8_t*)enumDescriptors, count, 1);
	}

	inline static uint16_t buildResponseEnumDescriptor16(uint8_t interfaceIndex, uint8_t propertyIndex, const IoTEnumDescriptor16* enumDescriptors, uint8_t count) {
		return buildResponseEnumDescriptor(interfaceIndex, propertyIndex, (const uint8_t*)enumDescriptors, count, 2);
	}

	inline static uint16_t buildResponseEnumDescriptor32(uint8_t interfaceIndex, uint8_t propertyIndex, const IoTEnumDescriptor32* enumDescriptors, uint8_t count) {
		return buildResponseEnumDescriptor(interfaceIndex, propertyIndex, (const uint8_t*)enumDescriptors, count, 4);
	}
};

uint8_t _IoTServer::state;

_IoTServer::_IoTClient _IoTServer::clients[IoTClientCount];
uint8_t _IoTServer::nextClientIndex, _IoTServer::nextClientId;

#ifndef IoTNoPassword
uint8_t _IoTServer::passwordLength;
uint8_t _IoTServer::password[IoTMaxPasswordLength];
uint8_t _IoTServer::clientPasswordLength;
uint8_t _IoTServer::clientPassword[IoTMaxPasswordLength];
#else
#undef passwordLength
#undef clientPasswordLength
#endif
uint8_t _IoTServer::clientPasswordLengthEscaped;
uint8_t _IoTServer::clientIndex;
uint8_t _IoTServer::clientId;
uint16_t _IoTServer::clientSequenceNumber;
uint8_t _IoTServer::clientMessage;
uint8_t _IoTServer::clientMessageRepeated;
uint16_t _IoTServer::clientPayloadIndex;
uint16_t _IoTServer::clientPayloadLength;
uint16_t _IoTServer::clientPayloadReceivedLength;
uint8_t _IoTServer::clientError;
uint16_t _IoTServer::clientResponseReady;
uint32_t _IoTServer::currentClientIP;
uint16_t _IoTServer::currentClientPort;

uint8_t _IoTServer::buffer[HeaderLength + IoTMaxPayloadLength + EndOfPacketLength];

_IoTServer IoTServer;

#undef IoTMaxPasswordLengthEscaped

#undef StartOfPacket
#undef Escape
#undef EndOfPacket
#undef HeaderLength
#undef EndOfPacketLength
#undef ServerStateMask
#undef ServerFlagEscape

#pragma pack(pop)

#endif
