/* (c) 2001-2005 by Marcin Wiacek, Walek and Michal Cihar */

#ifndef __gsm_misc_h
#define __gsm_misc_h

#include <gammu-file.h>
#include <gammu-info.h>
#include <gammu-memory.h>

/**
 * Resets all members of GSM_BatteryCharge structure.
 */
void GSM_ClearBatteryCharge(GSM_BatteryCharge *bat);

/**
 * Prints a line (terminated with CRLF) to a buffer with error checking.
 */
GSM_Error VC_StoreLine(char *Buffer, const size_t buff_len, size_t *Pos, const char *format, ...);

/**
 * Prints a string to a buffer with error checking.
 */
GSM_Error VC_Store(char *Buffer, const size_t buff_len, size_t *Pos, const char *format, ...);

gboolean ReadVCALDateTime(const char *Buffer, GSM_DateTime *dt);
GSM_Error VC_StoreDateTime(char *Buffer, const size_t buff_len, size_t *Length, const GSM_DateTime *Date, const char *Start);

gboolean ReadVCALDate(char *Buffer, const char *Start, GSM_DateTime *Date, gboolean *is_date_only);
GSM_Error VC_StoreDate(char *Buffer, const size_t buff_len, size_t *Length, const GSM_DateTime *Date, const char *Start);

/**
 * Store base64 encoded string to buffer.
 */
GSM_Error VC_StoreBase64(char *Buffer, const size_t buff_len, size_t *Pos, const unsigned char *data, const size_t length);

/**
 * Stores text in vCalendar/vCard formatted buffer.
 *
 * \param[out] Buffer Buffer where output will be stored.
 * \param[in] buff_len Size of Buffer.
 * \param[in,out] Length Current position in output buffer (will be updated).
 * \param[in] Text Value to be stored.
 * \param[in] Start Name of field which is being stored.
 * \param[in] UTF8 Whether text should be stored in UTF-8 without prefix.
 */
GSM_Error VC_StoreText(char *Buffer, const size_t buff_len, size_t *Length, const unsigned char *Text, const char *Start, const gboolean UTF8);

/**
 * Reads text for vCalendar/vCard formatted line.
 *
 * \todo We should try to detect utf-8 and automatically use it.
 *
 * \param Buffer Buffer with input data.
 * \param Start Start expression of line.
 * \param Value Storage for value.
 * \param UTF8 Whether string should be treated as utf-8.
 */
gboolean ReadVCALText(char *Buffer, const char *Start, unsigned char *Value, const gboolean UTF8, GSM_EntryLocation *location);

gboolean ReadVCALInt(char *Buffer, const char *Start, int *Value);

unsigned char *VCALGetTextPart(unsigned char *Buff, int *pos);

#endif

/* How should editor hadle tabs in this file? Add editor commands here.
 * vim: noexpandtab sw=8 ts=8 sts=8:
 */
