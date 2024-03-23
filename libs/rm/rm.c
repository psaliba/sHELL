
#include "../include/sHELL.h"
#include <windows.h>

const char Name[] = "rm";
const char Help[] = "removes files or directories. Use -r to recursively delete. Usage:\n"
                    "    rm [-r] <file1> <file2> ...\n";

InternalAPI *core = NULL;

LPVOID lpOut = NULL;
__declspec(dllexport) VOID CommandCleanup() {
  if (lpOut) {
    core->free(lpOut);
    lpOut = NULL;
  }
}

int removeDir(char *szDir) {
    WIN32_FIND_DATA ffd;
    size_t filesize = core->strlen(szDir);
    HANDLE hFind = INVALID_HANDLE_VALUE;

    char szDirCopy[MAX_PATH];
    core->strcpy(szDirCopy, szDir);
    szDirCopy[filesize] = '\\';
    szDirCopy[filesize + 1] = '*';
    szDirCopy[filesize + 2] = '\0';

    hFind = FindFirstFile(szDirCopy, &ffd);
    if (INVALID_HANDLE_VALUE == hFind) {
        core->wprintf(L"FindFirstFile failed (%d)\r\n", GetLastError());
        return 1;
    }

    do {
        if (core->strcmp(ffd.cFileName, ".") == 0 || core->strcmp(ffd.cFileName, "..") == 0) {
            continue;
        }

        core->strcpy(szDirCopy, szDir);
        core->strcpy(szDirCopy + filesize, "\\");
        core->strcpy(szDirCopy + filesize + 1, ffd.cFileName);

        core->wprintf(L"Removing %S....\n", szDirCopy);

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            removeDir(szDirCopy);
        } else {
            if (!DeleteFileA(szDirCopy)) {
                core->wprintf(L"Failed to remove file %S\n", szDirCopy);
                return 1;
            }
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);

    if (!RemoveDirectoryA(szDir)) {
        core->wprintf(L"Failed to remove directory %S\n", szDir);
        return 1;
    }
    return 0;
}

// initialization code
__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore) { core = lpCore; }

// Exported function - Name
__declspec(dllexport) const char *CommandNameA() { return Name; }

// Exported function - Help
__declspec(dllexport) const char *CommandHelpA() { return Help; }

// Exported function - Run
__declspec(dllexport) int CommandRunA(int argc, char **argv) {
    BOOL recursive = FALSE;

    if (argc < 2) {
        core->wprintf(L"Invalid arguments.\n%S", CommandHelpA());
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if ( core->strlen(argv[i]) == 2 && argv[i][1] == 'r') {
                recursive = TRUE;
            }
        } else {

            if (core->strlen(argv[i]) > MAX_PATH - 3) {
                core->wprintf(L"Path too long\n");
                return 1;
            }
            if (recursive) {
                removeDir(argv[i]);
            } else {
                if (!DeleteFileA(argv[i])) {
                    core->wprintf(L"Failed to remove file %S\n", argv[i]);
                    return 1;
                }
            }
        }
    }
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
