#include "pyhost.h"

#include <boost/python.hpp>
#include <libtorrent/torrent_handle.hpp>

#include "scopedgilrelease.h"
#include "../config.h"
#include "../picotorrent.h"
#include "bindings/module.h"

namespace py = boost::python;

BOOST_PYTHON_MODULE(libtorrent)
{
    bind_libtorrent();
}

BOOST_PYTHON_MODULE(picotorrent_api)
{
    py::def("add_torrent", &PyHost::AddTorrent);
    py::def("update_torrents", &PyHost::UpdateTorrents);
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
    PyImport_AppendInittab("libtorrent", initlibtorrent);
    PyImport_AppendInittab("picotorrent_api", initpicotorrent_api);
    Py_InitializeEx(0);
    PyEval_InitThreads();

    std::string sp = cfg.GetPyPath();
    char* sysPath = new char[sp.length() + 1];
    strcpy(sysPath, sp.c_str());
    PySys_SetPath(sysPath);
    delete[] sysPath;

    py::object module = py::import("__main__");
    ns_ = module.attr("__dict__");

    std::string bootstrapper = ""
        "import sys\n"
        "sys.dont_write_bytecode = True\n"

        // Set up paths
        "sys.path.insert(0, '" + cfg.GetPyRuntimePath() + "')\n"
        "sys.path.insert(1, '" + cfg.GetPyRuntimePath() + ".zip')\n"
        ;

    try
    {
        py::exec(py::str(bootstrapper), ns_);

        pt_ = py::import("picotorrent");
        pt_.attr("on_load")();

        ts_ = PyEval_SaveThread();
    }
    catch (const py::error_already_set& err)
    {
        std::string error = parse_python_exception();
    }
}

void PyHost::Unload()
{
    try
    {
        PyEval_RestoreThread(static_cast<PyThreadState*>(ts_));
        pt_.attr("on_unload")();
    }
    catch (const py::error_already_set& err)
    {
        std::string error = parse_python_exception();
    }
}

void PyHost::OnTorrentItemSelected(const libtorrent::sha1_hash& hash)
{
    PyEval_RestoreThread(static_cast<PyThreadState*>(ts_));
    pt_.attr("on_torrent_item_selected")(hash);
    ts_ = PyEval_SaveThread();
}

void PyHost::AddTorrent(const libtorrent::torrent_status& status)
{
    ScopedGILRelease scope;
    pico_->AddTorrent(status);
}

void PyHost::UpdateTorrents(py::dict torrents)
{
    std::map<libtorrent::sha1_hash, libtorrent::torrent_status> map;
    py::list keys = torrents.keys();
    
    for (int i = 0; i < py::len(keys); i++)
    {
        libtorrent::sha1_hash hash = py::extract<libtorrent::sha1_hash>(keys[i]);
        libtorrent::torrent_status status = py::extract<libtorrent::torrent_status>(torrents[hash]);

        map[hash] = status;
    }

    ScopedGILRelease scope;
    pico_->UpdateTorrents(map);
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
