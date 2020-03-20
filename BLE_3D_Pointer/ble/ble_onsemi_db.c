#include "../app_src/atmosphere_platform.h"
#include "ble_onsemi_db.h"

#define _ATMO_BLE_SERVICE_UUID_OrientationChar 0x1c, 0x19, 0xe1, 0xb, 0xd, 0x6e, 0x48, 0xb0, 0xfc, 0x44, 0x0, 0xc2, 0xca, 0x82, 0xc4, 0x69
#define _ATMO_BLE_CHARACTERISTIC_UUID_OrientationChar 0x1d, 0x19, 0xe1, 0xb, 0xd, 0x6e, 0x48, 0xb0, 0xfc, 0x44, 0x0, 0xc2, 0xca, 0x82, 0xc4, 0x69
static uint8_t _ATMO_BLE_CHARACTERISTIC_BUF_OrientationChar[12] = {0};
static uint8_t _ATMO_BLE_CHARACTERISTIC_CONFIG_BUF_OrientationChar[2] = {0};
#define _ATMO_BLE_SERVICE_UUID_undefined 0x2, 0xa4, 0x66, 0x96, 0xa5, 0xb0, 0xab, 0xab, 0x68, 0x43, 0x5e, 0x6b, 0xcf, 0x33, 0xe4, 0xbf
#define _ATMO_BLE_CHARACTERISTIC_UUID_undefined 0x2, 0xa4, 0x66, 0x96, 0xa5, 0xb0, 0xab, 0xab, 0x68, 0x43, 0x5f, 0x6b, 0xcf, 0x33, 0xe4, 0xbf
static uint8_t _ATMO_BLE_CHARACTERISTIC_BUF_undefined[64] = {0};
static uint8_t _ATMO_BLE_DeviceNameBuf[16] = {0};
static uint8_t _ATMO_BLE_Appearance[] = {0x00u, 0x03u, };
static uint8_t _ATMO_BLE_ServiceChanged[] = {0x01, 0xFF};
static uint8_t _ATMO_BLE_ServiceChangedDesc[2] = {0x0};
static uint8_t _ATMO_BLE_ProvData[64] = {0x0};
static uint8_t _ATMO_BLE_ProvDataDesc[] = {0x0, 0x00};
uint32_t _ATMO_ONSEMI_BLE_NumServices = 2;

static struct gattm_att_desc _ATMO_ONSEMI_BLE_Service_32[] = {
	ATT_DECL_CHAR(),
	ATT_DECL_CHAR_UUID_128({_ATMO_BLE_CHARACTERISTIC_UUID_OrientationChar}, PERM(WRITE_REQ, ENABLE) | PERM(WRITE_COMMAND, ENABLE) | PERM(RD, ENABLE) | PERM(NTF, ENABLE), 12),
	ATT_DECL_CHAR_CCC(),
};

static _ATMO_ONSEMI_BLE_Characteristic_t _ATMO_ONSEMI_BLE_Service_32_Desc[] = {
	{_ATMO_BLE_CHARACTERISTIC_BUF_OrientationChar,12, 12, 33, {0}, {0}, {0}, {0}},
};

static struct gattm_att_desc _ATMO_ONSEMI_BLE_Service_36[] = {
	ATT_DECL_CHAR(),
	ATT_DECL_CHAR_UUID_128({_ATMO_BLE_CHARACTERISTIC_UUID_undefined}, PERM(WRITE_REQ, ENABLE) | PERM(WRITE_COMMAND, ENABLE) | PERM(RD, ENABLE), 64),
	ATT_DECL_CHAR_CCC(),
};

static _ATMO_ONSEMI_BLE_Characteristic_t _ATMO_ONSEMI_BLE_Service_36_Desc[] = {
	{_ATMO_BLE_CHARACTERISTIC_BUF_undefined,64, 64, 37, {0}, {0}, {0}, {0}},
};

_ATMO_ONSEMI_BLE_Service_t _ATMO_ONSEMI_BLE_Services[] = {
	{
		{{0x1c, 0x19, 0xe1, 0xb, 0xd, 0x6e, 0x48, 0xb0, 0xfc, 0x44, 0x0, 0xc2, 0xca, 0x82, 0xc4, 0x69}, ATMO_UUID_Type_128_Bit, ATMO_ENDIAN_Type_Little},
		1,
		32,
		_ATMO_ONSEMI_BLE_Service_32_Desc,
		_ATMO_ONSEMI_BLE_Service_32
},
	{
		{{0x2, 0xa4, 0x66, 0x96, 0xa5, 0xb0, 0xab, 0xab, 0x68, 0x43, 0x5e, 0x6b, 0xcf, 0x33, 0xe4, 0xbf}, ATMO_UUID_Type_128_Bit, ATMO_ENDIAN_Type_Little},
		1,
		36,
		_ATMO_ONSEMI_BLE_Service_36_Desc,
		_ATMO_ONSEMI_BLE_Service_36
},
};