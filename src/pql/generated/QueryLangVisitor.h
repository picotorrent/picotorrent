
// Generated from .\QueryLang.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "QueryLangParser.h"


namespace pt::PQL {

/**
 * This class defines an abstract visitor for a parse tree
 * produced by QueryLangParser.
 */
class  QueryLangVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by QueryLangParser.
   */
    virtual antlrcpp::Any visitFilter(QueryLangParser::FilterContext *context) = 0;

    virtual antlrcpp::Any visitAndExpression(QueryLangParser::AndExpressionContext *context) = 0;

    virtual antlrcpp::Any visitPredicateExpression(QueryLangParser::PredicateExpressionContext *context) = 0;

    virtual antlrcpp::Any visitOrExpression(QueryLangParser::OrExpressionContext *context) = 0;

    virtual antlrcpp::Any visitReference(QueryLangParser::ReferenceContext *context) = 0;

    virtual antlrcpp::Any visitOperatorPredicate(QueryLangParser::OperatorPredicateContext *context) = 0;

    virtual antlrcpp::Any visitOper(QueryLangParser::OperContext *context) = 0;

    virtual antlrcpp::Any visitValue(QueryLangParser::ValueContext *context) = 0;


};

}  // namespace pt::PQL
