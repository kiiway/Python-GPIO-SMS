/* Test for decoding SMS on Nokia 6510 driver */

#include <gammu.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"

#include "../libgammu/protocol/protocol.h"	/* Needed for GSM_Protocol_Message */
#include "../libgammu/gsmstate.h"	/* Needed for state machine internals */

#include "../helper/message-display.h"

unsigned char data[] = {
	0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x7B, 0x00, 0x00, 0x02, 0x9F, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x11, 0x0D, 0x0D, 0x91, 0x34, 0x96, 0x19, 0x69, 0x38, 0x91, 0xF9, 0x00, 0x00, 0xFF, 0x7B, 0x4F,
	0x39, 0x1D, 0xFD, 0x86, 0xBB, 0x40, 0x4B, 0x24, 0xE8, 0x58, 0x96, 0xCF, 0xE9, 0xE8, 0xB7, 0x59,
	0x71, 0x4D, 0x97, 0xD9, 0xE5, 0x76, 0xD8, 0x3D, 0x3F, 0xBB, 0x40, 0x32, 0x1C, 0x0B, 0x14, 0x8B,
	0xE1, 0x60, 0x8A, 0x39, 0x3D, 0x4C, 0x4F, 0xBF, 0xDD, 0xA0, 0x20, 0x0B, 0xA4, 0x4D, 0xBB, 0x40,
	0xB1, 0x18, 0x4C, 0x71, 0x2F, 0xCB, 0xC9, 0x65, 0x90, 0xFD, 0x2D, 0x0F, 0xD7, 0xE7, 0xF3, 0xF4,
	0x18, 0x4D, 0x67, 0xBB, 0x40, 0xCD, 0xB7, 0xFC, 0x5C, 0x76, 0x83, 0x9E, 0x70, 0x17, 0xC8, 0x04,
	0x32, 0xCB, 0xCB, 0x75, 0x50, 0x3B, 0x3D, 0x46, 0xB3, 0x40, 0xF7, 0xB2, 0xDB, 0x0D, 0x4A, 0xA3,
	0xE5, 0x20, 0x6D, 0x39, 0x4D, 0x07, 0xA1, 0xC3, 0x62, 0xBA, 0x0B, 0x01, 0x01, 0x71, 0x01, 0x00,
	0x01, 0x01, 0x03, 0x00, 0xF8, 0x00, 0x4F, 0x00, 0x72, 0x00, 0x74, 0x00, 0x68, 0x00, 0x6F, 0x00,
	0x70, 0x00, 0x2E, 0x00, 0x20, 0x00, 0x4B, 0x00, 0x48, 0x00, 0x20, 0x00, 0x47, 0x00, 0x65, 0x00,
	0x72, 0x00, 0x73, 0x00, 0x74, 0x00, 0x68, 0x00, 0x6F, 0x00, 0x66, 0x00, 0x0A, 0x00, 0x57, 0x00,
	0x69, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x6D, 0x00, 0x61, 0x00, 0x6E, 0x00, 0x73, 0x00,
	0x67, 0x00, 0x2E, 0x00, 0x20, 0x00, 0x32, 0x00, 0x38, 0x00, 0x2C, 0x00, 0x20, 0x00, 0x31, 0x00,
	0x31, 0x00, 0x38, 0x00, 0x30, 0x00, 0x0A, 0x00, 0x73, 0x00, 0x74, 0x00, 0x61, 0x00, 0x74, 0x00,
	0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x20, 0x00, 0x41, 0x00, 0x2C, 0x00, 0x20, 0x00, 0x5A, 0x00,
	0x69, 0x00, 0x2E, 0x00, 0x20, 0x00, 0x31, 0x00, 0x31, 0x00, 0x30, 0x00, 0x0A, 0x00, 0x77, 0x00,
	0x65, 0x00, 0x72, 0x00, 0x64, 0x00, 0x65, 0x00, 0x20, 0x00, 0x76, 0x00, 0x6F, 0x00, 0x72, 0x00,
	0x61, 0x00, 0x75, 0x00, 0x73, 0x00, 0x73, 0x00, 0x69, 0x00, 0x63, 0x00, 0x68, 0x00, 0x74, 0x00,
	0x6C, 0x00, 0x2E, 0x00, 0x20, 0x00, 0x4D, 0x00, 0x6F, 0x00, 0x72, 0x00, 0x67, 0x00, 0x65, 0x00,
	0x6E, 0x00, 0x20, 0x00, 0x4F, 0x00, 0x70, 0x00, 0x2E, 0x00, 0x20, 0x00, 0x26, 0x00, 0x20, 0x00,
	0x66, 0x00, 0x72, 0x00, 0x65, 0x00, 0x75, 0x00, 0x20, 0x00, 0x6D, 0x00, 0x69, 0x00, 0x63, 0x00,
	0x68, 0x00, 0x2C, 0x00, 0x20, 0x00, 0x77, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x6E, 0x00, 0x20, 0x00,
	0x69, 0x00, 0x68, 0x00, 0x72, 0x00, 0x20, 0x00, 0x5A, 0x00, 0x65, 0x00, 0x69, 0x00, 0x74, 0x00,
	0x20, 0x00, 0x68, 0x00, 0x61, 0x00, 0x62, 0x00, 0x74, 0x00, 0x2E, 0x00, 0x00, 0x04, 0x00, 0x1E,
	0x00, 0x2B, 0x00, 0x34, 0x00, 0x33, 0x00, 0x36, 0x00, 0x39, 0x00, 0x39, 0x00, 0x31, 0x00, 0x39,
	0x00, 0x36, 0x00, 0x38, 0x00, 0x33, 0x00, 0x31, 0x00, 0x39, 0x00, 0x39, 0x00, 0x00, 0x0C, 0x00,
	0x02, 0x35, 0x00, 0x07, 0x00, 0x01, 0x00, 0x05, 0x00, 0x01, 0x02, 0x04, 0x00, 0x1E, 0x00, 0x2B,
	0x00, 0x34, 0x00, 0x33, 0x00, 0x36, 0x00, 0x39, 0x00, 0x39, 0x00, 0x31, 0x00, 0x32, 0x00, 0x30,
	0x00, 0x39, 0x00, 0x34, 0x00, 0x35, 0x00, 0x39, 0x00, 0x34, 0x00, 0x00, 0x0C, 0x00, 0x02, 0x36,
	0x00, 0x07, 0x00, 0x01, 0x00, 0x05, 0x00, 0x01, 0x02, 0x06, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x01, 0x00, 0x0B, 0x00, 0x01, 0x00, 0x09, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
};

const char text[] = "Orthop. KH Gersthof\nWielemansg. 28, 1180\nstation A, Zi. 110\nwerde voraussichtl. Morgen Op. & freu mich, wenn ihr Zeit habt.";

/* This is not part of API! */
extern GSM_Error N6510_DecodeFilesystemSMS(GSM_StateMachine * s, GSM_MultiSMSMessage * sms, GSM_File * FFF, int location);

int main(int argc UNUSED, char **argv UNUSED)
{
	GSM_Debug_Info *debug_info;
	GSM_StateMachine *s;
	GSM_File file;
	GSM_Error error;
	GSM_MultiSMSMessage sms;

	debug_info = GSM_GetGlobalDebug();
	GSM_SetDebugFileDescriptor(stderr, FALSE, debug_info);
	GSM_SetDebugLevel("textall", debug_info);

	/* Allocates state machine */
	s = GSM_AllocStateMachine();
	test_result(s != NULL);

	debug_info = GSM_GetDebug(s);
	GSM_SetDebugGlobal(TRUE, debug_info);

	/* Init file */
	file.Buffer = malloc(sizeof(data));
	memcpy(file.Buffer, data, sizeof(data));
	file.Used = sizeof(data);
	file.ID_FullName[0] = 0;
	file.ID_FullName[1] = 0;
	GSM_GetCurrentDateTime(&(file.Modified));

	/* Parse it */
	error = N6510_DecodeFilesystemSMS(s, &sms, &file, 0);

	/* Display message */
	DisplayMultiSMSInfo(&sms, FALSE, TRUE, NULL, NULL);
	DisplayMultiSMSInfo(&sms, TRUE, TRUE, NULL, NULL);

	/* Free state machine */
	GSM_FreeStateMachine(s);

	/* Check expected text */
	test_result(strcmp(text, DecodeUnicodeString(sms.SMS[0].Text)) == 0);

	gammu_test_result(error, "N6510_DecodeFilesystemSMS");

	return 0;
}

/* Editor configuration
 * vim: noexpandtab sw=8 ts=8 sts=8 tw=72:
 */
