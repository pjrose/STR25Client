/******************************************************************************
  * @attention
  *
  * COPYRIGHT 2024 STMicroelectronics, all rights reserved
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

/* !!! GENERATED FILE !!! DO NOT MODIFY !!! */


#ifndef _SER_HOST_H
#define _SER_HOST_H

#ifdef __cplusplus
extern "C" {
#endif


#ifndef SER_API
    #ifdef SER_EXPORTS
        #define SER_API __declspec( dllexport )
    #else
        #define SER_API __declspec( dllimport )
    #endif
#endif


/* forward declarations */
enum sysNfcDpoAdjustMethod;
enum sysNfcDpoMeasureMethod;
struct sysNfcDpoInfo;


SER_API bool serExecSysPing(void);
SER_API bool serExecSysGetVersion(uint8_t **version, uint16_t *versionLen, uint32_t *rfalVersion, uint32_t *fwVersion, uint32_t *serHash);
SER_API bool serExecSysGetConfigHashes(ReturnCode *ret, uint8_t configID, uint32_t *hashOriginal, uint32_t *hashFlash, uint32_t *hashRAM);
SER_API bool serExecSysWritePersistentConfig(ReturnCode *ret, uint8_t configID);
SER_API bool serExecSysGetUID(uint32_t *uidW0, uint32_t *uidW1, uint32_t *uidW2);
SER_API bool serExecSysGetDEVIDs(uint32_t *devId, uint8_t *st25rId);
SER_API bool serExecSysError(uint32_t hostError, uint32_t *devError);
SER_API bool serExecSysSpiTxRx(ReturnCode *ret, uint8_t *txBuf, uint16_t txBufLen, uint8_t **rxBuf, uint16_t *actLen);
SER_API bool serExecSysLogInitialize(bool enable, ReturnCode *ret);
SER_API bool serExecSysLogGet(ReturnCode *ret, uint8_t **log, uint8_t *logLength);
SER_API bool serExecSysLed(ReturnCode *ret, uint8_t ID, uint8_t action);
SER_API bool serExecSysNfcReset(ReturnCode *ret);
SER_API bool serExecSysDpoSetAdjustMethod(ReturnCode *ret, sysNfcDpoAdjustMethod adjustId);
SER_API bool serExecSysDpoSetMeasureMethod(ReturnCode *ret, sysNfcDpoMeasureMethod measureId);
SER_API bool serExecSysDpoGetInfo(ReturnCode *ret, sysNfcDpoInfo *info);
SER_API bool serExecSysRfu(ReturnCode *ret, uint8_t *txBuf, uint16_t txBufLen, uint8_t **rxBuf, uint16_t *rxBufLen);
SER_API bool serExecRfalInitialize(ReturnCode *ret);
SER_API bool serExecRfalCalibrate(ReturnCode *ret);
SER_API bool serExecRfalAdjustRegulators(ReturnCode *ret, uint16_t *result);
SER_API bool serExecRfalDeinitialize(ReturnCode *ret);
SER_API bool serExecRfalSetMode(ReturnCode *ret, rfalMode mode, rfalBitRate txBR, rfalBitRate rxBR);
SER_API bool serExecRfalSetBitRate(ReturnCode *ret, rfalBitRate txBR, rfalBitRate rxBR);
SER_API bool serExecRfalSetFDTListen(ReturnCode *ret, uint32_t FDTListen);
SER_API bool serExecRfalSetFDTPoll(ReturnCode *ret, uint32_t FDTPoll);
SER_API bool serExecRfalSetGT(ReturnCode *ret, uint32_t GT);
SER_API bool serExecRfalFieldOnAndStartGT(ReturnCode *ret);
SER_API bool serExecRfalFieldOff(ReturnCode *ret);
SER_API bool serExecRfalStartTransceive(ReturnCode *ret, uint8_t *txBuf, uint16_t txBufLen, uint16_t txDataLen, uint32_t flags, uint32_t fwt);
SER_API bool serExecRfalGetTransceiveStatus(ReturnCode *ret, uint8_t **rxBuf, uint16_t *rxBufLen, uint16_t *rxRcvdLen);
SER_API bool serExecRfalTransceiveBlockingTxRx(ReturnCode *ret, uint8_t *txBuf, uint16_t txBufLen, uint32_t flags, uint32_t fwt, uint8_t **rxBuf, uint16_t *actLen);
SER_API bool serExecRfalTransceiveBlockingTx(ReturnCode *ret, uint8_t *txBuf, uint16_t txBufLen, uint32_t flags, uint32_t fwt);
SER_API bool serExecRfalTransceiveBlockingRx(ReturnCode *ret, uint8_t **rxBuf, uint16_t *actLen);
SER_API bool serExecRfalISO14443ATransceiveShortFrame(ReturnCode *ret, rfal14443AShortFrameCmd txCmd, uint32_t fwt, uint8_t **rxBuf, uint16_t *rxBufLen, uint16_t *rxRcvdLen);
SER_API bool serExecRfalISO14443ATransceiveAnticollisionFrame(ReturnCode *ret, uint8_t *anticolBuf, uint16_t *anticolBufLen, uint32_t fwt, uint8_t *bytesToSent, uint8_t *bitsToSent, uint16_t *rxLength);
SER_API bool serExecRfalIsExtFieldOn(ReturnCode *ret, bool *extField);
SER_API bool serExecRfalNfcaPollerInitialize(ReturnCode *ret);
SER_API bool serExecRfalNfcaPollerCheckPresence(ReturnCode *ret, rfal14443AShortFrameCmd cmd, rfalNfcaSensRes *sensRes);
SER_API bool serExecRfalNfcaPollerSelect(ReturnCode *ret, uint8_t *nfcid1, uint8_t nfcidLen, rfalNfcaSelRes *selRes);
SER_API bool serExecRfalNfcaPollerSleep(ReturnCode *ret);
SER_API bool serExecRfalNfcaPollerTechnologyDetection(ReturnCode *ret, rfalComplianceMode compMode, rfalNfcaSensRes *sensRes);
SER_API bool serExecRfalNfcaPollerSingleCollisionResolution(ReturnCode *ret, uint8_t devLimit, bool *collPending, rfalNfcaSelRes *selRes, uint8_t **nfcId, uint8_t *nfcidLen);
SER_API bool serExecRfalNfcaPollerFullCollisionResolution(ReturnCode *ret, rfalComplianceMode compMode, uint8_t devLimit, rfalNfcaListenDevice *nfcaDevList, uint8_t *devCnt);
SER_API bool serExecRfalNfcbPollerInitialize(ReturnCode *ret);
SER_API bool serExecRfalNfcbPollerInitializeWithParams(ReturnCode *ret, uint8_t AFI, uint8_t PARAM);
SER_API bool serExecRfalNfcbPollerCheckPresence(ReturnCode *ret, rfalNfcbSensCmd cmd, rfalNfcbSlots nSlots, rfalNfcbSensbRes *sensbRes, uint8_t *sensbResLen);
SER_API bool serExecRfalNfcbPollerSleep(ReturnCode *ret, uint8_t *nfcid0, uint8_t nfcid0Len);
SER_API bool serExecRfalNfcbPollerSlotMarker(ReturnCode *ret, uint8_t slotCode, rfalNfcbSensbRes *sensbRes, uint8_t *sensbResLen);
SER_API bool serExecRfalNfcbPollerTechnologyDetection(ReturnCode *ret, rfalComplianceMode compMode, rfalNfcbSensbRes *sensbRes, uint8_t *sensbResLen);
SER_API bool serExecRfalNfcbPollerCollisionResolution(ReturnCode *ret, rfalComplianceMode compMode, uint8_t devLimit, rfalNfcbListenDevice *nfcbDevList, uint8_t *devCnt);
SER_API bool serExecRfalNfcfPollerInitialize(ReturnCode *ret, rfalBitRate bitRate);
SER_API bool serExecRfalFeliCaPoll(ReturnCode *ret, rfalFeliCaPollSlots nSlots, uint16_t sysCode, uint8_t reqCode, uint8_t pollResListSize, rfalFeliCaPollRes *pollResList, uint8_t *devicesDetected, uint8_t *collisionsDetected);
SER_API bool serExecRfalNfcfPollerCheckPresence(ReturnCode *ret);
SER_API bool serExecRfalNfcfPollerCollisionResolution(ReturnCode *ret, rfalComplianceMode compMode, uint8_t devLimit, rfalNfcfListenDevice *nfcfDevList, uint8_t *devCnt);
SER_API bool serExecRfalNfcfPollerCheck(ReturnCode *ret, uint8_t *nfcid2, uint8_t nfcidLen, uint16_t *servList, uint8_t numServ, rfalNfcfBlockListElem *blockList, uint8_t numBlock, uint8_t **rxBuf, uint16_t *rcvdlen);
SER_API bool serExecRfalNfcfPollerUpdate(ReturnCode *ret, uint8_t *nfcid2, uint8_t nfcidLen, uint16_t *servList, uint8_t numServ, rfalNfcfBlockListElem *blockList, uint8_t numBlock, uint8_t *blockData, uint16_t blockDatalen, uint8_t **rxBuf, uint16_t *rcvdlen);
SER_API bool serExecRfalNfcfPollerPoll(ReturnCode *ret, rfalFeliCaPollSlots nSlots, uint16_t sysCode, uint8_t reqCode, rfalFeliCaPollRes *cardList, uint8_t *devCnt, uint8_t *collisions);
SER_API bool serExecRfalISO15693TransceiveAnticollisionFrame(ReturnCode *ret, uint8_t *txBuf, uint16_t txBufLen, uint8_t **rxBuf, uint16_t *actLen);
SER_API bool serExecRfalISO15693TransceiveEOFAnticollision(ReturnCode *ret, uint8_t **rxBuf, uint16_t *actLen);
SER_API bool serExecRfalISO15693TransceiveEOF(ReturnCode *ret, uint8_t **rxBuf, uint16_t *actLen);
SER_API bool serExecRfalNfcvPollerInitialize(ReturnCode *ret);
SER_API bool serExecRfalNfcvPollerCheckPresence(ReturnCode *ret, rfalNfcvInventoryRes *invRes);
SER_API bool serExecRfalNfcvPollerInventory(ReturnCode *ret, rfalNfcvNumSlots nSlots, uint8_t *maskVal, uint8_t maskLen, rfalNfcvInventoryRes *invRes, uint16_t *rcvdLen);
SER_API bool serExecRfalNfcvPollerCollisionResolution(ReturnCode *ret, rfalComplianceMode compMode, uint8_t devLimit, rfalNfcvListenDevice *nfcvDevList, uint8_t *devCnt);
SER_API bool serExecRfalNfcvPollerSleepCollisionResolution(ReturnCode *ret, uint8_t devLimit, rfalNfcvListenDevice *nfcvDevList, uint8_t *devCnt);
SER_API bool serExecRfalNfcvPollerSleep(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen);
SER_API bool serExecRfalNfcvPollerSelect(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen);
SER_API bool serExecRfalNfcvPollerReadSingleBlock(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint8_t blockNum, uint8_t **rxBuf, uint16_t *rcvLen);
SER_API bool serExecRfalNfcvPollerExtendedReadSingleBlock(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint16_t blockNum, uint8_t **rxBuf, uint16_t *rcvLen);
SER_API bool serExecRfalNfcvPollerReadMultipleBlocks(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint8_t firstBlockNum, uint8_t numOfBlocks, uint8_t **rxBuf, uint16_t *rcvLen);
SER_API bool serExecRfalNfcvPollerExtendedReadMultipleBlocks(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint16_t firstBlockNum, uint16_t numOfBlocks, uint8_t **rxBuf, uint16_t *rcvLen);
SER_API bool serExecRfalNfcvPollerWriteSingleBlock(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint8_t blockNum, uint8_t *wrData, uint16_t blockLen);
SER_API bool serExecRfalNfcvPollerExtendedWriteSingleBlock(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint16_t blockNum, uint8_t *wrData, uint16_t blockLen);
SER_API bool serExecRfalNfcvPollerWriteMultipleBlocks(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint8_t firstBlockNum, uint8_t numOfBlocks, uint8_t *wrData, uint16_t wrDataLen, uint8_t blockLen);
SER_API bool serExecRfalNfcvPollerExtendedWriteMultipleBlocks(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint16_t firstBlockNum, uint16_t numOfBlocks, uint8_t *wrData, uint16_t wrDataLen, uint8_t blockLen);
SER_API bool serExecRfalNfcvPollerLockBlock(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint8_t blockNum);
SER_API bool serExecRfalNfcvPollerExtendedLockSingleBlock(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint16_t blockNum);
SER_API bool serExecRfalNfcvPollerGetSystemInformation(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint8_t **rxBuf, uint16_t *rcvLen);
SER_API bool serExecRfalNfcvPollerExtendedGetSystemInformation(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint8_t requestField, uint8_t **rxBuf, uint16_t *rcvLen);
SER_API bool serExecRfalSt25tbPollerInitialize(ReturnCode *ret);
SER_API bool serExecRfalSt25tbPollerCheckPresence(ReturnCode *ret, uint8_t *chipId);
SER_API bool serExecRfalSt25tbPollerCollisionResolution(ReturnCode *ret, uint8_t devLimit, rfalSt25tbListenDevice *st25tbDevList, uint8_t *devCnt);
SER_API bool serExecRfalSt25tbPollerInitiate(ReturnCode *ret, uint8_t *chipId);
SER_API bool serExecRfalSt25tbPollerPcall(ReturnCode *ret, uint8_t *chipId);
SER_API bool serExecRfalSt25tbPollerSlotMarker(ReturnCode *ret, uint8_t slotNum, uint8_t *chipIdRes);
SER_API bool serExecRfalSt25tbPollerSelect(ReturnCode *ret, uint8_t chipId);
SER_API bool serExecRfalSt25tbPollerGetUID(ReturnCode *ret, rfalSt25tbUID *UID);
SER_API bool serExecRfalSt25tbPollerReadBlock(ReturnCode *ret, uint8_t blockAddress, rfalSt25tbBlock *blockData);
SER_API bool serExecRfalSt25tbPollerWriteBlock(ReturnCode *ret, uint8_t blockAddress, rfalSt25tbBlock *blockData);
SER_API bool serExecRfalSt25tbPollerCompletion(ReturnCode *ret);
SER_API bool serExecRfalSt25tbPollerResetToInventory(ReturnCode *ret);
SER_API bool serExecRfalIsoDepInitialize(ReturnCode *ret);
SER_API bool serExecRfalIsoDepInitializeWithParams(ReturnCode *ret, rfalComplianceMode compMode, uint8_t maxRetriesR, uint8_t maxRetriesSnWTX, uint8_t maxRetriesSWTX, uint8_t maxRetriesSDSL, uint8_t maxRetriesI, uint8_t maxRetriesRATS);
SER_API bool serExecRfalIsoDepRATS(ReturnCode *ret, rfalIsoDepFSxI FSDI, uint8_t DID, rfalIsoDepAts *ats, uint8_t *atsLen);
SER_API bool serExecRfalIsoDepPPS(ReturnCode *ret, uint8_t DID, rfalBitRate DSI, rfalBitRate DRI, rfalIsoDepPpsRes *ppsRes);
SER_API bool serExecRfalIsoDepPollAHandleActivation(ReturnCode *ret, rfalIsoDepFSxI FSDI, uint8_t DID, rfalBitRate maxBR, rfalIsoDepInfo *info, rfalIsoDepAts *ats, uint8_t *atsLen);
SER_API bool serExecRfalIsoDepPollAStartActivation(ReturnCode *ret, rfalIsoDepFSxI FSDI, uint8_t DID, rfalBitRate maxBR);
SER_API bool serExecRfalIsoDepPollAGetActivationStatus(ReturnCode *ret, rfalIsoDepInfo *info, rfalIsoDepAts *ats, uint8_t *atsLen);
SER_API bool serExecRfalIsoDepATTRIB(ReturnCode *ret, uint8_t *nfcid0, uint8_t nfcid0Len, uint8_t PARAM1, rfalBitRate DSI, rfalBitRate DRI, rfalIsoDepFSxI FSDI, uint8_t PARAM3, uint8_t DID, uint8_t *HLInfo, uint8_t HLInfoLen, uint32_t fwt, rfalIsoDepAttribRes *attribRes, uint8_t *attribResLen);
SER_API bool serExecRfalIsoDepPollBHandleActivation(ReturnCode *ret, rfalIsoDepFSxI FSDI, uint8_t DID, rfalBitRate maxBR, uint8_t PARAM1, rfalNfcbListenDevice *nfcbDev, uint8_t *HLInfo, uint8_t HLInfoLen, rfalIsoDepInfo *info, rfalIsoDepAttribRes *attribRes, uint8_t *attribResLen);
SER_API bool serExecRfalIsoDepPollBStartActivation(ReturnCode *ret, rfalIsoDepFSxI FSDI, uint8_t DID, rfalBitRate maxBR, uint8_t PARAM1, rfalNfcbListenDevice *nfcbDev, uint8_t *HLInfo, uint8_t HLInfoLen);
SER_API bool serExecRfalIsoDepPollBGetActivationStatus(ReturnCode *ret, rfalIsoDepInfo *info, rfalIsoDepAttribRes *attribRes, uint8_t *attribResLen);
SER_API bool serExecRfalIsoDepGetMaxInfLen(uint16_t *maxInfLen);
SER_API bool serExecRfalIsoDepDeselect(ReturnCode *ret);
SER_API bool serExecRfalIsoDepStartApduTransceive(ReturnCode *ret, uint8_t *txBuf, uint16_t txBufLen, uint32_t FWT, uint32_t dFWT, uint16_t FSx, uint16_t ourFSx, uint8_t DID);
SER_API bool serExecRfalIsoDepGetApduTransceiveStatus(ReturnCode *ret, uint8_t **rxBuf, uint16_t *rxBufLen);
SER_API bool serExecRfalIsoDepStartTransceive(ReturnCode *ret, uint8_t *txBuf, uint16_t txBufLen, bool isTxChaining, uint32_t FWT, uint32_t dFWT, uint16_t FSx, uint16_t ourFSx, uint8_t DID);
SER_API bool serExecRfalIsoDepGetTransceiveStatus(ReturnCode *ret, uint8_t **rxBuf, uint16_t *rxBufLen, bool *isRxChaining);
SER_API bool serExecRfalNfcInitialize(ReturnCode *ret);
SER_API bool serExecRfalNfcDiscover(ReturnCode *ret, serRfalNfcDiscoverParam *disParams);
SER_API bool serExecRfalNfcGetState(rfalNfcState *state);
SER_API bool serExecRfalNfcGetDevicesFound(ReturnCode *ret, serFlatRfalNfcDevice *devList, uint8_t *devCnt);
SER_API bool serExecRfalNfcGetActiveDevice(ReturnCode *ret, serFlatRfalNfcDevice *dev, rfalNfcRfInterface *rfInterface, rfalIsoDepInfo *isoDepInfo, rfalNfcDepInfo *nfcDepInfo);
SER_API bool serExecRfalNfcSelect(ReturnCode *ret, uint8_t devIdx);
SER_API bool serExecRfalNfcDataExchangeStart(ReturnCode *ret, uint8_t *txData, uint16_t txDataBufLen, uint16_t txDataLen, uint32_t fwt);
SER_API bool serExecRfalNfcDataExchangeGetStatus(ReturnCode *ret, uint8_t **rxData, uint16_t *rxDataBufLen, uint16_t *rvdLen);
SER_API bool serExecRfalNfcDeactivate(ReturnCode *ret, rfalNfcDeactivateType deactType);
SER_API bool serExecRfalAnalogConfigInitialize(ReturnCode *ret);
SER_API bool serExecRfalAnalogConfigListWriteRaw(ReturnCode *ret, uint8_t *configTbl, uint16_t configTblSize);
SER_API bool serExecRfalAnalogConfigListReadRaw(ReturnCode *ret, uint8_t **configTbl, uint16_t *configTblSize);
SER_API bool serExecRfalDpoInitialize(ReturnCode *ret);
SER_API bool serExecRfalDpoTableWrite(ReturnCode *ret, rfalDpoEntry *powerTbl, uint8_t powerTblEntries);
SER_API bool serExecRfalDpoTableRead(ReturnCode *ret, uint8_t tblBufEntries, rfalDpoEntry *powerTbl, uint8_t *tableEntries);
SER_API bool serExecRfalDpoSetEnable(ReturnCode *ret, bool enable);
SER_API bool serExecRfalDpoIsEnabled(ReturnCode *ret, bool *enable);
SER_API bool serExecRfalDpoAdjust(ReturnCode *ret);
SER_API bool serExecRfalDpoGetInfo(ReturnCode *ret, bool *enable, rfalDpoEntry *powerTbl, uint8_t *tableEntries, uint8_t *index, uint8_t *adjustId, uint8_t *measureId, uint8_t *refMeasure);
SER_API bool serExecRfalChipWriteReg(ReturnCode *ret, uint16_t reg, uint8_t *values, uint8_t len);
SER_API bool serExecRfalChipReadReg(ReturnCode *ret, uint16_t reg, uint8_t len, uint8_t **values, uint8_t *actLen);
SER_API bool serExecRfalChipChangeRegBits(ReturnCode *ret, uint16_t reg, uint8_t valueMask, uint8_t value);
SER_API bool serExecRfalChipWriteTestReg(ReturnCode *ret, uint16_t reg, uint8_t value);
SER_API bool serExecRfalChipReadTestReg(ReturnCode *ret, uint16_t reg, uint8_t *value);
SER_API bool serExecRfalChipChangeTestRegBits(ReturnCode *ret, uint16_t reg, uint8_t valueMask, uint8_t value);
SER_API bool serExecRfalChipExecCmd(ReturnCode *ret, uint16_t cmd);
SER_API bool serExecRfalChipSetRFO(ReturnCode *ret, uint8_t RFO);
SER_API bool serExecRfalChipGetRFO(ReturnCode *ret, uint8_t *result);
SER_API bool serExecRfalChipMeasureAmplitude(ReturnCode *ret, uint8_t *result);
SER_API bool serExecRfalChipMeasurePhase(ReturnCode *ret, uint8_t *result);
SER_API bool serExecRfalChipMeasureCapacitance(ReturnCode *ret, uint8_t *result);
SER_API bool serExecRfalChipMeasurePowerSupply(ReturnCode *ret, uint8_t param, uint8_t *result);
SER_API bool serExecRfalChipMeasureIQ(ReturnCode *ret, int8_t *resI, int8_t *resQ);
SER_API bool serExecRfalChipMeasureCombinedIQ(ReturnCode *ret, uint8_t *result);
SER_API bool serExecRfalChipSetAntennaMode(ReturnCode *ret, bool single, bool rfiox);
SER_API bool serExecRfalChipSetLMMod(ReturnCode *ret, uint8_t mod, uint8_t unmod);
SER_API bool serExecRfalChipGetLMMod(ReturnCode *ret, uint8_t *mod, uint8_t *unmod);
SER_API bool serExecRfalWakeUpModeStart(ReturnCode *ret, bool useDefConf, serRfalWakeUpConfig *config);
SER_API bool serExecRfalWakeUpModeHasWoke(ReturnCode *ret, bool *hasWoke);
SER_API bool serExecRfalWakeUpModeGetInfo(ReturnCode *ret, bool force, serRfalWakeUpInfo *info);
SER_API bool serExecRfalWakeUpModeStop(ReturnCode *ret);
SER_API bool serExecRfalWlcPWptMonitorStart(ReturnCode *ret, bool useDefConf, serRfalWakeUpConfig *config);
SER_API bool serExecRfalWlcPWptMonitorStop(ReturnCode *ret);
SER_API bool serExecRfalWlcPWptIsFodDetected(ReturnCode *ret, bool *fodDetected);
SER_API bool serExecRfalWlcPWptIsStopDetected(ReturnCode *ret, bool *stopDetected);
SER_API bool serExecRfalLowPowerModeStart(ReturnCode *ret, rfalLpMode mode);
SER_API bool serExecRfalLowPowerModeStop(ReturnCode *ret);
SER_API bool serExecRfalST25xVPollerFastReadSingleBlock(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint8_t blockNum, uint8_t **rxBuf, uint16_t *rcvLen);
SER_API bool serExecRfalST25xVPollerFastExtendedReadSingleBlock(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint16_t blockNum, uint8_t **rxBuf, uint16_t *rcvLen);
SER_API bool serExecRfalST25xVPollerFastReadMultipleBlocks(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint8_t firstBlockNum, uint8_t numOfBlocks, uint8_t **rxBuf, uint16_t *rcvLen);
SER_API bool serExecRfalST25xVPollerFastExtReadMultipleBlocks(ReturnCode *ret, uint8_t flags, uint8_t *uid, uint8_t uidLen, uint16_t firstBlockNum, uint16_t numOfBlocks, uint8_t **rxBuf, uint16_t *rcvLen);
SER_API bool serExecEmvInitialize(ReturnCode *ret);
SER_API bool serExecEmvReset(ReturnCode *ret);
SER_API bool serExecEmvPowerOff(ReturnCode *ret);
SER_API bool serExecEmvPollOnce(ReturnCode *ret);
SER_API bool serExecEmvStartApplication(uint32_t application, ReturnCode *ret);
SER_API bool serExecEmvPollApplicationTagPresent(ReturnCode *ret, bool *removeTag);
SER_API bool serExecEmvWUPA(ReturnCode *ret);
SER_API bool serExecEmvWUPB(ReturnCode *ret);
SER_API bool serExecEmvRATS(ReturnCode *ret, uint8_t *uid, uint8_t *uidLength, rfalIsoDepAts *ats, uint8_t *atsLen);
SER_API bool serExecEmvATTRIB(ReturnCode *ret, uint8_t *uid, uint8_t *uidLength, rfalIsoDepAttribRes *attribRes, uint8_t *attribResLen);
SER_API bool serExecEmvTransacA(ReturnCode *ret);
SER_API bool serExecEmvTransacB(ReturnCode *ret);
SER_API bool serExecDemoRptGetBootMode(demoRptBootMode *mode);
SER_API bool serExecDemoRptSetBootMode(demoRptReturnCode *ret, demoRptBootMode mode);
SER_API bool serExecDemoRptGetReaderConfiguration(demoRptReturnCode *ret, demoRptReaderConfig *config);
SER_API bool serExecDemoRptSetReaderConfiguration(demoRptReturnCode *ret, demoRptReaderConfig *config);
SER_API bool serExecDemoRptGetTagConfiguration(demoRptReturnCode *ret, uint8_t useActivatedDevice, demoRptTagConfig *config);
SER_API bool serExecDemoRptSetTagConfiguration(demoRptReturnCode *ret, demoRptTagConfig *config);
SER_API bool serExecDemoRptStart(void);


/*
******************************************************************************
* GLOBAL TYPES
******************************************************************************
*/

#ifdef SER_USE_LOGGER
#define LOG_BUF_SIZE    4096
typedef struct logBuf
{
    char buf[LOG_BUF_SIZE];
    uint16_t bufLen;
} logBuf;
typedef void* callbackCallerCtx;
typedef void(*callbackSerReq)(callbackCallerCtx ctx, serBuf *buf, logBuf *log);
typedef void(*callbackSerRsp)(callbackCallerCtx ctx, serBuf *buf, logBuf *log);
#endif //SER_USE_LOGGER




#ifdef SER_USE_LOGGER
void registerSerReqLoggingCallback(callbackCallerCtx ctx, callbackSerReq cb, logBuf *usrMem);
void registerSerRspLoggingCallback(callbackCallerCtx ctx, callbackSerRsp cb, logBuf *usrMem);
#endif //SER_USE_LOGGER

extern void serDispatch( serBuf *buf );



#ifdef __cplusplus
}
#endif

#endif
