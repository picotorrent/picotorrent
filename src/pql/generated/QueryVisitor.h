
// Generated from .\Query.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "QueryParser.h"


namespace pt::PQL {

/**
 * This class defines an abstract visitor for a parse tree
 * produced by QueryParser.
 */
class  QueryVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by QueryParser.
   */
    virtual antlrcpp::Any visitFilter(QueryParser::FilterContext *context) = 0;

    virtual antlrcpp::Any visitAndExpression(QueryParser::AndExpressionContext *context) = 0;

    virtual antlrcpp::Any visitPredicateExpression(QueryParser::PredicateExpressionContext *context) = 0;

    virtual antlrcpp::Any visitOrExpression(QueryParser::OrExpressionContext *context) = 0;

    virtual antlrcpp::Any visitReference(QueryParser::ReferenceContext *context) = 0;

    virtual antlrcpp::Any visitOperatorPredicate(QueryParser::OperatorPredicateContext *context) = 0;

    virtual antlrcpp::Any visitOper(QueryParser::OperContext *context) = 0;

    virtual antlrcpp::Any visitValue(QueryParser::ValueContext *context) = 0;


};

}  // namespace pt::PQL
