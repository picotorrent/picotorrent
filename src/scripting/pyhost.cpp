#include "pyhost.h"

#include <boost/python.hpp>
#include "scopedgilrelease.h"
#include "../config.h"
#include "../picotorrent.h"

namespace py = boost::python;

BOOST_PYTHON_MODULE(picotorrent_api)
{
    py::def("prompt", &PyHost::Prompt);
    py::def("set_application_status", &PyHost::SetApplicationStatus);
}

std::string parse_python_exception() {
    PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
    // Fetch the exception info from the Python C API
    PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);

    // Fallback error
    std::string ret("Unfetchable Python error");
    // If the fetch got a type pointer, parse the type into the exception string
    if (type_ptr != NULL) {
        py::handle<> h_type(type_ptr);
        py::str type_pstr(h_type);
        // Extract the string from the boost::python object
        py::extract<std::string> e_type_pstr(type_pstr);
        // If a valid string extraction is available, use it 
        //  otherwise use fallback
        if (e_type_pstr.check())
            ret = e_type_pstr();
        else
            ret = "Unknown exception type";
    }
    // Do the same for the exception value (the stringification of the exception)
    if (value_ptr != NULL) {
        py::handle<> h_val(value_ptr);
        py::str a(h_val);
        py::extract<std::string> returned(a);
        if (returned.check())
            ret += ": " + returned();
        else
            ret += std::string(": Unparseable Python error: ");
    }
    // Parse lines from the traceback using the Python traceback module
    if (traceback_ptr != NULL) {
        py::handle<> h_tb(traceback_ptr);
        // Load the traceback module and the format_tb function
        py::object tb(py::import("traceback"));
        py::object fmt_tb(tb.attr("format_tb"));
        // Call format_tb to get a list of traceback strings
        py::object tb_list(fmt_tb(h_tb));
        // Join the traceback strings into a single string
        py::object tb_str(py::str("\n").join(tb_list));
        // Extract the string, check the extraction, and fallback in necessary
        py::extract<std::string> returned(tb_str);
        if (returned.check())
            ret += ": " + returned();
        else
            ret += std::string(": Unparseable Python traceback");
    }
    return ret;
}

PicoTorrent* PyHost::pico_ = NULL;

PyHost::PyHost(PicoTorrent* pico)
{
    PyHost::pico_ = pico;
}

void PyHost::Load()
{
    Config& cfg = Config::GetInstance();

    // Init Python
    PyImport_AppendInittab("picotorrent_api", initpicotorrent_api);
    Py_InitializeEx(0);

    std::string sp = cfg.GetPyPath();
    char* sysPath = new char[sp.length() + 1];
    strcpy(sysPath, sp.c_str());
    PySys_SetPath(sysPath);
    delete[] sysPath;

    py::object module = py::import("__main__");
    py::object ns = module.attr("__dict__");

    std::string bootstrapper = ""
        "import sys\n"
        "sys.dont_write_bytecode = True\n"

        // Set up paths
        "sys.path.insert(0, '" + cfg.GetPyRuntimePath() + ".zip')\n"

        // Import and run picotorrent.on_load
        "import picotorrent\n"
        "picotorrent.on_load()"
        ;

    try
    {
        // Run our bootstrapper script
        py::exec(py::str(bootstrapper), ns);
        ts_ = PyEval_SaveThread();
    }
    catch (const py::error_already_set& err)
    {
        std::string error = parse_python_exception();
    }
}

void PyHost::Unload()
{
}

bool PyHost::Prompt(std::string message)
{
    ScopedGILRelease scope;
    return pico_->Prompt(message);
}

void PyHost::SetApplicationStatus(std::string status)
{
    ScopedGILRelease scope;
    pico_->SetApplicationStatusText(status);
}
