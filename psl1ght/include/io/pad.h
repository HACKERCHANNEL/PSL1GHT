#pragma once

#include <psl1ght/types.h>
#include <stdlib.h>

EXTERN_BEGIN

#define MAX_PADS		127
#define MAX_PORT_NUM	7
typedef struct PadInfo
{
	u32 max;					/* max pads allowed to connect */
	u32 connected;				/* how many pads connected */
	u32 info;					/* bit 0 lets the system intercept pad? other bits are reserved */
	u16 vendor_id[MAX_PADS];	/* vendor id */
	u16 product_id[MAX_PADS];	/* product id */
	u8  status[MAX_PADS];		/* 0: Not connected, 1: Connected, 5: Connected to custom controller.*/
} PadInfo;

typedef struct PadInfo2
{
	u32 max;								/* max pads allowed to connect */
	u32 connected;							/* how many pads connected */
	u32 info;								/* Bit 0 lets the system intercept pad? other bits are reserved */
    u32 port_status[MAX_PORT_NUM];			/* Bit 0: Connected (0: Disconnected, 1: Connected), Bit 1: assign changes? */
    u32 port_setting[MAX_PORT_NUM];			/* Bit 1: Pressure sensitivity turned on, Bit 2: Sensors turned on */
    u32 device_capability[MAX_PORT_NUM];	/* See: PadCapabilityInfo. Bit 0: PS3SPEC, Bit 1: has_pressure, Bit 2: has_sensor, Bit 3: has_hps, Bit 4: has_vibrate */
    u32 device_type[MAX_PORT_NUM];			/* 0: Standard,  4: Bluray Remote, 5: LDD */ 
} PadInfo2;

#define PAD_MAX_CODES 64
typedef struct PadData
{
	s32 len;
	union{
		u16 button[PAD_MAX_CODES];
		struct {
			u16 zeroes;
			unsigned int				:  8; 		/* reserved */
			unsigned int seven 			:  4;		/* always 0x7 */
			unsigned int halflen 		:  4;		/* len/2 */
			unsigned int				:  8;		/* reserved */
			/* Button information */
			/* 0: UP, 1: DOWN     */
			unsigned int BTN_LEFT 		:  1;
			unsigned int BTN_DOWN 		:  1;
			unsigned int BTN_RIGHT 		:  1;
			unsigned int BTN_UP 		:  1;
			unsigned int BTN_START 		:  1;
			unsigned int BTN_R3 		:  1;
			unsigned int BTN_L3 		:  1;
			unsigned int BTN_SELECT		:  1;
			
			unsigned int				:  8;		/* reserved */
			unsigned int BTN_SQUARE 	:  1;
			unsigned int BTN_CROSS 		:  1;
			unsigned int BTN_CIRCLE 	:  1;
			unsigned int BTN_TRIANGLE	:  1;
			unsigned int BTN_R1 		:  1;
			unsigned int BTN_L1 		:  1;
			unsigned int BTN_R2 		:  1;
			unsigned int BTN_L2			:  1;
			
			/* Analog nub information */
			/* 0x0000 - 0x00FF        */
			unsigned int ANA_R_H		: 16;
			unsigned int ANA_R_V		: 16;
			unsigned int ANA_L_H		: 16;
			unsigned int ANA_L_V		: 16;
			
			/* Pressure-sensitive information */
			/* 0x0000 - 0x00FF                */
			unsigned int PRE_RIGHT		: 16;
			unsigned int PRE_LEFT		: 16;
			unsigned int PRE_UP			: 16;
			unsigned int PRE_DOWN		: 16;
			unsigned int PRE_TRIANGLE	: 16;
			unsigned int PRE_CIRCLE		: 16;
			unsigned int PRE_CROSS		: 16;
			unsigned int PRE_SQUARE		: 16;
			unsigned int PRE_L1			: 16;
			unsigned int PRE_R1			: 16;
			unsigned int PRE_L2			: 16;
			unsigned int PRE_R2			: 16;
			
			/* Sensor information */
			/* 0x0000 - 0x03FF    */
			unsigned int SENSOR_X		: 16;
			unsigned int SENSOR_Y		: 16;
			unsigned int SENSOR_Z		: 16;
			unsigned int SENSOR_G		: 16;
			u8 reserved[80];		/* reserved */
		};
	};
} PadData;

#define PAD_CAPABILITY_INFO_MAX 32
typedef struct PadCapabilityInfo
{
	union {
		u32 info[PAD_CAPABILITY_INFO_MAX];
		struct {
			unsigned int 				: 27;		/* reserved */
			unsigned int has_vibrate	:  1;		/* controller has inbuilt vibration motor */
			unsigned int has_hps		:  1;		/* controller has high precision stick */
			unsigned int has_sensors	:  1;		/* controller has sensors, e.g. sixaxis, X,Y,Z,G */
			unsigned int has_pressure	:  1;		/* controller has pressure-sensitive buttons */
			unsigned int ps3spec		:  1;		/* controller meets ps3 specifications */
			u32 reserved[PAD_CAPABILITY_INFO_MAX-1];
		};
	};
} PadCapabilityInfo;

#define PAD_ACTUATOR_MAX	(2)
typedef struct PadActParam
{
	union {
		u8 motor[PAD_ACTUATOR_MAX];
		struct {
			/*  Small Motor  */
			/* 0: OFF, 1: On */
			u8 small_motor;
			/*  Large Motor                                  */
			/* 0: OFF, 1-255: Motor Speed, higher == faster. */
			u8 large_motor;
		};
	};
	u8 reserved[6];
} PadActParam;

typedef struct PadPeriphInfo
{
	u32 max;					/* max pads allowed to connect */
	u32 connected;				/* how many pads connected */
	u32 info;					/* bit 0 lets the system intercept pad? other bits are reserved */
    u32 port_status[MAX_PORT_NUM];			/* Bit 0: Connected (0: Disconnected, 1: Connected), Bit 1: assign changes? */
    u32 port_setting[MAX_PORT_NUM];			/* Bit 1: Pressure sensitivity turned on, Bit 2: Sensors turned on */
    u32 device_capability[MAX_PORT_NUM];	/* See: PadCapabilityInfo. Bit 0: PS3SPEC, Bit 1: has_pressure, Bit 2: has_sensor, Bit 3: has_hps, Bit 4: has_vibrate */
    u32 device_type[MAX_PORT_NUM];			/* 0: Standard,  4: Bluray Remote, 5: LDD */ 
    u32 pclass_type[MAX_PORT_NUM];
    u32 pclass_profile[MAX_PORT_NUM];
} PadPeriphInfo;

typedef struct PadPeriphData
{
  u32 pclass_type;
  u32 pclass_profile;
  s32 len;
  u16 button[PAD_MAX_CODES];
} PadPeriphData;

/**
 * \brief Initialize Pad library
 *
 * \param[in] max : maximum pads allowed to connect
 * \return 0 if OK or else error code
 */
s32 ioPadInit(const u32 max);

/**
 * \brief Close/End Pad library
 *
 * \return 0 if OK or else error code
 */
s32 ioPadEnd(void);

 /**
 * \brief Get Pad connection information
 *
 * \param[out] pad_info : pad information
 * \return 0 if OK or else error code
 */
s32 ioPadGetInfo(PadInfo* pad_info);

 /**
 * \brief Clear Pad Buffer Data
 *
 * \param[in] port : pad port number
 * \return 0 if OK or else error code
 */
s32 ioPadClearBuf(const u32 port);

 /**
 * \brief Get Pad Capability Info
 *
 * \param[in] port : pad port number
 * \param[out] pad_capinfo : pad capability info
 * \return 0 if OK or else error code
 */
s32 ioPadGetCapabilityInfo(const u32 port, PadCapabilityInfo* pad_capinfo);

 /**
 * \brief Get Pad Data Info
 *
 * \param[in] port : pad port number
 * \param[out] pad_data : pad data info
 * \return 0 if OK or else error code
 */
s32 ioPadGetData(const u32 port, PadData* pad_data);

 /**
 * \brief Pad Info Press Mode
 *
 * \param[in] port : pad port number
 * \return TBD
 */
s32 ioPadInfoPressMode(const u32 port);

 /**
 * \brief Set Pad Press Mode
 *
 * \param[in] port : pad port number
 * \param[in] mode : pad mode
 * \return TBD
 */
s32 ioPadSetPressMode(const u32 port, const u32 mode);

 /**
 * \brief Get Pad Info Sensor Mode
 *
 * \param[in] port : pad port number
 * \return TBD
 */
s32 ioPadInfoSensorMode(const u32 port);

 /**
 * \brief Set Pad Sensor Mode
 *
 * \param[in] port : pad port number
 * \param[in] mode : pad mode
 * \return TBD
 */
s32 ioPadSetSensorMode(const u32 port, const u32 mode);

 /**
 * \brief Set Pad Actuator Data to vibrate Pad
 *
 * \param[in] port : pad port number
 * \param[in] pad_actparam : pad actuator data
 * \return 0 if OK or else error code
 */
u32 ioPadSetActDirect(const u32 port, const PadActParam* pad_actparam);

 /**
 * \brief Set/Notify Custom Pad Button
 *
 * \param[in] handle : pad device handle returned by cellPadLddRegisterController()
 * \param[in] pad_data : pad data button to notify
 * \return 0 if OK or else error code
 */
u32 ioPadLddDataInsert(const s32 handle, const PadData* pad_data);

 /**
 * \brief Get Custom Pad port number
 *
 * \param[in] handle : pad device handle returned by cellPadLddRegisterController()
 * \return pad port number or error if >= 0x80121101
 */
s32 ioPadLddGetPortNo(const s32 handle);

 /**
 * \brief Register Custom Pad
 *
 * \return pad device handle
 */
s32 ioPadLddRegisterController(void);

 /**
 * \brief Unregister Custom Pad
 *
 * \param[in] handle : pad device handle returned by cellPadLddRegisterController()
 * \return 0 if OK or else error code
 */
s32 ioPadLddUnregisterController(const s32 handle);

/* new in 3.41 */
 /**
 * \brief Get Pad Information
 *
 * \param[out] pad_info2 : pad informations
 * \return 0 if OK or else error code
 */
s32 ioPadGetInfo2(PadInfo2* pad_info2);

 /**
 * \brief Get Pad Peripheral Information
 *
 * \param[out] pad_periphinfo : pad peripheral informations
 * \return 0 if OK or else error code
 */
s32 ioPadPeriphGetInfo(PadPeriphInfo* pad_periphinfo);

 /**
 * \brief Get Pad Peripheral Data
 *
 * \param[in] port : pad port number 
 * \param[out] pad_periphdata : pad peripheral data
 * \return 0 if OK or else error code
 */
s32 ioPadPeriphGetData(const u32 port, PadPeriphData* pad_periphdata);

 /**
 * \brief Set Pad port mode
 *
 * \param[in] port : pad port number 
 * \param[in] setting : pad mode setting (bit1 Pressure-sensitivity mode on, bit2 Six-acis sensor more on)
 * \return 0 if OK or else error code
 */
s32 ioPadSetPortSetting(const u32 port, const u32 setting);

EXTERN_END
