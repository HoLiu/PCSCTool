#ifndef __READER_H__
#define __READER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "easylogging++.h"

#ifdef UNICODE
typedef wchar_t char_t;
#define strlen_t wcslen
#define sprintf_t wsprintf
#define strcpy_t wcscpy
#define strcmp_t wcscmp

#ifndef _T 
#   define _T(x) L##x
#endif

#else
typedef char char_t;
#define strlen_t strlen
#define sprintf_t sprintf
#define strcpy_t strcpy
#define strcmp_t strcmp

#ifndef _T 
#   define _T(x) x
#endif

#endif

const char *GetScardErrMsg(int code);

int RD_find(char *name, char **list);
int RD_open(char *name, int mode, int protocol, unsigned char *atr, unsigned long *atr_len);
int RD_transceiver(int rd, unsigned char *tx, unsigned int tx_len, unsigned char *rx, unsigned long *rx_len);
int RD_close(int rd);


#endif __READER_H__