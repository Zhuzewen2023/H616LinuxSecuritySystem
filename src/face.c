#include "face.h"

void face_init()
{
    Py_Initialize();
    PyObject* sys = PyImport_ImportModule("sys");
    PyObject* path = PyObject_GetAttrString(sys, "path");
    PyList_Append(path, PyUnicode_FromString("."));
}

void face_final()
{
    Py_Finalize();
}

double face_score()
{
    double result = 0.0;
    system(WGET_CMD);
    if(-1 == access(FACE_FILE, F_OK))
    {
        printf("%s|%s|%d: access failed\n", __FILE__, __func__, __LINE__);
        return result;
    }
    printf("%s|%s|%d: access success\n", __FILE__, __func__, __LINE__);
    PyObject* pModule = PyImport_ImportModule("face");
    if(!pModule)
    {
        PyErr_Print();
        printf("%s|%s|%d: ImportModule failed\n", __FILE__, __func__, __LINE__);
        printf("Error: failed to load face.py\n");
        goto FAILED_MODULE;
    }
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);

    PyObject* pFunc = PyObject_GetAttrString(pModule, "alibaba_face");
    if(!pFunc)
    {
        PyErr_Print();
        printf("Error: failed to load alibaba_face\n");
        goto FAILED_FUNC;
    }
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    PyObject* pValue = PyObject_CallObject(pFunc, NULL);
    if(!pValue)
    {
        PyErr_Print();
        printf("Error:function call failed\n");
        goto FAILED_VALUE;
    }
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    if(!PyArg_Parse(pValue, "d", &result))
    {
        PyErr_Print();
        printf("Error:parse failed\n");
        goto FAILED_RESULT;
    }

FAILED_RESULT:
    Py_DECREF(pValue);
FAILED_VALUE:
    Py_DECREF(pFunc);
FAILED_FUNC:
    Py_DECREF(pModule);
FAILED_MODULE:
    return result;
}

#if 0
int main(int argc, char const *argv[])
{
    double face_result = 0;
    face_init();
    face_result = face_score();
    printf("face_reult from C main = %.2lf\n", face_result);
    face_final();
    return 0;
}

#endif
