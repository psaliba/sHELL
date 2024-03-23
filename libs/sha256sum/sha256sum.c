
#include "../include/sHELL.h"
#include <windows.h>
#include <bcrypt.h>
#include "../include/opcodes.h"
#include "../include/readf.h"

const char Name[] = "sha256sum";
const char Help[] = "Print the SHA256 checksums.\n"
                    "Usage:\n"
                    "    sha256sum [file]\n";
#define INDEX_sha256sum_readf 0
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
InternalAPI *core = NULL;
CommandDependency deps[] = {DECLARE_DEP(OPCODE_readf), DECLARE_DEP(0)};
CommandA *readf = NULL;

LPVOID lpOut = NULL;

char **FileNameToArgv(int *argc, char *lpFileName)
{
    char fmt[] = "readf %s";
    char *argvBuff =
        (char *)core->malloc(core->strlen(lpFileName) + core->strlen(fmt) - 1);
    core->sprintf(argvBuff, fmt, lpFileName);
    char **readfArgv = core->CommandLineToArgvA(argvBuff, argc);
    // free templated string
    core->free(argvBuff);
    return readfArgv;
}

PBYTE hashBuffer(PUCHAR buffer, DWORD dwSize)
{

    BCRYPT_ALG_HANDLE hAlg = NULL;
    NTSTATUS status = 0;
    DWORD cbData = 0;
    DWORD cbHash = 0;
    PBYTE pbHash = NULL;

    status = BCryptOpenAlgorithmProvider(
        &hAlg,
        BCRYPT_SHA256_ALGORITHM,
        NULL,
        0);
    if (status != 0)
    {
        core->wprintf(L"**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n", status);
        return NULL;
    }

    // calculate the length of the hash
    if (!NT_SUCCESS(status = BCryptGetProperty(
                        hAlg,
                        BCRYPT_HASH_LENGTH,
                        (PBYTE)&cbHash,
                        sizeof(DWORD),
                        &cbData,
                        0)))
    {
        core->wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return NULL;
    }

    // allocate the hash buffer on the heap
    pbHash = core->malloc(cbHash);
    if (NULL == pbHash)
    {
        core->wprintf(L"**** memory allocation failed\n");
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return NULL;
    }

    if (!NT_SUCCESS(status = BCryptHash(
                        hAlg,
                        NULL,
                        0,
                        buffer,
                        dwSize,
                        pbHash,
                        cbHash)))
    {
        core->wprintf(L"**** Error 0x%x returned by BCryptHash\n", status);
        core->free(pbHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return NULL;
    }
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return pbHash;
}

__declspec(dllexport) VOID CommandCleanup()
{
    if (lpOut)
    {
        core->free(lpOut);
        lpOut = NULL;
    }
}
// initialization code
__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore)
{
    core = lpCore;
    debug_wprintf(L"[+] Initializing %S\n", Name);
    if (!core->ResolveCommandDependnecies(deps))
    {
        core->wprintf(L"Dependency failed!\n");
        return FALSE;
    }
    debug_wprintf(L"Setting readf to lpCMD...\n");
    readf = deps[INDEX_sha256sum_readf].lpCmd;
    debug_wprintf(L"Found readf: %p\n", (void *)readf);
    return TRUE;
}

// Exported function - Name
__declspec(dllexport) const char *CommandNameA() { return Name; }

// Exported function - Help
__declspec(dllexport) const char *CommandHelpA() { return Help; }

// Exported function - Run
__declspec(dllexport) LPVOID CommandRunA(int argc, char **argv)
{
    // Example implementation: print arguments and return count
    if (argc != 2)
    {
        core->wprintf(L"Invalid arguments.\n%S", CommandHelpA());
        return NULL;
    }
    CommandOut_readf *readfOut = NULL;

    char **readfArgv = FileNameToArgv(&argc, argv[1]);
    if (readfArgv == NULL)
    {
        core->wprintf(L"Failed to allocate memory for readfArgv.\n");
        return NULL;
    }

    readfOut = readf->fnRun(argc, readfArgv);
    if (readfOut == NULL)
    {
        core->wprintf(L"Error running readf.\n");
        return NULL;
    }

    PBYTE out = hashBuffer((PUCHAR)readfOut->lpBuffer, readfOut->qwFileSize.QuadPart);
    if (out == NULL)
    {
        core->wprintf(L"Error hashing buffer.\n");
        return NULL;
    }

    for (DWORD i = 0; i < 32; ++i)
    {
        core->wprintf(L"%02x", out[i]);
    }
    core->wprintf(L"  %S\n", argv[1]);

    core->free(out);
    core->free(readfArgv);
    return 0;
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
