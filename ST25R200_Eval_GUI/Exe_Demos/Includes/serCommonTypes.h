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

#ifndef _SER_COMMON_TYPES_H
#define _SER_COMMON_TYPES_H

// Needed for T5T 
#define SER_T5T_DATA_BUF_SZ 2032U

#ifdef __cplusplus
extern "C" {
#endif

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include <stdint.h>
#include "rfal_nfc.h"
#include "rfal_analogConfig.h"
#include "rfal_chip.h"
#if RFAL_FEATURE_DPO
#include "rfal_dpo.h"
#endif /* RFAL_FEATURE_DPO */
#if EMV_DTE
#include "serEmv.h"
#endif /* EMV_DTE */
#include "demo_rpt.h"

/*
******************************************************************************
* GLOBAL DEFINES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL MACROS
******************************************************************************
*/
/*! Sets the discover parameters to its default values */
#define serRfalNfcDefaultDiscParams( dp )  if( (dp) != NULL) {                                     \
                                        RFAL_MEMSET( (dp), 0x00, sizeof(serRfalNfcDiscoverParam) );\
                                        ((dp))->compMode               = RFAL_COMPLIANCE_MODE_NFC; \
                                        ((dp))->devLimit               = 1U;                       \
                                        ((dp))->nfcfBR                 = RFAL_BR_212;              \
                                        ((dp))->ap2pBR                 = RFAL_BR_424;              \
                                        ((dp))->maxBR                  = RFAL_BR_KEEP;             \
                                        ((dp))->isoDepFS               = RFAL_ISODEP_FSXI_256;     \
                                        ((dp))->nfcDepLR               = RFAL_NFCDEP_LR_254;       \
                                        ((dp))->GBLen                  = 0U;                       \
                                        ((dp))->p2pNfcaPrio            = false;                    \
                                        ((dp))->wakeupEnabled          = false;                    \
                                        ((dp))->wakeupConfigDefault    = true;                     \
                                        ((dp))->wakeupPollBefore       = false;                    \
                                        ((dp))->wakeupNPolls           = 1U;                       \
                                        ((dp))->totalDuration          = 1000U;                    \
                                        ((dp))->techs2Find             = RFAL_NFC_TECH_NONE;       \
                                        ((dp))->techs2Bail             = RFAL_NFC_TECH_NONE;       \
                                        }

/*
 ******************************************************************************
 * GLOBAL TYPES
 ******************************************************************************
*/
typedef struct serBuf
{
    uint8_t *buf;
    uint8_t *cur;
    uint16_t bufLen;
    uint16_t curLen;
} serBuf;

typedef struct serFlatRfalNfcDevice
{
    rfalNfcDevType         type;                /*!< Device's type                */
    rfalNfcaListenDevice   nfca;                /*!< NFC-A Listen Device instance */
    rfalNfcbListenDevice   nfcb;                /*!< NFC-B Listen Device instance */
    rfalNfcfListenDevice   nfcf;                /*!< NFC-F Listen Device instance */
    rfalNfcvListenDevice   nfcv;                /*!< NFC-V Listen Device instance */
    rfalSt25tbListenDevice st25tb;   
} serFlatRfalNfcDevice;

typedef struct serRfalWakeUpConfig
{
    rfalWumPeriod        period;          /*!< Wake-Up Timer period;how often measurement(s) is performed */
    bool                 irqTout;         /*!< IRQ IRQ due to Wake-up timer and measurement event (wutme) */
    bool                 autoAvg;         /*!< Use the HW Auto Averaging feature on the enabled channel(s)*/
    bool                 skipCal;         /*!< Do not preform calibration starting WU mode                */
    bool                 skipReCal;       /*!< Do not preform recalibration during WU mode                */
    bool                 delCal;          /*!< Delay calibration step starting WU mode                    */
    bool                 delRef;          /*!< Delay reference step starting WU mode                      */
    rfalWumMeasDuration  measDur;         /*!< Wake-up measurement duration config                        */
    rfalWumMeasFilter    measFil;         /*!< Wake-up measurement filter config                          */
    
    rfalWumMeasChannel   I;               /*!< I channel Configuration                                    */
    rfalWumMeasChannel   Q;               /*!< Q channel Configuration                                    */
} serRfalWakeUpConfig;

typedef struct serRfalWakeUpInfo
{
    bool                 irqWut;          /*!< Wake-Up timer and measurement event IRQ (cleared upon read)*/
    uint8_t              status;          /*!< Wake-Up status                                             */

    struct{
        uint8_t          lastMeas;        /*!< Value of the latest measurement                            */
        uint8_t          reference;       /*!< Current reference value                                    */
        uint8_t          calib;           /*!< Current calibration value                                  */
        bool             irqWu;           /*!< Amplitude WU IRQ received (cleared upon read)              */
    }I;                                   /*!< I channel Information                                      */

    struct{
        uint8_t          lastMeas;        /*!< Value of the latest measurement                            */
        uint8_t          reference;       /*!< Current reference value                                    */
        uint8_t          calib;           /*!< Current calibration value                                  */
        bool             irqWu;           /*!< Phase WU IRQ received (cleared upon read)                  */
    }Q;                                   /*!< Q channel Information                                      */
    
    struct{
        bool             irqWptFod;       /*!< WLC WPT FOD detected                                       */
        bool             irqWptStop;      /*!< WLC WPT Stop condition detected                            */
    }WLC;                                 /*!< WLC Wireless Power Transfer phase                          */
} serRfalWakeUpInfo;

typedef struct{                                                                                             
    rfalComplianceMode     compMode;                         /*!< Compliancy mode to be used                                         */
    uint16_t               techs2Find;                       /*!< Technologies to search for                                         */
    uint16_t               techs2Bail;                       /*!< Bail-out after certain NFC technologies                            */
    uint16_t               totalDuration;                    /*!< Duration of a whole Poll + Listen cycle        NCI 2.1 Table 46    */
    uint8_t                devLimit;                         /*!< Max number of devices                      Activity 2.1  Table 11  */
    rfalBitRate            maxBR;                            /*!< Max Bit rate to be used                        NCI 2.1  Table 28   */
                                                                                                                   
    rfalBitRate            nfcfBR;                           /*!< Bit rate to poll for NFC-F                     NCI 2.1  Table 27   */
    uint8_t                nfcid3[RFAL_NFCDEP_NFCID3_LEN];   /*!< NFCID3 to be used on the ATR_REQ/ATR_RES                           */
    uint8_t                GB[RFAL_NFCDEP_GB_MAX_LEN];       /*!< General bytes to be used on the ATR-REQ        NCI 2.1  Table 29   */
    uint8_t                GBLen;                            /*!< Length of the General Bytes                    NCI 2.1  Table 29   */
    rfalBitRate            ap2pBR;                           /*!< Bit rate to poll for AP2P                      NCI 2.1  Table 31   */
    bool                   p2pNfcaPrio;                      /*!< NFC-A P2P (true) or ISO14443-4/T4T (false) priority                */
    rfalNfcPropCallbacks   propNfc;                          /*!< Proprietary Technlogy callbacks                                    */
                                                                                                                                    
                                                                                                                                    
    rfalIsoDepFSxI         isoDepFS;                         /*!< ISO-DEP Poller announced maximum frame size   Digital 2.2 Table 60 */
    uint8_t                nfcDepLR;                         /*!< NFC-DEP Poller & Listener maximum frame size  Digital 2.2 Table 90 */
                                                                                                                                    
    rfalLmConfPA           lmConfigPA;                       /*!< Configuration for Passive Listen mode NFC-A                        */
    rfalLmConfPF           lmConfigPF;                       /*!< Configuration for Passive Listen mode NFC-A                        */
                                                                                                                                     
    void                   (*notifyCb)( rfalNfcState st );   /*!< Callback to Notify upper layer                                     */
                                                                                                                                     
    bool                   wakeupEnabled;                    /*!< Enable Wake-Up mode before polling                                 */
    bool                   wakeupConfigDefault;              /*!< Wake-Up mode default configuration                                 */
    serRfalWakeUpConfig    wakeupConfig;                     /*!< Wake-Up mode configuration                                         */
    bool                   wakeupPollBefore;                 /*!< Flag to Poll wakeupNPolls times before entering Wake-up            */
    uint16_t               wakeupNPolls;                     /*!< Number of polling cycles before|after entering Wake-up             */
} serRfalNfcDiscoverParam;

#ifndef SER_COM_TXRX
#define SER_COM_TXRX(sb) do {(sb)->cur = (sb)->buf; serDispatch_extended(__FUNCTION__, (sb));(sb)->cur = (sb)->buf;} while (0)
#endif

int serDispatch_extended(const char*, struct serBuf*);



#ifdef __cplusplus
}
#endif

#endif /* _SER_COMMON_TYPES_H */