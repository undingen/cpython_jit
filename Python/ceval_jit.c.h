// HACK: this file just gets included at the end of ceval.c so lives in the same translation unit...


#undef pcframe
struct JitHelperState {
    PyThreadState *tstate;
    _PyInterpreterFrame *frame;
    //unsigned char opcode;
    //unsigned int oparg;
    _Py_atomic_int * const eval_breaker;
    _PyCFrame *pcframe;
    PyObject *names;
    PyObject *consts;
    _Py_CODEUNIT *next_instr;
    PyObject **stack_pointer;
    PyObject *kwnames;
    //int throwflag;
    //binaryfunc binary_ops[];
};
typedef struct JitHelperState JitHelperState;


// TODO: generate this defines
#undef DISPATCH
#define DISPATCH() return (PyObject*)(unsigned long)0
#undef DISPATCH_SAME_OPARG
#define DISPATCH_SAME_OPARG() return (PyObject*)(unsigned long)1
#undef DISPATCH_INLINED
#define DISPATCH_INLINED(NEW_FRAME) return (PyObject*)(unsigned long)2
#define DISPATCH_CHECK_EVAL_BREAKER() return (PyObject*)(unsigned long)3

#undef GOTO_UNBOUND_LOCAL_ERROR
#define GOTO_UNBOUND_LOCAL_ERROR return (PyObject*)(unsigned long)4
#undef GOTO_EXCEPTION_UNWIND
#define GOTO_EXCEPTION_UNWIND return (PyObject*)(unsigned long)5
#undef GOTO_ERROR
#define GOTO_ERROR return (PyObject*)(unsigned long)6
#undef GOTO_POP_1_ERROR
#define GOTO_POP_1_ERROR return (PyObject*)(unsigned long)7
#undef GOTO_POP_2_ERROR
#define GOTO_POP_2_ERROR return (PyObject*)(unsigned long)8
#undef GOTO_POP_3_ERROR
#define GOTO_POP_3_ERROR return (PyObject*)(unsigned long)9
#undef GOTO_POP_4_ERROR
#define GOTO_POP_4_ERROR return (PyObject*)(unsigned long)10
#undef GOTO_RESUME_FRAME
#define GOTO_RESUME_FRAME return (PyObject*)(unsigned long)11
#undef GOTO_RESUME_WITH_ERROR
#define GOTO_RESUME_WITH_ERROR return (PyObject*)(unsigned long)12
#undef GOTO_HANDLE_EVAL_BREAKER
#define GOTO_HANDLE_EVAL_BREAKER return (PyObject*)(unsigned long)13
#undef GOTO_EXIT_UNWIND
#define GOTO_EXIT_UNWIND return (PyObject*)(unsigned long)14

#define names  ((state->names))
#define consts  ((state->consts))
#define stack_pointer  ((state->stack_pointer))
#define kwnames  ((state->kwnames))
#define next_instr  ((state->next_instr))
#define tstate  ((state->tstate))
#define frame  ((state->frame))
#define pcframe  ((state->pcframe))
#define eval_breaker ((state->eval_breaker))

#undef GO_TO_INSTRUCTION
#define GO_TO_INSTRUCTION(INSTNAME) return JIT_HELPER_##INSTNAME(state, oparg)

#undef DEOPT_IF
#define DEOPT_IF(COND, INSTNAME)                            \
    if ((COND)) {                                           \
        /* This is only a single jump on release builds! */ \
        UPDATE_MISS_STATS((INSTNAME));                      \
        assert(_PyOpcode_Deopt[opcode] == (INSTNAME));      \
        return JIT_HELPER_##INSTNAME(state, oparg);         \
    }

#include "generated_jit_helper.c.h"

#if 0
#define IS_16BIT_VAL(x) ((unsigned long)(x) <= UINT16_MAX)
#define IS_32BIT_VAL(x) ((unsigned long)(x) <= UINT32_MAX)
#define IS_32BIT_SIGNED_VAL(x) ((int32_t)(x) == (int64_t)(x))

static int can_use_relative_call(void *addr) {
    return IS_32BIT_VAL((long)addr);
}

static void emit_call_ext_func(Jit* Dst, void* addr) {
    if (can_use_relative_call(addr)) {
        // This emits a relative call. The dynasm syntax is confusing
        // it will not actually take the address of addr (even though it says &addr).
        // Put instead just take the value of addr and calculate the difference to the emitted instruction address. (generated code: dasm_put(Dst, 135, (ptrdiff_t)(addr)))

        // We emit a relative call to the destination function here which can be patched.
        // The address of the call is retrieved via __builtin_return_address(0) inside the AOT func and
        // then the destination of the call instruction (=relative address of the function to call) is modified.
        | call qword &addr // 5byte inst
    } else {
        | mov64 res, (unsigned long)addr
        | call res // compiles to: 0xff 0xd0
    }
}

// emits: $r_dst = val
static void emit_mov_imm(Jit* Dst, int r_idx, unsigned long val) {
    if (val == 0) {
        | xor Rd(r_idx), Rd(r_idx)
    } else if (IS_32BIT_VAL(val)) {
        | mov Rd(r_idx), (unsigned int)val
    } else {
        | mov64 Rq(r_idx), (unsigned long)val
    }
}

/*
HOW TO iterate over the instructions:
    _Py_CODEUNIT *instructions = _PyCode_CODE(frame->f_code);
    for (int i = 0; i < Py_SIZE(frame->f_code); i++) {
        int opcode = _PyOpcode_Deopt[_Py_OPCODE(instructions[i])];
        int caches = _PyOpcode_Caches[opcode];
        if (caches) {
            i += caches;
            continue;
        }

    }

*/

void jit_func() {
    _Py_CODEUNIT *instructions = _PyCode_CODE(frame->f_code);
    for (int i = 0; i < Py_SIZE(frame->f_code); i++) {
        int opcode = _PyOpcode_Deopt[_Py_OPCODE(instructions[i])];
        int caches = _PyOpcode_Caches[opcode];
        if (caches) {
            i += caches;
            continue;
        }

    }
}
#endif


