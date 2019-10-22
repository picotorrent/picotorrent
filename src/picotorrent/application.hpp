#pragma once

#include <QApplication>

#include <memory>

namespace google_breakpad
{
    class ExceptionHandler;
}

namespace pt
{
    class Application : public QApplication
    {
    public:
        Application(int& argc, char **argv);
        virtual ~Application();

        void activateOtherInstance();
        bool isSingleInstance();

    private:
        struct Mutex;

        std::unique_ptr<Mutex> m_singleInstanceMutex;
        bool m_isSingleInstance;
    };
}
