#pragma once

#include <psl1ght/types.h>

EXTERN_BEGIN

/* Message Dialog Maximum String Size including null terminator */
#define MSGDIALOG_STRING_SIZE_MAX	512

/* Message Dialog Progress Bar Maximum String Size including null terminator */
#define MSGDIALOG_PROGRESSBAR_STRING_SIZE_MAX	64

/* Pressed button Type */
typedef enum msgDialogButtonType {
	MSGDIALOG_BUTTON_NONE		= -1,
	MSGDIALOG_BUTTON_INVALID	= 0,
	MSGDIALOG_BUTTON_OK			= 1,
	MSGDIALOG_BUTTON_YES		= 1,
	MSGDIALOG_BUTTON_NO			= 2,
	MSGDIALOG_BUTTON_ESCAPE  	= 3
} msgDialogButtonType;

typedef void (* msgDialogCallback)(msgDialogButtonType buttonType, void *userData);

typedef enum msgDialogType {
	MSGDIALOG_DIALOG_TYPE_ERROR		= 0x00000000,
	MSGDIALOG_DIALOG_TYPE_NORMAL	= 0x00000001,
	MSGDIALOG_BUTTON_TYPE_NONE		= 0x00000000,
	MSGDIALOG_BUTTON_TYPE_YESNO		= 0x00000010,
	MSGDIALOG_DEFAULT_CURSOR_YES	= 0x00000000,
	MSGDIALOG_DEFAULT_CURSOR_NO		= 0x00000100,
} msgDialogType;

/* Type definition for msgDialogOpen2() */
/* Bit 31-14 reserved (shall be set to 0) */
#define MSGDIALOG_TYPE_PROGRESSBAR_NONE		(0<<12)
#define MSGDIALOG_TYPE_PROGRESSBAR_SINGLE	(1<<12)
#define MSGDIALOG_TYPE_PROGRESSBAR_DOUBLE	(2<<12)
/* Bit 11,10 reserved (shall be set to 0) */
#define MSGDIALOG_TYPE_DEFAULT_CURSOR_NONE	(0<<8)
#define MSGDIALOG_TYPE_DEFAULT_CURSOR_YES	(0<<8)
#define MSGDIALOG_TYPE_DEFAULT_CURSOR_NO	(1<<8)
#define MSGDIALOG_TYPE_DEFAULT_CURSOR_OK	(0<<8)

#define MSGDIALOG_TYPE_DISABLE_CANCEL_OFF	(0<<7)
#define MSGDIALOG_TYPE_DISABLE_CANCEL_ON	(1<<7)

#define MSGDIALOG_TYPE_BUTTON_TYPE_NONE		(0<<4)
#define MSGDIALOG_TYPE_BUTTON_TYPE_YESNO	(1<<4)
#define MSGDIALOG_TYPE_BUTTON_TYPE_OK 		(2<<4)
/* Bit 3 reserved (shall be set to 0) */
#define MSGDIALOG_TYPE_BG_VISIBLE			(0<<2)
#define MSGDIALOG_TYPE_BG_INVISIBLE			(1<<2)

#define MSGDIALOG_TYPE_SE_MUTE_OFF			(0<<1)
#define MSGDIALOG_TYPE_SE_MUTE_ON			(1<<1)

#define MSGDIALOG_TYPE_SE_TYPE_ERROR		(0<<0)
#define MSGDIALOG_TYPE_SE_TYPE_NORMAL		(1<<0)

typedef enum msgDialogProgressBarIndex {
	MSGDIALOG_PROGRESSBAR_INDEX_SINGLE			= 0,
	MSGDIALOG_PROGRESSBAR_INDEX_DOUBLE_UPPER	= 0,
	MSGDIALOG_PROGRESSBAR_INDEX_DOUBLE_LOWER	= 1
} msgDialogProgressBarIndex;

/**
 * \brief Force Any Message Dialog to Close
 *
 * \return 0 if OK or else error code
 */ 
s32 msgDialogAbort(void);

/**
 * \brief Close the Message Dialog after delay expired
 *
 * \param[in] delay_ms : Delay to wait in millisecond before to close Message Dialog
 * \return 0 if OK or else error code
 */ 
s32 msgDialogClose(float delay_ms);

/**
 * \brief Open the Message Dialog with specified string
 *
 * \param[in] type : Type of Message (can be combined) Dialog see msgDialogType
 * \param[in] msgString : Message string to display (see MSGDIALOG_STRING_SIZE_MAX for maximum size)
 * \param[in] func : Callback function pointer called with result of the message dialog
 * \param[in] userData : Application defined user data used by the callback
 * \param[in] nullParam : Shall be forced to NULL
 * \return 0 if OK or else error code
 */ 
s32 msgDialogOpen(msgDialogType type, const char *msgString, msgDialogCallback func, void *userData, void *nullParam);

/**
 * \brief Open the Message Dialog with specified string
 *
 * \param[in] type : Type of Message (can be combined) Dialog see define MSGDIALOG_TYPE_XXX
 * \param[in] msgString : Message string to display (see MSGDIALOG_STRING_SIZE_MAX for maximum size)
 * \param[in] func : Callback function pointer called with result of the message dialog
 * \param[in] userData : Application defined user data used by the callback
 * \param[in] nullParam : Shall be forced to NULL
 * \return 0 if OK or else error code
 */
s32 msgDialogOpen2(u32 type, const char *msgString, msgDialogCallback func, void *userData, void *nullParam);

/**
 * \brief Open the Message Dialog with specified error code
 *
 * \param[in] errorCode : Error code to display
 * \param[in] func : Callback function pointer called with result of the message dialog
 * \param[in] userData : Application defined user data used by the callback
 * \param[in] nullParam : Shall be forced to NULL
 * \return 0 if OK or else error code
 */
s32 cellMsgDialogOpenErrorCode(u32 errorCode, msgDialogCallback func, void *userData, void *nullParam);

/**
 * \brief Increment Message Dialog Progress Bar
 *
 * \param[in] progressBarIndex : Target Message Dialog Progress Bar
 * \param[in] delta : Progress to add in Percent
 * \return 0 if OK or else error code
 */
s32 msgDialogProgressBarInc(msgDialogProgressBarIndex progressBarIndex, u32 delta);

/**
 * \brief Reset Message Dialog Progress Bar
 *
 * \param[in] progressBarIndex : Target Message Dialog Progress
 * \return 0 if OK or else error code
 */
s32 msgDialogProgressBarReset(msgDialogProgressBarIndex progressBarIndex);

/**
 * \brief Set String to Message Dialog Progress Bar
 *
 * \param[in] progressBarIndex : Target Message Dialog Progress
 * \param[in] msgString : Message string to display (see MSGDIALOG_PROGRESSBAR_STRING_SIZE_MAX for maximum size)
 * \return 0 if OK or else error code
 */
s32 msgDialogProgressBarSetMsg(msgDialogProgressBarIndex progressBarIndex, const char *msgString);

EXTERN_END
