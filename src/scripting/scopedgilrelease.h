#ifndef _PT_SCRIPTING_SCOPEDGILRELEASE_H
#define _PT_SCRIPTING_SCOPEDGILRELEASE_H

#include <boost/python.hpp>

class ScopedGILRelease
{
public:
    inline ScopedGILRelease()
    {
        state_ = PyEval_SaveThread();
    }

    inline ~ScopedGILRelease()
    {
        PyEval_RestoreThread(state_);
        state_ = NULL;
    }

private:
    PyThreadState* state_;
};

#endif
