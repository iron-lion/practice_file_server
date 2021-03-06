#include <Python.h>
#include "python_caller.h"
char*
call_python(int argc, char *argv[])
{
    PyObject *pName, *pModule, *pDict, *pFunc;
    PyObject *pArgs, *pValue,  *pResult;
    int i;
    FILE* exp_file;
    char* addr = "../src/common/meta_filelist.py";
    char* result;

    Py_Initialize();
    exp_file = fopen(addr, "r");
    PyRun_SimpleFile(exp_file, addr);
    pModule = PyImport_AddModule("__main__");
    pDict = PyModule_GetDict(pModule);
    pFunc = PyDict_GetItemString(pDict, argv[0]);

    pArgs = PyTuple_New(argc);
    for(i=0;i<argc;i++)
        PyTuple_SetItem(pArgs, i, PyString_FromString(argv[i+1]));
    pResult = PyObject_CallObject(pFunc, pArgs);

    if( pResult == NULL ){
        return NULL;
    }
    result = PyString_AsString(pResult);
    Py_INCREF(pResult);
    fclose(exp_file);
    return result;
}
