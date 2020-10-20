#include "torrentconsole.hpp"

#include "ids.hpp"
#include "models/torrentlistmodel.hpp"
#include "torrentlistview.hpp"

#define ERROR_TMP ERROR
#undef ERROR
#include <antlr4-runtime.h>
#include <QueryLangBaseVisitor.h>
#include <QueryLangLexer.h>
#include <QueryLangParser.h>
#define ERROR ERROR_TMP
#undef ERROR_TMP

using pt::UI::TorrentConsole;

class Torrent
{
public:
    std::string name;
    int64_t size;
};

class Filter
{
};

enum class Operator
{
    Unknown,
    GreaterThan
};

typedef std::function<bool(Torrent const&)> func_t;

class FilterVisitor : public pt::PQL::QueryLangBaseVisitor
{
public:
    virtual antlrcpp::Any visitAndExpression(pt::PQL::QueryLangParser::AndExpressionContext* ctx) override
    {
        std::vector<func_t> funcs;

        for (auto expr : ctx->expression())
        {
            funcs.push_back(this->visit(expr));
        }

        return func_t([funcs](Torrent const& t)
        {
            return std::all_of(
                funcs.begin(),
                funcs.end(),
                [t](auto const& f)
                {
                    return f(t);
                });
        });
    }

    virtual antlrcpp::Any visitFilter(pt::PQL::QueryLangParser::FilterContext* ctx) override
    {
        return this->visit(ctx->expression());
    }

    virtual antlrcpp::Any visitOper(pt::PQL::QueryLangParser::OperContext* ctx) override
    {
        std::string oper = ctx->getText();
        if (oper == ">") return Operator::GreaterThan;
        return Operator::Unknown;
    }

    virtual antlrcpp::Any visitOperatorPredicate(pt::PQL::QueryLangParser::OperatorPredicateContext* ctx) override
    {
        std::string ref = this->visit(ctx->reference());
        // Operator oper = this->visit(ctx->oper());

        if (ref == "size")
        {
            int64_t term = this->visit(ctx->value());
            return func_t([term](Torrent const& t) { return t.size > term; });
        }

        if (ref == "name")
        {
            std::string term = this->visit(ctx->value());
            return func_t([term](Torrent const& t) { return t.name == term; });
        }

        throw new std::exception();
    }

    virtual antlrcpp::Any visitPredicateExpression(pt::PQL::QueryLangParser::PredicateExpressionContext* ctx) override
    {
        return this->visit(ctx->predicate());
    }

    virtual antlrcpp::Any visitReference(pt::PQL::QueryLangParser::ReferenceContext* ctx) override
    {
        return ctx->getText();
    }

    virtual antlrcpp::Any visitValue(pt::PQL::QueryLangParser::ValueContext* ctx) override
    {
        if (auto val = ctx->INT())
        {
            return std::stoll(val->getText());
        }

        if (auto val = ctx->STRING())
        {
            std::string text = val->getText();
            if (text.size() > 0 && text[0] == '\"') { text = text.substr(1); }
            if (text.size() > 0 && text[text.size() - 1] == '\"') { text = text.substr(0, text.size() - 1); }
            return text;
        }

        return visitChildren(ctx);
    }
};

class ExceptionErrorListener : public antlr4::BaseErrorListener {
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
        oss << "line: " << line << ":" << charPositionInLine << " " << msg;
        error_msg = oss.str();
        throw antlr4::ParseCancellationException(error_msg);
    }

private:
    std::string error_msg;
};

TorrentConsole::TorrentConsole(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id),
    m_input(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT | wxTE_PROCESS_ENTER))
{
    m_input->SetFont(
        wxFont(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas")));

    wxIcon funnel(L"ICO_FUNNEL", wxBITMAP_TYPE_ICO_RESOURCE, FromDIP(16), FromDIP(16));

    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(new wxStaticBitmap(this, wxID_ANY, funnel), 0, wxALIGN_CENTER);
    sizer->Add(m_input, 1, wxEXPAND | wxALL, FromDIP(2));

    this->SetBackgroundColour(*wxWHITE);
    this->SetSizerAndFit(sizer);

    this->Bind(wxEVT_TEXT_ENTER, &TorrentConsole::CreateFilter, this);
}

void TorrentConsole::CreateFilter(wxCommandEvent&)
{
    antlr4::ANTLRInputStream input(m_input->GetValue().ToStdString());

    pt::PQL::QueryLangLexer lexer(&input);
    lexer.removeErrorListeners();
    lexer.addErrorListener(new ExceptionErrorListener());

    antlr4::CommonTokenStream tokens(&lexer);

    pt::PQL::QueryLangParser parser(&tokens);
    parser.removeErrorListeners();
    parser.addErrorListener(new ExceptionErrorListener());

    try
    {
        FilterVisitor visitor;
        func_t func = visitor.visitFilter(parser.filter());

        Torrent t;
        t.name = "dbn";
        t.size = 1002;

        func(t);
    }
    catch (antlr4::ParseCancellationException const& ex)
    {
        OutputDebugStringA(ex.what());
        OutputDebugStringA("\n");
    }
}
