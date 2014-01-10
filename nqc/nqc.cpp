/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The Initial Developer of this code is David Baum.
 * Portions created by David Baum are Copyright (C) 1999 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */
#ifdef WIN32
#include <windows.h>
#endif

#include <cstdio>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <cerrno>

#include "Program.h"
#include "RCX_Image.h"
#include "RCX_Link.h"
#include "Symbol.h"
#include "PreProc.h"
#include "parser.h"
#include "Macro.h"
#include "RCX_Cmd.h"
#include "RCX_Log.h"
#include "SRecord.h"
#include "AutoFree.h"
#include "DirList.h"
#include "Buffer.h"
#include "Error.h"
#include "Compiler.h"
#include "CmdLine.h"
#include "version.h"
#include "PDebug.h"

using std::fopen;
using std::printf;
using std::time_t;
using std::localtime;
using std::tm;


// use this to check for memory leaks in the compiler
//#define CHECK_LEAKS

// use these to debug the LEXER
//#define TEST_LEXER    1   // test lexer only
//#define TEST_LEXER    2   // test lexer and pre-processor


#ifdef CHECK_LEAKS
#include <DebugNew.h>
#endif

// some win32 consoles are lame...use stdout instead of stderr for usage, etc.
#ifdef WIN32
#define STDERR stdout
#else
#define STDERR stderr
#endif

FILE *gErrorStream = stderr;

#define kMaxPrintedErrors   10



class AutoLink : public RCX_Link
{
public:
    AutoLink() : fSerialPort(0), fOpen(false) {}
    ~AutoLink() { Close(); }

    RCX_Result Open();
    void Close();
    RCX_Result Send(const RCX_Cmd *cmd, bool retry=true);

    void SetSerialPort(const char *sp)   { fSerialPort = sp; }

    bool DownloadProgress(int soFar, int total);

private:
    const char* fSerialPort;
    bool fOpen;
};



class MyCompiler : public Compiler, public ErrorHandler
{
public:
    MyCompiler() {}

    Buffer *CreateBuffer(const char *name);

    void AddError(const Error &e, const LexLocation *loc);
    void AddDir(const char *dirspec) { fDirs.Add(dirspec); }

private:
    DirList fDirs;
} gMyCompiler;


#define kMaxFirmware 65536
#define kOldIncludePathEnv "NQCC_INCLUDE"
#define kNewIncludePathEnv "NQC_INCLUDE"
#define kOptionsEnv "NQC_OPTIONS"
#define kLowBattery 6600
#define kLow45Battery 3300

#define kRCXFileExtension ".rcx"
#define kNQCFileExtension ".nqc"


// error codes in addition to RCX_Result codes
#define kUsageError (kRCX_LastError - 1)
#define kQuietError (kRCX_LastError - 2)

// codes for the actions
enum {
    kFirstActionCode = 256,
    kDatalogCode = kFirstActionCode,
    kDatalogFullCode,
    kClearMemoryCode,
    kFirmwareCode,
    kFirmware4xCode,
    kGetVersion,
    kGetBatteryLevel,
    kNearCode,
    kFarCode,
    kWatchCode,
    kSleepCode,
    kRunCode,
    kProgramCode,
    kMessageCode,
    kRawCode,
    kRaw1Code,
    kRemoteCode,
    kApiCode,
    kHelpCode,
    kCompileStdinCode
};

// these must be in the same order as the codes for the long options
static const char *sActionNames[] = {
    "datalog", "datalog_full", "clear", "firmware", "firmfast",
    "getversion", "batterylevel", "near", "far", "watch", "sleep",
    "run", "pgm", "msg", "raw", "raw1", "remote", "api", "help", ""
};

// these MUST be in the same order as the RCX_TargetType values
static const char *sTargetNames[] = {
    "RCX",
    "CM",
    "Scout",
    "RCX2",
    "Spy",
    "Swan"
};

struct Request {
    const char *fSourceFile;
    const char *fOutputFile;
    const char *fListFile;
    bool fListing;
    bool fSourceListing;
    bool fDownload;
    bool fBinary;
    bool fGenLASM;
    int fFlags;
};

static int GetActionCode(const char *arg);
static RCX_Result ProcessCommandLine(int argc, char **argv);
static void PrintError(RCX_Result error, const char *filename = 0);
static void PrintUsage();
static void DefineMacro(const char *text);
static RCX_Result ProcessFile(const char *sourceFile,
    const Request &req);
static char *CreateFilename(const char *source, const char *oldExt,
    const char *newExt);
static const char *LeafName(const char *filename);
static int CheckExtension(const char *s1, const char *ext);
static RCX_Image *Compile(const char *sourceFile,  int flags);
static bool GenerateListing(RCX_Image *image, const char *filename,
    bool includeSource, bool generateLASM);
static RCX_Result Download(RCX_Image *image);
static RCX_Result UploadDatalog(bool verbose);
static RCX_Result DownloadFirmware(const char *filename, bool fast);
static RCX_Result GetVersion();
static RCX_Result GetBatteryLevel();
static RCX_Result SetWatch(const char *timeSpec);
static RCX_Result SetErrorFile(const char *filename);
static RCX_Result RedirectOutput(const char *filename);
static RCX_Result SendRawCommand(const char *text, bool retry);
static RCX_Result ClearMemory();
static RCX_Result SetTarget(const char *name);
static RCX_Result SendRemote(const char *event, int repeat);
#ifdef TEST_LEXER
static void PrintToken(int t, TokenVal v);
#endif
static void PrintApi(bool compatMode);
static bool SameString(const char *s1, const char *s2);
static void PrintVersion();

AutoLink gLink;

// I assume everyone is running 0328 or later.
RCX_TargetType gTargetType = kRCX_RCX2Target;
bool gVerbose = false;
int gTimeout = 0;
bool gQuiet = false;


int main(int argc, char **argv)
{
    RCX_Result result;

    // add any default include paths
    gMyCompiler.AddDir(getenv(kOldIncludePathEnv));
    gMyCompiler.AddDir(getenv(kNewIncludePathEnv));

    // TODO: make this return a struct instead so we can pass back the
    // result code and a context specific extra error condition.
    result = ProcessCommandLine(argc, argv);
    PrintError(result);

    gLink.Close();

    if (gErrorStream != stderr && gErrorStream != stdout) {
        fclose(gErrorStream);
    }

#ifdef DEBUG_NEW
    printf("%d allocations, %d total bytes, %d max\n",
        gDebugNewAllocCount, gDebugNewAllocCurr, gDebugNewAllocMax);
#endif

    return RCX_ERROR(result) ? result : 0;
}


RCX_Result ProcessCommandLine(int argc, char ** argv)
{
    bool optionsOK = true;
    bool fileProcessed = false;
    Request req = { 0 };    // rest will be zero'ed
    CmdLine args;
    RCX_Result result = kRCX_OK;
    RCX_Cmd cmd;

    // first add environment options
    args.Parse(getenv(kOptionsEnv));

    // add all command line args after the first one
#ifdef WIN32
    // For Win32, bypass the argc/argv array and get the raw command
    // line. This is for backwards compatability with BricxCC which
    // doesn't like its args to be escaped by the runtime.
    #pragma unused(argc, argv)
    args.Parse(GetCommandLineA(), 1);
#else
    if (argc > 1) {
        args.Add(argc-1, argv+1);
    }
#endif

    // Process the args.
    while(args.Remain() && !RCX_ERROR(result)) {
        const char* a=args.Next();
        bool isOption = (a[0] == '-');

#ifdef WIN32
        if (a[0]=='/') {
            isOption = true;
        }
#endif

        if (isOption) {
            int code = GetActionCode(a+1);

            if (code) {
                optionsOK = false;
            }
            else {
                if (!optionsOK) return kUsageError;
                code = a[1];
            }

            switch(code) {
                // options
                case '1':
                    req.fFlags |= Compiler::kCompat_Flag;
                    break;
                case 'D':
                    if  (*(a+2)=='\0') return kUsageError;
                    DefineMacro(a+2);
                    break;
                case 'E':
                    result = SetErrorFile(a+2);
                    break;
                case 'I':
                    if  (*(a+2)=='\0') return kUsageError;
                    gMyCompiler.AddDir(a+2);
                    break;
                case 'L':
                    req.fListing = true;
                    req.fListFile = a[2] ? a+2 : 0;
                    break;
                case 'R':
                    result = RedirectOutput(a+2);
                    break;
                case 's':
                    req.fSourceListing = true;
                    break;
                case 'c':
                    req.fGenLASM = true;
                    break;
                case 'O':
                    if  (*(a+2)=='\0') return kUsageError;
                    req.fOutputFile = a+2;
                    break;
                case 'S':
                    if  (*(a+2)=='\0') return kUsageError;
                    gLink.SetSerialPort(a+2);
                    break;
                case 'T':
                    if  (*(a+2)=='\0') return kUsageError;
                    result = SetTarget(a+2);
                    break;
                case 'x':
                    gLink.SetOmitHeader(true);
                    break;
                case 'f':
                    if  (*(a+2)=='\0') return kUsageError;
                        gLink.SetRCXFirmwareChunkSize(atoi(a+2));
                     break;
                case 'w':
                    if  (*(a+2)=='\0') return kUsageError;
                        gLink.SetDownloadWaitTime(atoi(a+2));
                    break;
                case 'U':
                    if  (*(a+2)=='\0') return kUsageError;
                    Compiler::Get()->Undefine(a+2);
                    break;
                case 'd':
                case 'u':
                    req.fDownload = true;
                    break;
                case 'l':
                    req.fListing = true;
                    break;
                case 'n':
                    req.fFlags |= Compiler::kNoSysFile_Flag;
                    break;
                case 'b':
                    req.fBinary = true;
                    break;
                case 't':
                    if (!args.Remain()) return kUsageError;
                    gTimeout = args.NextInt();
                    break;
                case 'v':
                    gVerbose = true;
                    break;
                case 'q':
                    gQuiet = true;
                    break;

                // actions
                case kCompileStdinCode:
                    result = ProcessFile(nil, req);
                    fileProcessed = true;
                    break;
                case kDatalogCode:
                    result = UploadDatalog(false);
                    break;
                case kDatalogFullCode:
                    result = UploadDatalog(true);
                    break;
                case kClearMemoryCode:
                    result = ClearMemory();
                    break;
                case kFirmwareCode:
                    if (!args.Remain()) return kUsageError;
                    result = DownloadFirmware(args.Next(), false);
                    break;
                case kFirmware4xCode:
                    if (!args.Remain()) return kUsageError;
                    result = DownloadFirmware(args.Next(), true);
                    break;
                case kGetVersion:
                    result = GetVersion();
                    break;
                case kGetBatteryLevel:
                    result = GetBatteryLevel();
                    break;
                case kNearCode:
                    result = gLink.Send(cmd.Set(kRCX_IRModeOp, 0));
                    break;
                case kFarCode:
                    result = gLink.Send(cmd.Set(kRCX_IRModeOp, 1));
                    break;
                case kWatchCode:
                    if (!args.Remain()) return kUsageError;
                    result = SetWatch(args.Next());
                    break;
                case kSleepCode:
                    if (!args.Remain()) return kUsageError;
                    result = gLink.Send(cmd.Set(kRCX_AutoOffOp,
                        (UByte)args.NextInt()));
                    break;
                case kRunCode:
                    result = gLink.Send(cmd.Set(kRCX_StartTaskOp,
                        getTarget(gTargetType)->fRanges[kRCX_TaskChunk].fBase));
                    break;
                case kProgramCode:
                    if (!args.Remain()) return kUsageError;
                    result = gLink.Send(cmd.Set(kRCX_SelectProgramOp,
                        (UByte)(args.NextInt()-1)));
                    break;
                case kMessageCode:
                    if (!args.Remain()) return kUsageError;
                    result = gLink.Send(cmd.Set(kRCX_Message,
                        (UByte)(args.NextInt())), false);
                    break;
                case kRawCode:
                case kRaw1Code: // one-time send, no retry
                    if (!args.Remain()) return kUsageError;
                    result = SendRawCommand(args.Next(), code == kRawCode);
                    break;
                case kRemoteCode:
                    if (args.Remain() < 2) return kUsageError;
                    {
                        const char *event = args.Next();
                        int repeat = args.NextInt();
                        result = SendRemote(event, repeat);
                    }
                    break;
                case kApiCode:
                    PrintApi(req.fFlags & Compiler::kCompat_Flag);
                    break;
                case kHelpCode:
                    PrintUsage();
                    break;
                default:
                    return kUsageError;
            }
        }
        else if (!fileProcessed) {
            result = ProcessFile(a, req);

#ifdef CHECK_LEAKS
            int firstAllocCount = gDebugNewAllocCount;
            Compiler::Get()->Reset();
            result = ProcessFile(a, req);
            printf("%d leaked allocations\n",
                gDebugNewAllocCount - firstAllocCount);
#endif
            optionsOK = false;
            fileProcessed = true;
        }
        else {
            return kUsageError;
        }
    }

    // check if we did anything (compile and/or actions)
    if (optionsOK) {
        return kUsageError;
    }

    return result;
}


void PrintApi(bool compatMode)
{
    Buffer *b = Compiler::CreateApiBuffer(compatMode);
    fwrite(b->GetData(), b->GetLength(), 1, stdout);
    delete b;
}

/**
 * Set the target type of the brick for this action.
 *
 * @param name the case-insensitive name of the target brick as a string
 * @return kRCX_OK if we successfully set the target type based on the name, kUsageError if not
 * @see sTargetNames
 * @see RCX_TargetType
 */
RCX_Result SetTarget(const char *name)
{
    for (unsigned i=0; i < sizeof(sTargetNames) / sizeof(const char *); ++i) {
        if (SameString(name, sTargetNames[i])) {
            gTargetType = (RCX_TargetType)i;
            return kRCX_OK;
        }
    }
    return kUsageError;
}


RCX_Result SetWatch(const char *timeSpec)
{
    int hour;
    int minute;
    RCX_Cmd cmd;

    if (strcmp(timeSpec, "now")==0) {
        time_t t;
        struct tm *tmp;

        time(&t);
        tmp = localtime(&t);
        hour = tmp->tm_hour;
        minute = tmp->tm_min;
    }
    else {
        int t = atoi(timeSpec);
        hour = t / 100;
        minute = t % 100;
    }

    return gLink.Send(cmd.Set(kRCX_SetWatchOp, (UByte)hour, (UByte)minute));
}


RCX_Result ProcessFile(const char *sourceFile, const Request &req)
{
    RCX_Image *image;
    RCX_Result result = kRCX_OK;
    bool ok = true;
    bool compiled = false;

    if (sourceFile && (req.fBinary || CheckExtension(sourceFile, kRCXFileExtension))) {
        // load RCX image file
        image = new RCX_Image();
        result = image->Read(sourceFile);
        if (RCX_ERROR(result)) {
            PrintError(result, sourceFile);
            delete image;
            return kQuietError;
        }
    } else {
        // compile file
        compiled = true;
        image = Compile(sourceFile, req.fFlags);

        if (!image) {
            int errors = ErrorHandler::Get()->GetErrorCount();

            if (errors)
                fprintf(gErrorStream, "# %d error%s during compilation\n", errors, errors==1 ? "" : "s");
            return kQuietError;
        }

        const char *outputFile = req.fOutputFile;
        char *newFilename = 0;

        if (!req.fDownload && !req.fListing && !req.fOutputFile && sourceFile) {
            outputFile =
            newFilename = 
                CreateFilename(LeafName(sourceFile),
                    kNQCFileExtension, kRCXFileExtension);
        }

        if (outputFile) {
            errno = 0;
            if (!image->Write(outputFile)) {
                fprintf(gErrorStream, "Error: could not create output file \"%s\" (%d)\n", outputFile, errno);
                ok = false;
            }
        }

        if (newFilename)
            delete [] newFilename;
    }

    // generate the listing
    if (req.fListing) {
        if (!GenerateListing(image, req.fListFile, compiled && req.fSourceListing, req.fGenLASM))
            ok = false;
    }

    // reset the compiler after generating the listing so that the Compiler's
    // buffers will be available for inserting source code into the listing
    if (compiled)
        Compiler::Get()->Reset();

    if (req.fDownload) {
        result = Download(image);
    }

    // Check for the case of a listing or output file failure but
    // download ok.  In this case an error code must still be returned
    // so that command line processing stops and main() indicates
    // the failure
    if (result==kRCX_OK && !ok)
        result = kQuietError;

    delete image;
    return result;
}


bool GenerateListing(RCX_Image *image, const char *fileName, bool includeSource, bool generateLASM)
{
    FILE *fp;

    if (fileName) {
        fp = fopen(fileName, "w");
        if (!fp) {
            fprintf(STDERR, "Error: could not open file \"%s\" (%d)\n", fileName, errno);
            return false;
        }
    } else {
        fp = stdout;
    }

    RCX_StdioPrinter dst(fp);
    image->Print(&dst, includeSource ? Compiler::Get() : 0, generateLASM);

    if (fp != stdout)
        fclose(fp);

    return true;
}

RCX_Result GetBatteryLevel()
{
    RCX_Result result;

    result = gLink.Open();
    if (RCX_ERROR(result)) return result;

    result = gLink.GetBatteryLevel();
    if (!RCX_ERROR(result)) {
        fprintf(STDERR, "Battery Level = %3.1fV\n", (double)result / 1000);
        int lowBatt = (gTargetType == kRCX_SpyboticsTarget) ? kLow45Battery : kLowBattery;
        if (result < lowBatt)
            fprintf(STDERR, "Warning: battery level is low.\n");

        return kRCX_OK;
    }
    else {
        fprintf(STDERR, "Error: Could not fetch battery status.\n");
        return result;
    }
}

RCX_Result Download(RCX_Image *image)
{
    RCX_Result result;
    RCX_Cmd cmd;

    fprintf(STDERR, "Sending program:");

    result = gLink.Open();
    if (result != kRCX_OK) goto ErrorReturn;

    result = image->Download(&gLink);
    fputc('\n', STDERR);
    if (result != kRCX_OK) goto ErrorReturn;

    fprintf(STDERR, "Ok\n");

    return kRCX_OK;

ErrorReturn:
    fprintf(STDERR, "Error: program transfer failed (%d)\n", result);
    return result;
}


RCX_Image *Compile(const char *sourceFile, int flags)
{
    Buffer *mainBuf;

    if (sourceFile) {
        FILE *fp  = fopen(sourceFile, "rb");
        if (!fp) {
            fprintf(gErrorStream, "Error: could not open file \"%s\" (%d)\n", sourceFile, errno);
            return nil;
        }

        mainBuf = new Buffer();
        mainBuf->Create(sourceFile, fp);
        fclose(fp);
    }
    else {
        mainBuf = new Buffer();
        mainBuf->Create("<stdin>", stdin);
    }

#ifdef TEST_LEXER
    LexPush(mainBuf);

    int t;
    TokenVal v;
    LexLocation loc;

#if TEST_LEXER > 1
    while((t=gPreProc->Get(v)) != 0) {
#else
    while((t=yylex(v)) != 0) {
#endif
        LexCurrentLocation(loc);
        printf("%3d (%2d) : ", loc.fOffset, loc.fLength);
        PrintToken(t, v);
    }

    LexCurrentLocation(loc);
    printf("%3d (%2d) : EOF\n", loc.fOffset, loc.fLength);

    return 0;
#else
    Buffer *b = new Buffer();
    b->Create(mainBuf->GetName(), mainBuf->GetData(), mainBuf->GetLength());

    return Compiler::Get()->Compile(mainBuf, getTarget(gTargetType), flags);
#endif
}


RCX_Result UploadDatalog(bool verbose)
{
    RCX_Log log;
    RCX_Result result;
    int i;

    fprintf(STDERR, "Fetching Datalog");

    result = gLink.Open();
    if (RCX_ERROR(result)) return result;

    result = log.Upload(&gLink);
    if (RCX_ERROR(result)) return result;

    fputc('\n', STDERR);

    for (i=0; i<log.GetLength(); i++) {
        char line[256];
        log.SPrintEntry(line, i, verbose);
        printf("%s\n", line);
    }

    return kRCX_OK;
}


RCX_Result ClearMemory()
{
    RCX_Result result;
    RCX_Cmd cmd;

    // halt all tasks
    result = gLink.Send(cmd.Set(kRCX_StopAllOp));
    if (RCX_ERROR(result)) return result;

    for (UByte p=0; p<5; p++) {
        // select and delete program
        result = gLink.Send(cmd.Set(kRCX_SelectProgramOp, p));
        if (RCX_ERROR(result)) return result;

        result = gLink.Send(cmd.MakeDeleteSubs());
        if (RCX_ERROR(result)) return result;

        result = gLink.Send(cmd.MakeDeleteTasks());
        if (RCX_ERROR(result)) return result;
    }

    result = gLink.Send(cmd.Set(kRCX_SetDatalogOp, 0, 0));
    return result;
}

RCX_Result GetVersion()
{
    RCX_Result result;
    ULong rom, ram;

    result = gLink.Open();
    if (RCX_ERROR(result)) return result;

    result = gLink.GetVersion(rom, ram);
    if (RCX_ERROR(result)) return result;

    fprintf(STDERR, "Current Version: %08lx/%08lx\n", rom, ram);

    return result;  
}

RCX_Result DownloadFirmware(const char *filename, bool fast)
{
    FILE *fp;
    SRecord srec;
    bool ok;
    RCX_Result result;

    fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(STDERR, "Error: could not open file \'%s\'\n", filename);
        return kQuietError;
    }

    ok = srec.Read(fp, kMaxFirmware);
    fclose(fp);

    if (!ok) {
        fprintf(STDERR, "Error: \'%s\' is not a valid S-Record file\n", filename);
        return kQuietError;
    }

    fprintf(STDERR, "Sending firmware:");

    result = gLink.Open();
    if (RCX_ERROR(result)) goto ErrorReturn;

    result = gLink.DownloadFirmware(srec.GetData(), srec.GetLength(), srec.GetStart(), fast);
    fputc('\n', STDERR);
    if (RCX_ERROR(result)) goto ErrorReturn;

    fprintf(STDERR, "Ok\n");
    return kRCX_OK;

ErrorReturn:
    fprintf(STDERR, "Error: firmware transfer failed (%d)\n", result);
    return result;
}


RCX_Result SendRawCommand(const char *text, bool retry)
{
    int length = (int)strlen(text);
    RCX_Cmd cmd;
    RCX_Result result;

    // we need an even number of chars in text
    if (length & 1) return kUsageError;

    // determine actual length of command
    length /= 2;
    cmd.SetLength(length);

    for (int i=0; i<length; i++) {
        int byte = SRecord::ReadHexByte(text);
        if (byte == -1) return kUsageError;
        cmd[i] = (UByte) byte;
        text+=2;
    }

    result = gLink.Send(&cmd, retry);

    if (result > 0) {
        for (int i=0; i<result; i++)
            printf("%02x ", gLink.GetReplyByte(i));
        printf("\n");
    }

    return result;
}


RCX_Result SendRemote(const char *event, int repeat)
{
    RCX_Cmd cmd;
    int low, high;

    if (repeat == 0) return kUsageError;

    if (strlen(event) != 4)
        return kUsageError;

    high = SRecord::ReadHexByte(event);
    low = SRecord::ReadHexByte(event+2);
    if (high==-1 || low==-1) return kUsageError;


    cmd.Set(kRCX_Remote, low, high);

    while(repeat--) {
        gLink.Send(&cmd, false);
    }

    return kRCX_OK;
}


char *CreateFilename(const char *source, const char *oldExt, const char *newExt)
{
    char *filename;
    size_t n = strlen(source);
    size_t newExtLen = strlen(newExt);

    if (CheckExtension(source, oldExt))
        n -= strlen(oldExt);

    filename = new char[n + newExtLen + 1];
    memcpy(filename, source, n);
    strcpy(filename + n, newExt);

    return filename;
}


int CheckExtension(const char *s1, const char *ext)
{
    size_t slen = strlen(s1);
    size_t plen = strlen(ext);
    unsigned i;

    if (slen < plen) return 0;

    for (i=0; i<plen; i++) {
        if (tolower(s1[slen-plen+i]) != tolower(ext[i])) {
            return 0;
        }
    }

    return 1;
}


const char *LeafName(const char *filename)
{
    const char *ptr;

    // start at end and work towards beginning
    ptr = filename + strlen(filename) - 1;

    // test each character (including first one)
    while(ptr >= filename) {
        // check for delimiter
        if (*ptr == DIR_DELIMITER) break;
#ifdef WIN32
        // extra check for ':' in paths for Windows
        if (*ptr == ':') break;
#endif
        --ptr;
    }

    // we either stopped at the char before the first, or at
    // the delimiter - either way return pointer to next char
    return ptr+1;
}


RCX_Result SetErrorFile(const char *filename)
{
    FILE *fp;

    if (*filename==0) {
        gErrorStream = stdout;
        return kRCX_OK;
    }

    fp = fopen(filename, "w");
    if (!fp) {
        fprintf(STDERR, "Error: could not open error file \'%s\'\n", filename);
        return kQuietError;
    }

    gErrorStream = fp;
    return kRCX_OK;
}


RCX_Result RedirectOutput(const char *filename)
{
    if (*filename==0) {
        fprintf(STDERR, "Error: -R requires a filename\n");
        return kQuietError;
    }

    if (!freopen(filename, "w", stdout)) {
        fprintf(STDERR, "Error: could not open output file \'%s\'\n", filename);
        return kQuietError;
    }

    return kRCX_OK;
}


void DefineMacro(const char *text)
{
    const char *body;
    body = strchr(text, '=');
    if (body) {
        // create a copy of the symbol name
        int length = body - text;
        char *name = new char[length+1];
        memcpy(name, text, (size_t)length);
        name[length] = 0;

        Compiler::Get()->Define(name, body+1);

        delete [] name;
    }
    else {
        Compiler::Get()->Define(text);
    }
}


int GetActionCode(const char *arg)
{
    for (int i=0; i< (int)(sizeof(sActionNames)/sizeof(const char *)); ++i)
        if (strcmp(sActionNames[i], arg)==0) return i+kFirstActionCode;

    return 0;
}


void PrintError(RCX_Result error, const char *filename)
{
    const char *targetName = getTarget(gTargetType)->fName;
    if (!filename) filename = "?";

    if (error >= 0) return;

    switch(error) {
        case kRCX_RequestError:
            fprintf(STDERR, "Request error\n");
            break;
        case kRCX_OpenSerialError:
            fprintf(STDERR, "Could not open serial port or USB device\n");
            break;
        case kRCX_IREchoError:
            fprintf(STDERR, "Problem talking to IR device\n");
            break;
        case kRCX_ReplyError:
            if (gLink.WasErrorFromMissingFirmware()) {
                fprintf(STDERR, "No firmware installed on %s\n", targetName);
            }
            else {
                fprintf(STDERR, "No reply from %s\n", targetName);
            }
            break;
        case kRCX_MemFullError:
            fprintf(STDERR, "Not enough free memory in %s to download program\n", targetName);
            break;
        case kRCX_FileError:
            fprintf(STDERR, "Could not access file \'%s\'\n", filename);
            break;
        case kRCX_FormatError:
            fprintf(STDERR, "File \'%s\' is not a valid RCX image\n", filename);
            break;
        case kUsageError:
            PrintVersion();
            fprintf(STDERR, "Usage error: try \'nqc -help\' to display options\n");
            break;
        case kQuietError:
            break;
        case kRCX_PipeModeError:
            fprintf(STDERR, "USB driver does not support -firmfast, CyberMaster, or Spybotics\n");
            break;
        case kRCX_USBUnsupportedError:
            fprintf(STDERR, "USB Tower not supported\n");
            break;
        case kRCX_GhostNotFoundError:
            fprintf(STDERR," Ghost libraries are not installed properly\n");
            break;
        default:
            fprintf(STDERR, "Error #%d\n", -error);
            break;
    }
}

void PrintVersion()
{
    fprintf(STDERR,"nqc version %s (built %s, %s)\n",
        VERSION_STRING, __DATE__, __TIME__);
    fprintf(STDERR,"     Copyright (C) 2005 John Hansen.  All Rights Reserved.\n");
}

void PrintUsage()
{
    const char *targetName = getTarget(gTargetType)->fName;

    PrintVersion();
    fprintf(stdout,"Usage: nqc [options] [actions] [ - | filename ] [actions]\n");
    fprintf(stdout,"   - : read from stdin instead of a source_file\n");
    fprintf(stdout,"Options:\n");
    fprintf(stdout,"   -T<target>: target is one of:");
    for (unsigned i=0; i < sizeof(sTargetNames) / sizeof(const char *); ++i) {
        fprintf(stdout, " %s", sTargetNames[i]);
    }
    fprintf(stdout, " (target=%s)\n", targetName);
    fprintf(stdout,"   -u: send program to \%s\n", targetName);
    fprintf(stdout,"   -n: prevent the API header file from being included\n");
    fprintf(stdout,"   -D<sym>[=<value>] : define macro <sym>\n");
    fprintf(stdout,"   -U<sym>: undefine macro <sym>\n");
    fprintf(stdout,"   -E[<filename>] : write compiler errors to <filename> (or stdout)\n");
    fprintf(stdout,"   -R<filename> : redirect text output (datalog, etc) to <filename>\n");
    fprintf(stdout,"   -I<path>: search <path> for include files\n");
    fprintf(stdout,"   -L[<filename>] : generate code listing to <filename> (or stdout)\n");
    fprintf(stdout,"   -s: include source code in listings if possible\n");
    fprintf(stdout,"   -c: generate LASM compatible listings\n");
    fprintf(stdout,"   -v: verbose\n");
    fprintf(stdout,"   -q: quiet; suppress action sounds\n");
    fprintf(stdout,"   -x: omit packet header (RCX, RCX2 targets only)\n");
    fprintf(stdout,"   -f<size>: set firmware chunk size in bytes\n");
    fprintf(stdout,"   -w<ms>: set the download wait timeout in milliseconds\n");
    fprintf(stdout,"   -O<outfile>: specify output file\n");
    fprintf(stdout,"   -S<portname>: specify tower serial port\n");
    fprintf(stdout,"   -b: treat input file as a binary file (don't compile it)\n");
    fprintf(stdout,"   -1: use NQC API 1.x compatibility mode\n");
    fprintf(stdout,"Actions:\n");
    fprintf(stdout,"   -run: run current program\n");
    fprintf(stdout,"   -pgm <number>: select program number\n");
    fprintf(stdout,"   -datalog | -datalog_full: fetch datalog from %s\n", targetName);
    fprintf(stdout,"   -near | -far: set IR tower to near or far mode\n");
    fprintf(stdout,"   -watch <hhmm> | now: set %s clock to <hhmm> or system time\n", targetName);
    fprintf(stdout,"   -firmware <filename>: send firmware to %s\n", targetName);
    fprintf(stdout,"   -firmfast <filename>: send firmware to %s at quad speed\n", targetName);
    fprintf(stdout,"   -getversion: report %s firmware version\n", targetName);
    fprintf(stdout,"   -batterylevel: report battery level in volts\n");
    fprintf(stdout,"   -sleep <timeout>: set %s sleep timeout in minutes\n", targetName);
    fprintf(stdout,"   -msg <number>: send IR message to %s\n", targetName);
    fprintf(stdout,"   -raw <data>: format data as a packet and send to %s\n", targetName);
    fprintf(stdout,"   -remote <value> <repeat>: invoke a remote command on the %s\n", targetName);
    fprintf(stdout,"   -clear: erase all programs and datalog on %s\n", targetName);
    fprintf(stdout,"   -api: dump the standard API header file to stdout\n");
    fprintf(stdout,"   -help: display command line options\n");
}


RCX_Result AutoLink::Open()
{
    RCX_Result result;

    if (!fOpen) {
        ULong options = gTimeout & RCX_Link::kRxTimeoutMask;
        if (gVerbose) options |= RCX_Link::kVerboseMode;

        result = RCX_Link::Open(gTargetType, fSerialPort, options);
        if (RCX_ERROR(result)) return result;

        fOpen = true;
    }
    return kRCX_OK;
}


void AutoLink::Close()
{
    if (fOpen) {
        RCX_Link::Close();
        fOpen = false;
    }
}


RCX_Result AutoLink::Send(const RCX_Cmd *cmd, bool retry)
{
    RCX_Result result;

    result = Open();
    if (RCX_ERROR(result)) return result;

    if (retry) {
        result = Sync();
        if (RCX_ERROR(result)) return result;
    }

    result = RCX_Link::Send(cmd, retry);

    return retry ? result : kRCX_OK;
}


bool AutoLink::DownloadProgress(int /* soFar */, int /* total */)
{
    fputc('.', STDERR);
    fflush(STDERR);
    return true;
}


void MyCompiler::AddError(const Error &e, const LexLocation *loc)
{
    // check the error count
    if (!e.IsWarning())
    {
        int errorCount = ErrorHandler::Get()->GetErrorCount();
        // only print the first few errors
        if (errorCount > kMaxPrintedErrors) {
            if (errorCount == kMaxPrintedErrors+1)
                fprintf(gErrorStream, "Too many errors - only first %d reported\n", kMaxPrintedErrors);
            return;
        }
    }

    char msg[Error::kMaxErrorMsg];
    e.SPrint(msg);

    const char *errorType = e.IsWarning() ? "Warning" : "Error";
    fprintf(gErrorStream, "# %s: %s\n", errorType, msg);

    if (loc && loc->fIndex!=kIllegalSrcIndex) {
        Buffer *b = Compiler::Get()->GetBuffer(loc->fIndex);
        int lineStart = loc->fOffset;
        int line = b->FindLine(lineStart);
        const char *ptr;
        int i;

        // print file/line info
        fprintf(gErrorStream, "File \"%s\" ; line %d\n", b->GetName(), line);

        // print the code line
        fprintf(gErrorStream, "# ");
        for (ptr=b->GetData() + lineStart; *ptr != '\n'; ++ptr) {
            putc((*ptr == '\t') ? ' ' : *ptr, gErrorStream);
        }

        // mark the token
        fprintf(gErrorStream,"\n# ");
        for (i=0; i<(loc->fOffset-lineStart); i++)
            putc(' ', gErrorStream);
        for (i=0; i<loc->fLength; i++)
            putc('^', gErrorStream);
        fprintf(gErrorStream,"\n");
    }

    fprintf(gErrorStream, "#----------------------------------------------------------\n");

    // this is a hack for Windows!
    fflush(gErrorStream);
}


Buffer *MyCompiler::CreateBuffer(const char *name)
{
    static char pathname[DirList::kMaxPathname];

    if (!fDirs.Find(name, pathname))
        return nil;

    Buffer *buf = new Buffer();

    if (buf->Create(name, pathname)) {
        return buf;
    }
    else {
        delete buf;
        return 0;
    }
}


#ifdef TEST_LEXER
void PrintToken(int t, TokenVal v)
{
    printf("%3d / ", t);
    if (t < 256) {
        printf("%c", t);
    }
    else {
        switch(t) {
            case ID:
                printf("%s",v.fSymbol->GetKey());
                break;
            case NUMBER:
                printf("%d", v.fInt);
                break;
            case IF:
                printf("if");
                break;
            case STRING:
                printf("%s", v.fString);
                break;
            default:
                printf("0x%08x", v.fInt);
                break;
        }
    }

    printf("\n");
}
#endif


bool SameString(const char *s1, const char *s2)
{
    char c1, c2;

    while(true) {
        c1 = *s1++;
        c2 = *s2++;

        if (c1==0 && c2==0) return true;

        if (tolower(c1) != tolower(c2)) return false;
    }
}
