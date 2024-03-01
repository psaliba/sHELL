#include <windows.h>

#include "../include/sHELL.h"

const char Name[] = "b64";
const char Help[] =
    "Base64 encode or decode input.\r\n"
    "Usage:\n"
    "    b64 encode <input> - Encodes the input string\r\n"
    "    b64 decode <input> - Decodes the base64 encoded string\r\n";

InternalAPI *core = NULL;

LPVOID lpOut = NULL;

// Initialization code
__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore) { core = lpCore; }

// Exported function - Name
__declspec(dllexport) const char *CommandNameA() { return Name; }

// Exported function - Help
__declspec(dllexport) const char *CommandHelpA() { return Help; }

// Clenaup
__declspec(dllexport) VOID CommandCleanup()
{
  if (lpOut)
  {
    core->free(lpOut);
    lpOut = NULL;
  }
}

// Utility function to encode a string to base64
BOOL Base64Encode(const char *input, char **output, DWORD *outputSize)
{

  BOOL encodeSucess = CryptBinaryToStringA((const BYTE *)input, core->strlen(input), CRYPT_STRING_BASE64, NULL, outputSize);
  if (!encodeSucess)
  {
    return FALSE;
  }
  *output = (char *)core->malloc(*outputSize);
  if (*output == NULL)
  {
    return FALSE;
  }
  encodeSucess = CryptBinaryToStringA((const BYTE *)input, core->strlen(input), CRYPT_STRING_BASE64, *output, outputSize);
  if (!encodeSucess)
  {
    core->free(*output);
    return FALSE;
  }
  return TRUE;
}

// Utility function to decode a base64 string
BOOL Base64Decode(const char *input, BYTE **output, DWORD *outputSize)
{
  BOOL decodeSuccess = CryptStringToBinaryA(input, core->strlen(input), CRYPT_STRING_BASE64, NULL, outputSize, NULL, NULL);
  if (!decodeSuccess)
  {
    return FALSE;
  }
  *output = (BYTE *)core->malloc(*outputSize);
  if (*output == NULL)
  {
    return FALSE;
  }
  decodeSuccess = CryptStringToBinaryA(input, core->strlen(input), CRYPT_STRING_BASE64, *output, outputSize, NULL, NULL);
  if (!decodeSuccess)
  {
    core->free(*output);
    return FALSE;
  }
  return TRUE;
}

// Exported function - Run
__declspec(dllexport) LPVOID CommandRunA(int argc, char **argv)
{
  if (argc != 3)
  {
    core->wprintf(L"Invalid arguments.\r\n%S", CommandHelpA());
    return NULL; // Error code for invalid arguments
  }

  if (core->strcmp(argv[1], "encode") == 0)
  {
    char *encodedString = NULL;
    DWORD encodedSize = 0;
    if (Base64Encode(argv[2], &encodedString, &encodedSize))
    {
      core->wprintf(L"%S\n", encodedString);
      lpOut = (LPVOID)encodedString;
    }
    else
    {
      core->wprintf(L"Error encoding string.\r\n");
      return (LPVOID)NULL;
    }
  }
  else if (core->strcmp(argv[1], "decode") == 0)
  {
    BYTE *decodedBytes = NULL;
    DWORD decodedSize = 0;
    if (Base64Decode(argv[2], &decodedBytes, &decodedSize))
    {
      // Assuming decoded output is also an ANSI string
      core->wprintf(L"%.*S\n", decodedSize, decodedBytes);
      lpOut = (LPVOID)decodedBytes;
    }
    else
    {
      core->wprintf(L"Error decoding string.\r\n");
      return NULL;
    }
  }
  else
  {
    core->wprintf(L"Invalid command. Use 'encode' or 'decode'.\r\n");
    return NULL; // Error code for invalid command
  }

  return lpOut; // Success
}

// Entrypoint for the DLL
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  switch (fdwReason)
  {
  case DLL_PROCESS_ATTACH:
    // Initialization code for when the DLL is loaded
    break;
  case DLL_PROCESS_DETACH:
    // Cleanup code for when the DLL is unloaded
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    // Thread-specific initialization or cleanup
    break;
  }
  return TRUE; // Indicate successful DLL process attachment or detachment
}
