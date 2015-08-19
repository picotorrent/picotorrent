#include "pyhost.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/python.hpp>
#include <codecvt>
#include <libtorrent/torrent_handle.hpp>
#include <locale>

#include "scopedgilrelease.h"
#include "../common.h"
#include "../picotorrent.h"
#include "bindings/module.h"

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
namespace py = boost::python;

BOOST_PYTHON_MODULE(libtorrent)
{
    bind_libtorrent();
}

BOOST_PYTHON_MODULE(picotorrent_api)
{
    py::def("add_torrent", &PyHost::AddTorrent);
    py::def("exit", &PyHost::Exit);
    py::def("log", &PyHost::Log);
    py::def("update_torrents", &PyHost::UpdateTorrents);
    py::def("prompt", &PyHost::Prompt);
    py::def("set_application_status", &PyHost::SetApplicationStatus);

    py::enum_<MenuItem>("menu_item_t")
        .value("FILE_ADD_TORRENT", ptID_FILE_ADD_TORRENT)
        .value("FILE_EXIT", ptID_FILE_EXIT)
        .value("VIEW_LOG", ptID_VIEW_LOG)
        ;
}

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

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

void PyHost::Init()
{
    // Init Python
    PyImport_AppendInittab("libtorrent", PyInit_libtorrent);
    PyImport_AppendInittab("picotorrent_api", PyInit_picotorrent_api);
    Py_InitializeEx(0);
    PyEval_InitThreads();

    // Set sys.argv
    PySys_SetArgvEx(pico_->argc, pico_->argv, 0);

    // Set sys.path
    std::wstring path = converter.from_bytes(GetPyPath());
    PySys_SetPath(path.c_str());

    try
    {
        pt_ = py::import("picotorrent");
        ts_ = PyEval_SaveThread();
    }
    catch (const py::error_already_set& err)
    {
        std::string error = parse_python_exception();
    }
}

void PyHost::Load()
{
    try
    {
        PyEval_RestoreThread(static_cast<PyThreadState*>(ts_));
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

void PyHost::OnInstanceAlreadyRunning()
{
    PyEval_RestoreThread(static_cast<PyThreadState*>(ts_));
    pt_.attr("on_instance_already_running")();
    ts_ = PyEval_SaveThread();
}

void PyHost::OnMenuItemClicked(int id)
{
    PyEval_RestoreThread(static_cast<PyThreadState*>(ts_));
    pt_.attr("on_menu_item_clicked")(id);
    ts_ = PyEval_SaveThread();
}

void PyHost::OnTorrentItemActivated(const libtorrent::sha1_hash& hash)
{
    PyEval_RestoreThread(static_cast<PyThreadState*>(ts_));
    pt_.attr("on_torrent_item_activated")(hash);
    ts_ = PyEval_SaveThread();
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

void PyHost::Exit()
{
    ScopedGILRelease scope;
    pico_->Exit();
}

void PyHost::Log(std::string message)
{
    ScopedGILRelease scope;
    pico_->AppendLog(message);
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

std::string PyHost::GetPyPath()
{
    std::string file = "PicoTorrent.json";
    std::string defaultPath = "lib.zip;python34.zip";

    if (!fs::exists(file))
    {
        // Beware! Code smell! This is the default python paths that we use
        // when running a release version of PicoTorrent. It can be overridden
        // by the PicoTorrent.json file.
        return defaultPath;
    }

    pt::ptree ptree;
    pt::read_json(file, ptree);

    return ptree.get<std::string>("pyPath", defaultPath);
}
