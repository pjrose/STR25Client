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
#ifndef RFAL_PLATFORM_H
#define RFAL_PLATFORM_H

#define RFAL_PLATFORM_R200_H

// remove FW declaration part
#define RFAL_PUBLIC_API_ONLY

// Define the target
#define ST25R200
#define DEFAULT_TARGET_NAME "STEVAL-25R200"
#define APP_SHORT_NAME      "ST25R200"

#ifndef RC_INVOKED
#define ST25_APP_GUID              "AD6880DA-333A-4AA6-AFE1-ED0C44DF9F2C" /* R200 historical value */
#pragma message ("--> R200 platform")
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#define platformLog   printf
#define platformDelay Sleep

#define RFAL_FEATURE_DPO  true
#define RFAL_FEATURE_DLMA false
#include "rfal_features.h"

#endif // #ifdef RC_INVOKED


#endif //RFAL_PLATFORM_H
