
#include "../include/sHELL.h"
#include <windows.h>

const char Name[] = "ls";
const char Help[] = "List the contents of the passed directory. If no directory is passed, lists the current directory\n"
                    "Usage: ls <dir> \n"
                    ">>> file.txt\n";

InternalAPI *core = NULL;

// initialization code
__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore) { core = lpCore; }

// Exported function - Name
__declspec(dllexport) const char *CommandName() { return Name; }

// Exported function - Help
__declspec(dllexport) const char *CommandHelp() { return Help; }

// Exported function - Run
__declspec(dllexport) int CommandRun(int argc, char **argv) {
if (argc > 2) {
    core->wprintf(L"Invalid arguments.\n%s", CommandHelp());
    return 1; // Error code for invalid arguments
  }

    WIN32_FIND_DATAA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    




  return 0;
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
