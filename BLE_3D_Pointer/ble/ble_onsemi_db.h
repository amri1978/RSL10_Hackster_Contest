/**
 * @file ble_onsemi_db.h
 * @author your name (you@domain.com)
 * @brief This file is intended to be a helper to store the GATT database information for the RSL10
 * @version 0.1
 * @date 2019-07-15
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _ATMO_ONSEMI_BLE_DB_H_
#define _ATMO_ONSEMI_BLE_DB_H_

#include "../app_src/atmosphere_platform.h"

/** \brief <i>Characteristic</i> declaration attribute UUID */
#define ATT_DECL_CHARACTERISTIC_128     { 0x03, 0x28, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00 }

/** \brief <i>Client Characteristic Configuration</i> attribute UUID */
#define ATT_DESC_CLIENT_CHAR_CFG_128    { 0x02, 0x29, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00 }

#define ATT_DECL_CHAR() \
    { ATT_DECL_CHARACTERISTIC_128, PERM(RD, ENABLE), 0, 0 }

#define ATT_DECL_CHAR_UUID_128(uuid, perm, max_length) \
    { uuid, perm, max_length, PERM(RI, ENABLE) | PERM(UUID_LEN, UUID_128) }

#define ATT_DECL_CHAR_CCC()                                                     \
    { ATT_DESC_CLIENT_CHAR_CFG_128, PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE), \
      0, PERM(RI, ENABLE) }

#define ATT_DECL_CHAR_USER_DESC(max_length)                      \
    { ATT_DESC_CHAR_USER_DESC_128, PERM(RD, ENABLE), max_length, \
      PERM(RI, ENABLE) }

#define ATMO_ONSEMI_BLE_MAX_ABILITIES_PER_EVENT 5

typedef struct {
    uint8_t *data;
    uint8_t maxLength;
    uint8_t currentLength;
    uint8_t handle;
    ATMO_AbilityHandle_t ability[ATMO_BLE_Characteristic_NumEvents][ATMO_ONSEMI_BLE_MAX_ABILITIES_PER_EVENT];
    uint8_t numAbilities[ATMO_BLE_Characteristic_NumEvents];
    ATMO_Callback_t callbacks[ATMO_BLE_Characteristic_NumEvents][ATMO_ONSEMI_BLE_MAX_ABILITIES_PER_EVENT];
    uint8_t numCallbacks[ATMO_BLE_Characteristic_NumEvents];
} _ATMO_ONSEMI_BLE_Characteristic_t;

/**
 * @brief Structure that defines a BLE service in the OnSemi Environment.
 * 
 */
typedef struct {
    ATMO_UUID_t uuid;
    uint8_t numCharacteristics;
    uint16_t startHandle;
    _ATMO_ONSEMI_BLE_Characteristic_t *characteristicDesc;
    struct gattm_att_desc *characteristics;
} _ATMO_ONSEMI_BLE_Service_t;

extern uint32_t _ATMO_ONSEMI_BLE_NumServices;
extern _ATMO_ONSEMI_BLE_Service_t _ATMO_ONSEMI_BLE_Services[];

#endif