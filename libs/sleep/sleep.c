

#include "../include/sHELL.h"
#include <shlwapi.h>
#include <windows.h>

const char Name[] = "sleep";
const char Help[] = "Pause execution (sleep) for the current thread.\r\n"
                    "Example Sleeping for 1 second:\r\n"
                    ">>>sleep 1000 \r\n"
                    ">>>\r\n";

InternalAPI *core = NULL;

LPVOID lpOut = NULL;
__declspec(dllexport) VOID CommandCleanup() {
  if (lpOut) {
    core->free(lpOut);
    lpOut = NULL;
  }
}
// initialization code
__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore) {
  core = lpCore;
  return TRUE;
}

// Exported function - Name
__declspec(dllexport) const char *CommandNameA() { return Name; }

// Exported function - Help
__declspec(dllexport) const char *CommandHelpA() { return Help; }

// Exported function - Run
__declspec(dllexport) LPVOID CommandRunA(int argc, char **argv) {
  if (argc != 2) {
    core->wprintf(L"Invalid input:\r\n %S\r\n", Help);
    return NULL;
  }

  DWORD dwMilliseconds = StrToIntA(argv[1]);
  // error checking for atoi
  if (dwMilliseconds == 0) {
    core->wprintf(L"Invalid input:\r\n %S\r\n", Help);
    return NULL;
  }

  Sleep(dwMilliseconds);
  
  return NULL;
}

// Entrypoint for the DLL
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    // Code to run when the DLL is loaded
    break;
  case DLL_PROCESS_DETACH:
    // Code to run when the DLL is unloaded
    break;
  case DLL_THREAD_ATTACH:
    // Code to run when a thread is created during DLL's existence
    break;
  case DLL_THREAD_DETACH:
    // Code to run when a thread ends normally
    break;
  }
  return TRUE; // Successful
}
