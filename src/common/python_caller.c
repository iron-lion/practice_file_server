#include "python_caller.h"

int
call_python(char *argv[])
{
    PyObject *pName, *pModule, *pDict, *pFunc;
    PyObject *pArgs, *pValue;
    int i;
    FILE* exp_file;
    char* addr = "../src/common/meta_filelist.py";

    Py_Initialize();
    exp_file = fopen(addr, "r");
    PyRun_SimpleFile(exp_file, addr);
    pModule = PyImport_AddModule("__main__");
    pDict = PyModule_GetDict(pModule);
    pFunc = PyDict_GetItemString(pDict, argv[0]);

    pArgs = PyTuple_New(2);
    PyTuple_SetItem(pArgs, 0, PyString_FromString(argv[1]));
    PyTuple_SetItem(pArgs, 1, PyString_FromString(argv[2]));

    PyObject_CallObject(pFunc, pArgs);
}
