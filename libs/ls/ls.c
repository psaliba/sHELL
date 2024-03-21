
#include "../include/sHELL.h"
#include <windows.h>
#include <timezoneapi.h>

const char Name[] = "ls";
const char Help[] = "List information about the files (the current directory by default).\n"
                    "Flags:\n"
                    "    -R  List subdirectories recursively.\n"
                    "    -a  Show all files, including hidden ones.\n"
                    "    -l  Use a long listing format showing file attributes.\n"
                    "Usage:\n"
                    "    ls [-Ral] [path]";

InternalAPI *core = NULL;

LPVOID lpOut = NULL;
__declspec(dllexport) VOID CommandCleanup()
{
  if (lpOut)
  {
    core->free(lpOut);
    lpOut = NULL;
  }
}

int listFiles(char *szDir, BOOL recursive, BOOL allFiles, BOOL longFormat, size_t length_of_arg)
{
  WIN32_FIND_DATA ffd;
  LARGE_INTEGER filesize;
  FILETIME  ftAccess;
  HANDLE hFind = INVALID_HANDLE_VALUE;
  DWORD dwError = 0;


 // Add a wildcard to the end of the directory string
  szDir[length_of_arg] = '\\';
  szDir[length_of_arg + 1] = '*';
  szDir[length_of_arg + 2] = '\0';

  core->wprintf(L"target Directory of %S\r\n", szDir);

  hFind = FindFirstFile(szDir, &ffd);
  if (INVALID_HANDLE_VALUE == hFind)
  {
    core->wprintf(L"FindFirstFile failed (%d)\r\n", GetLastError());
    return 1;
  }

  do
  {
    if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      
      if (recursive && core->strcmp(ffd.cFileName, ".") != 0 && core->strcmp(ffd.cFileName, "..") != 0)
      {
        core->wprintf(L"  %S   <DIR> Listing recursively...\n", ffd.cFileName);
        char *newDir = (char *)core->malloc(MAX_PATH);
        if (newDir == NULL)
        {
          core->wprintf(L"Failed to allocate memory for new directory path.\n");
          return 1; // Error code for memory allocation failure
        }
        core->strcpy(newDir, szDir);
        core->strcpy(newDir + length_of_arg + 1, ffd.cFileName);
        listFiles(newDir, recursive, allFiles, longFormat, length_of_arg + core->strlen(ffd.cFileName) + 1);
        core->free(newDir);
      } else {
        core->wprintf(L"  %S   <DIR>\n", ffd.cFileName);
      }
    } else if (!allFiles && ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
    { // skip if hidden and -a is not passed
      continue;
    }
    else
    {
      if (longFormat)
      {
      filesize.LowPart = ffd.nFileSizeLow;
      filesize.HighPart = ffd.nFileSizeHigh;
      SYSTEMTIME st;
      ftAccess = ffd.ftLastAccessTime;
      FileTimeToSystemTime(&ftAccess, &st);
      core->wprintf(L"  %S |  %lld bytes | Last Accessed at %04d-%02d-%02d %02d:%02d:%02d\n", ffd.cFileName, filesize.QuadPart, st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond);
    } else {
      core->wprintf(L"  %S\n", ffd.cFileName);
    }
    }
  } while (FindNextFile(hFind, &ffd) != 0);
  dwError = GetLastError();
  if (dwError != ERROR_NO_MORE_FILES)
  {
    core->wprintf(L"FindFirstFile failed (%d)\r\n", dwError);
  }

  FindClose(hFind);
  return dwError;
}






// initialization code
__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore) { core = lpCore; }

// Exported function - Name
__declspec(dllexport) const char *CommandNameA() { return Name; }

// Exported function - Help
__declspec(dllexport) const char *CommandHelpA() { return Help; }

// Exported function - Run
__declspec(dllexport) int CommandRunA(int argc, char **argv)
{
  BOOL allFiles = FALSE;
  BOOL longFormat = FALSE;
  BOOL recursive = FALSE;
  BOOL useDefault = TRUE;
  
  size_t length_of_arg;
  TCHAR szDir[MAX_PATH];

  char *tDir = (char *)core->malloc(MAX_PATH);
  if (tDir == NULL)
  {
    core->wprintf(L"Failed to allocate memory for Dir path.\n");
    return 1; // Error code for memory allocation failure
  }

  for (int i = 1; i < argc; i++)
  {
    if (argv[i][0] == '-')
    {
      for (int j = 0; j < core->strlen(argv[i]); j++)
      {
        if (argv[i][j] == 'R')
        {
          recursive = TRUE;
        }
        else if (argv[i][j] == 'a')
        {
          allFiles = TRUE;
        }
        else if (argv[i][j] == 'l')
        {
          longFormat = TRUE;
        }
      }
    }
    else
    {
      core->strcpy(tDir, argv[i]);
      useDefault = FALSE;
      break;
    }
  }

  if (useDefault)
  {
    if (GetCurrentDirectoryA(MAX_PATH, tDir) == 0)
    {
      core->wprintf(L"Error getting current directory: %d\n", GetLastError());
      core->free(tDir);
      return 1; // Error code for directory retrieval failure
    }
  }

  core->wprintf(L"found recursive: %d, allFiles: %d, longFormat: %d, useDefault: %d\r\n", recursive, allFiles, longFormat, useDefault);

  length_of_arg = core->strlen(tDir); // Get the length of the string
  if (length_of_arg > (MAX_PATH - 3))
  {
    core->wprintf(L"Directory path is too long.\r\n");
    return 1;
  }

  core->strcpy(szDir, tDir);
  core->free(tDir);

  listFiles(szDir, recursive, allFiles, longFormat, length_of_arg);
  return 0; // Success
}

// Entrypoint for the DLL
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  switch (fdwReason)
  {
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
