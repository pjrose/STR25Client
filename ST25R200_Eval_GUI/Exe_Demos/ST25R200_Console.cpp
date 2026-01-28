/******************************************************************************
  * @attention
  *
  * COPYRIGHT 2023 STMicroelectronics, all rights reserved
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
  * AND SPECIFICALLY DISCLAIMING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
******************************************************************************/

#define SER_USE_LOGGER
#define RFAL_FEATURE_NFC_RF_BUF_LEN 1024
#include <windows.h>
#include "serRfal_api.h"

#define SIZE_OF_ARRAY(a) (sizeof(a)/sizeof(a[0]))

char* hex2Str ( unsigned char* data, size_t dataLen, bool invert );

int main ( int argc, char * argv [ ] )
{
	int          error;
	ReturnCode   ret;
	bool         res;
	const char * comPort = "COM30"; // Default port com number
	if ( argc >= 2 )
	{
		comPort = argv [1];
	}
	//serSetTraceFunction ( NULL);  // disable all internal high level trace. Can also be set to 'print' to re-enable traces
	//serSetTraceMode ( 0 );// globally disable serRfal trace
	serSetTraceFunction (printf);                   // 'print' is the default trace function for console
	serSetTraceMode     ( SerTrace_ConsoleDefault );// globally enable serRfal trace

	printf ( "Test serial on Comport='%s'\n", comPort );
	if ( serComOpen ( comPort, error ) != true )
	{
		printf ( "Error in opening/configuring serial port %s\n", comPort );
		std::vector<std::string> ports = serComEnumeratePorts ();
		printf ( "\nCompatible port(s):");
		for (const auto & port : ports)
		{
			printf ( "\n%s", port.c_str() );
		}
		return( -1 );
	}
	printf ( "opening/configuring serial port successful\n" );

	// Initialisation of the RFAL
	res = serExecRfalInitialize ( & ret );

	enum
	{
		ePollingState_Disabled = 0,
		ePollingState_NotInit,
		ePollingState_Deactivated,
		ePollingState_TechsDetect,
		ePollingState_Wakeup,
		ePollingState_WakeupEnd,
		ePollingState_CardFound,
		ePollingState_Mask = 0x07,
	};

	int       pollingState     = ePollingState_NotInit;
	const int MAX_NB_OF_CARDS  = 1;
	do
	{
		if ( pollingState == ePollingState_NotInit || pollingState == ePollingState_Deactivated )
		{
			serRfalNfcDiscoverParam discParam;
			serRfalNfcDefaultDiscParams ( & discParam );
			discParam.devLimit            = MAX_NB_OF_CARDS;
			discParam.totalDuration       = 200;
			discParam.techs2Find          = RFAL_NFC_POLL_TECH_A | RFAL_NFC_POLL_TECH_B | RFAL_NFC_POLL_TECH_V | RFAL_NFC_POLL_TECH_ST25TB; //only valid one for ST25R100
			discParam.wakeupConfigDefault = false;
			discParam.wakeupEnabled       = false;
			discParam.wakeupNPolls        = 0;

			if ( pollingState == ePollingState_NotInit )
			{
				res = serExecRfalNfcInitialize ( & ret );
			}

			res = serExecRfalNfcDiscover ( & ret, & discParam );
			if ( ret == RFAL_ERR_NONE )
			{
				pollingState = ePollingState_TechsDetect;
			}
			printf ( "Activate Polling\n" );
		}
		if ( ( pollingState == ePollingState_TechsDetect ) || ( pollingState == ePollingState_WakeupEnd ) )
		{
			int          maxLoop = 20;
			int          loop = 0;
			rfalNfcState state;
			do
			{
				if (loop != 0)
					Sleep(100); //Make a small tempo
				res = serExecRfalNfcGetState ( & state );
				if ( res == true )
				{
					if ( state == RFAL_NFC_STATE_WAKEUP_MODE )
					{
						pollingState = ePollingState_Wakeup;
						break; // Stay in same state and don't restart the timer
					}
					else if ( ( state == RFAL_NFC_STATE_ACTIVATED ) || ( state == RFAL_NFC_STATE_POLL_SELECT ) )
					{
						pollingState     = ePollingState_CardFound;
						serFlatRfalNfcDevice devList [MAX_NB_OF_CARDS];
						uint8_t              devCnt  = SIZE_OF_ARRAY ( devList );
						// At least one card found
						res = serExecRfalNfcGetDevicesFound(& ret, devList, & devCnt );
						if ( ret == RFAL_ERR_NONE )
						{
							for ( int index = 0; index < devCnt; index++ )
							{
								switch(devList [index].type)
								{
								case RFAL_NFC_LISTEN_TYPE_NFCA:
									printf("\nType A: Uid=%s\n", hex2Str ( devList [index].nfca.nfcId1, devList [index].nfca.nfcId1Len, false ));
									break;
								case RFAL_NFC_LISTEN_TYPE_NFCB:
									printf("\nType B: Uid=%s\n", hex2Str ( devList [index].nfcb.sensbRes.nfcid0,  RFAL_NFCB_NFCID0_LEN, false) );
									break;
								case RFAL_NFC_LISTEN_TYPE_ST25TB:
									printf("\nType ST25TB: Uid=%s\n", hex2Str ( devList [index].st25tb.UID,  RFAL_ST25TB_UID_LEN, true) );
									break;
								case RFAL_NFC_LISTEN_TYPE_NFCV:
									printf("\nType V: Uid=%s\n", hex2Str (  devList [index].nfcv.InvRes.UID, 8, true) );
									break;
								}
							}
						}
						Sleep(1000); //Make a small tempo
						// DeActivate before making new discover
						res          = serExecRfalNfcDeactivate (& ret, RFAL_NFC_DEACTIVATE_IDLE );
						pollingState = ePollingState_Deactivated;
						break;
					}
				}
				++ loop;
			} while ( maxLoop > loop );
		}
	} while ( pollingState != ePollingState_Disabled );
	return(0);
}


#define MAX_INPUT_LENGTH  128
static const char* hex = "0123456789ABCDEF";
static char         hexStr[ ( MAX_INPUT_LENGTH << 1 ) + 1 ];

char * hex2Str ( unsigned char* data, size_t dataLen, bool invert )
{ // For dumping buffer in Hex
	unsigned char* pin = data;
	char* pout = hexStr;
	if ( dataLen > MAX_INPUT_LENGTH )
	{
		dataLen = MAX_INPUT_LENGTH;
	}
	if ( invert == true )
	{
		pout += ( dataLen << 1 );
		*pout = 0;
		for ( size_t i = 0; i < dataLen; ++i )
		{
			pout -= 2;
			pout[ 0 ] = hex[ ( *pin >> 4 ) & 0xF ];
			pout[ 1 ] = hex[ ( *pin++ ) & 0xF ];
		}
	}
	else
	{
		for ( size_t i = 0; i < dataLen; ++i )
		{
			*pout++ = hex[ ( *pin >> 4 ) & 0xF ];
			*pout++ = hex[ ( *pin++ ) & 0xF ];
		}
		*pout = 0;
	}

	return hexStr;
}
