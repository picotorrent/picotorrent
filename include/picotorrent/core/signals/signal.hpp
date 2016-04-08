#pragma once

#include <algorithm>
#include <functional>
#include <picotorrent/common.hpp>
#include <vector>

namespace picotorrent
{
namespace core
{
namespace signals
{
    template<typename TRet, typename TArg>
    struct function_wrapper
    {
        using function_type = std::function<TRet(TArg)>;
    };

    template<typename TRet>
    struct function_wrapper<TRet, void>
    {
        using function_type = std::function<TRet()>;
    };

    template<typename TRet, typename TArg>
    class signal_connector
    {
    public:
        using function_type = typename function_wrapper<TRet, TArg>::function_type;

        DLL_EXPORT signal_connector()
            : callbacks_()
        {
        }

        DLL_EXPORT signal_connector(const signal_connector& that)
            : callbacks_(that.callbacks_)
        {
        }

        DLL_EXPORT void connect(const function_type &callback)
        {
            callback_item item{ callback };
            callbacks_.push_back(item);
        }

        DLL_EXPORT void disconnect(const function_type &callback)
        {
            auto it = std::find_if(callbacks_.begin(), callbacks_.end(),
                [callback](callback_item &item)
            {
                return item.callback = callback;
            });

            if (it != callbacks_.end())
            {
                callbacks_.erase(it);
            }
        }

    protected:
        class callback_item
        {
        public:
            function_type callback;
        };

        inline std::vector<callback_item>& items() { return callbacks_; }

    private:
        std::vector<callback_item> callbacks_;
    };

    template<typename TRet, typename TArg>
    class signal : public signal_connector<TRet, TArg>
    {
    public:
        DLL_EXPORT std::vector<TRet> emit(TArg args)
        {
            std::vector<TRet> result;
            for (auto &item : signal_connector<TRet, TArg>::items())
            {
                result.push_back(item.callback(args));
            }
            return result;
        }
    };

    template<typename TArg>
    class signal<void, TArg> : public signal_connector<void, TArg>
    {
    public:
        DLL_EXPORT void emit(TArg args)
        {
            for (auto &item : signal_connector<void, TArg>::items())
            {
                item.callback(args);
            }
        }
    };

    template<typename TRet>
    class signal<TRet, void> : public signal_connector<TRet, void>
    {
    public:
        DLL_EXPORT std::vector<TRet> emit()
        {
            std::vector<TRet> result;
            for (auto &item : signal_connector<TRet, void>::items())
            {
                result.push_back(item.callback());
            }
            return result;
        }
    };

    template<>
    class signal<void, void> : public signal_connector<void, void>
    {
    public:
        DLL_EXPORT void emit()
        {
            for (auto &item : signal_connector<void, void>::items())
            {
                item.callback();
            }
        }
    };
}
}
}
