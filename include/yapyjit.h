#pragma once
#include <memory>
#include <map>
#include <vector>
#include <Python.h>
#include <mpyo.h>
#include <pyast.h>

static_assert(sizeof(Py_ssize_t) == 8, "Only 64 bit machines are supported");

namespace yapyjit {
    extern MIRContext mir_ctx;
	extern std::unique_ptr<AST> ast_py2native(ManagedPyo ast);
	extern MIR_item_t generate_mir(Function& func);
    extern int initialize_wf(PyObject* m);

	inline ManagedPyo get_py_ast(PyObject* pyfunc) {
        auto locals = ManagedPyo(PyDict_New());
        PyDict_SetItemString(locals.borrow(), "a", pyfunc);

        auto pyast = PyRun_String(
            "__import__('ast').parse(__import__('textwrap').dedent(__import__('inspect').getsource(a))).body[0]",
            Py_eval_input,
            locals.borrow(),
            locals.borrow()
        );
        if (!pyast) throw registered_pyexc();
        return pyast;
	}

    inline std::unique_ptr<yapyjit::Function> get_ir(ManagedPyo pyast, ManagedPyo pyfunc) {
        auto ast = yapyjit::ast_py2native(pyast);
        // lifetime - pyast no longer available
        auto funcast = dynamic_cast<yapyjit::FuncDef*>(ast.get());
        if (!funcast) {
            throw std::runtime_error("BUG: AST root is not function definition.");
        }

        return funcast->emit_ir_f(pyfunc);
    }
}

#define N_TYPE_TRACE_ENTRY 5
typedef struct {
    PyTypeObject* types[N_TYPE_TRACE_ENTRY];
    int idx;  // round-robin
} TypeTraceEntry;

inline void update_type_trace_entry(TypeTraceEntry* entry, PyTypeObject* ty) {
    entry->types[entry->idx++] = ty;
    if (entry->idx >= N_TYPE_TRACE_ENTRY)
        entry->idx = 0;
}

typedef struct {
    PyObject_HEAD
    PyObject* wrapped;
    std::unique_ptr<yapyjit::Function> compiled;
    MIR_item_t generated;
    std::map<std::string, int>* argid_lookup;
    std::vector<PyObject*>* defaults;
    std::vector<PyObject*>* call_args_fill;
    std::vector<TypeTraceEntry>* call_args_type_traces;
    vectorcallfunc callable_impl;
    PyObject* extra_attrdict;
    int call_count;
} WrappedFunctionObject;

extern PyTypeObject wf_type;
