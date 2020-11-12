
// Generated from .\Query.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "QueryVisitor.h"


namespace pt::PQL {

/**
 * This class provides an empty implementation of QueryVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  QueryBaseVisitor : public QueryVisitor {
public:

  virtual antlrcpp::Any visitFilter(QueryParser::FilterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAndExpression(QueryParser::AndExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPredicateExpression(QueryParser::PredicateExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitOrExpression(QueryParser::OrExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitReference(QueryParser::ReferenceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitOperatorPredicate(QueryParser::OperatorPredicateContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitOper(QueryParser::OperContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitValue(QueryParser::ValueContext *ctx) override {
    return visitChildren(ctx);
  }


};

}  // namespace pt::PQL
