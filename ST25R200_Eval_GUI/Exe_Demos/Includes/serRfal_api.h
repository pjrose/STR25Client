#ifndef _SER_RFAL_API_H_
#define _SER_RFAL_API_H_

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <vector>
#include <string>

#if (defined(_WIN64) || defined(_WIN32))
#include <windows.h>
#endif


#pragma warning(disable: 4200) // warning C4200: nonstandard extension used: zero-sized array in struct/union because of rfal_analogconfig.h

// remove FW declaration part
#include "rfal_platform.h"

// Prevent inclusion of common.h
#ifndef _SER_COMMON_H
	#define _SER_COMMON_H
#endif
#ifndef _SER_RFAL_API_FLAT
	#include "../serialization/Inc/serCommonTypes.h"
	#include "rfal_dpo.h"
	#include "../serialization/Inc/demo_rpt.h"
	#include "../serialization/Inc/serHost.h"
#else
	// Serialization api is delivered to customer in a flat way
	#include "serCommonTypes.h"
	#include "rfal_dpo.h"
	#include "demo_rpt.h"
	#include "serHost.h"
#endif


typedef int (* t_fnLog) (const char *, ...);

enum
{
	SerTrace_Enable          = 0x0001,
	SerTrace_StdLog          = 0x0002, // Normal trace used in log window
	SerTrace_ProtocolErrors  = 0x0004,
	SerTrace_WrongReturnCode = 0x0008,
	SerTrace_FunctionCall    = 0x0010,
	SerTrace_BufferContent   = 0x0020,
	SerTrace_ConsoleDefault  = SerTrace_Enable | SerTrace_ProtocolErrors | SerTrace_WrongReturnCode | SerTrace_FunctionCall | SerTrace_BufferContent,
	SerTrace_Default         = SerTrace_Enable | SerTrace_StdLog,
	SerTrace_StdLogEnable    = SerTrace_Enable | SerTrace_StdLog,
};

#define DEFAULT_RX_TIMEOUT	2000
#define DEFAULT_TX_TIMEOUT	200

typedef struct serComParameters
{
	int BaudRate; // by default 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200,
	int ByteSize; // 4 to 8
	int StopBits; // OneStop = 1, TwoStop = 2, OneAndHalfStop = 3, UnknownStopBits = -1
	int Parity  ; // NoParity = 0, EvenParity = 2, OddParity = 3, SpaceParity = 4, MarkParity = 5, UnknownParity = -1
	// NoFlowControl = 0,  HardwareControl, SoftwareControl, UnknownFlowControl = -1
} serComParameters;

SER_API bool serComClose                                    (int32_t & error);
SER_API bool serComOpen                                     (const char* lpComName, int32_t& error);
SER_API bool serComPurge                                    ();
SER_API bool serComRawSnd                                   (uint8_t* buf, uint16_t bufLen);
SER_API bool serComRawRcv                                   (uint8_t* buf, uint16_t& bufLen);
SER_API bool serComNciRcv                                   (uint8_t* buf, uint16_t& bufLen);
SER_API bool serComSetDriver                                (void * handle );

SER_API bool serIsNciTunneled                               ( bool & isTunneled );
SER_API bool serSetNciTunneled                              ( const bool tunneled );

SER_API bool serSetTraceMode                                (int32_t value);
SER_API bool serGetTraceMode                                (int & value);
SER_API bool serSetTraceFunction                            (t_fnLog fn);
SER_API bool serGetVersion                                  (uint32_t & version);
SER_API bool serGetHash                                     (uint32_t & hash);

#ifdef SER_USE_LOGGER
typedef void* callerCtx;
typedef void(*serReqLoggingCallback)                        (callerCtx ctx, serBuf* buf, logBuf* log);
typedef void(*serRspLoggingCallback)                        (callerCtx ctx, serBuf* buf, logBuf* log);
SER_API bool serRegisterSerReqLoggingCallback               (callerCtx ctx, serReqLoggingCallback cb, logBuf* logBufMemory);
SER_API bool serRegisterSerRspLoggingCallback               (callerCtx ctx, serRspLoggingCallback cb, logBuf* logBufMemory);
#endif

SER_API bool serSetTimeouts                                 (const unsigned int  rxTimeout, const unsigned int  txTimeout);
SER_API bool serGetTimeouts                                 (unsigned int * rxTimeout, unsigned int * txTimeout);
#if (defined(_WIN64) || defined(_WIN32))
SER_API bool serSetRawTimeouts                              (const COMMTIMEOUTS timeouts);
SER_API bool serGetRawTimeouts                              (COMMTIMEOUTS* timeouts);
#endif
SER_API bool serSetSerialParams                             (const serComParameters  & serialParam);
SER_API bool serApplySerialParams                           (const serComParameters  & serialParam);
SER_API bool serGetSerialParams                             (serComParameters  & serialParam);
SER_API bool serInitDefaultSerialParams                     (serComParameters  & serialParam);
SER_API std::vector<std::string>  serComEnumeratePorts ();

#endif // _SER_API_H_



