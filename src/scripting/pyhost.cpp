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
#include "../controllers/addtorrentcontroller.h"
#include "bindings/gil.hpp"
#include "bindings/module.h"

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
namespace py = boost::python;

template <typename T>
struct py_deleter
{
    void operator()(T* t)
    {
        lock_gil lock;
        delete t;
    }
};

class AddTorrentControllerWrapper
    : public AddTorrentController,
    public py::wrapper<AddTorrentController>
{
public:
    void PerformAdd()
    {
        lock_gil lock;
        get_override("perform_add")();
    }

    std::string GetName(int index)
    {
        lock_gil lock;
        return get_override("get_name")(index);
    }

    std::string GetComment(int index)
    {
        lock_gil lock;
        return get_override("get_comment")(index);
    }

    std::string GetCreationDate(int index)
    {
        lock_gil lock;
        return get_override("get_creation_date")(index);
    }

    std::string GetCreator(int index)
    {
        lock_gil lock;
        return get_override("get_creator")(index);
    }

    std::string GetSavePath(int index)
    {
        lock_gil lock;
        return get_override("get_save_path")(index);
    }

    std::string GetSize(int index)
    {
        lock_gil lock;
        return get_override("get_size")(index);
    }

    int GetCount()
    {
        lock_gil lock;
        return get_override("get_count")();
    }

    int GetFileCount(int torrentIndex)
    {
        lock_gil lock;
        return get_override("get_file_count")(torrentIndex);
    }

    std::string GetFileName(int torrentIndex, int fileIndex)
    {
        lock_gil lock;
        return get_override("get_file_name")(torrentIndex, fileIndex);
    }

    std::string GetFileSize(int torrentIndex, int fileIndex)
    {
        lock_gil lock;
        return get_override("get_file_size")(torrentIndex, fileIndex);
    }

    int GetFilePriority(int torrentIndex, int fileIndex)
    {
        lock_gil lock;
        return get_override("get_file_priority")(torrentIndex, fileIndex);
    }

    void SetSavePath(int torrentIndex, std::string savePath)
    {
        lock_gil lock;
        get_override("set_save_path")(torrentIndex, savePath);
    }

    void SetFileName(int torrentIndex, int fileIndex, std::string name)
    {
        lock_gil lock;
        get_override("set_file_name")(torrentIndex, fileIndex, name);
    }

    void SetFilePriority(int torrentIndex, int fileIndex, int prio)
    {
        lock_gil lock;
        get_override("set_file_priority")(torrentIndex, fileIndex, prio);
    }

    std::string ToFriendlyPriority(int priority)
    {
        lock_gil lock;
        return get_override("to_friendly_priority")(priority);
    }
};

boost::shared_ptr<AddTorrentController> create_wrapper()
{
    return boost::shared_ptr<AddTorrentController>(
        new AddTorrentControllerWrapper(),
        py_deleter<AddTorrentController>());
}

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
    py::def("show_add_torrent_dialog", &PyHost::ShowAddTorrentDialog);
    py::def("show_open_file_dialog", &PyHost::ShowOpenFileDialog);

    py::class_<AddTorrentControllerWrapper, boost::noncopyable, boost::shared_ptr<AddTorrentControllerWrapper>>
        ("AddTorrentController")//, py::no_init)
        //.def("__init__", py::make_constructor(&create_wrapper))
        .def("perform_add", py::pure_virtual(&AddTorrentController::PerformAdd))
        .def("get_name", py::pure_virtual(&AddTorrentController::GetName))
        .def("get_comment", py::pure_virtual(&AddTorrentController::GetComment))
        .def("get_creation_date", py::pure_virtual(&AddTorrentController::GetCreationDate))
        .def("get_creator", py::pure_virtual(&AddTorrentController::GetCreator))
        .def("get_save_path", py::pure_virtual(&AddTorrentController::GetSavePath))
        .def("get_size", py::pure_virtual(&AddTorrentController::GetSize))
        .def("get_count", py::pure_virtual(&AddTorrentController::GetCount))
        .def("get_file_count", py::pure_virtual(&AddTorrentController::GetFileCount))
        .def("get_file_name", py::pure_virtual(&AddTorrentController::GetFileName))
        .def("get_file_size", py::pure_virtual(&AddTorrentController::GetFileSize))
        .def("get_file_priority", py::pure_virtual(&AddTorrentController::GetFilePriority))
        .def("set_save_path", py::pure_virtual(&AddTorrentController::SetSavePath))
        .def("set_file_name", py::pure_virtual(&AddTorrentController::SetFileName))
        .def("set_file_priority", py::pure_virtual(&AddTorrentController::SetFilePriority))
        .def("to_friendly_priority", py::pure_virtual(&AddTorrentController::ToFriendlyPriority))
        ;

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
    lock_gil lock;
    pt_.attr("on_instance_already_running")();
}

void PyHost::OnMenuItemClicked(int id)
{
    lock_gil lock;
    pt_.attr("on_menu_item_clicked")(id);
}

void PyHost::OnTorrentItemActivated(const libtorrent::sha1_hash& hash)
{
    lock_gil lock;
    pt_.attr("on_torrent_item_activated")(hash);
}

void PyHost::OnTorrentItemSelected(const libtorrent::sha1_hash& hash)
{
    lock_gil lock;
    pt_.attr("on_torrent_item_selected")(hash);
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

void PyHost::ShowAddTorrentDialog(boost::shared_ptr<AddTorrentController> controller)
{
    pico_->ShowAddTorrentDialog(controller);
}

py::tuple PyHost::ShowOpenFileDialog()
{
    bool result;
    std::vector<std::string> files;
    {
        ScopedGILRelease scope;
        result = pico_->ShowOpenFileDialog(files);
    }

    py::list l;
    for (std::string& file : files)
    {
        l.append(file);
    }

    return py::make_tuple(result, l);
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
