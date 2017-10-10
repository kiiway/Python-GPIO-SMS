/* Samsung-specific functions
 * Copyright (C) 2004 Claudio Matsuoka <cmatsuoka@gmail.com>
 * Copyright (c) 2009 Michal Cihar <michal@cihar.com>
 */

#include <gammu-config.h>

#ifdef GSM_ENABLE_ATGEN

#include <string.h>
#include <time.h>
#include <ctype.h>

#include "../../misc/coding/coding.h"
#include "../../gsmcomon.h"
#include "../pfunc.h"

#include "atgen.h"
#include "samsung.h"

/* Binary frame size */
#define BLKSZ 1024

struct ModelRes {
	const char *model;
	const size_t width;
	const size_t height;
};

static struct ModelRes modres[] = {
	{ "S100", 128, 128 },
	{ "S200", 128, 113 },
	{ "S300", 128,  97 },
	{ "S500", 128, 128 },
	{ "T100", 128, 128 },
	{ "E700", 128, 128 },
	{ NULL, 0, 0 }
};

/*
 * CRC functions from the Granch SBNI12 Linux driver by
 * Denis I. Timofeev <timofeev@granch.ru>
 */
static unsigned int crc32tab[] = {
	0xD202EF8D,  0xA505DF1B,  0x3C0C8EA1,  0x4B0BBE37,
	0xD56F2B94,  0xA2681B02,  0x3B614AB8,  0x4C667A2E,
	0xDCD967BF,  0xABDE5729,  0x32D70693,  0x45D03605,
	0xDBB4A3A6,  0xACB39330,  0x35BAC28A,  0x42BDF21C,
	0xCFB5FFE9,  0xB8B2CF7F,  0x21BB9EC5,  0x56BCAE53,
	0xC8D83BF0,  0xBFDF0B66,  0x26D65ADC,  0x51D16A4A,
	0xC16E77DB,  0xB669474D,  0x2F6016F7,  0x58672661,
	0xC603B3C2,  0xB1048354,  0x280DD2EE,  0x5F0AE278,
	0xE96CCF45,  0x9E6BFFD3,  0x0762AE69,  0x70659EFF,
	0xEE010B5C,  0x99063BCA,  0x000F6A70,  0x77085AE6,
	0xE7B74777,  0x90B077E1,  0x09B9265B,  0x7EBE16CD,
	0xE0DA836E,  0x97DDB3F8,  0x0ED4E242,  0x79D3D2D4,
	0xF4DBDF21,  0x83DCEFB7,  0x1AD5BE0D,  0x6DD28E9B,
	0xF3B61B38,  0x84B12BAE,  0x1DB87A14,  0x6ABF4A82,
	0xFA005713,  0x8D076785,  0x140E363F,  0x630906A9,
	0xFD6D930A,  0x8A6AA39C,  0x1363F226,  0x6464C2B0,
	0xA4DEAE1D,  0xD3D99E8B,  0x4AD0CF31,  0x3DD7FFA7,
	0xA3B36A04,  0xD4B45A92,  0x4DBD0B28,  0x3ABA3BBE,
	0xAA05262F,  0xDD0216B9,  0x440B4703,  0x330C7795,
	0xAD68E236,  0xDA6FD2A0,  0x4366831A,  0x3461B38C,
	0xB969BE79,  0xCE6E8EEF,  0x5767DF55,  0x2060EFC3,
	0xBE047A60,  0xC9034AF6,  0x500A1B4C,  0x270D2BDA,
	0xB7B2364B,  0xC0B506DD,  0x59BC5767,  0x2EBB67F1,
	0xB0DFF252,  0xC7D8C2C4,  0x5ED1937E,  0x29D6A3E8,
	0x9FB08ED5,  0xE8B7BE43,  0x71BEEFF9,  0x06B9DF6F,
	0x98DD4ACC,  0xEFDA7A5A,  0x76D32BE0,  0x01D41B76,
	0x916B06E7,  0xE66C3671,  0x7F6567CB,  0x0862575D,
	0x9606C2FE,  0xE101F268,  0x7808A3D2,  0x0F0F9344,
	0x82079EB1,  0xF500AE27,  0x6C09FF9D,  0x1B0ECF0B,
	0x856A5AA8,  0xF26D6A3E,  0x6B643B84,  0x1C630B12,
	0x8CDC1683,  0xFBDB2615,  0x62D277AF,  0x15D54739,
	0x8BB1D29A,  0xFCB6E20C,  0x65BFB3B6,  0x12B88320,
	0x3FBA6CAD,  0x48BD5C3B,  0xD1B40D81,  0xA6B33D17,
	0x38D7A8B4,  0x4FD09822,  0xD6D9C998,  0xA1DEF90E,
	0x3161E49F,  0x4666D409,  0xDF6F85B3,  0xA868B525,
	0x360C2086,  0x410B1010,  0xD80241AA,  0xAF05713C,
	0x220D7CC9,  0x550A4C5F,  0xCC031DE5,  0xBB042D73,
	0x2560B8D0,  0x52678846,  0xCB6ED9FC,  0xBC69E96A,
	0x2CD6F4FB,  0x5BD1C46D,  0xC2D895D7,  0xB5DFA541,
	0x2BBB30E2,  0x5CBC0074,  0xC5B551CE,  0xB2B26158,
	0x04D44C65,  0x73D37CF3,  0xEADA2D49,  0x9DDD1DDF,
	0x03B9887C,  0x74BEB8EA,  0xEDB7E950,  0x9AB0D9C6,
	0x0A0FC457,  0x7D08F4C1,  0xE401A57B,  0x930695ED,
	0x0D62004E,  0x7A6530D8,  0xE36C6162,  0x946B51F4,
	0x19635C01,  0x6E646C97,  0xF76D3D2D,  0x806A0DBB,
	0x1E0E9818,  0x6909A88E,  0xF000F934,  0x8707C9A2,
	0x17B8D433,  0x60BFE4A5,  0xF9B6B51F,  0x8EB18589,
	0x10D5102A,  0x67D220BC,  0xFEDB7106,  0x89DC4190,
	0x49662D3D,  0x3E611DAB,  0xA7684C11,  0xD06F7C87,
	0x4E0BE924,  0x390CD9B2,  0xA0058808,  0xD702B89E,
	0x47BDA50F,  0x30BA9599,  0xA9B3C423,  0xDEB4F4B5,
	0x40D06116,  0x37D75180,  0xAEDE003A,  0xD9D930AC,
	0x54D13D59,  0x23D60DCF,  0xBADF5C75,  0xCDD86CE3,
	0x53BCF940,  0x24BBC9D6,  0xBDB2986C,  0xCAB5A8FA,
	0x5A0AB56B,  0x2D0D85FD,  0xB404D447,  0xC303E4D1,
	0x5D677172,  0x2A6041E4,  0xB369105E,  0xC46E20C8,
	0x72080DF5,  0x050F3D63,  0x9C066CD9,  0xEB015C4F,
	0x7565C9EC,  0x0262F97A,  0x9B6BA8C0,  0xEC6C9856,
	0x7CD385C7,  0x0BD4B551,  0x92DDE4EB,  0xE5DAD47D,
	0x7BBE41DE,  0x0CB97148,  0x95B020F2,  0xE2B71064,
	0x6FBF1D91,  0x18B82D07,  0x81B17CBD,  0xF6B64C2B,
	0x68D2D988,  0x1FD5E91E,  0x86DCB8A4,  0xF1DB8832,
	0x616495A3,  0x1663A535,  0x8F6AF48F,  0xF86DC419,
	0x660951BA,  0x110E612C,  0x88073096,  0xFF000000
};

static unsigned int GetCRC(char *data, int size)
{
	unsigned int crc = 0;

	while (size--)
		crc = crc32tab[(crc ^ *data++) & 0xff] ^ ((crc >> 8) & 0x00FFFFFF);

	return crc;
}

/*
 * Frame transfer
 */

static GSM_Error WaitFor(GSM_StateMachine *s, const char *t, int ttl)
{
	char 		readbuf[100]={0};
	int 		n=0,sec=0;
        GSM_DateTime    Date;

        GSM_GetCurrentDateTime (&Date);
        sec = Date.Second;

	n = s->Device.Functions->ReadDevice(s, readbuf, sizeof(readbuf) - 1);
	readbuf[n] = '\0';

	while (strstr(readbuf, t) == NULL && (sec + ttl) >= Date.Second) {
		usleep(500000);
		n = s->Device.Functions->ReadDevice(s, readbuf, sizeof(readbuf) - 1);
		readbuf[n] = '\0';
        	GSM_GetCurrentDateTime (&Date);
	}

	return (sec + ttl) >= Date.Second ? ERR_NONE : ERR_TIMEOUT;
}

static GSM_Error SetSamsungFrame(GSM_StateMachine *s, unsigned char *buff, int size, GSM_Phone_RequestID id)
{
	GSM_Phone_Data		*Phone = &s->Phone.Data;
	GSM_Error		error;
	int			i, count;

	count = size / BLKSZ;

	for (i = 0; i < count; i++) {
		error = WaitFor(s, ">", 4);
 		if (error!=ERR_NONE) return error;

 		error = s->Protocol.Functions->WriteMessage(s,
			buff + i * BLKSZ, BLKSZ, 0x00);
 		if (error!=ERR_NONE) return error;
	}

	error = WaitFor(s, ">", 4);
 	if (error!=ERR_NONE) return error;
	error = s->Protocol.Functions->WriteMessage(s,
		buff + i * BLKSZ, size%BLKSZ, 0x00);
	if (error!=ERR_NONE) return error;

	error = GSM_WaitFor(s, "", 0, 0x00, 4, id);
	if (error!=ERR_NONE) return error;

	return Phone->DispatchError;
}

/* Answer format for binary data transfer
 *
 * SDNDCRC = 0xa : RECEIVECRC = 0xcbf53a1c : BINSIZE = 5
 * CRCERR
 */
static GSM_Error ReplySetSamsungFrame(GSM_Protocol_Message *msg, GSM_StateMachine *s)
{
	unsigned long 		txcrc, rxcrc;
	int 			binsize;
	char 			*pos;

	/* Parse SDNDCRC */
	pos = strchr(msg->Buffer, '=');
	if (!pos) return ERR_UNKNOWN;
	pos++;
	txcrc = strtoul(pos, NULL, 0);
	smprintf(s, "Sent CRC     : 0x%lx\n", txcrc);

	/* Parse RECEIVECRC */
	pos = strchr(pos, '=');
	if (!pos) return ERR_UNKNOWN;
	pos++;
	rxcrc = strtoul(pos, NULL, 0);
	smprintf(s, "Reveived CRC : 0x%lx\n", rxcrc);

	/* Parse BINSIZE */
	pos = strchr(pos, '=');
	if (!pos) return ERR_UNKNOWN;
	pos++;
	binsize = strtoul(pos, NULL, 0);
	smprintf(s, "Binary size  : %d\n", binsize);

	return txcrc == rxcrc ? ERR_NONE : ERR_WRONGCRC;
}

/*
 * Bitmaps
 */

GSM_Error SAMSUNG_ReplyGetBitmap(GSM_Protocol_Message *msg, GSM_StateMachine *s)
{
 	GSM_Phone_ATGENData 	*Priv = &s->Phone.Data.Priv.ATGEN;
	char 		buffer[32];
	char 			*pos;
	int			location, count;

 	switch (Priv->ReplyState) {
 	case AT_Reply_OK:
		smprintf(s, "Bitmap info received\n");
		/* Parse +IMGR:location,name,0,0,0,0 */

		/* Parse location */
		pos = strchr(msg->Buffer, ':');
		if (!pos) return ERR_UNKNOWN;
		pos++;
		location = atoi(pos);
		smprintf(s, "Location : %d\n", location);

		/* Parse name */
		pos = strchr(pos, '"');
		if (!pos) return ERR_UNKNOWN;
		pos++;
		for (count = 0; count < 31; count++) {
			if (pos[count] == '"')
				break;
			buffer[count] = pos[count];
		}
		buffer[count] = 0;
		smprintf(s, "Name     : %s\n", buffer);
		EncodeUnicode(s->Phone.Data.Bitmap->Name, buffer, strlen(buffer));

		s->Phone.Data.Bitmap->Location = location;

		return ERR_NONE;
	case AT_Reply_Error:
		return ERR_NOTSUPPORTED;
	case AT_Reply_CMSError:
	        return ATGEN_HandleCMSError(s);
	case AT_Reply_CMEError:
	        return ATGEN_HandleCMEError(s);
 	default:
		return ERR_UNKNOWNRESPONSE;
	}
}

GSM_Error SAMSUNG_ReplySetBitmap(GSM_Protocol_Message *msg, GSM_StateMachine *s)
{
	smprintf(s, "Bitmap sent\n");
	return ReplySetSamsungFrame(msg, s);
}

GSM_Error SAMSUNG_GetBitmap(GSM_StateMachine *s, GSM_Bitmap *Bitmap)
{
	unsigned char req[100];
	size_t len;

	s->Phone.Data.Bitmap=Bitmap;
	smprintf(s, "Getting bitmap\n");
	len = sprintf(req, "AT+IMGR=%d\r", Bitmap->Location-1);
	return GSM_WaitFor (s, req, len, 0x00, 4, ID_GetBitmap);
}

GSM_Error SAMSUNG_SetBitmap(GSM_StateMachine *s, GSM_Bitmap *Bitmap)
{
	unsigned char	req[100];
	unsigned long	crc;
	GSM_Error	error;
	char		name[50], *dot;
    const char *model;
	GSM_Phone_Data  *Data = &s->Phone.Data;
	int 		i;
	size_t len;

	s->Phone.Data.Bitmap = Bitmap;
	smprintf(s, "Setting bitmap\n");

	if (Bitmap->Type != GSM_PictureBinary) {
		smprintf(s, "Invalid picture type\n");
		return ERR_INVALIDDATA;
	}

	if (Bitmap->BinaryPic.Type != PICTURE_GIF) {
		smprintf(s, "Invalid binary picture type\n");
		return ERR_INVALIDDATA;
	}

	/* Check if picture size matches phone model */
	model = Data->ModelInfo->model;
	smprintf(s, "Checking picture size for %s\n", model);
	for (i = 0; modres[i].model; i++) {
		if (!strcmp(model, modres[i].model)) {
			if (Bitmap->BitmapWidth != modres[i].width ||
			    Bitmap->BitmapHeight != modres[i].height) {
				smprintf(s, "Model %s must use %ld x %ld picture size\n",
					modres[i].model,
					(long)modres[i].width,
					(long)modres[i].height);
				return ERR_INVALIDDATA;
			}
			break;
		}
	}
	if (modres[i].model == NULL) {
		smprintf(s, "Model \"%s\" is not supported.\n", Data->Model);
		return ERR_NOTSUPPORTED;
	}

	crc = GetCRC(Bitmap->BinaryPic.Buffer, Bitmap->BinaryPic.Length);

	/* Remove extension from file name */
	strncpy(name, DecodeUnicodeString(Bitmap->Name), 50);
	name[49] = '\0';
	if ((dot = strrchr(name, '.')) != NULL)
		*dot = 0;

	len = sprintf(req, "AT+IMGW=0,\"%s\",2,0,0,0,0,100,%ld,%u\r", name,
		(long int)Bitmap->BinaryPic.Length, (unsigned int)crc);

	error = s->Protocol.Functions->WriteMessage(s, req, len, 0x00);
	if (error!=ERR_NONE) return error;

	return SetSamsungFrame(s, Bitmap->BinaryPic.Buffer,
		Bitmap->BinaryPic.Length, ID_SetBitmap);
}

/*
 * Ringtones
 */

GSM_Error SAMSUNG_ReplyGetRingtone(GSM_Protocol_Message *msg, GSM_StateMachine *s)
{
 	GSM_Phone_ATGENData 	*Priv = &s->Phone.Data.Priv.ATGEN;
	unsigned char 		buffer[32];
	char 			*pos;
	int			location, length, count;

 	switch (Priv->ReplyState) {
 	case AT_Reply_OK:
		smprintf(s, "Ringtone info received\n");
		/* Parse +MELR:location,name,size */

		/* Parse location */
		pos = strchr(msg->Buffer, ':');
		if (!pos) return ERR_UNKNOWN;
		pos++;
		location = atoi(pos);
		smprintf(s, "Location : %d\n", location);

		/* Parse name */
		pos = strchr(pos, '"');
		if (!pos) return ERR_UNKNOWN;
		pos++;
		/* Ringtone.Name size is 20 chars */
		for (count = 0; count < 19; count++) {
			if (pos[count] == '"')
				break;
			buffer[count] = pos[count];
		}
		buffer[count] = 0;
		smprintf(s, "Name     : %s\n", buffer);
		EncodeUnicode(s->Phone.Data.Ringtone->Name,buffer,strlen(buffer));

		/* Parse ringtone length */
		pos = strchr(pos, ',');
		if (!pos) return ERR_UNKNOWN;
		pos++;
		length = atoi(pos);
		smprintf(s, "Length   : %d\n", length);

		/* S300 ringtones are always MMF */
		s->Phone.Data.Ringtone->Format = RING_MMF;
		s->Phone.Data.Ringtone->Location = location;
		s->Phone.Data.Ringtone->BinaryTone.Length = length;

		return ERR_NONE;
	case AT_Reply_Error:
		return ERR_UNKNOWN;
	case AT_Reply_CMSError:
	        return ATGEN_HandleCMSError(s);
	case AT_Reply_CMEError:
	        return ATGEN_HandleCMEError(s);
 	default:
		return ERR_UNKNOWNRESPONSE;
	}
}

GSM_Error SAMSUNG_GetRingtone(GSM_StateMachine *s, GSM_Ringtone *Ringtone, gboolean PhoneRingtone UNUSED)
{
	unsigned char req[100];
	size_t len;

	s->Phone.Data.Ringtone = Ringtone;
	smprintf(s, "Getting ringtone\n");
	len = sprintf(req, "AT+MELR=%d\r", Ringtone->Location-1);
	return GSM_WaitFor (s, req, len, 0x00, 4, ID_GetRingtone);
}

GSM_Error SAMSUNG_ReplySetRingtone(GSM_Protocol_Message *msg, GSM_StateMachine *s)
{
	smprintf(s, "Ringtone sent\n");
	return ReplySetSamsungFrame(msg, s);
}

GSM_Error SAMSUNG_SetRingtone(GSM_StateMachine *s, GSM_Ringtone *Ringtone, int *maxlength UNUSED)
{
	unsigned char	req[100];
	unsigned long	crc;
	GSM_Error	error;
	char		name[50], *dot;
	size_t len;

	s->Phone.Data.Ringtone = Ringtone;
	smprintf(s, "Setting ringtone\n");

	if (Ringtone->Format != RING_MMF) {
		smprintf(s, "Not MMF ringtone\n");
		return ERR_INVALIDDATA;
	}

	/* Remove extension from file name */
	strncpy(name, DecodeUnicodeString(Ringtone->Name), 50);
	name[49] = '\0';
	if ((dot = strrchr(name, '.')) != NULL) *dot = 0;

	crc = GetCRC(Ringtone->BinaryTone.Buffer, Ringtone->BinaryTone.Length);
	len = sprintf(req, "AT+MELW=0,\"%s\",4,%ld,%u\r", name,
		(long)Ringtone->BinaryTone.Length, (unsigned int)crc);

	error = s->Protocol.Functions->WriteMessage(s, req, len, 0x00);
	if (error!=ERR_NONE) return error;

	return SetSamsungFrame(s, Ringtone->BinaryTone.Buffer,
		Ringtone->BinaryTone.Length, ID_SetRingtone);
}

/**
 * Check what variant of calendar protocol for Samsung is supported.
 */
GSM_Error SAMSUNG_CheckCalendar(GSM_StateMachine *s)
{
 	GSM_Phone_ATGENData *Priv = &s->Phone.Data.Priv.ATGEN;
	GSM_Error error;

	if (Priv->SamsungCalendar != 0) {
		return ERR_NONE;
	}

	smprintf(s, "Checking for supported calendar commands\n");

	error = ATGEN_WaitForAutoLen(s, "AT+SSHT?\r", 0x00, 10, ID_GetProtocol);
	if (error == ERR_NONE) {
		Priv->SamsungCalendar = SAMSUNG_SSH;
		return ERR_NONE;
	}

	error = ATGEN_WaitForAutoLen(s, "AT+ORGI?\r", 0x00, 10, ID_GetProtocol);
	if (error == ERR_NONE) {
		Priv->SamsungCalendar = SAMSUNG_ORG;
		return ERR_NONE;
	}

	Priv->SamsungCalendar = SAMSUNG_NONE;
	return ERR_NONE;

}

GSM_Error SAMSUNG_ReplyGetMemoryInfo(GSM_Protocol_Message *msg, GSM_StateMachine *s)
{
 	GSM_Phone_ATGENData 	*Priv = &s->Phone.Data.Priv.ATGEN;

	Priv->PBK_SPBR = AT_NOTAVAILABLE;

 	switch (Priv->ReplyState) {
 	case AT_Reply_OK:
		/* FIXME: does phone give also some useful infromation here? */
		Priv->PBK_SPBR = AT_AVAILABLE;

		return ERR_NONE;
	case AT_Reply_Error:
		return ERR_UNKNOWN;
	case AT_Reply_CMSError:
	        return ATGEN_HandleCMSError(s);
	case AT_Reply_CMEError:
	        return ATGEN_HandleCMEError(s);
 	default:
		return ERR_UNKNOWNRESPONSE;
	}
}

GSM_Error SAMSUNG_ReplyGetMemory(GSM_Protocol_Message *msg, GSM_StateMachine *s)
{
 	GSM_Phone_ATGENData 	*Priv = &s->Phone.Data.Priv.ATGEN;
 	GSM_MemoryEntry		*Memory = s->Phone.Data.Memory;
	GSM_Error error;
	const char *str;
	int i, j, year = 1900, month = 0, day = 0;

	switch (Priv->ReplyState) {
	case AT_Reply_OK:
 		smprintf(s, "Phonebook entry received\n");
		Memory->EntriesNum = 12;
		Memory->Entries[0].EntryType = PBK_Number_Mobile;
		Memory->Entries[0].Location = PBK_Location_Unknown;
		Memory->Entries[0].AddError = ERR_NONE;
		Memory->Entries[0].VoiceTag = 0;
		Memory->Entries[0].SMSList[0] = 0;
		Memory->Entries[1].EntryType = PBK_Number_General;
		Memory->Entries[1].Location = PBK_Location_Home;
		Memory->Entries[1].AddError = ERR_NONE;
		Memory->Entries[1].VoiceTag = 0;
		Memory->Entries[1].SMSList[0] = 0;
		Memory->Entries[2].EntryType = PBK_Number_General;
		Memory->Entries[2].Location = PBK_Location_Work;
		Memory->Entries[2].AddError = ERR_NONE;
		Memory->Entries[2].VoiceTag = 0;
		Memory->Entries[2].SMSList[0] = 0;
		Memory->Entries[3].EntryType = PBK_Number_Fax;
		Memory->Entries[3].Location = PBK_Location_Unknown;
		Memory->Entries[3].AddError = ERR_NONE;
		Memory->Entries[3].VoiceTag = 0;
		Memory->Entries[3].SMSList[0] = 0;
		Memory->Entries[4].EntryType = PBK_Number_General;
		Memory->Entries[4].Location = PBK_Location_Unknown;
		Memory->Entries[4].AddError = ERR_NONE;
		Memory->Entries[4].VoiceTag = 0;
		Memory->Entries[4].SMSList[0] = 0;
		Memory->Entries[5].EntryType = PBK_Text_Email;
		Memory->Entries[5].Location = PBK_Location_Unknown;
		Memory->Entries[5].AddError = ERR_NONE;
		Memory->Entries[5].VoiceTag = 0;
		Memory->Entries[5].SMSList[0] = 0;
		Memory->Entries[6].EntryType = PBK_Text_FirstName;
		Memory->Entries[6].Location = PBK_Location_Unknown;
		Memory->Entries[6].AddError = ERR_NONE;
		Memory->Entries[6].VoiceTag = 0;
		Memory->Entries[6].SMSList[0] = 0;
		Memory->Entries[7].EntryType = PBK_Text_LastName;
		Memory->Entries[7].Location = PBK_Location_Unknown;
		Memory->Entries[7].AddError = ERR_NONE;
		Memory->Entries[7].VoiceTag = 0;
		Memory->Entries[7].SMSList[0] = 0;
		Memory->Entries[8].EntryType = PBK_Text_Note;
		Memory->Entries[8].Location = PBK_Location_Unknown;
		Memory->Entries[8].AddError = ERR_NONE;
		Memory->Entries[8].VoiceTag = 0;
		Memory->Entries[8].SMSList[0] = 0;
		Memory->Entries[9].EntryType = PBK_Text_Note;
		Memory->Entries[9].Location = PBK_Location_Unknown;
		Memory->Entries[9].AddError = ERR_NONE;
		Memory->Entries[9].VoiceTag = 0;
		Memory->Entries[9].SMSList[0] = 0;
		EncodeUnicode(Memory->Entries[9].Text, "", 0);
		Memory->Entries[10].EntryType = PBK_Text_Note;
		Memory->Entries[10].Location = PBK_Location_Unknown;
		Memory->Entries[10].AddError = ERR_NONE;
		Memory->Entries[10].VoiceTag = 0;
		Memory->Entries[10].SMSList[0] = 0;
		EncodeUnicode(Memory->Entries[10].Text, "", 0);
		Memory->Entries[11].EntryType = PBK_Text_Note;
		Memory->Entries[11].Location = PBK_Location_Unknown;
		Memory->Entries[11].AddError = ERR_NONE;
		Memory->Entries[11].VoiceTag = 0;
		Memory->Entries[11].SMSList[0] = 0;
		EncodeUnicode(Memory->Entries[11].Text, "", 0);

		/* Get line from reply */
		str = GetLineString(msg->Buffer, &Priv->Lines, 2);

		/* Empty entry */
		if (strcmp(str, "OK") == 0) return ERR_EMPTY;

		/* Philips has it's SPBR as well, but different reply */
		if ( Priv->Manufacturer == AT_Philips) {
			error = ATGEN_ParseReply(s,
						GetLineString(msg->Buffer, &Priv->Lines, 2),
						"+SPBR: @n, @u, @p",
						&Memory->Location,
						Memory->Entries[0].Text, sizeof(Memory->Entries[0].Text),
						Memory->Entries[1].Text, sizeof(Memory->Entries[1].Text));
			if (error == ERR_NONE) {
				/* Set name type */
				Memory->Entries[0].EntryType = PBK_Text_Name;
				Memory->Entries[0].Location = PBK_Location_Unknown;

				/* Set number type */
				Memory->Entries[1].EntryType = PBK_Number_General;
				Memory->Entries[1].Location = PBK_Location_Unknown;
				Memory->Entries[1].VoiceTag = 0;
				Memory->Entries[1].SMSList[0] = 0;

				return ERR_NONE;
			}
		}

		/*
		 * Parse reply string
		 *
		 * The last string seems to be always empty, so it is
		 * not handled in rest of the code.
		 */
		error = ATGEN_ParseReply(s, str,
					"+SPBR: @i, @p, @p, @p, @p, @p, @s, @T, @T, @T, @T",
					&Memory->Location,
					Memory->Entries[0].Text, sizeof(Memory->Entries[0].Text),
					Memory->Entries[1].Text, sizeof(Memory->Entries[1].Text),
					Memory->Entries[2].Text, sizeof(Memory->Entries[2].Text),
					Memory->Entries[3].Text, sizeof(Memory->Entries[3].Text),
					Memory->Entries[4].Text, sizeof(Memory->Entries[4].Text),
					Memory->Entries[5].Text, sizeof(Memory->Entries[5].Text),
					Memory->Entries[6].Text, sizeof(Memory->Entries[6].Text),
					Memory->Entries[7].Text, sizeof(Memory->Entries[7].Text),
					Memory->Entries[8].Text, sizeof(Memory->Entries[8].Text),
					Memory->Entries[9].Text, sizeof(Memory->Entries[9].Text));
		if (error != ERR_NONE) {
			/*
			 * Some phones have different string:
			 * +SPBR: 1,"+999999999999","","","","","aaaaaaaaaa@gmail.com","6,Aaaaaa","8,Ssssssss",1999,1,31,"2,Me","0,"
			 */
			error = ATGEN_ParseReply(s, str,
						"+SPBR: @i, @p, @p, @p, @p, @p, @s, @T, @T, @i, @i, @i, @T, @T",
						&Memory->Location,
						Memory->Entries[0].Text, sizeof(Memory->Entries[0].Text),
						Memory->Entries[1].Text, sizeof(Memory->Entries[1].Text),
						Memory->Entries[2].Text, sizeof(Memory->Entries[2].Text),
						Memory->Entries[3].Text, sizeof(Memory->Entries[3].Text),
						Memory->Entries[4].Text, sizeof(Memory->Entries[4].Text),
						Memory->Entries[5].Text, sizeof(Memory->Entries[5].Text),
						Memory->Entries[6].Text, sizeof(Memory->Entries[6].Text),
						Memory->Entries[7].Text, sizeof(Memory->Entries[7].Text),
						&year, &month, &day,
						Memory->Entries[8].Text, sizeof(Memory->Entries[8].Text),
						Memory->Entries[9].Text, sizeof(Memory->Entries[9].Text));
		}
		if (error != ERR_NONE) {
			/*
			 * Some phones have different string:
			 * +SPBR:1,"5,37217201","0,","0,","0,","0,","14,admrede@inf.ufsc.br","0,","11,Admrede Inf","4,Ufsc","0,","0,",1900,1,1,"0,"
			 */
			error = ATGEN_ParseReply(s, str,
						"+SPBR: @i, @T, @T, @T, @T, @T, @T, @T, @T, @T, @T, @T, @i, @i, @i, @T",
						&Memory->Location,
						Memory->Entries[0].Text, sizeof(Memory->Entries[0].Text),
						Memory->Entries[1].Text, sizeof(Memory->Entries[1].Text),
						Memory->Entries[2].Text, sizeof(Memory->Entries[2].Text),
						Memory->Entries[3].Text, sizeof(Memory->Entries[3].Text),
						Memory->Entries[4].Text, sizeof(Memory->Entries[4].Text),
						Memory->Entries[5].Text, sizeof(Memory->Entries[5].Text),
						Memory->Entries[6].Text, sizeof(Memory->Entries[6].Text),
						Memory->Entries[7].Text, sizeof(Memory->Entries[7].Text),
						Memory->Entries[9].Text, sizeof(Memory->Entries[9].Text),
						Memory->Entries[10].Text, sizeof(Memory->Entries[10].Text),
						Memory->Entries[11].Text, sizeof(Memory->Entries[11].Text),
						&year, &month, &day,
						Memory->Entries[8].Text, sizeof(Memory->Entries[8].Text));
		}
		if (error != ERR_NONE) {
			return error;
		}
		/* Remove empty entries */
		for (i = 0; i < Memory->EntriesNum; i++) {
			if (UnicodeLength(Memory->Entries[i].Text) == 0) {
				for (j = i + 1; j < Memory->EntriesNum; j++) {
					CopyUnicodeString(Memory->Entries[j - 1].Text, Memory->Entries[j].Text);
					Memory->Entries[j - 1].EntryType = Memory->Entries[j].EntryType;
					Memory->Entries[j - 1].Location = Memory->Entries[j].Location;
				}
				Memory->EntriesNum--;
			}
		}
		/* Was there stored birthday? */
		if (year > 1900) {
			Memory->Entries[Memory->EntriesNum].EntryType = PBK_Date;
			Memory->Entries[Memory->EntriesNum].Location = PBK_Location_Unknown;
			Memory->Entries[Memory->EntriesNum].Date.Year = year;
			Memory->Entries[Memory->EntriesNum].Date.Month = month;
			Memory->Entries[Memory->EntriesNum].Date.Day = day;
			Memory->Entries[Memory->EntriesNum].Date.Hour = 0;
			Memory->Entries[Memory->EntriesNum].Date.Minute = 0;
			Memory->Entries[Memory->EntriesNum].Date.Second = 0;
			Memory->Entries[Memory->EntriesNum].Date.Timezone = 0;
			Memory->EntriesNum++;
		}
		if (Memory->EntriesNum == 0) {
			return ERR_EMPTY;
		}
		return ERR_NONE;
	case AT_Reply_Error:
                return ERR_UNKNOWN;
	case AT_Reply_CMSError:
 	        return ATGEN_HandleCMSError(s);
	case AT_Reply_CMEError:
		/* Empty location */
		if (Priv->ErrorCode == 28) {
			return ERR_EMPTY;
		}
	        return ATGEN_HandleCMEError(s);
	default:
		break;
	}
	return ERR_UNKNOWNRESPONSE;
}

GSM_Error SAMSUNG_SetMemory(GSM_StateMachine *s, GSM_MemoryEntry *entry)
{
 	GSM_Phone_ATGENData *Priv = &s->Phone.Data.Priv.ATGEN;

	SAMSUNG_CheckCalendar(s);

	if (Priv->SamsungCalendar == SAMSUNG_NONE) {
		return ERR_NOTSUPPORTED;
	} else if (Priv->SamsungCalendar == SAMSUNG_SSH) {
		return ERR_NOTIMPLEMENTED;
	}

	/* FIXME: Here you have to implement conversion of GSM_MemoryEntry to AT command */
	smprintf(s, "Setting memory for Samsung not implemented yet!\n");
	return ERR_NOTIMPLEMENTED;
}

GSM_Error SAMSUNG_ORG_ReplyGetCalendarStatus(GSM_Protocol_Message *msg, GSM_StateMachine *s)
{
	GSM_Phone_ATGENData	*Priv = &s->Phone.Data.Priv.ATGEN;
	GSM_Error error;
	int ignore;

	if (Priv->ReplyState != AT_Reply_OK) {
		switch (s->Phone.Data.Priv.ATGEN.ReplyState) {
		case AT_Reply_Error:
			return ERR_NOTSUPPORTED;
		case AT_Reply_CMSError:
			return ATGEN_HandleCMSError(s);
		case AT_Reply_CMEError:
			return ATGEN_HandleCMEError(s);
		default:
			return ERR_UNKNOWNRESPONSE;
		}
	}

	if (strcmp(GetLineString(msg->Buffer, &Priv->Lines, 2), "OK") == 0) {
		return ERR_NOTSUPPORTED;
	}

	error = ATGEN_ParseReply(s,
		GetLineString(msg->Buffer, &Priv->Lines, 2),
		"+ORGI: @i, @i, @i, @i, @i",
		&s->Phone.Data.CalStatus->Used,
		&s->Phone.Data.CalStatus->Free,
		&ignore,
		&ignore,
		&ignore);
	if (error != ERR_NONE) return error;
	s->Phone.Data.CalStatus->Free -= s->Phone.Data.CalStatus->Used;
	return ERR_NONE;
}

GSM_Error SAMSUNG_SSH_ReplyGetCalendarStatus(GSM_Protocol_Message *msg, GSM_StateMachine *s)
{
	GSM_Phone_ATGENData	*Priv = &s->Phone.Data.Priv.ATGEN;
	GSM_Error error;
	int ignore;

	if (Priv->ReplyState != AT_Reply_OK) {
		switch (s->Phone.Data.Priv.ATGEN.ReplyState) {
		case AT_Reply_Error:
			return ERR_NOTSUPPORTED;
		case AT_Reply_CMSError:
			return ATGEN_HandleCMSError(s);
		case AT_Reply_CMEError:
			return ATGEN_HandleCMEError(s);
		default:
			return ERR_UNKNOWNRESPONSE;
		}
	}

	error = ATGEN_ParseReply(s,
		GetLineString(msg->Buffer, &Priv->Lines, 2),
		"+SSHI: @i, @i, @i",
		&s->Phone.Data.CalStatus->Used,
		&s->Phone.Data.CalStatus->Free,
		&ignore);
	if (error != ERR_NONE) return error;
	s->Phone.Data.CalStatus->Free -= s->Phone.Data.CalStatus->Used;
	return ERR_NONE;
}

GSM_Error SAMSUNG_GetCalendarStatus(GSM_StateMachine *s, GSM_CalendarStatus *Status)
{
	GSM_Error error;
 	GSM_Phone_ATGENData *Priv = &s->Phone.Data.Priv.ATGEN;

	s->Phone.Data.CalStatus = Status;

	SAMSUNG_CheckCalendar(s);

	if (Priv->SamsungCalendar == SAMSUNG_NONE) {
		return ERR_NOTSUPPORTED;
	} else if (Priv->SamsungCalendar == SAMSUNG_SSH) {
		error = ATGEN_WaitForAutoLen(s, "AT+SSHI?\r", 0x00, 10, ID_GetCalendarNotesInfo);
		return error;
	} else if (Priv->SamsungCalendar == SAMSUNG_ORG) {
		error = ATGEN_WaitForAutoLen(s, "AT+ORGI?\r", 0x00, 10, ID_GetCalendarNotesInfo);
		return error;
	}

	return ERR_BUG;
}

GSM_Error SAMSUNG_ParseAppointment(GSM_StateMachine *s, const char *line)
{
	GSM_Error error;
	int ignore, alarm_flag, alarm_time, alarm_quantity, alarm_repeat;
	char ignorestring[10];
	GSM_CalendarEntry *Note = s->Phone.Data.Cal;
	/*
par03: Organizer entry short name
par04: Organizer entry detailed description
par05: Start day
par06: Start month
par07: Start year
par08: Start hour
par09: Start minute
par10: End day
par11: End month
par12: End year
par13: End hour
par14: End minute
par15: Location
par16: Alarm flag (0=no, 1=yes)
par17: Alarm time unit (1=minutes, 2=hours, days, 4=weeks)
par18: Alarm items quantity
par19: Alarm repeat flag (0 or empty=no, 2=yes)
par20: Empty
par21: Empty
par22: Repeat until day
par23: Repeat until month
par24: Repeat until year
*/
	Note->Entries[0].EntryType = CAL_TEXT;
	Note->Entries[1].EntryType = CAL_DESCRIPTION;
	Note->Entries[2].EntryType = CAL_START_DATETIME;
	Note->Entries[2].Date.Timezone = 0;
	Note->Entries[2].Date.Second = 0;
	Note->Entries[3].EntryType = CAL_END_DATETIME;
	Note->Entries[3].Date.Timezone = 0;
	Note->Entries[3].Date.Second = 0;
	Note->Entries[4].EntryType = CAL_LOCATION;
	Note->EntriesNum = 4;
	error = ATGEN_ParseReply(s,
		line,
		"+ORGR: @i, @i, @S, @S, @i, @i, @i, @i, @i, @i, @i, @i, @i, @i, @s, @I, @I, @I, @I, @s, @s, @I, @I, @I",
		&ignore,
		&ignore,
		Note->Entries[0].Text, sizeof(Note->Entries[0].Text),
		Note->Entries[1].Text, sizeof(Note->Entries[1].Text),
		&(Note->Entries[2].Date.Day),
		&(Note->Entries[2].Date.Month),
		&(Note->Entries[2].Date.Year),
		&(Note->Entries[2].Date.Hour),
		&(Note->Entries[2].Date.Minute),
		&(Note->Entries[3].Date.Day),
		&(Note->Entries[3].Date.Month),
		&(Note->Entries[3].Date.Year),
		&(Note->Entries[3].Date.Hour),
		&(Note->Entries[3].Date.Minute),
		Note->Entries[4].Text, sizeof(Note->Entries[4].Text),
		&alarm_flag,
		&alarm_time,
		&alarm_quantity,
		&alarm_repeat,
		ignorestring, sizeof(ignorestring),
		ignorestring, sizeof(ignorestring),
		&(Note->Entries[5].Date.Day),
		&(Note->Entries[5].Date.Month),
		&(Note->Entries[5].Date.Year)
		);
	if (error != ERR_NONE) return error;
	return ERR_NONE;
}

GSM_Error SAMSUNG_ParseAniversary(GSM_StateMachine *s, const char *line)
{
	GSM_Error error;
	int ignore, alarm_flag, alarm_time, alarm_quantity, alarm_repeat;
	char ignorestring[10];
	GSM_CalendarEntry *Note = s->Phone.Data.Cal;
	/*
par03: Empty
par04: Ocassion name
par05: Alarm day
par06: Alarm month
par07: Alarm year
par08: Alarm hour
par09: Alarm minutes
par10: Empty
par11: Empty
par12: Empty
par13: Empty
par14: Empty
par15: Empty
par16: Alarm flag (0=no, 1=yes)
par17: Alarm time unit (1=minutes, 2=hours, days, 4=weeks)
par18: Alarm items quantity
par19: Repeat each year (0=no, 4=yes)
par20: Empty
par21: Empty
par22: Empty
par23: Empty
par24: Empty
*/
	Note->Entries[0].EntryType = CAL_TEXT;
	Note->Entries[1].EntryType = CAL_TONE_ALARM_DATETIME;
	Note->Entries[1].Date.Timezone = 0;
	Note->Entries[1].Date.Second = 0;
	Note->EntriesNum = 2;
	error = ATGEN_ParseReply(s,
		line,
		"+ORGR: @i, @i, @S, @S, @i, @i, @i, @i, @i, @s, @s, @s, @s, @s, @s, @i, @i, @i, @i, @0",
		&ignore,
		&ignore,
		ignorestring, sizeof(ignorestring),
		Note->Entries[0].Text, sizeof(Note->Entries[0].Text),
		&(Note->Entries[1].Date.Day),
		&(Note->Entries[1].Date.Month),
		&(Note->Entries[1].Date.Year),
		&(Note->Entries[1].Date.Hour),
		&(Note->Entries[1].Date.Minute),
		ignorestring, sizeof(ignorestring),
		ignorestring, sizeof(ignorestring),
		ignorestring, sizeof(ignorestring),
		ignorestring, sizeof(ignorestring),
		ignorestring, sizeof(ignorestring),
		ignorestring, sizeof(ignorestring),
		&alarm_flag,
		&alarm_time,
		&alarm_quantity,
		&alarm_repeat
		);
	if (error != ERR_NONE) return error;
	return ERR_NONE;
}

GSM_Error SAMSUNG_ParseTask(GSM_StateMachine *s, const char *line)
{
	GSM_Error error;
	int ignore, alarm_flag, alarm_time, alarm_quantity, priority, status;
	char ignorestring[10];
	GSM_CalendarEntry *Note = s->Phone.Data.Cal;
	/*
par03: Empty
par04: Task name
par05: Start day
par06: Start month
par07: Start year
par08: Alarm hour
par09: Alarm minute
par10: Due day
par11: Due month
par12: Due year
par13: Empty
par14: Empty
par15: Empty
par16: Alarm flag (0=no, 1=yes)
par17: Alarm time unit (1=minutes, 2=hours, days, 4=weeks)
par18: Alarm items quantity
par19: Empty
par20: Task priority (1=high, 2=normal, 3=low)
par21: Task status (0=undone, 1=done)
par22: Empty
par23: Empty
par24: Empty
*/
	Note->Entries[0].EntryType = CAL_TEXT;
	Note->Entries[1].EntryType = CAL_TONE_ALARM_DATETIME;
	Note->Entries[1].Date.Timezone = 0;
	Note->Entries[1].Date.Second = 0;
	Note->Entries[2].EntryType = CAL_END_DATETIME;
	Note->Entries[2].Date.Timezone = 0;
	Note->Entries[2].Date.Second = 0;
	Note->Entries[2].Date.Hour = 0;
	Note->Entries[2].Date.Minute = 0;
	Note->EntriesNum = 3;
	error = ATGEN_ParseReply(s,
		line,
		"+ORGR: @i, @i, @S, @S, @i, @i, @i, @i, @i, @i, @i, @i, @s, @s, @s, @i, @i, @i, @s, @i, @i, @0",
		&ignore,
		&ignore,
		ignorestring, sizeof(ignorestring),
		Note->Entries[0].Text, sizeof(Note->Entries[0].Text),
		&(Note->Entries[1].Date.Day),
		&(Note->Entries[1].Date.Month),
		&(Note->Entries[1].Date.Year),
		&(Note->Entries[1].Date.Hour),
		&(Note->Entries[1].Date.Minute),
		&(Note->Entries[2].Date.Day),
		&(Note->Entries[2].Date.Month),
		&(Note->Entries[2].Date.Year),
		ignorestring, sizeof(ignorestring),
		ignorestring, sizeof(ignorestring),
		ignorestring, sizeof(ignorestring),
		&alarm_flag,
		&alarm_time,
		&alarm_quantity,
		ignorestring, sizeof(ignorestring),
		&priority,
		&status
		);
	if (error != ERR_NONE) return error;
	return ERR_NONE;
}

GSM_Error SAMSUNG_ORG_ReplyGetCalendar(GSM_Protocol_Message *msg, GSM_StateMachine *s)
{
	GSM_Phone_ATGENData	*Priv = &s->Phone.Data.Priv.ATGEN;
	GSM_Error error;
	int ignore, type;
	const char *line;

	if (Priv->ReplyState != AT_Reply_OK) {
		switch (s->Phone.Data.Priv.ATGEN.ReplyState) {
		case AT_Reply_Error:
			return ERR_NOTSUPPORTED;
		case AT_Reply_CMSError:
			return ATGEN_HandleCMSError(s);
		case AT_Reply_CMEError:
			return ATGEN_HandleCMEError(s);
		default:
			return ERR_UNKNOWNRESPONSE;
		}
	}

	line = GetLineString(msg->Buffer, &Priv->Lines, 2);

	if (strcmp("OK", line) == 0) {
		return ERR_EMPTY;
	}

	error = ATGEN_ParseReply(s,
		line,
		"+ORGR: @i, @i, @0",
		&ignore,
		&type);
	if (error != ERR_NONE) return error;

	switch (type) {
		case 1:
			s->Phone.Data.Cal->Type = GSM_CAL_MEETING;
			return SAMSUNG_ParseAppointment(s, line);
		case 2:
			s->Phone.Data.Cal->Type = GSM_CAL_BIRTHDAY;
			return SAMSUNG_ParseAniversary(s, line);
		case 3:
			/* TODO: This should be rather turned into todo entry */
			s->Phone.Data.Cal->Type = GSM_CAL_REMINDER;
			return SAMSUNG_ParseTask(s, line);
		case 4:
			s->Phone.Data.Cal->Type = GSM_CAL_MEMO;
			return SAMSUNG_ParseAppointment(s, line);
		default:
			smprintf(s, "WARNING: Unknown entry type %d, treating as memo!\n", type);
			s->Phone.Data.Cal->Type = GSM_CAL_MEMO;
			return SAMSUNG_ParseAppointment(s, line);
	}
}

GSM_Error SAMSUNG_SSH_ReplyGetCalendar(GSM_Protocol_Message *msg, GSM_StateMachine *s)
{
	return ERR_NOTIMPLEMENTED;
}

GSM_Error SAMSUNG_GetNextCalendar(GSM_StateMachine *s, GSM_CalendarEntry *Note, gboolean start)
{
	GSM_Error error;
 	GSM_Phone_ATGENData *Priv = &s->Phone.Data.Priv.ATGEN;

	SAMSUNG_CheckCalendar(s);

	if (Priv->SamsungCalendar == SAMSUNG_NONE) {
		return ERR_NOTSUPPORTED;
	}

	if (start) {
		/* One below actual first position */
		Note->Location = 0;
		error = SAMSUNG_GetCalendarStatus(s, &Priv->CalendarStatus);
		if (error != ERR_NONE) {
			return error;
		}
		Priv->CalendarRead = 0;
	}
	s->Phone.Data.Cal 	= Note;
	Note->EntriesNum 	= 0;
	smprintf(s, "Getting calendar entry\n");
	error = ERR_EMPTY;
	while (error == ERR_EMPTY) {
		Note->Location++;
		if (Note->Location >= Priv->CalendarStatus.Used + Priv->CalendarStatus.Free) {
			/* We're at the end */
			return ERR_EMPTY;
		}
		if (Priv->CalendarRead >= Priv->CalendarStatus.Used) {
			/* We've read all entries */
			return ERR_EMPTY;
		}
		error = SAMSUNG_GetCalendar(s, Note);
		if (error == ERR_NONE) {
			Priv->CalendarRead++;
		}
	}
	return error;
}

GSM_Error SAMSUNG_GetCalendar(GSM_StateMachine *s, GSM_CalendarEntry *Note)
{
	char req[50];
	GSM_Error error;
	size_t len;
 	GSM_Phone_ATGENData *Priv = &s->Phone.Data.Priv.ATGEN;

	s->Phone.Data.Cal = Note;

	SAMSUNG_CheckCalendar(s);

	if (Priv->SamsungCalendar == SAMSUNG_NONE) {
		return ERR_NOTSUPPORTED;
	} else if (Priv->SamsungCalendar == SAMSUNG_ORG) {
		len = sprintf(req, "AT+ORGR=%d\r", Note->Location - 1);
	} else if (Priv->SamsungCalendar == SAMSUNG_SSH) {
		len = sprintf(req, "AT+SSHR=%d\r", Note->Location);
	} else {
		return ERR_BUG;
	}

	error = ATGEN_WaitFor(s, req, len, 0x00, 10, ID_GetCalendarNote);
	return error;
}

GSM_Error SAMSUNG_ORG_ReplySetCalendar(GSM_Protocol_Message *msg, GSM_StateMachine *s)
{
	return ERR_NOTIMPLEMENTED;
}

GSM_Error SAMSUNG_DelCalendar(GSM_StateMachine *s, GSM_CalendarEntry *Note)
{
	char req[50];
	GSM_Error error;
	size_t len;
 	GSM_Phone_ATGENData *Priv = &s->Phone.Data.Priv.ATGEN;

	SAMSUNG_CheckCalendar(s);

	if (Priv->SamsungCalendar == SAMSUNG_NONE) {
		return ERR_NOTSUPPORTED;
	} else if (Priv->SamsungCalendar == SAMSUNG_ORG) {
		len = sprintf(req, "AT+ORGD=%d\r", Note->Location - 1);
	} else if (Priv->SamsungCalendar == SAMSUNG_SSH) {
		len = sprintf(req, "AT+SSHD=%d\r", Note->Location);
	} else {
		return ERR_BUG;
	}

	error = ATGEN_WaitFor(s, req, len, 0x00, 10, ID_DeleteCalendarNote);
	return error;
}

GSM_Error SAMSUNG_SetCalendar(GSM_StateMachine *s, GSM_CalendarEntry *Note)
{
 	GSM_Phone_ATGENData *Priv = &s->Phone.Data.Priv.ATGEN;

	SAMSUNG_CheckCalendar(s);

	if (Priv->SamsungCalendar == SAMSUNG_NONE) {
		return ERR_NOTSUPPORTED;
	} else if (Priv->SamsungCalendar == SAMSUNG_ORG) {
		return ERR_NOTIMPLEMENTED;
	} else {
		return ERR_NOTIMPLEMENTED;
	}
}

GSM_Error SAMSUNG_AddCalendar(GSM_StateMachine *s, GSM_CalendarEntry *Note)
{
 	GSM_Phone_ATGENData *Priv = &s->Phone.Data.Priv.ATGEN;

	SAMSUNG_CheckCalendar(s);

	if (Priv->SamsungCalendar == SAMSUNG_NONE) {
		return ERR_NOTSUPPORTED;
	} else if (Priv->SamsungCalendar == SAMSUNG_ORG) {
		return ERR_NOTIMPLEMENTED;
	} else {
		return ERR_NOTIMPLEMENTED;
	}
}
#endif

/* How should editor hadle tabs in this file? Add editor commands here.
 * vim: noexpandtab sw=8 ts=8 sts=8:
 */
