/* nr3python.h */
// version 0.4
// This file is a version of nr3.h with hooks that
// make it easy to interface to Python
// See http://www.nr.com/nr3_python_tutorial.html
#ifndef _NR3_H_
#define _NR3_H_
#ifndef Py_PYTHON_H
#include "Python.h"
#endif
#ifndef Py_ARRAYOBJECT_H
#include "numpy/arrayobject.h"
#endif

#define _CHECKBOUNDS_ 1
//#define _USESTDVECTOR_ 1
//#define _USENRERRORCLASS_ 1
#define _TURNONFPES_ 1

// all the system #include's we'll ever need
#include <fstream>
#include <cmath>
#include <complex>
#include <iostream>
#include <iomanip>
#include <vector>
#include <limits>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

using namespace std;

// NaN: uncomment one of the following 3 methods of defining a global NaN
// you can test by verifying that (NaN != NaN) is true

static const double NaN = numeric_limits<double>::quiet_NaN();

//Uint proto_nan[2]={0xffffffff, 0x7fffffff};
//double NaN = *( double* )proto_nan;

//Doub NaN = sqrt(-1.);

// Python glue Part I starts here (Part II at end of file)

PyObject* NRpyException(char *str, int die=1, int val=0) {
    PyErr_Format(PyExc_RuntimeError,str,val);
    PyErr_Print();
    if (die) Py_Exit(1);  // is there a way to avoid killing the interactive interpreter?
    return Py_None;
}

char NRpyMainName[] = "__main__";
PyObject* NRpyGetByName(char *name, char *dict = NULL) {
// get a PyObject from Python namespace (__main__ by default)
    if (dict == NULL) dict = NRpyMainName;
    PyObject* pymodule = PyImport_AddModule(dict);
    PyObject* dictobj = PyModule_GetDict(pymodule);
    PyObject* bb = PyDict_GetItemString(dictobj,name);
    if (! bb) NRpyException("Failed to fetch a Python object by name.");
    return bb;
}

struct NRpyArgs {
// make arguments from Python individually addressable by [] subscript
    PyObject* pyargs;
    NRpyArgs(PyObject* pyaargs) : pyargs(pyaargs) {}
    int size() {return PyTuple_Size(pyargs);}
    PyObject* operator[](int i) {
        if (i<PyTuple_Size(pyargs)) return PyTuple_GetItem(pyargs,i);
            // Returns a borrowed (unprotected) ref.  Args refs owned by calling routine.
        else NRpyException("Index out of range in NRpyArgs.");
        return Py_None;
    }
};

// explicitly construct scalars and strings from PyObjects or Python namespace

int NRpyIsNumber(PyObject* ob) {return (PyInt_Check(ob) || PyFloat_Check(ob));}

// type Int
int NRpyInt(PyObject* ob) {
    if (ob == Py_None) return 0;
    if (NRpyIsNumber(ob)) return PyInt_AsLong(ob); // casts ob to int
    else NRpyException("NRpyInt argument is not a number.");
    return 0;
}
int NRpyInt(char *name, char *dict = NULL) {
    return NRpyInt(NRpyGetByName(name,dict));
}

// type Doub
double NRpyDoub(PyObject* ob) {
    if (ob == Py_None) return NaN;
    else if (NRpyIsNumber(ob)) return PyFloat_AsDouble(ob); // casts ob to double
    else NRpyException("NRpyDoub argument is not a number.");
    return 0.;
}
double NRpyDoub(char *name, char *dict = NULL) {
    return NRpyDoub(NRpyGetByName(name,dict));
}

// type char* (string)
char* NRpyCharP(PyObject *ob) {
    if (PyString_Check(ob)) return PyString_AsString(ob);
    else NRpyException("NRpyCharP argument is not a string.");
    return NULL;
}
char* NRpyCharP(char *name, char *dict = NULL) {
    return NRpyCharP(NRpyGetByName(name,dict));
}

// type encapsulated function pointer (note different syntax so that templating can work)
template<class T> void NRpyCFunction(T* &fptr, PyObject* ob) {
    if (! PyCapsule_CheckExact(ob)) NRpyException("NRpyCFunction arg is not a C++ function capsule.");
    fptr = (T*)PyCapsule_GetPointer(ob,NULL);
    return;
}

// wrapper class for Python List, implementing only simple operations
struct NRpyList {
    PyObject* p;
    int n;
    int isnew;
    NRpyList(int nn) : p(PyList_New(nn)), n(nn), isnew(1) {
        for (int i=0;i<nn;i++) {
            Py_INCREF(Py_None); // needed?
            PyList_SetItem(p,i,Py_None);
        }
        if (! PyList_Check(p)) NRpyException("NRpyList not successfully created.");
    }
    NRpyList(PyObject *pp) : p(pp), isnew(0) {
        if (p == NULL) p = Py_None;
        n = (PyList_Check(p) ? PyList_Size(p) : 0);
    }
    int size() {return n;}
    template <class T> int set(int i, T val) {
        int flag = PyList_SetItem(p, i, NRpyObject(val));
        return flag;
    }
    NRpyList operator[](int i) {
        if (i >= n || i < 0) NRpyException("NRpyList subscript out of range.");
        return NRpyList(PyList_GetItem(p,i));
        // Returns a borrowed (unprotected) ref, but assumes List is bound by calling routine.
    }
};
int NRpyInt(NRpyList &list) { // cast list to integer value of its 1st element
    return NRpyInt(PyList_GetItem(list.p,0));
}
// ToDo: also make NRpyList constructors for NRvector and NRmatrix

// wrapper class for Python Dict
struct NRpyDict {
    PyObject* p;
    int isnew;
    NRpyDict() : p(PyDict_New()), isnew(1) {}
    NRpyDict(PyObject *pp) : p(pp), isnew(0) {
        if (! PyDict_Check(pp)) NRpyException("Argument not a dict in NRpyDict constructor.");
    }
    template <class T, class U> int set(T key, U val) {
        PyObject *thekey = NRpyObject(key), *theval = NRpyObject(val);
        int flag = PyDict_SetItem(p, thekey, theval);
        Py_DECREF(thekey); // because PyDict_SetItem INCREFs both key and val
        Py_DECREF(theval);
        return flag; // returns 0 for success, -1 for failure
    }
    template <class T> PyObject* get(const T key) {
        PyObject *thekey = NRpyObject(key), *theval;
        theval = PyDict_GetItem(p, thekey); // borrowed ref
        Py_DECREF(thekey);
        if (theval) return theval; // intended use is immediate conversion so borrowed ref is ok
        else return Py_None; // ditto, won't be decremented because won't be returned to Python
    }
};

// overloaded functions to turn anything into a PyObject (vector and matrix are in Part II below)

template<class T> PyObject* NRpyObject(T &a) {
// default applies to all function objects or other structs
    PyObject *thing = PyCapsule_New((void*)a,NULL,NULL);
    return thing;
}
PyObject* NRpyObject(const double a) {return PyFloat_FromDouble(a);}
PyObject* NRpyObject(const int a) {return PyInt_FromLong(a);}
PyObject* NRpyObject(const bool a) {return PyBool_FromLong(a);}
PyObject* NRpyObject(const char *a) {return PyString_FromString(a);} // string is copied
PyObject* NRpyObject() {return Py_BuildValue("");} // Python None
PyObject* NRpyObject(NRpyList &a) {
    if (! a.isnew) Py_INCREF(a.p); // make a new reference to return (should happen rarely)
    return a.p;
}
PyObject* NRpyObject(NRpyDict &a) {
    if (! a.isnew) Py_INCREF(a.p); // make a new reference to return (should happen rarely)
    return a.p;
}
// NRpyObjects are generally return values.  Hence they pass clear title to ownership.
// If you create an NRpyObject and don't return it, you need to Py_DECREF it later.

// send an object into Python namespace (except for scalars, will become shared ref)
template <class T>
void NRpySend(T &a, char *name, char *dict=NULL) {
    if (dict == NULL) dict = NRpyMainName;
    PyObject* pymodule = PyImport_AddModule(dict);
    PyObject* dictobj = PyModule_GetDict(pymodule);
    PyObject* aa = NRpyObject(a);
    int ret = PyDict_SetItemString(dictobj, name, aa);
    if (ret) NRpyException("Failed to share an NR object with Python.");
    Py_XDECREF(aa); // because dictobj now has the responsibility
}

// templated check of a PyObject's type (used in initpyvec and initpymat below)
template <class T> inline int NRpyTypeOK(PyObject *a) {return 0;}
template <> inline int NRpyTypeOK<double>(PyObject *a) {return PyArray_ISFLOAT(a);}
template <> inline int NRpyTypeOK<int>(PyObject *a) {return PyArray_ISINTEGER(a);}

// templated return a PyObject's type (used in NRpyObject on vector and matrix args)
template <class T> inline int NRpyDataType() {return PyArray_INT;}
template <> inline int NRpyDataType<double>() {return PyArray_DOUBLE;}
template <> inline int NRpyDataType<int>() {return PyArray_INT;}

// tempated cast a PyObject's type (used in NRpyPyFunction for return type)
template <class T>  T NRpyCast(PyObject *a) {return (T*)NULL; }
template <> double NRpyCast<double>(PyObject *a) {return NRpyDoub(a);}
template <> int NRpyCast<int>(PyObject *a) {return NRpyInt(a);}
template <> char* NRpyCast<char*>(PyObject *a) {return NRpyCharP(a);}

// end Python glue Part I  (see Part II at end of file)

// macro-like inline functions

template<class T>
inline T SQR(const T a) {return a*a;}

template<class T>
inline const T &MAX(const T &a, const T &b)
{return b > a ? (b) : (a);}

inline float MAX(const double &a, const float &b)
{return b > a ? (b) : float(a);}

inline float MAX(const float &a, const double &b)
{return b > a ? float(b) : (a);}

template<class T>
inline const T &MIN(const T &a, const T &b)
{return b < a ? (b) : (a);}

inline float MIN(const double &a, const float &b)
{return b < a ? (b) : float(a);}

inline float MIN(const float &a, const double &b)
{return b < a ? float(b) : (a);}

template<class T>
inline T SIGN(const T &a, const T &b)
{return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}

inline float SIGN(const float &a, const double &b)
{return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}

inline float SIGN(const double &a, const float &b)
{return (float)(b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a));}

template<class T>
inline void SWAP(T &a, T &b)
{T dum=a; a=b; b=dum;}

// exception handling

#ifndef _USENRERRORCLASS_
#define throw(message) \
{printf("ERROR: %s\n     in file %s at line %d\n", message,__FILE__,__LINE__); throw(1);}
#else
struct NRerror {
	char *message;
	char *file;
	int line;
	NRerror(char *m, char *f, int l) : message(m), file(f), line(l) {}
};
#define throw(message) throw(NRerror(message,__FILE__,__LINE__));
void NRcatch(NRerror err) {
	printf("ERROR: %s\n     in file %s at line %d\n",
		err.message, err.file, err.line);
	exit(1);
}
#endif

// usage example:
//
//	try {
//		somebadroutine();
//	}
//	catch(NRerror s) {NRcatch(s);}
//
// (You can of course substitute any other catch body for NRcatch(s).)


// Vector and Matrix Classes

#ifdef _USESTDVECTOR_
#define NRvector vector
#else

template <class T>
class NRvector {
private:
    int nn;	// size of array. upper index is nn-1
    T *v;
public:
    int ownsdata; // 1 for normal NRmatrix, 0 if Python owns the data
    PyObject *pyident; // if I don't own my data, who does?
    NRvector();
    explicit NRvector(int n);		// Zero-based array
    NRvector(PyObject *a);			// construct from Python array
    NRvector(char *name, char *dict = NULL); // construct from name in Python scope
    void initpyvec(PyObject *a); // helper function used by above
    NRvector(int n, const T &a);	//initialize to constant value
    NRvector(int n, const T *a);	// Initialize to array
    NRvector(const NRvector &rhs);	// Copy constructor
    NRvector & operator=(const NRvector &rhs);	//assignment
    typedef T value_type; // make T available externally
    inline T & operator[](const int i);	//i'th element
    inline const T & operator[](const int i) const;
    inline int size() const;
    void resize(int newn); // resize (contents not preserved)
    void assign(int newn, const T &a); // resize and assign a constant value
    void assign(char *name, char *dict = NULL); // assign to a name in Python scope
    ~NRvector();
};

// NRvector definitions

template <class T>
NRvector<T>::NRvector() : nn(0), v(NULL), ownsdata(1){}

template <class T>
NRvector<T>::NRvector(int n) : nn(n), ownsdata(1), v(n>0 ? (T*)PyMem_Malloc(n*sizeof(T)) : NULL) {}

template <class T>
void NRvector<T>::initpyvec(PyObject *a) {
    ownsdata = 0;
    pyident = a;
    if (! PyArray_CheckExact(a)) NRpyException("PyObject is not an Array in NRvector constructor.");
    if (! PyArray_ISCARRAY_RO(a)) NRpyException("Python Array must be contiguous (e.g., not strided).");
    if (! NRpyTypeOK<T>(a)) NRpyException("Python Array type does not agree with NRvector type.");
    Int i, ndim = PyArray_NDIM(a), *dims = (Int*)PyArray_DIMS(a);
    nn = 1;
    for (i=0;i<ndim;i++) nn *= dims[i];
    v = (nn>0 ? (T*)PyArray_DATA(a) : NULL);
}
template <class T> NRvector<T>::NRvector(PyObject *a) {
    initpyvec(a);
}
template <class T> NRvector<T>::NRvector(char *name, char *dict = NULL) {
    initpyvec(NRpyGetByName(name,dict));
}

template <class T>
NRvector<T>::NRvector(int n, const T& a) : nn(n), ownsdata(1), v(n>0 ? (T*)PyMem_Malloc(n*sizeof(T)) : NULL)
{
    for(int i=0; i<n; i++) v[i] = a;
}

template <class T>
NRvector<T>::NRvector(int n, const T *a) : nn(n), ownsdata(1), v(n>0 ? (T*)PyMem_Malloc(n*sizeof(T)) : NULL)
{
    for(int i=0; i<n; i++) v[i] = *a++;
}

template <class T>
NRvector<T>::NRvector(const NRvector<T> &rhs) : nn(rhs.nn), ownsdata(1),
                                                v(nn>0 ? (T*)PyMem_Malloc(nn*sizeof(T)) : NULL) {
    for(int i=0; i<nn; i++) v[i] = rhs[i];
}

template <class T>
NRvector<T> & NRvector<T>::operator=(const NRvector<T> &rhs) {
    if (this != &rhs) {
        if (nn != rhs.nn) {
            resize(rhs.nn);
            nn=rhs.nn;
        }
        for (int i=0; i<nn; i++)
            v[i]=rhs[i];
    }
    return *this;
}

template <class T>
inline T & NRvector<T>::operator[](const int i)	//subscripting
{
#ifdef _CHECKBOUNDS_
    if (i<0 || i>=nn) {
        throw("NRvector subscript out of bounds");
    }
#endif
    return v[i];
}

template <class T>
inline const T & NRvector<T>::operator[](const int i) const	//subscripting
{
#ifdef _CHECKBOUNDS_
    if (i<0 || i>=nn) {
        throw("NRvector subscript out of bounds");
    }
#endif
    return v[i];
}

template <class T>
inline int NRvector<T>::size() const
{
    return nn;
}

template <class T>
void NRvector<T>::resize(int newn) {
    if (newn != nn) {
        nn = newn;
        if (ownsdata) {
            if (v != NULL) PyMem_Free(v);
            v = nn > 0 ? (T*)PyMem_Malloc(nn*sizeof(T)) : NULL;
        } else { // Python
            int dm[1];
            dm[0] = newn;
            PyArray_Dims mydims;
            mydims.ptr = dm;
            mydims.len = 1;
            PyArray_Resize((PyArrayObject *)pyident, &mydims, 0, NPY_CORDER);
            // the return value is garbage, or maybe PyNone, contrary to Numpy docs
            // I think it's a Numpy bug, but following is correct
            v = nn>0 ? (T*)PyArray_DATA(pyident) : NULL;
        }
    }
}

template <class T>
void NRvector<T>::assign(int newn, const T& a) {
    resize(newn);
    for (int i=0;i<nn;i++) v[i] = a;
}

template <class T>
void NRvector<T>::assign(char *name, char *dict = NULL) {
    if (! ownsdata) NRpyException("Attempt to assign Python array to another Python array.");
    if (v != NULL) PyMem_Free(v);
    initpyvec(NRpyGetByName(name,dict));
}

template <class T>
NRvector<T>::~NRvector()
{
    if (v != NULL && ownsdata) {
        PyMem_Free(v);
    }
}

// end of NRvector definitions

#endif //ifdef _USESTDVECTOR_

template <class T>
class NRmatrix {
private:
    int nn;
    int mm;
    T **v;
public:
    int ownsdata; // 1 for normal NRmatrix, 0 if Python owns the data
    PyObject *pyident;
    NRmatrix();
    NRmatrix(int n, int m);			// Zero-based array
    NRmatrix(PyObject *a); // construct from Python array
    NRmatrix(char *name, char *dict = NULL); // construct from name in Python scope
    void initpymat(PyObject *a); // helper function used by above
    NRmatrix(int n, int m, const T &a);	//Initialize to constant
    NRmatrix(int n, int m, const T *a);	// Initialize to array
    NRmatrix(const NRmatrix &rhs);		// Copy constructor
    NRmatrix & operator=(const NRmatrix &rhs);	//assignment
    typedef T value_type; // make T available externally
    inline T* operator[](const int i);	//subscripting: pointer to row i
    inline const T* operator[](const int i) const;
    inline int nrows() const;
    inline int ncols() const;
    void resize(int newn, int newm); // resize (contents not preserved)
    void assign(int newn, int newm, const T &a); // resize and assign a constant value
    void assign(char *name, char *dict = NULL); // assign to a Python name and scope
    ~NRmatrix();
};

template <class T>
NRmatrix<T>::NRmatrix() : nn(0), mm(0), ownsdata(1), v(NULL) {}

template <class T>
NRmatrix<T>::NRmatrix(int n, int m) : nn(n), mm(m), ownsdata(1), v(n>0 ? new T*[n] : NULL)
{
    int i,nel=m*n;
    if (v) v[0] = nel>0 ? (T*)PyMem_Malloc(nel*sizeof(T)) : NULL;
    for (i=1;i<n;i++) v[i] = v[i-1] + m;
}

template <class T>
void NRmatrix<T>::initpymat(PyObject *a) {
    pyident = a;
    ownsdata = 0;
    if (! PyArray_CheckExact(a)) NRpyException("PyObject is not an Array in NRmatrix constructor.");
    if (! PyArray_ISCARRAY_RO(a)) NRpyException("Python Array must be contiguous (e.g., not strided).");
    if (PyArray_NDIM(a) != 2) NRpyException("Python Array must be 2-dim in NRmatrix constructor.");
    if (! NRpyTypeOK<T>(a)) NRpyException("Python Array type does not agree with NRmatrix type.");
    Int i, nel, *dims = (Int*)PyArray_DIMS(a);
    nn = dims[0];
    mm = dims[1];
    nel = mm*nn;
    v = (nn>0 ? new T*[nn] : NULL);
    if (v) v[0] = nel>0 ? (T*)PyArray_DATA(a) : NULL;
    for (i=1;i<nn;i++) v[i] = v[i-1] + mm;
}
template <class T> NRmatrix<T>::NRmatrix(PyObject *a) {
    initpymat(a);
}
template <class T> NRmatrix<T>::NRmatrix(char *name, char *dict = NULL) {
    initpymat(NRpyGetByName(name,dict));
}

template <class T>
NRmatrix<T>::NRmatrix(int n, int m, const T &a) : nn(n), mm(m), ownsdata(1), v(n>0 ? new T*[n] : NULL)
{
    int i, j, nel=m*n;
    if (v) v[0] = nel>0 ? (T*)PyMem_Malloc(nel*sizeof(T)) : NULL;
    for (i=1; i< n; i++) v[i] = v[i-1] + m;
    for (i=0; i< n; i++) for (j=0; j<m; j++) v[i][j] = a;
}

template <class T>
NRmatrix<T>::NRmatrix(int n, int m, const T *a) : nn(n), mm(m), ownsdata(1), v(n>0 ? new T*[n] : NULL)
{
    int i,j,nel=m*n;
    if (v) v[0] = nel>0 ? (T*)PyMem_Malloc(nel*sizeof(T)) : NULL;
    for (i=1; i< n; i++) v[i] = v[i-1] + m;
    for (i=0; i< n; i++) for (j=0; j<m; j++) v[i][j] = *a++;
}

template <class T>
NRmatrix<T>::NRmatrix(const NRmatrix &rhs) : nn(rhs.nn), mm(rhs.mm), ownsdata(1), v(nn>0 ? new T*[nn] : NULL)
{
    int i,j,nel=mm*nn;
    if (v) v[0] = nel>0 ? (T*)PyMem_Malloc(nel*sizeof(T)) : NULL;
    for (i=1; i< nn; i++) v[i] = v[i-1] + mm;
    for (i=0; i< nn; i++) for (j=0; j<mm; j++) v[i][j] = rhs[i][j];
}

template <class T>
NRmatrix<T> & NRmatrix<T>::operator=(const NRmatrix<T> &rhs) {
    if (this != &rhs) {
        int i,j,nel;
        if (nn != rhs.nn || mm != rhs.mm) {
            resize(rhs.nn,rhs.mm);
            nn=rhs.nn;
            mm=rhs.mm;
        }
        for (i=0; i<nn; i++) for (j=0; j<mm; j++) v[i][j] = rhs[i][j];
    }
    return *this;
}

template <class T>
inline T* NRmatrix<T>::operator[](const int i)	//subscripting: pointer to row i
{
#ifdef _CHECKBOUNDS_
    if (i<0 || i>=nn) {
        throw("NRmatrix subscript out of bounds");
    }
#endif
    return v[i];
}

template <class T>
inline const T* NRmatrix<T>::operator[](const int i) const
{
#ifdef _CHECKBOUNDS_
    if (i<0 || i>=nn) {
        throw("NRmatrix subscript out of bounds");
    }
#endif
    return v[i];
}

template <class T>
inline int NRmatrix<T>::nrows() const
{
    return nn;
}

template <class T>
inline int NRmatrix<T>::ncols() const
{
    return mm;
}

template <class T>
void NRmatrix<T>::resize(int newn, int newm) {
    int i,nel;
    if (newn != nn || newm != mm) {
        nn = newn;
        mm = newm;
        nel = mm*nn;
        if (ownsdata) {
            if (v != NULL) {
                PyMem_Free(v[0]);
                delete[] (v);
            }
            v = nn>0 ? new T*[nn] : NULL;
            if (v) v[0] = nel>0 ? (T*)PyMem_Malloc(nel*sizeof(T)) : NULL;
        } else {
            if (v != NULL) delete[] (v);
            int dm[2];
            dm[0] = newn; dm[1] = newm;
            PyArray_Dims mydims;
            mydims.ptr = dm;
            mydims.len = 2;
            PyArray_Resize((PyArrayObject *)pyident, &mydims, 0, NPY_CORDER);
            // the return value is garbage, or maybe PyNone, contrary to Numpy docs
            // I think it's a Numpy bug, but following is correct
            v = (nn>0 ? new T*[nn] : NULL);
            if (v) v[0] = nel>0 ? (T*)PyArray_DATA(pyident) : NULL;
        }
        for (i=1; i<nn; i++) v[i] = v[i-1] + mm;
    }
}

template <class T>
void NRmatrix<T>::assign(int newn, int newm, const T& a) {
    int i,j;
    resize(newn,newm);
    for (i=0; i< nn; i++) for (j=0; j<mm; j++) v[i][j] = a;
}

template <class T>
void NRmatrix<T>::assign(char *name, char *dict = NULL) {
    if (! ownsdata) NRpyException("Attempt to assign Python matrix to another Python matrix");
    if (v != NULL) {
        PyMem_Free(v[0]);
        delete[] (v);
    }
    initpymat(NRpyGetByName(name,dict));
}

template <class T>
NRmatrix<T>::~NRmatrix()
{
    if (v != NULL) {
        if (ownsdata) PyMem_Free(v[0]); // pointer to the data
        delete[] (v); // pointer to the pointers
    }
}

template <class T>
class NRMat3d {
private:
    int nn;
    int mm;
    int kk;
    T ***v;
public:
    NRMat3d();
    NRMat3d(int n, int m, int k);
    inline T** operator[](const int i);	//subscripting: pointer to row i
    inline const T* const * operator[](const int i) const;
    inline int dim1() const;
    inline int dim2() const;
    inline int dim3() const;
    ~NRMat3d();
};

template <class T>
NRMat3d<T>::NRMat3d(): nn(0), mm(0), kk(0), v(NULL) {}

template <class T>
NRMat3d<T>::NRMat3d(int n, int m, int k) : nn(n), mm(m), kk(k), v(new T**[n])
{
    int i,j;
    v[0] = new T*[n*m];
    v[0][0] = new T[n*m*k];
    for(j=1; j<m; j++) v[0][j] = v[0][j-1] + k;
    for(i=1; i<n; i++) {
        v[i] = v[i-1] + m;
        v[i][0] = v[i-1][0] + m*k;
        for(j=1; j<m; j++) v[i][j] = v[i][j-1] + k;
    }
}

template <class T>
inline T** NRMat3d<T>::operator[](const int i) //subscripting: pointer to row i
{
    return v[i];
}

template <class T>
inline const T* const * NRMat3d<T>::operator[](const int i) const
{
    return v[i];
}

template <class T>
inline int NRMat3d<T>::dim1() const
{
    return nn;
}

template <class T>
inline int NRMat3d<T>::dim2() const
{
    return mm;
}

template <class T>
inline int NRMat3d<T>::dim3() const
{
    return kk;
}

template <class T>
NRMat3d<T>::~NRMat3d()
{
    if (v != NULL) {
        delete[] (v[0][0]);
        delete[] (v[0]);
        delete[] (v);
    }
}


// basic type names (redefine if your bit lengths don't match)

typedef int Int; // 32 bit integer
typedef unsigned int Uint;

#ifdef _MSC_VER
typedef __int64 Llong; // 64 bit integer
typedef unsigned __int64 Ullong;
#else
typedef long long int Llong; // 64 bit integer
typedef unsigned long long int Ullong;
#endif

typedef char Char; // 8 bit integer
typedef unsigned char Uchar;

typedef double Doub; // default floating type
typedef long double Ldoub;

typedef complex<double> Complex; // default complex type

typedef bool Bool;

// vector types

typedef const NRvector<Int> VecInt_I;
typedef NRvector<Int> VecInt, VecInt_O, VecInt_IO;

typedef const NRvector<Uint> VecUint_I;
typedef NRvector<Uint> VecUint, VecUint_O, VecUint_IO;

typedef const NRvector<Llong> VecLlong_I;
typedef NRvector<Llong> VecLlong, VecLlong_O, VecLlong_IO;

typedef const NRvector<Ullong> VecUllong_I;
typedef NRvector<Ullong> VecUllong, VecUllong_O, VecUllong_IO;

typedef const NRvector<Char> VecChar_I;
typedef NRvector<Char> VecChar, VecChar_O, VecChar_IO;

typedef const NRvector<Char*> VecCharp_I;
typedef NRvector<Char*> VecCharp, VecCharp_O, VecCharp_IO;

typedef const NRvector<Uchar> VecUchar_I;
typedef NRvector<Uchar> VecUchar, VecUchar_O, VecUchar_IO;

typedef const NRvector<Doub> VecDoub_I;
typedef NRvector<Doub> VecDoub, VecDoub_O, VecDoub_IO;

typedef const NRvector<Doub*> VecDoubp_I;
typedef NRvector<Doub*> VecDoubp, VecDoubp_O, VecDoubp_IO;

typedef const NRvector<Complex> VecComplex_I;
typedef NRvector<Complex> VecComplex, VecComplex_O, VecComplex_IO;

typedef const NRvector<Bool> VecBool_I;
typedef NRvector<Bool> VecBool, VecBool_O, VecBool_IO;

// matrix types

typedef const NRmatrix<Int> MatInt_I;
typedef NRmatrix<Int> MatInt, MatInt_O, MatInt_IO;

typedef const NRmatrix<Uint> MatUint_I;
typedef NRmatrix<Uint> MatUint, MatUint_O, MatUint_IO;

typedef const NRmatrix<Llong> MatLlong_I;
typedef NRmatrix<Llong> MatLlong, MatLlong_O, MatLlong_IO;

typedef const NRmatrix<Ullong> MatUllong_I;
typedef NRmatrix<Ullong> MatUllong, MatUllong_O, MatUllong_IO;

typedef const NRmatrix<Char> MatChar_I;
typedef NRmatrix<Char> MatChar, MatChar_O, MatChar_IO;

typedef const NRmatrix<Uchar> MatUchar_I;
typedef NRmatrix<Uchar> MatUchar, MatUchar_O, MatUchar_IO;

typedef const NRmatrix<Doub> MatDoub_I;
typedef NRmatrix<Doub> MatDoub, MatDoub_O, MatDoub_IO;

typedef const NRmatrix<Bool> MatBool_I;
typedef NRmatrix<Bool> MatBool, MatBool_O, MatBool_IO;

// 3D matrix types

typedef const NRMat3d<Doub> Mat3DDoub_I;
typedef NRMat3d<Doub> Mat3DDoub, Mat3DDoub_O, Mat3DDoub_IO;

// Floating Point Exceptions for Microsoft compilers

#ifdef _TURNONFPES_
#ifdef _MSC_VER
struct turn_on_floating_exceptions {
	turn_on_floating_exceptions() {
		int cw = _controlfp( 0, 0 );
		cw &=~(EM_INVALID | EM_OVERFLOW | EM_ZERODIVIDE );
		_controlfp( cw, MCW_EM );
	}
};
turn_on_floating_exceptions yes_turn_on_floating_exceptions;
#endif /* _MSC_VER */
#endif /* _TURNONFPES */

// Python glue Part II begins here

// NRpyObject for vector and matrix
template<class T> PyObject* NRpyObject(NRvector<T> &a) {
    if (a.ownsdata == 0) {Py_INCREF(a.pyident); return a.pyident;}
    npy_int nd=1, dims[1];
    dims[0] = a.size();
    PyObject *thing;
    if (dims[0] > 0) {
        thing = PyArray_SimpleNewFromData(nd, dims, NRpyDataType<T>(), &a[0]);
    } else {
        thing = PyArray_SimpleNew(nd, dims, NRpyDataType<T>()); // zero size
    }
    PyArray_FLAGS(thing) |= NPY_OWNDATA;
    a.ownsdata = 0;
    a.pyident = thing;
    return thing;
}
template<class T> PyObject* NRpyObject(NRmatrix<T> &a) {
    if (a.ownsdata == 0) {Py_INCREF(a.pyident); return a.pyident;}
    npy_int nd=2, dims[2];
    dims[0] = a.nrows(); dims[1] = a.ncols();
    PyObject *thing;
    if (dims[0]*dims[1] > 0) {
        thing = PyArray_SimpleNewFromData(nd, dims, NRpyDataType<T>(), &a[0][0]);
    } else {
        thing = PyArray_SimpleNew(nd, dims, NRpyDataType<T>()); // zero size
    }
    PyArray_FLAGS(thing) |= NPY_OWNDATA;
    a.ownsdata = 0;
    a.pyident = thing;
    return thing;
}

// PyObject(tuple) must go down here because it uses an NRvector
PyObject* NRpyTuple(PyObject *first, ...) {
    int MAXARGS=1024, i, nargs=1;
    NRvector<PyObject*> argslist(MAXARGS);
    argslist[0] = first;
    va_list vl;
    va_start(vl,first);
    for (;;) {
        argslist[nargs] = va_arg(vl,PyObject*);
        if (argslist[nargs] == NULL) break;
        else ++nargs;
    }
    va_end(vl);
    PyObject* tuple = PyTuple_New(nargs);
    // assumes that tuple will be returned to Python, so we are not responsible for it.
    for(i=0;i<nargs;i++) PyTuple_SetItem(tuple, i, argslist[i]);
    return tuple;
}
// If you create an NRpyTuple and don't return it, you need to Py_DECREF it later.

// macros used to make and use persistent objects

#define NRpyCONNECT(CLASS,METHOD) \
static PyObject *METHOD(PyObject *self, PyObject *pyargs) { \
	NRpyArgs args(pyargs); \
	CLASS *p = (CLASS *)PyCapsule_GetPointer(args[0],NULL); \
	return p->METHOD(args); }
// To-Do: check that args[0] exists and is a PyCapsule before using it

// destructor to register with PyCapsule, calls actual destructor
// then, constructor calls actual constructor to create instance, returns it
#define NRpyCONSTRUCTOR(CLASS,METHOD) \
void NRpyDestroy(PyObject *myself) { \
	((CLASS*)PyCapsule_GetPointer(myself,NULL))->~CLASS(); } \
static PyObject *METHOD(PyObject *self, PyObject *pyargs) { \
	NRpyArgs args(pyargs); \
	CLASS *instance = new CLASS(args); \
	return PyCapsule_New(instance,NULL,NRpyDestroy); }

// functor class to help with calling Python functions from within C++ modules
template <class R> struct NRpyPyFunction {
    PyObject *ob;
    int argcount;
    NRpyPyFunction() : ob(NULL), argcount(0) {}
    NRpyPyFunction(PyObject *obb) : ob(obb) {
        if(! PyCallable_Check(ob)) NRpyException("NRpyPyFunction: non-callable object.");
        PyCodeObject *code = (PyCodeObject *)PyFunction_GetCode(ob);
        argcount = code->co_argcount; // caution, uses not-officially-exposed value
    }
    inline void argcheck(int argn) {
        if (argn != argcount)
            NRpyException("NRpyPyFunction: should be %d args in PyFunction call.",1,argcount);
    }
    // constructors for 0 to 4 args.  You can add more if you want.
    R operator()() {
        argcheck(0);
        return NRpyCast<R>(PyObject_CallObject(ob,NULL));
    }
    template <class T> R operator()(T x1) {
        PyObject *tuple;
        argcheck(1);
        tuple = NRpyTuple(NRpyObject(x1),NULL);
        PyObject *tmp = PyObject_CallObject(ob,tuple);
        if (tmp == NULL) NRpyException("Error in evaluating a Python function called from C++");
        return NRpyCast<R>(tmp);
    }
    template <class T, class U> R operator()(T x1, U x2) {
        PyObject *tuple;
        argcheck(2);
        tuple = NRpyTuple(NRpyObject(x1),NRpyObject(x2),NULL);
        PyObject *tmp = PyObject_CallObject(ob,tuple);
        if (tmp == NULL) NRpyException("Error in evaluating a Python function called from C++");
        return NRpyCast<R>(tmp);
    }
    template <class T, class U, class V> R operator()(T x1, U x2, V x3) {
        PyObject *tuple;
        argcheck(3);
        tuple = NRpyTuple(NRpyObject(x1),NRpyObject(x2),NRpyObject(x3),NULL);
        PyObject *tmp = PyObject_CallObject(ob,tuple);
        if (tmp == NULL) NRpyException("Error in evaluating j Python function called from C++");
        return NRpyCast<R>(tmp);
    }
    template <class T, class U, class V, class W> R operator()(T x1, U x2, V x3, W x4) {
        PyObject *tuple;
        argcheck(4);
        tuple = NRpyTuple(NRpyObject(x1),NRpyObject(x2),NRpyObject(x3),NRpyObject(x4),NULL);
        PyObject *tmp = PyObject_CallObject(ob,tuple);
        if (tmp == NULL) NRpyException("Error in evaluating a Python function called from C++");
        return NRpyCast<R>(tmp);
    }
};

// functor class for calling a function of one arg that may be either C or Python
// T is return type, U is argument type
template <class T, class U>
struct NRpyAnyFunction {
    T (*cfunc)(U);
    NRpyPyFunction<T> ftor;
    Int ispy;

    NRpyAnyFunction(PyObject *ob) {
        if(PyCallable_Check(ob)) {
            ispy = 1;
            ftor = NRpyPyFunction<T>(ob);
        } else if (PyCapsule_CheckExact(ob)) {
            ispy = 0;
            NRpyCFunction(cfunc,ob);
        } else NRpyException("Not a function object of either type in NRpyAnyFunction.");
    }
    inline T operator()(U x) {
        if (ispy) return ftor(x);
        else return cfunc(x);
    }
};

#endif /* _NR3_H_ */
