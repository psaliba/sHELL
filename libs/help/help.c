
#include "../include/sHELL.h"
#include <windows.h>

const char Name[] = "help";
const char Help[] =
    "Print help messages for commands.\r\n"
    "Passed with no command names, it prints all avaiable commands help message"
    ">>>help echo\r\n"
    "echo a string back to the terminal. Example:...\r\n";

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
  long long unsigned int mods = *core->gModuleCount;

  // use djbhash to compare the command name with the passed args
  // got to iterate through all the passed args tho
  // maybe build an array of mods that are passed? 

  if (argc > 1) {

    for (size_t i = 0; i < mods; i++) {
      
      // cant use strcmp
      if (strcmp(core->gaCommandsA[i].fnName(), argv[1]) == 0) {
        core->wprintf(L"Module Name: %S\r\n", core->gaCommandsA[i].fnName());
        core->wprintf(L"Module Help: %S\r\n", core->gaCommandsA[i].fnHelp());
        return NULL;
      }
    }
    core->wprintf(L"Command not found: %S\r\n", argv[1]);
    return NULL;
  }
  for (size_t i = 0; i < mods; i++) {
    core->wprintf(L"Module Name: %S\r\n", core->gaCommandsA[i].fnName());
    core->wprintf(L"Module Help: %S\r\n", core->gaCommandsA[i].fnHelp());
    core->wprintf(L"---------------------------\r\n");
  }
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
