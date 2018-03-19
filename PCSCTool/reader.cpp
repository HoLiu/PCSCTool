#include "reader.h"
#include <winscard.h>
#include <atlconv.h>

#ifdef _MSC_VER
#pragma comment(lib, "winscard.lib")
#endif

SCARDCONTEXT hContext = NULL;
char *reader_list = NULL;
SCARDHANDLE				hCard;
unsigned long			dwActProtocol;
SCARD_IO_REQUEST		ioRequest;
bool is_escape = FALSE;

const char *GetScardErrMsg(int code)
{
	switch (code)
	{
		// Smartcard Reader interface errors
	case SCARD_E_CANCELLED:
		return ("The action was canceled by an SCardCancel request.");
		break;
	case SCARD_E_CANT_DISPOSE:
		return ("The system could not dispose of the media in the requested manner.");
		break;
	case SCARD_E_CARD_UNSUPPORTED:
		return ("The smart card does not meet minimal requirements for support.");
		break;
	case SCARD_E_DUPLICATE_READER:
		return ("The reader driver didn't produce a unique reader name.");
		break;
	case SCARD_E_INSUFFICIENT_BUFFER:
		return ("The data buffer for returned data is too small for the returned data.");
		break;
	case SCARD_E_INVALID_ATR:
		return ("An ATR string obtained from the registry is not a valid ATR string.");
		break;
	case SCARD_E_INVALID_HANDLE:
		return ("The supplied handle was invalid.");
		break;
	case SCARD_E_INVALID_PARAMETER:
		return ("One or more of the supplied parameters could not be properly interpreted.");
		break;
	case SCARD_E_INVALID_TARGET:
		return ("Registry startup information is missing or invalid.");
		break;
	case SCARD_E_INVALID_VALUE:
		return ("One or more of the supplied parameter values could not be properly interpreted.");
		break;
	case SCARD_E_NOT_READY:
		return ("The reader or card is not ready to accept commands.");
		break;
	case SCARD_E_NOT_TRANSACTED:
		return ("An attempt was made to end a non-existent transaction.");
		break;
	case SCARD_E_NO_MEMORY:
		return ("Not enough memory available to complete this command.");
		break;
	case SCARD_E_NO_SERVICE:
		return ("The smart card resource manager is not running.");
		break;
	case SCARD_E_NO_SMARTCARD:
		return ("The operation requires a smart card, but no smart card is currently in the device.");
		break;
	case SCARD_E_PCI_TOO_SMALL:
		return ("The PCI receive buffer was too small.");
		break;
	case SCARD_E_PROTO_MISMATCH:
		return ("The requested protocols are incompatible with the protocol currently in use with the card.");
		break;
	case SCARD_E_READER_UNAVAILABLE:
		return ("The specified reader is not currently available for use.");
		break;
	case SCARD_E_READER_UNSUPPORTED:
		return ("The reader driver does not meet minimal requirements for support.");
		break;
	case SCARD_E_SERVICE_STOPPED:
		return ("The smart card resource manager has shut down.");
		break;
	case SCARD_E_SHARING_VIOLATION:
		return ("The smart card cannot be accessed because of other outstanding connections.");
		break;
	case SCARD_E_SYSTEM_CANCELLED:
		return ("The action was canceled by the system, presumably to log off or shut down.");
		break;
	case SCARD_E_TIMEOUT:
		return ("The user-specified timeout value has expired.");
		break;
	case SCARD_E_UNKNOWN_CARD:
		return ("The specified smart card name is not recognized.");
		break;
	case SCARD_E_UNKNOWN_READER:
		return ("The specified reader name is not recognized.");
		break;
	case SCARD_F_COMM_ERROR:
		return ("An internal communications error has been detected.");
		break;
	case SCARD_F_INTERNAL_ERROR:
		return ("An internal consistency check failed.");
		break;
	case SCARD_F_UNKNOWN_ERROR:
		return ("An internal error has been detected, but the source is unknown.");
		break;
	case SCARD_F_WAITED_TOO_LONG:
		return ("An internal consistency timer has expired.");
		break;
	case SCARD_W_REMOVED_CARD:
		return ("The smart card has been removed and no further communication is possible.");
		break;
	case SCARD_W_RESET_CARD:
		return ("The smart card has been reset, so any shared state information is invalid.");
		break;
	case SCARD_W_UNPOWERED_CARD:
		return ("Power has been removed from the smart card and no further communication is possible.");
		break;
	case SCARD_W_UNRESPONSIVE_CARD:
		return ("The smart card is not responding to a reset.");
		break;
	case SCARD_W_UNSUPPORTED_CARD:
		return ("The reader cannot communicate with the card due to ATR string configuration conflicts.");
		break;
	}
	return ("Error is not documented.");
}

int RD_find(char *name, char **list)
{
	int ret = 0;
	DWORD dwReaders;
	int i;
	int count = 0;
	LPTSTR lpReaderNames = NULL;

	if (hContext){
		SCardReleaseContext(hContext);
	}
	ret = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hContext);
	if (ret != SCARD_S_SUCCESS)
	{
		LOG(ERROR) << "SCardEstablishContext failed : " << GetScardErrMsg(ret);
		goto err;
	}

	ret = SCardListReaders(hContext, NULL, NULL, &dwReaders);
	if (ret == SCARD_E_NO_READERS_AVAILABLE){
		LOG(INFO) << "SCardListReaders cannot find a smart card reader.";
		ret = 0;
		goto err;
	}
	else if (ret != SCARD_S_SUCCESS){
		LOG(ERROR) << "SCardListReaders failed : " << GetScardErrMsg(ret);
		goto err;
	}
	if (reader_list) {
		free(reader_list);
	}
	reader_list = (char *)malloc(dwReaders + 1);
	if (reader_list == NULL){
		ret = SCARD_E_NO_MEMORY;
		goto err;
	}
	memset(reader_list, 0, dwReaders);

	lpReaderNames = (char_t *)malloc(dwReaders * sizeof(char_t));
	if (lpReaderNames == NULL){
		ret = SCARD_E_NO_MEMORY;
		goto err;
	}

	ret = SCardListReaders(hContext, NULL, lpReaderNames, &dwReaders);
	if (ret == SCARD_E_NO_READERS_AVAILABLE) {
		LOG(INFO) << "SCardListReaders cannot find a smart card reader.";
		ret = 0;
		goto err;
	}
	else if (ret != SCARD_S_SUCCESS){
		LOG(ERROR) << "SCardListReaders failed : " << GetScardErrMsg(ret);
		goto err;
	}

	char_t *lpRdName = lpReaderNames;
	char * read_name = reader_list;
	char *p;
	USES_CONVERSION;
	for (i = 0; i < 128; i++){
		if (*lpRdName == 0)
			break;
		p = T2A(lpRdName);
		if (name == NULL || strstr(p, name)) {
			strcpy(read_name, p);
			read_name += strlen(read_name) + 1;
			++count;
		}
		lpRdName += strlen_t(lpRdName) + 1;
	}

	free((void *)lpReaderNames);
	lpReaderNames = NULL;
	
	*list = reader_list;
	return count;

err:
	if (reader_list) {
		free(reader_list);
		reader_list = NULL;
	}
	if (lpReaderNames) {
		free(lpReaderNames);
		lpReaderNames = NULL;
	}
	return ret;
}

int RD_open(char *name, int mode, int protocol, unsigned char *atr, unsigned long *atr_len)
{
	int ret;
	char_t rName[128];
	DWORD length = 128, dwState;

	if (mode == SCARD_SHARE_DIRECT && protocol == SCARD_PROTOCOL_UNDEFINED)
		is_escape = TRUE;
	else
		is_escape = FALSE;

	USES_CONVERSION;
	ret = SCardConnect(hContext, A2T(name), mode, protocol, &hCard, &dwActProtocol);
	if (ret != SCARD_S_SUCCESS)
	{
		LOG(ERROR) << "SCardConnect failed : " << GetScardErrMsg(ret);
		goto err;
	}
	
	*atr_len = 32;
	ret = SCardStatus(hCard, rName, &length, &dwState, &dwActProtocol, atr, atr_len);
	if (ret != SCARD_S_SUCCESS)
	{
		LOG(ERROR) << "SCardStatus failed : " << GetScardErrMsg(ret);
		goto err;
	}

err:
	return ret;
}

int RD_transceiver(int mode, unsigned char *tx, unsigned int tx_len, unsigned char *rx, unsigned long *rx_len)
{
	int ret;

	if (is_escape) {
		ret = SCardControl(hCard, SCARD_CTL_CODE(3500), tx, tx_len, rx, *rx_len, rx_len);
		if (ret != SCARD_S_SUCCESS)
		{
			*rx_len = 0;
			LOG(ERROR) << "SCardControl failed : " << GetScardErrMsg(ret);
			goto err;
		}
	}
	else {
		ioRequest.dwProtocol = dwActProtocol;
		ioRequest.cbPciLength = sizeof(SCARD_IO_REQUEST);
		ret = SCardTransmit(hCard, &ioRequest, tx, tx_len, NULL, rx, rx_len);
		if (ret != SCARD_S_SUCCESS)
		{
			*rx_len = 0;
			LOG(ERROR) << "SCardTransmit failed : " << GetScardErrMsg(ret);
			goto err;
		}
	}

err:
	return ret;
}

int RD_close(int rd)
{
	int ret;
	DWORD dwDisposition = (is_escape ? SCARD_LEAVE_CARD : SCARD_UNPOWER_CARD);
	ret = SCardDisconnect(hCard, dwDisposition);
	if (ret != SCARD_S_SUCCESS)
	{
		LOG(ERROR) << "SCardDisconnect failed : " << GetScardErrMsg(ret);
		return ret;
	}
	return ret;
}