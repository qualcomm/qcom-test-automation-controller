// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//         copyright notice, this list of conditions and the following
//         disclaimer in the documentation and/or other materials provided
//         with the distribution.
//
//     * Neither the name of Qualcomm Technologies, Inc. nor the names of its
//         contributors may be used to endorse or promote products derived
//         from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "TACApplication.h"

#include <QApplication>

// ---------------------------------------------------------------------------
// Crash / diagnostic logger
// ---------------------------------------------------------------------------
#include <windows.h>
#include <dbghelp.h>
#include <cstdio>
#include <ctime>

#pragma comment(lib, "dbghelp.lib")

FILE* gCrashLog = nullptr;

static void crashLogOpen()
{
    gCrashLog = fopen("qtac_crash.log", "w");
    if (!gCrashLog) return;
    std::time_t t = std::time(nullptr);
    fprintf(gCrashLog, "qtac-app started: %s\n", std::ctime(&t));
    fflush(gCrashLog);
}

static void crashLog(const char* msg)
{
    if (!gCrashLog) return;
    fputs(msg, gCrashLog);
    fputc('\n', gCrashLog);
    fflush(gCrashLog);
}

static LONG CALLBACK vectoredExceptionHandler(PEXCEPTION_POINTERS ep)
{
    if (!gCrashLog) return EXCEPTION_CONTINUE_SEARCH;

    DWORD code = ep->ExceptionRecord->ExceptionCode;
    // Skip C++ exceptions and non-fatal codes
    if (code == 0xE06D7363 /*C++ exception*/ || code == EXCEPTION_BREAKPOINT)
        return EXCEPTION_CONTINUE_SEARCH;

    fprintf(gCrashLog, "\n=== CRASH: ExceptionCode=0x%08X Address=%p ===\n",
            (unsigned)code, ep->ExceptionRecord->ExceptionAddress);

    // Stack walk
    SymInitialize(GetCurrentProcess(), nullptr, TRUE);

    CONTEXT ctx = *ep->ContextRecord;
    STACKFRAME64 sf = {};
#ifdef _WIN64
    sf.AddrPC.Offset    = ctx.Rip;
    sf.AddrFrame.Offset = ctx.Rbp;
    sf.AddrStack.Offset = ctx.Rsp;
    DWORD machType = IMAGE_FILE_MACHINE_AMD64;
#else
    sf.AddrPC.Offset    = ctx.Eip;
    sf.AddrFrame.Offset = ctx.Ebp;
    sf.AddrStack.Offset = ctx.Esp;
    DWORD machType = IMAGE_FILE_MACHINE_I386;
#endif
    sf.AddrPC.Mode = sf.AddrFrame.Mode = sf.AddrStack.Mode = AddrModeFlat;

    char symBuf[sizeof(SYMBOL_INFO) + 256];
    SYMBOL_INFO* sym = reinterpret_cast<SYMBOL_INFO*>(symBuf);
    sym->SizeOfStruct = sizeof(SYMBOL_INFO);
    sym->MaxNameLen   = 255;

    for (int frame = 0; frame < 32; ++frame)
    {
        if (!StackWalk64(machType, GetCurrentProcess(), GetCurrentThread(),
                         &sf, &ctx, nullptr, SymFunctionTableAccess64,
                         SymGetModuleBase64, nullptr))
            break;
        if (sf.AddrPC.Offset == 0) break;

        DWORD64 disp = 0;
        if (SymFromAddr(GetCurrentProcess(),
                        static_cast<DWORD64>(sf.AddrPC.Offset), &disp, sym))
        {
            fprintf(gCrashLog, "  #%02d %p  %s + 0x%llx\n",
                    frame,
                    reinterpret_cast<void*>(sf.AddrPC.Offset),
                    sym->Name, (unsigned long long)disp);
        }
        else
        {
            fprintf(gCrashLog, "  #%02d %p  <unknown>\n",
                    frame,
                    reinterpret_cast<void*>(sf.AddrPC.Offset));
        }
    }

    SymCleanup(GetCurrentProcess());
    fflush(gCrashLog);
    return EXCEPTION_CONTINUE_SEARCH; // let Windows handle it normally
}

// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    crashLogOpen();
    AddVectoredExceptionHandler(1, vectoredExceptionHandler);

    TACApplication app(argc, argv);

    TACApplication::createTACWindow();
    TACApplication::instance()->tryOpenLastDevice();

    int ret = app.exec();

    if (gCrashLog)
    {
        crashLog("=== clean exit ===");
        fclose(gCrashLog);
    }
    return ret;
}
