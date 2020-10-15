
// Generated from .\QueryLang.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "QueryLangVisitor.h"


namespace pt::PQL {

/**
 * This class provides an empty implementation of QueryLangVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  QueryLangBaseVisitor : public QueryLangVisitor {
public:

  virtual antlrcpp::Any visitFilter(QueryLangParser::FilterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAndExpression(QueryLangParser::AndExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPredicateExpression(QueryLangParser::PredicateExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitOrExpression(QueryLangParser::OrExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitReference(QueryLangParser::ReferenceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitOperatorPredicate(QueryLangParser::OperatorPredicateContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitOper(QueryLangParser::OperContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitValue(QueryLangParser::ValueContext *ctx) override {
    return visitChildren(ctx);
  }


};

}  // namespace pt::PQL
