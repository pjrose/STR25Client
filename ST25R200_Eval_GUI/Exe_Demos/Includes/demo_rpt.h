/******************************************************************************
  * @attention
  *
  * COPYRIGHT 2022 STMicroelectronics, all rights reserved
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

/*! \file
 *
 *  \author 
 *
 *  \brief 
 *
 */

#ifndef DEMORPT_H
#define DEMORPT_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include <stdint.h>
#include <stdbool.h>

/*
******************************************************************************
* GLOBAL DEFINES
******************************************************************************
*/
#define DEMO_RPT_MAIN_UNIT_NAME_LENGTH 16
#define DEMO_RPT_URI_LENGTH 64
#define DEMO_RPT_PRODUCT_NAME_LENGTH 16
/*
******************************************************************************
* GLOBAL MACROS
******************************************************************************
*/
/*
******************************************************************************
* GLOBAL ENUMS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL TYPES
******************************************************************************
*/
typedef enum demoRptLedFeature
{
    DEMO_RPT_LED_FEATURE_NO_USAGE,
    DEMO_RPT_LED_FEATURE_LIFE_LED_ON_OFF,
    DEMO_RPT_LED_FEATURE_ACC_AUTH_OK_NOK,
    DEMO_RPT_LED_FEATURE_ACC_VALID_PARAM,
    DEMO_RPT_LED_FEATURE_PARAM_SPEED,
    DEMO_RPT_LED_FEATURE_ACC1_ON_OFF,
    DEMO_RPT_LED_FEATURE_ACC2_ON_OFF,
    DEMO_RPT_LED_FEATURE_ACC3_ON_OFF,
    DEMO_RPT_LED_FEATURE_ACC4_ON_OFF,
    DEMO_RPT_LED_FEATURE_TAG_DETECTED,
} demoRptLedFeature;

typedef enum demoRptBootMode 
{
    DEMO_RPT_BOOTMODE_GUI                  =  0,   /*!< GUI Boot mode       */
    DEMO_RPT_BOOTMODE_DEMO                 =  1,   /*!< Demo boot mode      */
}demoRptBootMode;

typedef enum demoRptPwmTriggerFeature
{
    DEMO_RPT_PWM_TRIGGER_FEATURE_NO_FEATURE,
    DEMO_RPT_PWM_TRIGGER_FEATURE_ACCESSORY_NOK,
    DEMO_RPT_PWM_TRIGGER_FEATURE_ACCESSORY_P01,
    DEMO_RPT_PWM_TRIGGER_FEATURE_ACCESSORY_P02,
    DEMO_RPT_PWM_TRIGGER_FEATURE_ACCESSORY_P03,
    DEMO_RPT_PWM_TRIGGER_FEATURE_ACCESSORY_P04,
    DEMO_RPT_PWM_TRIGGER_FEATURE_DIG_SIGNATURE_NOK,
} demoRptPwmTriggerFeature;

typedef enum demoRptPwmModeFeature
{
   DEMO_RPT_PWM_MODE_FEATURE_NO_FEATURE,
   DEMO_RPT_PWM_MODE_FEATURE_TOGGLE_ON,
   DEMO_RPT_PWM_MODE_FEATURE_TOGGLE_OFF,
   DEMO_RPT_PWM_MODE_FEATURE_PWM,
} demoRptPwmModeFeature;

typedef enum demoRptPwmVibrationFeature
{
    DEMO_RPT_PWM_VIBRATION_FEATURE_NO_FEATURE,
    DEMO_RPT_PWM_VIBRATION_FEATURE_VIBRATION_HIGH_FREQ,
    DEMO_RPT_PWM_VIBRATION_FEATURE_VIBRATION_LOW_FREQ,
    DEMO_RPT_PWM_VIBRATION_FEATURE_VIBRATION_VARIABLE,
} demoRptPwmVibrationFeature;

typedef enum demoRptReturnCode
{
    DEMO_RPT_ERR_NONE                   = 0,
    DEMO_RPT_ERR_TIMEOUT                = 0x0101,
    DEMO_RPT_ERR_REQUEST                = 0x0102,
    DEMO_RPT_ERR_PARAM                  = 0x0103,
    DEMO_RPT_ERR_SYSTEM                 = 0x0104,
    DEMO_RPT_ERR_PROTO                  = 0x0105,
    DEMO_RPT_ERR_INTERNAL               = 0x0106,
    DEMO_RPT_ERR_WRITE                  = 0x0107,
    DEMO_RPT_ERR_READ                   = 0x0108,
    DEMO_RPT_ERR_PWD_AREA1              = 0x0109,
    DEMO_RPT_ERR_PWD_AREA2              = 0x010A,
    DEMO_RPT_ERR_PWD_CONFIG             = 0x010B,
    DEMO_RPT_ERR_UNSUPP_TAG             = 0x010C,
    DEMO_RPT_ERR_TAG_NOT_CONFIGURED     = 0x010D,
    DEMO_RPT_ERR_OTHER                  = 0x010E
} demoRptReturnCode;

typedef enum demoRptAccessoryName
{
    DEMO_RPT_ACCNAME_NONE,
    DEMO_RPT_ACCNAME_P01,
    DEMO_RPT_ACCNAME_P02,
    DEMO_RPT_ACCNAME_P03,
    DEMO_RPT_ACCNAME_P04,
} demoRptAccessoryName;

typedef struct demoRptReaderConfig
{
    uint8_t                     mainUnitName[DEMO_RPT_MAIN_UNIT_NAME_LENGTH];
    demoRptLedFeature           led1Feature;
    demoRptLedFeature           led2Feature;
    demoRptLedFeature           led3Feature;
    bool                        outputOn;
    demoRptPwmTriggerFeature    outputTrigger;
    demoRptPwmModeFeature       outputMode;
    demoRptPwmVibrationFeature  outputPwm;
    uint32_t                    pwmRatioValue;
} demoRptReaderConfig;

typedef struct demoRptTagConfig
{
    uint8_t uri[DEMO_RPT_URI_LENGTH];
    uint8_t productType[DEMO_RPT_PRODUCT_NAME_LENGTH];
    demoRptAccessoryName accName;
    uint32_t numberOfUsage;
    uint32_t usageDuration;
    uint32_t speed;
    bool signatureValid;
    bool authenticated;
} demoRptTagConfig;


#ifndef RFAL_PUBLIC_API_ONLY

/*
******************************************************************************
* GLOBAL VARIABLES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*!
 *****************************************************************************
 * \brief R+T Demo Persistant Storage Initialize
 *
 * Initialiaze the R+T demo persitant storage
 * 
 *****************************************************************************
 */
void demoRptPersitantStorageInitialize(void);


/*! 
 *****************************************************************************
 * \brief  R+T Demo Set Boot Mode
 *  
 * Sets the boot mode 
 * 
 * \param[in]  mode : boot mode
 *
 * \return ERR_NONE         : No error
 * 
 *****************************************************************************
 */
demoRptReturnCode demoRptSetBootMode(demoRptBootMode mode);


/*! 
 *****************************************************************************
 * \brief  R+T Demo Get Boot Mode
 *  
 * Gets the boot mode 
 *
 * \return DEMO_RPT_BOOTMODE_GUI         : GUI boot Mode
 * \return DEMO_RPT_BOOTMODE_DEMO        : R+T Demo boot mode
 * 
 *****************************************************************************
 */
demoRptBootMode demoRptGetBootMode(void);


/*! 
 *****************************************************************************
 * \brief  R+T Demo Get Reader Config
 *  
 * Retrieves the reader configuration
 * 
 * \param[out]  config : reader configuration
 *
 * \return ERR_NONE         : No error
 * 
 *****************************************************************************
 */
demoRptReturnCode demoRptGetReaderConfiguration(demoRptReaderConfig *config);


/*! 
 *****************************************************************************
 * \brief  R+T Demo Set Reader Config
 *  
 * Sets the reader configuration
 * 
 * \param[in]  config : reader configuration
 *
 * \return ERR_NONE         : No error
 * 
 *****************************************************************************
 */
demoRptReturnCode demoRptSetReaderConfiguration(const demoRptReaderConfig *config);


/*! 
 *****************************************************************************
 * \brief  R+T Demo Get Tag Config
 *  
 * Retrieves the tag configuration
 * 
 * \param[out]  config : tag configuration
 *
 * \return ERR_NONE         : No error
 * 
 *****************************************************************************
 */
demoRptReturnCode demoRptGetTagConfiguration(uint8_t useActivatedDevice, demoRptTagConfig *config);


/*! 
 *****************************************************************************
 * \brief  R+T Demo Set Tag Config
 *  
 * Sets the tag configuration
 * 
 * \param[in]  config : tag configuration
 *
 * \return ERR_NONE         : No error
 * 
 *****************************************************************************
 */
demoRptReturnCode demoRptSetTagConfiguration(const demoRptTagConfig *config);
/*! 
 *****************************************************************************
 * \brief  R+T Demo Entry Point
 *  
 * Demo Entry point
 * 
 *****************************************************************************
 */
void demoRptMain(void);
 

#endif // RFAL_PUBLIC_API_ONLY

#endif /* DEMORPT_H */
