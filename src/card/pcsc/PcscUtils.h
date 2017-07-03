/*!
 * PcscUtils.h
 *
 * \brief toString method for PCSC_RETURNCODE and platform dependent
 * typedefs for PCSC types.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QString>
#include <QtGlobal>

// TODO: Check these codes
#ifdef Q_OS_WIN
#include <winscard.h>
typedef LONG PCSC_RETURNCODE;
typedef SCARDHANDLE PCSC_CARDHANDLE;
typedef DWORD PCSC_UINT;
typedef DWORD PCSC_INT;
typedef DWORD* PCSC_INT_PTR;
#ifdef UNICODE
typedef WCHAR PCSC_CHAR;
#else
typedef CHAR PCSC_CHAR;
#endif
typedef LPTSTR PCSC_CHAR_PTR;
typedef BYTE PCSC_UCHAR;
typedef LPBYTE PCSC_UCHAR_PTR;
typedef LPCBYTE PCSC_CUCHAR_PTR;
#elif defined Q_OS_OSX
#include <winscard.h>
// The return type is actually int32_t (in the system headers), but the return
// code macros are defined without a cast operator, which makes them
// unsigned int due to a missing cast.
typedef int64_t PCSC_RETURNCODE;
typedef SCARDHANDLE PCSC_CARDHANDLE;
typedef uint32_t PCSC_INT;
typedef PCSC_INT* PCSC_INT_PTR;
typedef char PCSC_CHAR;
typedef char* PCSC_CHAR_PTR;
typedef uchar PCSC_UCHAR;
typedef uchar* PCSC_UCHAR_PTR;
typedef const uchar* PCSC_CUCHAR_PTR;
#elif defined Q_OS_UNIX
#include <winscard.h>
typedef LONG PCSC_RETURNCODE;
typedef SCARDHANDLE PCSC_CARDHANDLE;
typedef DWORD PCSC_INT;
typedef PCSC_INT* PCSC_INT_PTR;
typedef char PCSC_CHAR;
typedef char* PCSC_CHAR_PTR;
typedef uchar PCSC_UCHAR;
typedef uchar* PCSC_UCHAR_PTR;
typedef const uchar* PCSC_CUCHAR_PTR;
#endif

namespace governikus
{

class PcscUtils
{
	private:
		PcscUtils() = delete;
		~PcscUtils() = delete;

	public:
		static QString toString(PCSC_RETURNCODE pCode);

		/**
		 * Error codes from http://msdn.microsoft.com/en-us/library/aa924526.aspx
		 */
		enum PcscReturnCode : PCSC_RETURNCODE
		{
			Scard_S_Success = static_cast<PCSC_RETURNCODE>(SCARD_S_SUCCESS), /**< No error was encountered. */
			Scard_F_Internal_Error = static_cast<PCSC_RETURNCODE>(SCARD_F_INTERNAL_ERROR), /**< An internal consistency check failed. */
			Scard_E_Cancelled = static_cast<PCSC_RETURNCODE>(SCARD_E_CANCELLED), /**< The action was cancelled by an SCardCancel request. */
			Scard_E_Invalid_Handle = static_cast<PCSC_RETURNCODE>(SCARD_E_INVALID_HANDLE), /**< The supplied handle was invalid. */
			Scard_E_Invalid_Parameter = static_cast<PCSC_RETURNCODE>(SCARD_E_INVALID_PARAMETER), /**< One or more of the supplied parameters could not be properly interpreted. */
			Scard_E_Invalid_Target = static_cast<PCSC_RETURNCODE>(SCARD_E_INVALID_TARGET), /**< Registry startup information is missing or invalid. */
			Scard_E_No_Memory = static_cast<PCSC_RETURNCODE>(SCARD_E_NO_MEMORY), /**< Not enough memory available to complete this command. */
			Scard_F_Waited_Too_Long = static_cast<PCSC_RETURNCODE>(SCARD_F_WAITED_TOO_LONG), /**< An internal consistency timer has expired. */
			Scard_E_Insufficient_Buffer = static_cast<PCSC_RETURNCODE>(SCARD_E_INSUFFICIENT_BUFFER), /**< The data buffer to receive returned data is too small for the returned data. */
			Scard_E_Unknown_Reader = static_cast<PCSC_RETURNCODE>(SCARD_E_UNKNOWN_READER), /**< The specified reader name is not recognized. */
			Scard_E_Timeout = static_cast<PCSC_RETURNCODE>(SCARD_E_TIMEOUT), /**< The user-specified timeout value has expired. */
			Scard_E_Sharing_Violation = static_cast<PCSC_RETURNCODE>(SCARD_E_SHARING_VIOLATION), /**< The smart card cannot be accessed because of other connections outstanding. */
			Scard_E_No_Smartcard = static_cast<PCSC_RETURNCODE>(SCARD_E_NO_SMARTCARD), /**< The operation requires a Smart Card, but no Smart Card is currently in the device. */
			Scard_E_Unknown_Card = static_cast<PCSC_RETURNCODE>(SCARD_E_UNKNOWN_CARD), /**< The specified smart card name is not recognized. */
			Scard_E_Cant_Dispose = static_cast<PCSC_RETURNCODE>(SCARD_E_CANT_DISPOSE), /**< The system could not dispose of the media in the requested manner. */
			Scard_E_Proto_Mismatch = static_cast<PCSC_RETURNCODE>(SCARD_E_PROTO_MISMATCH), /**< The requested protocols are incompatible with the protocol currently in use with the smart card. */
			Scard_E_Not_Ready = static_cast<PCSC_RETURNCODE>(SCARD_E_NOT_READY), /**< The reader or smart card is not ready to accept commands. */
			Scard_E_Invalid_Value = static_cast<PCSC_RETURNCODE>(SCARD_E_INVALID_VALUE), /**< One or more of the supplied parameters values could not be properly interpreted. */
			Scard_E_System_Cancelled = static_cast<PCSC_RETURNCODE>(SCARD_E_SYSTEM_CANCELLED), /**< The action was cancelled by the system, presumably to log off or shut down. */
			Scard_F_Comm_Error = static_cast<PCSC_RETURNCODE>(SCARD_F_COMM_ERROR), /**< An internal communications error has been detected. */
			Scard_F_Unknown_Error = static_cast<PCSC_RETURNCODE>(SCARD_F_UNKNOWN_ERROR), /**< An internal error has been detected, but the source is unknown. */
			Scard_E_Invalid_Atr = static_cast<PCSC_RETURNCODE>(SCARD_E_INVALID_ATR), /**< An ATR obtained from the registry is not a valid ATR string. */
			Scard_E_Not_Transacted = static_cast<PCSC_RETURNCODE>(SCARD_E_NOT_TRANSACTED), /**< An attempt was made to end a non-existent transaction. */
			Scard_E_Reader_Unavailable = static_cast<PCSC_RETURNCODE>(SCARD_E_READER_UNAVAILABLE), /**< The specified reader is not currently available for use. */
			Scard_P_Shutdown = static_cast<PCSC_RETURNCODE>(SCARD_P_SHUTDOWN), /**< The operation has been aborted to allow the server application to exit. */
			Scard_E_Pci_Too_Small = static_cast<PCSC_RETURNCODE>(SCARD_E_PCI_TOO_SMALL), /**< The PCI Receive buffer was too small. */
			Scard_E_Reader_Unsupported = static_cast<PCSC_RETURNCODE>(SCARD_E_READER_UNSUPPORTED), /**< The reader driver does not meet minimal requirements for support. */
			Scard_E_Duplicate_Reader = static_cast<PCSC_RETURNCODE>(SCARD_E_DUPLICATE_READER), /**< The reader driver did not produce a unique reader name. */
			Scard_E_Card_Unsupported = static_cast<PCSC_RETURNCODE>(SCARD_E_CARD_UNSUPPORTED), /**< The smart card does not meet minimal requirements for support. */
			Scard_E_No_Service = static_cast<PCSC_RETURNCODE>(SCARD_E_NO_SERVICE), /**< The Smart card resource manager is not running. */
			Scard_E_Service_Stopped = static_cast<PCSC_RETURNCODE>(SCARD_E_SERVICE_STOPPED), /**< The Smart card resource manager has shut down. */
			Scard_E_Unexpected = static_cast<PCSC_RETURNCODE>(SCARD_E_UNEXPECTED), /**< An unexpected card error has occurred. */
			Scard_E_Unsupported_Feature = static_cast<PCSC_RETURNCODE>(SCARD_E_UNSUPPORTED_FEATURE), /**< This smart card does not support the requested feature. */
			Scard_E_Icc_Installation = static_cast<PCSC_RETURNCODE>(SCARD_E_ICC_INSTALLATION), /**< No primary provider can be found for the smart card. */
			Scard_E_Icc_Createorder = static_cast<PCSC_RETURNCODE>(SCARD_E_ICC_CREATEORDER), /**< The requested order of object creation is not supported. */

			Scard_E_Dir_Not_Found = static_cast<PCSC_RETURNCODE>(SCARD_E_DIR_NOT_FOUND), /**< The identified directory does not exist in the smart card. */
			Scard_E_File_Not_Found = static_cast<PCSC_RETURNCODE>(SCARD_E_FILE_NOT_FOUND), /**< The identified file does not exist in the smart card. */
			Scard_E_No_Dir = static_cast<PCSC_RETURNCODE>(SCARD_E_NO_DIR), /**< The supplied path does not represent a smart card directory. */
			Scard_E_No_File = static_cast<PCSC_RETURNCODE>(SCARD_E_NO_FILE), /**< The supplied path does not represent a smart card file. */
			Scard_E_No_Access = static_cast<PCSC_RETURNCODE>(SCARD_E_NO_ACCESS), /**< Access is denied to this file. */
			Scard_E_Write_Too_Many = static_cast<PCSC_RETURNCODE>(SCARD_E_WRITE_TOO_MANY), /**< The smart card does not have enough memory to store the information. */
			Scard_E_Bad_Seek = static_cast<PCSC_RETURNCODE>(SCARD_E_BAD_SEEK), /**< There was an error trying to set the smart card file object pointer. */
			Scard_E_Invalid_Chv = static_cast<PCSC_RETURNCODE>(SCARD_E_INVALID_CHV), /**< The supplied PIN is incorrect. */
			Scard_E_Unknown_Res_Mng = static_cast<PCSC_RETURNCODE>(SCARD_E_UNKNOWN_RES_MNG), /**< An unrecognized error code was returned from a layered component. */
			Scard_E_No_Such_Certificate = static_cast<PCSC_RETURNCODE>(SCARD_E_NO_SUCH_CERTIFICATE), /**< The requested certificate does not exist. */
			Scard_E_Certificate_Unavailable = static_cast<PCSC_RETURNCODE>(SCARD_E_CERTIFICATE_UNAVAILABLE), /**< The requested certificate could not be obtained. */
			Scard_E_No_Readers_Available = static_cast<PCSC_RETURNCODE>(SCARD_E_NO_READERS_AVAILABLE), /**< Cannot find a smart card reader. */
			Scard_E_Comm_Data_Lost = static_cast<PCSC_RETURNCODE>(SCARD_E_COMM_DATA_LOST), /**< A communications error with the smart card has been detected. Retry the operation. */
			Scard_E_No_Key_Container = static_cast<PCSC_RETURNCODE>(SCARD_E_NO_KEY_CONTAINER), /**< The requested key container does not exist on the smart card. */
			Scard_E_Server_Too_Busy = static_cast<PCSC_RETURNCODE>(SCARD_E_SERVER_TOO_BUSY), /**< The Smart Card Resource Manager is too busy to complete this operation. */

			Scard_W_Unsupported_Card = static_cast<PCSC_RETURNCODE>(SCARD_W_UNSUPPORTED_CARD), /**< The reader cannot communicate with the card, due to ATR string configuration conflicts. */
			Scard_W_Unresponsive_Card = static_cast<PCSC_RETURNCODE>(SCARD_W_UNRESPONSIVE_CARD), /**< The smart card is not responding to a reset. */
			Scard_W_Unpowered_Card = static_cast<PCSC_RETURNCODE>(SCARD_W_UNPOWERED_CARD), /**< Power has been removed from the smart card, so that further communication is not possible. */
			Scard_W_Reset_Card = static_cast<PCSC_RETURNCODE>(SCARD_W_RESET_CARD), /**< The smart card has been reset, so any shared state information is invalid. */
			Scard_W_Removed_Card = static_cast<PCSC_RETURNCODE>(SCARD_W_REMOVED_CARD), /**< The smart card has been removed, so further communication is not possible. */

			Scard_W_Security_Violation = static_cast<PCSC_RETURNCODE>(SCARD_W_SECURITY_VIOLATION), /**< Access was denied because of a security violation. */
			Scard_W_Wrong_Chv = static_cast<PCSC_RETURNCODE>(SCARD_W_WRONG_CHV), /**< The card cannot be accessed because the wrong PIN was presented. */
			Scard_W_Chv_Blocked = static_cast<PCSC_RETURNCODE>(SCARD_W_CHV_BLOCKED), /**< The card cannot be accessed because the maximum number of PIN entry attempts has been reached. */
			Scard_W_Eof = static_cast<PCSC_RETURNCODE>(SCARD_W_EOF), /**< The end of the smart card file has been reached. */
			Scard_W_Cancelled_By_User = static_cast<PCSC_RETURNCODE>(SCARD_W_CANCELLED_BY_USER), /**< The user pressed "Cancel" on a Smart Card Selection Dialog. */
			Scard_W_Card_Not_Authenticated = static_cast<PCSC_RETURNCODE>(SCARD_W_CARD_NOT_AUTHENTICATED) /**< No PIN was presented to the smart card. */
		};

};


/**
 * Make sure we do not use these macros directly in our code.
 */
#undef SCARD_S_SUCCESS
#undef SCARD_F_INTERNAL_ERROR
#undef SCARD_E_CANCELLED
#undef SCARD_E_INVALID_HANDLE
#undef SCARD_E_INVALID_PARAMETER
#undef SCARD_E_INVALID_TARGET
#undef SCARD_E_NO_MEMORY
#undef SCARD_F_WAITED_TOO_LONG
#undef SCARD_E_INSUFFICIENT_BUFFER
#undef SCARD_E_UNKNOWN_READER
#undef SCARD_E_TIMEOUT
#undef SCARD_E_SHARING_VIOLATION
#undef SCARD_E_NO_SMARTCARD
#undef SCARD_E_UNKNOWN_CARD
#undef SCARD_E_CANT_DISPOSE
#undef SCARD_E_PROTO_MISMATCH
#undef SCARD_E_NOT_READY
#undef SCARD_E_INVALID_VALUE
#undef SCARD_E_SYSTEM_CANCELLED
#undef SCARD_F_COMM_ERROR
#undef SCARD_F_UNKNOWN_ERROR
#undef SCARD_E_INVALID_ATR
#undef SCARD_E_NOT_TRANSACTED
#undef SCARD_E_READER_UNAVAILABLE
#undef SCARD_P_SHUTDOWN
#undef SCARD_E_PCI_TOO_SMALL
#undef SCARD_E_READER_UNSUPPORTED
#undef SCARD_E_DUPLICATE_READER
#undef SCARD_E_CARD_UNSUPPORTED
#undef SCARD_E_NO_SERVICE
#undef SCARD_E_SERVICE_STOPPED
#undef SCARD_E_UNEXPECTED
#undef SCARD_E_UNSUPPORTED_FEATURE
#undef SCARD_E_ICC_INSTALLATION
#undef SCARD_E_ICC_CREATEORDER

#undef SCARD_E_DIR_NOT_FOUND
#undef SCARD_E_FILE_NOT_FOUND
#undef SCARD_E_NO_DIR
#undef SCARD_E_NO_FILE
#undef SCARD_E_NO_ACCESS
#undef SCARD_E_WRITE_TOO_MANY
#undef SCARD_E_BAD_SEEK
#undef SCARD_E_INVALID_CHV
#undef SCARD_E_UNKNOWN_RES_MNG
#undef SCARD_E_NO_SUCH_CERTIFICATE
#undef SCARD_E_CERTIFICATE_UNAVAILABLE
#undef SCARD_E_NO_READERS_AVAILABLE
#undef SCARD_E_COMM_DATA_LOST
#undef SCARD_E_NO_KEY_CONTAINER
#undef SCARD_E_SERVER_TOO_BUSY

#undef SCARD_W_UNSUPPORTED_CARD
#undef SCARD_W_UNRESPONSIVE_CARD
#undef SCARD_W_UNPOWERED_CARD
#undef SCARD_W_RESET_CARD
#undef SCARD_W_REMOVED_CARD

#undef SCARD_W_SECURITY_VIOLATION
#undef SCARD_W_WRONG_CHV
#undef SCARD_W_CHV_BLOCKED
#undef SCARD_W_EOF
#undef SCARD_W_CANCELLED_BY_USER
#undef SCARD_W_CARD_NOT_AUTHENTICATED


} /* namespace governikus */
