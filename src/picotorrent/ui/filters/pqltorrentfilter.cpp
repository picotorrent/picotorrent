#include "pqltorrentfilter.hpp"

#include <boost/log/trivial.hpp>

#include <antlr4-runtime.h>
#include <QueryBaseVisitor.h>
#include <QueryLexer.h>
#include <QueryParser.h>

#include "../../bittorrent/torrenthandle.hpp"
#include "../../bittorrent/torrentstatus.hpp"

using pt::BitTorrent::TorrentHandle;
using pt::BitTorrent::TorrentStatus;
using pt::UI::Filters::PqlTorrentFilter;

typedef std::function<bool(TorrentStatus const&)> FilterFunc;

class ExceptionErrorListener : public antlr4::BaseErrorListener
{
public:
    virtual void syntaxError(
        antlr4::Recognizer* /* recognizer */,
        antlr4::Token* /* offendingSymbol */,
        size_t line,
        size_t charPositionInLine,
        const std::string& msg,
        std::exception_ptr e) override {

        e = nullptr;
        std::ostringstream oss;
        oss << "Syntax error - " << msg << " at " << line << ":" << charPositionInLine;
        error_msg = oss.str();
        throw antlr4::ParseCancellationException(error_msg);
    }

private:
    std::string error_msg;
};

class QueryException : public std::exception
{
public:
    QueryException(std::string const& msg, size_t line, size_t pos)
    {
        std::ostringstream oss;
        oss << msg << " at " << line << ":" << pos;
        m_msg = oss.str();
    }

    virtual const char* what() const NOEXCEPT override
    {
        return m_msg.c_str();
    }

private:
    std::string m_msg;
};

enum class Operator
{
    CONTAINS,
    LT,
    LTE,
    EQ,
    GT,
    GTE,
};

template<typename TLeft, typename TRight>
bool Compare(TLeft const& lhs, TRight const& rhs, Operator oper)
{
    switch (oper)
    {
    case Operator::LT: return lhs < rhs;
    case Operator::LTE: return lhs <= rhs;
    case Operator::EQ: return lhs == rhs;
    case Operator::GT: return lhs > rhs;
    case Operator::GTE: return lhs >= rhs;
    }

    throw std::exception("Unknown operator");
}

class FilterVisitor : public pt::PQL::QueryBaseVisitor
{
public:
    virtual antlrcpp::Any visitAndExpression(pt::PQL::QueryParser::AndExpressionContext* ctx) override
    {
        std::vector<FilterFunc> funcs;

        for (auto expr : ctx->expression())
        {
            funcs.push_back(this->visit(expr));
        }

        return FilterFunc([funcs](TorrentStatus const& ts)
            {
                return std::all_of(
                    funcs.begin(),
                    funcs.end(),
                    [&ts](auto const& f)
                    {
                        return f(ts);
                    });
            });
    }

    virtual antlrcpp::Any visitFilter(pt::PQL::QueryParser::FilterContext* ctx) override
    {
        return this->visit(ctx->expression());
    }

    virtual antlrcpp::Any visitOrExpression(pt::PQL::QueryParser::OrExpressionContext* ctx) override
    {
        std::vector<FilterFunc> funcs;

        for (auto expr : ctx->expression())
        {
            funcs.push_back(this->visit(expr));
        }

        return FilterFunc([funcs](TorrentStatus const& ts)
            {
                return std::any_of(
                    funcs.begin(),
                    funcs.end(),
                    [&ts](auto const& f)
                    {
                        return f(ts);
                    });
            });
    }

    virtual antlrcpp::Any visitOper(pt::PQL::QueryParser::OperContext* ctx) override
    {
        if (ctx->CONTAINS()) return Operator::CONTAINS;
        if (ctx->EQ()) return Operator::EQ;
        if (ctx->GT()) return Operator::GT;
        if (ctx->GTE()) return Operator::GTE;
        if (ctx->LT()) return Operator::LT;
        if (ctx->LTE()) return Operator::LTE;

        throw QueryException(
            "Unknown operator",
            ctx->getStart()->getLine(),
            ctx->getStart()->getCharPositionInLine());
    }

    virtual antlrcpp::Any visitOperatorPredicate(pt::PQL::QueryParser::OperatorPredicateContext* ctx) override
    {
        std::string ref = this->visit(ctx->reference());
        Operator oper = this->visit(ctx->oper());
        antlrcpp::Any value = this->visit(ctx->value());

        if ((ref == "dl" || ref == "ul") && (value.is<int64_t>() || value.is<float>()))
        {
            float term = value.is<float>()
                ? value.as<float>()
                : (float)value.as<int64_t>();

            if (ref == "dl") return FilterFunc([oper, term](TorrentStatus const& ts) { return Compare(ts.downloadPayloadRate, term, oper); });
            if (ref == "ul") return FilterFunc([oper, term](TorrentStatus const& ts) { return Compare(ts.uploadPayloadRate, term, oper); });
        }

        if (ref == "name" && value.is<std::string>())
        {
            std::string term = value;

            if (oper == Operator::CONTAINS)
            {
                return FilterFunc(
                    [term](TorrentStatus const& ts)
                    {
                        return ts.name.find(term) != std::string::npos;
                    });
            }

            return FilterFunc([oper, term](TorrentStatus const& ts) { return Compare(ts.name, term, oper); });
        }

        if (ref == "progress" && (value.is<int64_t>() || value.is<float>()))
        {
            float term = value.is<float>()
                ? value.as<float>()
                : (float)value.as<int64_t>();

            return FilterFunc([oper, term](TorrentStatus const& ts) { return Compare(ts.progress * 100, term, oper); });
        }

        if (ref == "size" && value.is<int64_t>())
        {
            int64_t term = value;
            return FilterFunc([oper, term](TorrentStatus const& ts) { return Compare(ts.totalWanted, term, oper); });
        }

        if (ref == "status" && value.is<std::string>())
        {
            std::string term = value;

            return FilterFunc(
                [oper, term](TorrentStatus const& ts)
                {
                    if (term == "error" && ts.state == TorrentStatus::State::Error) return true;
                    if (term == "downloading")
                    {
                        return ts.state == TorrentStatus::State::Downloading
                            || ts.state == TorrentStatus::State::DownloadingChecking
                            || ts.state == TorrentStatus::State::DownloadingMetadata
                            || ts.state == TorrentStatus::State::DownloadingPaused
                            || ts.state == TorrentStatus::State::DownloadingQueued;
                    }
                    if (term == "paused")
                    {
                        return ts.state == TorrentStatus::State::DownloadingPaused
                            || ts.state == TorrentStatus::State::UploadingPaused;
                    }
                    if (term == "queued")
                    {
                        return ts.state == TorrentStatus::State::DownloadingQueued
                            || ts.state == TorrentStatus::State::UploadingQueued;
                    }
                    if (term == "seeding")
                    {
                        return ts.state == TorrentStatus::State::Uploading;
                    }
                    if (term == "uploading")
                    {
                        return ts.state == TorrentStatus::State::Uploading
                            || ts.state == TorrentStatus::State::UploadingPaused
                            || ts.state == TorrentStatus::State::UploadingQueued;
                    }
                    return false;
                });
        }

        throw QueryException(
            "Unknown field: '" + ref + "'",
            ctx->getStart()->getLine(),
            ctx->getStart()->getCharPositionInLine());
    }

    virtual antlrcpp::Any visitPredicateExpression(pt::PQL::QueryParser::PredicateExpressionContext* ctx) override
    {
        return this->visit(ctx->predicate());
    }

    virtual antlrcpp::Any visitReference(pt::PQL::QueryParser::ReferenceContext* ctx) override
    {
        return ctx->getText();
    }

    virtual antlrcpp::Any visitValue(pt::PQL::QueryParser::ValueContext* ctx) override
    {
        if (auto val = ctx->FLOAT())
        {
            return std::stof(val->getText());
        }

        if (auto val = ctx->INT())
        {
            long long multiplier = 1;

            if (auto suffix = ctx->SIZE_SUFFIX())
            {
                std::string suffixString = suffix->getText();
                if (suffixString == "kb" || suffixString == "kbps") { multiplier = 1024; }
                if (suffixString == "mb" || suffixString == "mbps") { multiplier = 1048576; }
                if (suffixString == "gb" || suffixString == "gbps") { multiplier = 1073741824; }
            }

            return std::stoll(val->getText()) * multiplier;
        }

        if (auto val = ctx->STRING())
        {
            std::string text = val->getText();
            if (text.size() > 0 && text[0] == '\"') { text = text.substr(1); }
            if (text.size() > 0 && text[text.size() - 1] == '\"') { text = text.substr(0, text.size() - 1); }
            return text;
        }

        throw QueryException(
            "Unknown value: '" + ctx->getText() + "'",
            ctx->getStart()->getLine(),
            ctx->getStart()->getCharPositionInLine());
    }
};

PqlTorrentFilter::PqlTorrentFilter(std::function<bool(BitTorrent::TorrentStatus const&)> const& filter)
    : m_filter(filter)
{
}

PqlTorrentFilter::~PqlTorrentFilter()
{
}

std::unique_ptr<pt::UI::Filters::TorrentFilter> PqlTorrentFilter::Create(std::string const& input, std::string* error)
{
    antlr4::ANTLRInputStream inputStream(input);

    pt::PQL::QueryLexer lexer(&inputStream);
    lexer.removeErrorListeners();
    lexer.addErrorListener(new ExceptionErrorListener());

    antlr4::CommonTokenStream tokens(&lexer);

    pt::PQL::QueryParser parser(&tokens);
    parser.removeErrorListeners();
    parser.addErrorListener(new ExceptionErrorListener());

    try
    {
        FilterVisitor visitor;
        FilterFunc func = visitor.visitFilter(parser.filter());

        return std::unique_ptr<TorrentFilter>(new PqlTorrentFilter(func));
    }
    catch (antlr4::ParseCancellationException const& ex)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to parse query: " << ex.what();
        *error = ex.what();
    }
    catch (QueryException const& ex)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to execute query: " << ex.what();
        *error = ex.what();
    }

    return nullptr;
}

bool PqlTorrentFilter::Includes(BitTorrent::TorrentHandle const& torrent)
{
    TorrentStatus ts = torrent.Status();
    return m_filter(ts);
}
