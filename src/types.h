#pragma once

extern "C" {
#include "switch/runtime/diag.h"
#include "switch/types.h"
}

#include <cstddef>
#include <cstdint>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t size;
typedef uintptr_t paddr;

#define ALWAYSINLINE __attribute__((always_inline)) inline
#define UNROLL __attribute__((optimize("unroll-loops")))
#define NOINLINE __attribute__((noinline))
#define ALIGN_PTR(ptr, alignment) \
    (((ptr) + ((alignment) - 1)) & ~((alignment) - 1))

#define R_TRY(RC)         \
    {                     \
        Result rc = RC(); \
        if (R_FAILED(rc)) \
            return rc;    \
    }
#define R_ABORT_UNLESS(RC)           \
    {                                \
        Result rc = RC;              \
        if (R_FAILED(rc))            \
            diagAbortWithResult(rc); \
    }

namespace nxdb {

    namespace svc {

        enum ProcessExitReason : u32
        {
            ProcessExitReason_ExitProcess = 0,
            ProcessExitReason_TerminateProcess = 1,
            ProcessExitReason_Exception = 2,
        };
        enum ThreadExitReason : u32
        {
            ThreadExitReason_ExitThread = 0,
            ThreadExitReason_TerminateThread = 1,
            ThreadExitReason_ExitProcess = 2,
            ThreadExitReason_TerminateProcess = 3,
        };
        enum BreakPointType : u32
        {
            BreakPointType_HardwareInstruction = 0,
            BreakPointType_HardwareData = 1,
        };
        enum BreakReason : u32
        {
            BreakReason_Panic = 0,
            BreakReason_Assert = 1,
            BreakReason_User = 2,
            BreakReason_PreLoadDll = 3,
            BreakReason_PostLoadDll = 4,
            BreakReason_PreUnloadDll = 5,
            BreakReason_PostUnloadDll = 6,
            BreakReason_CppException = 7,

            BreakReason_NotificationOnlyFlag = 0x80000000,
        };
        enum DebugException : u32
        {
            DebugException_UndefinedInstruction = 0,
            DebugException_InstructionAbort = 1,
            DebugException_DataAbort = 2,
            DebugException_AlignmentFault = 3,
            DebugException_DebuggerAttached = 4,
            DebugException_BreakPoint = 5,
            DebugException_UserBreak = 6,
            DebugException_DebuggerBreak = 7,
            DebugException_UndefinedSystemCall = 8,
            DebugException_MemorySystemError = 9,
        };
        enum DebugEvent : u32
        {
            DebugEvent_CreateProcess = 0,
            DebugEvent_CreateThread = 1,
            DebugEvent_ExitProcess = 2,
            DebugEvent_ExitThread = 3,
            DebugEvent_Exception = 4,
        };
        enum ContinueFlag : u32
        {
            ContinueFlag_ExceptionHandled = (1u << 0),
            ContinueFlag_EnableExceptionEvent = (1u << 1),
            ContinueFlag_ContinueAll = (1u << 2),
            ContinueFlag_ContinueOthers = (1u << 3),

            ContinueFlag_AllMask = (1u << 4) - 1,
        };
        struct DebugInfoCreateProcess {
            u64 program_id;
            u64 process_id;
            char name[0xC];
            u32 flags;
            u64 user_exception_context_address; /* 5.0.0+ */
        };

        struct DebugInfoCreateThread {
            u64 thread_id;
            u64 tls_address;
            /* Removed in 11.0.0 u64 entrypoint; */
        };

        struct DebugInfoExitProcess {
            ProcessExitReason reason;
        };

        struct DebugInfoExitThread {
            ThreadExitReason reason;
        };

        struct DebugInfoUndefinedInstructionException {
            u32 insn;
        };

        struct DebugInfoDataAbortException {
            u64 address;
        };

        struct DebugInfoAlignmentFaultException {
            u64 address;
        };

        struct DebugInfoBreakPointException {
            BreakPointType type;
            u64 address;
        };

        struct DebugInfoUserBreakException {
            BreakReason break_reason;
            u64 address;
            u64 size;
        };

        struct DebugInfoDebuggerBreakException {
            u64 active_thread_ids[4];
        };

        struct DebugInfoUndefinedSystemCallException {
            u32 id;
        };

        union DebugInfoSpecificException {
            DebugInfoUndefinedInstructionException undefined_instruction;
            DebugInfoDataAbortException data_abort;
            DebugInfoAlignmentFaultException alignment_fault;
            DebugInfoBreakPointException break_point;
            DebugInfoUserBreakException user_break;
            DebugInfoDebuggerBreakException debugger_break;
            DebugInfoUndefinedSystemCallException undefined_system_call;
            u64 raw;
        };

        struct DebugInfoException {
            DebugException type;
            u64 address;
            DebugInfoSpecificException specific;
        };

        union DebugInfo {
            DebugInfoCreateProcess create_process;
            DebugInfoCreateThread create_thread;
            DebugInfoExitProcess exit_process;
            DebugInfoExitThread exit_thread;
            DebugInfoException exception;
        };

        struct DebugEventInfo {
            DebugEvent type;
            u32 flags;
            u64 thread_id;
            DebugInfo info;
        };

        enum MemoryState : u32
        {
            MemoryState_Free = 0x00,
            MemoryState_Io = 0x01,
            MemoryState_Static = 0x02,
            MemoryState_Code = 0x03,
            MemoryState_CodeData = 0x04,
            MemoryState_Normal = 0x05,
            MemoryState_Shared = 0x06,
            MemoryState_Alias = 0x07,
            MemoryState_AliasCode = 0x08,
            MemoryState_AliasCodeData = 0x09,
            MemoryState_Ipc = 0x0A,
            MemoryState_Stack = 0x0B,
            MemoryState_ThreadLocal = 0x0C,
            MemoryState_Transfered = 0x0D,
            MemoryState_SharedTransfered = 0x0E,
            MemoryState_SharedCode = 0x0F,
            MemoryState_Inaccessible = 0x10,
            MemoryState_NonSecureIpc = 0x11,
            MemoryState_NonDeviceIpc = 0x12,
            MemoryState_Kernel = 0x13,
            MemoryState_GeneratedCode = 0x14,
            MemoryState_CodeOut = 0x15,
            MemoryState_Coverage = 0x16,
            MemoryState_Insecure = 0x17,
        };

        enum MemoryAttribute : u32
        {
            MemoryAttribute_Locked = (1 << 0),
            MemoryAttribute_IpcLocked = (1 << 1),
            MemoryAttribute_DeviceShared = (1 << 2),
            MemoryAttribute_Uncached = (1 << 3),
            MemoryAttribute_PermissionLocked = (1 << 4),
        };

        enum MemoryPermission : u32
        {
            MemoryPermission_None = (0 << 0),

            MemoryPermission_Read = (1 << 0),
            MemoryPermission_Write = (1 << 1),
            MemoryPermission_Execute = (1 << 2),

            MemoryPermission_ReadWrite = MemoryPermission_Read | MemoryPermission_Write,
            MemoryPermission_ReadExecute = MemoryPermission_Read | MemoryPermission_Execute,

            MemoryPermission_DontCare = (1 << 28), /* For SharedMemory */
        };

        struct MemoryInfo {
            u64 base_address;
            u64 size;
            MemoryState state;
            MemoryAttribute attribute;
            MemoryPermission permission;
            u32 ipc_count;
            u32 device_count;
            u32 padding;
        };

        struct PageInfo {
            u32 flags;
        };

        enum HardwareBreakPointRegisterName : u32
        {
            HardwareBreakPointRegisterName_I0 = 0,
            HardwareBreakPointRegisterName_I1 = 1,
            HardwareBreakPointRegisterName_I2 = 2,
            HardwareBreakPointRegisterName_I3 = 3,
            HardwareBreakPointRegisterName_I4 = 4,
            HardwareBreakPointRegisterName_I5 = 5,
            HardwareBreakPointRegisterName_I6 = 6,
            HardwareBreakPointRegisterName_I7 = 7,
            HardwareBreakPointRegisterName_I8 = 8,
            HardwareBreakPointRegisterName_I9 = 9,
            HardwareBreakPointRegisterName_I10 = 10,
            HardwareBreakPointRegisterName_I11 = 11,
            HardwareBreakPointRegisterName_I12 = 12,
            HardwareBreakPointRegisterName_I13 = 13,
            HardwareBreakPointRegisterName_I14 = 14,
            HardwareBreakPointRegisterName_I15 = 15,
            HardwareBreakPointRegisterName_D0 = 16,
            HardwareBreakPointRegisterName_D1 = 17,
            HardwareBreakPointRegisterName_D2 = 18,
            HardwareBreakPointRegisterName_D3 = 19,
            HardwareBreakPointRegisterName_D4 = 20,
            HardwareBreakPointRegisterName_D5 = 21,
            HardwareBreakPointRegisterName_D6 = 22,
            HardwareBreakPointRegisterName_D7 = 23,
            HardwareBreakPointRegisterName_D8 = 24,
            HardwareBreakPointRegisterName_D9 = 25,
            HardwareBreakPointRegisterName_D10 = 26,
            HardwareBreakPointRegisterName_D11 = 27,
            HardwareBreakPointRegisterName_D12 = 28,
            HardwareBreakPointRegisterName_D13 = 29,
            HardwareBreakPointRegisterName_D14 = 30,
            HardwareBreakPointRegisterName_D15 = 31,
        };

        struct DBGBCRn_EL1 {
            union {
                struct {
                    int res0 : 8;
                    int bt : 4;
                    int lbn : 4;
                    int ssc : 2;
                    bool hmc : 1;
                    int res1 : 4;
                    int bas : 4;
                    int res2 : 2;
                    int pmc : 2;
                    bool e : 1;
                };
                u32 value;
            };
        };

        enum class PseudoHandle : Handle
        {
            CurrentThread = 0xFFFF8000,
            CurrentProcess = 0xFFFF8001,
        };

    } // namespace svc

} // namespace nxdb

inline u64 constexpr operator""_ns(unsigned long long ns) { return ns; }
inline u64 constexpr operator""_us(unsigned long long µs) { return µs * 1000; }
inline u64 constexpr operator""_ms(unsigned long long ms) { return ms * 1000000; }
inline u64 constexpr operator""_s(unsigned long long s) { return s * 1000000000; }
inline u64 constexpr operator""_B(unsigned long long b) { return b; }
inline u64 constexpr operator""_KB(unsigned long long kb) { return kb * 1024; }
inline u64 constexpr operator""_MB(unsigned long long mb) { return mb * 1024_KB; }
inline u64 constexpr operator""_GB(unsigned long long gb) { return gb * 1024_MB; }
