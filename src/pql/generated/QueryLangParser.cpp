
// Generated from .\QueryLang.g4 by ANTLR 4.8


#include "QueryLangVisitor.h"

#include "QueryLangParser.h"


using namespace antlrcpp;
using namespace pt::PQL;
using namespace antlr4;

QueryLangParser::QueryLangParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

QueryLangParser::~QueryLangParser() {
  delete _interpreter;
}

std::string QueryLangParser::getGrammarFileName() const {
  return "QueryLang.g4";
}

const std::vector<std::string>& QueryLangParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& QueryLangParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- FilterContext ------------------------------------------------------------------

QueryLangParser::FilterContext::FilterContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

QueryLangParser::ExpressionContext* QueryLangParser::FilterContext::expression() {
  return getRuleContext<QueryLangParser::ExpressionContext>(0);
}


size_t QueryLangParser::FilterContext::getRuleIndex() const {
  return QueryLangParser::RuleFilter;
}


antlrcpp::Any QueryLangParser::FilterContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryLangVisitor*>(visitor))
    return parserVisitor->visitFilter(this);
  else
    return visitor->visitChildren(this);
}

QueryLangParser::FilterContext* QueryLangParser::filter() {
  FilterContext *_localctx = _tracker.createInstance<FilterContext>(_ctx, getState());
  enterRule(_localctx, 0, QueryLangParser::RuleFilter);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(12);
    expression(0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionContext ------------------------------------------------------------------

QueryLangParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t QueryLangParser::ExpressionContext::getRuleIndex() const {
  return QueryLangParser::RuleExpression;
}

void QueryLangParser::ExpressionContext::copyFrom(ExpressionContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- AndExpressionContext ------------------------------------------------------------------

std::vector<QueryLangParser::ExpressionContext *> QueryLangParser::AndExpressionContext::expression() {
  return getRuleContexts<QueryLangParser::ExpressionContext>();
}

QueryLangParser::ExpressionContext* QueryLangParser::AndExpressionContext::expression(size_t i) {
  return getRuleContext<QueryLangParser::ExpressionContext>(i);
}

tree::TerminalNode* QueryLangParser::AndExpressionContext::AND() {
  return getToken(QueryLangParser::AND, 0);
}

QueryLangParser::AndExpressionContext::AndExpressionContext(ExpressionContext *ctx) { copyFrom(ctx); }


antlrcpp::Any QueryLangParser::AndExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryLangVisitor*>(visitor))
    return parserVisitor->visitAndExpression(this);
  else
    return visitor->visitChildren(this);
}
//----------------- PredicateExpressionContext ------------------------------------------------------------------

QueryLangParser::PredicateContext* QueryLangParser::PredicateExpressionContext::predicate() {
  return getRuleContext<QueryLangParser::PredicateContext>(0);
}

QueryLangParser::PredicateExpressionContext::PredicateExpressionContext(ExpressionContext *ctx) { copyFrom(ctx); }


antlrcpp::Any QueryLangParser::PredicateExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryLangVisitor*>(visitor))
    return parserVisitor->visitPredicateExpression(this);
  else
    return visitor->visitChildren(this);
}
//----------------- OrExpressionContext ------------------------------------------------------------------

std::vector<QueryLangParser::ExpressionContext *> QueryLangParser::OrExpressionContext::expression() {
  return getRuleContexts<QueryLangParser::ExpressionContext>();
}

QueryLangParser::ExpressionContext* QueryLangParser::OrExpressionContext::expression(size_t i) {
  return getRuleContext<QueryLangParser::ExpressionContext>(i);
}

tree::TerminalNode* QueryLangParser::OrExpressionContext::OR() {
  return getToken(QueryLangParser::OR, 0);
}

QueryLangParser::OrExpressionContext::OrExpressionContext(ExpressionContext *ctx) { copyFrom(ctx); }


antlrcpp::Any QueryLangParser::OrExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryLangVisitor*>(visitor))
    return parserVisitor->visitOrExpression(this);
  else
    return visitor->visitChildren(this);
}

QueryLangParser::ExpressionContext* QueryLangParser::expression() {
   return expression(0);
}

QueryLangParser::ExpressionContext* QueryLangParser::expression(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  QueryLangParser::ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, parentState);
  QueryLangParser::ExpressionContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 2;
  enterRecursionRule(_localctx, 2, QueryLangParser::RuleExpression, precedence);

    

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    _localctx = _tracker.createInstance<PredicateExpressionContext>(_localctx);
    _ctx = _localctx;
    previousContext = _localctx;

    setState(15);
    predicate();
    _ctx->stop = _input->LT(-1);
    setState(25);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(23);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<AndExpressionContext>(_tracker.createInstance<ExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpression);
          setState(17);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(18);
          match(QueryLangParser::AND);
          setState(19);
          expression(4);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<OrExpressionContext>(_tracker.createInstance<ExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpression);
          setState(20);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(21);
          match(QueryLangParser::OR);
          setState(22);
          expression(3);
          break;
        }

        } 
      }
      setState(27);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- ReferenceContext ------------------------------------------------------------------

QueryLangParser::ReferenceContext::ReferenceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QueryLangParser::ReferenceContext::ID() {
  return getToken(QueryLangParser::ID, 0);
}


size_t QueryLangParser::ReferenceContext::getRuleIndex() const {
  return QueryLangParser::RuleReference;
}


antlrcpp::Any QueryLangParser::ReferenceContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryLangVisitor*>(visitor))
    return parserVisitor->visitReference(this);
  else
    return visitor->visitChildren(this);
}

QueryLangParser::ReferenceContext* QueryLangParser::reference() {
  ReferenceContext *_localctx = _tracker.createInstance<ReferenceContext>(_ctx, getState());
  enterRule(_localctx, 4, QueryLangParser::RuleReference);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(28);
    match(QueryLangParser::ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PredicateContext ------------------------------------------------------------------

QueryLangParser::PredicateContext::PredicateContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t QueryLangParser::PredicateContext::getRuleIndex() const {
  return QueryLangParser::RulePredicate;
}

void QueryLangParser::PredicateContext::copyFrom(PredicateContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- OperatorPredicateContext ------------------------------------------------------------------

QueryLangParser::ReferenceContext* QueryLangParser::OperatorPredicateContext::reference() {
  return getRuleContext<QueryLangParser::ReferenceContext>(0);
}

QueryLangParser::OperContext* QueryLangParser::OperatorPredicateContext::oper() {
  return getRuleContext<QueryLangParser::OperContext>(0);
}

QueryLangParser::ValueContext* QueryLangParser::OperatorPredicateContext::value() {
  return getRuleContext<QueryLangParser::ValueContext>(0);
}

QueryLangParser::OperatorPredicateContext::OperatorPredicateContext(PredicateContext *ctx) { copyFrom(ctx); }


antlrcpp::Any QueryLangParser::OperatorPredicateContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryLangVisitor*>(visitor))
    return parserVisitor->visitOperatorPredicate(this);
  else
    return visitor->visitChildren(this);
}
QueryLangParser::PredicateContext* QueryLangParser::predicate() {
  PredicateContext *_localctx = _tracker.createInstance<PredicateContext>(_ctx, getState());
  enterRule(_localctx, 6, QueryLangParser::RulePredicate);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    _localctx = dynamic_cast<PredicateContext *>(_tracker.createInstance<QueryLangParser::OperatorPredicateContext>(_localctx));
    enterOuterAlt(_localctx, 1);
    setState(30);
    reference();
    setState(31);
    oper();
    setState(32);
    value();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OperContext ------------------------------------------------------------------

QueryLangParser::OperContext::OperContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QueryLangParser::OperContext::EQ() {
  return getToken(QueryLangParser::EQ, 0);
}

tree::TerminalNode* QueryLangParser::OperContext::CONTAINS() {
  return getToken(QueryLangParser::CONTAINS, 0);
}

tree::TerminalNode* QueryLangParser::OperContext::GREATER_THAN() {
  return getToken(QueryLangParser::GREATER_THAN, 0);
}


size_t QueryLangParser::OperContext::getRuleIndex() const {
  return QueryLangParser::RuleOper;
}


antlrcpp::Any QueryLangParser::OperContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryLangVisitor*>(visitor))
    return parserVisitor->visitOper(this);
  else
    return visitor->visitChildren(this);
}

QueryLangParser::OperContext* QueryLangParser::oper() {
  OperContext *_localctx = _tracker.createInstance<OperContext>(_ctx, getState());
  enterRule(_localctx, 8, QueryLangParser::RuleOper);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(34);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << QueryLangParser::EQ)
      | (1ULL << QueryLangParser::CONTAINS)
      | (1ULL << QueryLangParser::GREATER_THAN))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ValueContext ------------------------------------------------------------------

QueryLangParser::ValueContext::ValueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QueryLangParser::ValueContext::INT() {
  return getToken(QueryLangParser::INT, 0);
}

tree::TerminalNode* QueryLangParser::ValueContext::DOUBLE() {
  return getToken(QueryLangParser::DOUBLE, 0);
}

tree::TerminalNode* QueryLangParser::ValueContext::STRING() {
  return getToken(QueryLangParser::STRING, 0);
}


size_t QueryLangParser::ValueContext::getRuleIndex() const {
  return QueryLangParser::RuleValue;
}


antlrcpp::Any QueryLangParser::ValueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryLangVisitor*>(visitor))
    return parserVisitor->visitValue(this);
  else
    return visitor->visitChildren(this);
}

QueryLangParser::ValueContext* QueryLangParser::value() {
  ValueContext *_localctx = _tracker.createInstance<ValueContext>(_ctx, getState());
  enterRule(_localctx, 10, QueryLangParser::RuleValue);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(36);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << QueryLangParser::INT)
      | (1ULL << QueryLangParser::DOUBLE)
      | (1ULL << QueryLangParser::STRING))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool QueryLangParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 1: return expressionSempred(dynamic_cast<ExpressionContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool QueryLangParser::expressionSempred(ExpressionContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 3);
    case 1: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> QueryLangParser::_decisionToDFA;
atn::PredictionContextCache QueryLangParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN QueryLangParser::_atn;
std::vector<uint16_t> QueryLangParser::_serializedATN;

std::vector<std::string> QueryLangParser::_ruleNames = {
  "filter", "expression", "reference", "predicate", "oper", "value"
};

std::vector<std::string> QueryLangParser::_literalNames = {
  "", "'and'", "'or'", "'='", "'~'", "'>'"
};

std::vector<std::string> QueryLangParser::_symbolicNames = {
  "", "AND", "OR", "EQ", "CONTAINS", "GREATER_THAN", "WS", "INT", "DOUBLE", 
  "STRING", "ID"
};

dfa::Vocabulary QueryLangParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> QueryLangParser::_tokenNames;

QueryLangParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  _serializedATN = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
    0x3, 0xc, 0x29, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x3, 
    0x2, 0x3, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x7, 0x3, 0x1a, 0xa, 0x3, 0xc, 0x3, 
    0xe, 0x3, 0x1d, 0xb, 0x3, 0x3, 0x4, 0x3, 0x4, 0x3, 0x5, 0x3, 0x5, 0x3, 
    0x5, 0x3, 0x5, 0x3, 0x6, 0x3, 0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x2, 
    0x3, 0x4, 0x8, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0x2, 0x4, 0x3, 0x2, 0x5, 
    0x7, 0x3, 0x2, 0x9, 0xb, 0x2, 0x24, 0x2, 0xe, 0x3, 0x2, 0x2, 0x2, 0x4, 
    0x10, 0x3, 0x2, 0x2, 0x2, 0x6, 0x1e, 0x3, 0x2, 0x2, 0x2, 0x8, 0x20, 
    0x3, 0x2, 0x2, 0x2, 0xa, 0x24, 0x3, 0x2, 0x2, 0x2, 0xc, 0x26, 0x3, 0x2, 
    0x2, 0x2, 0xe, 0xf, 0x5, 0x4, 0x3, 0x2, 0xf, 0x3, 0x3, 0x2, 0x2, 0x2, 
    0x10, 0x11, 0x8, 0x3, 0x1, 0x2, 0x11, 0x12, 0x5, 0x8, 0x5, 0x2, 0x12, 
    0x1b, 0x3, 0x2, 0x2, 0x2, 0x13, 0x14, 0xc, 0x5, 0x2, 0x2, 0x14, 0x15, 
    0x7, 0x3, 0x2, 0x2, 0x15, 0x1a, 0x5, 0x4, 0x3, 0x6, 0x16, 0x17, 0xc, 
    0x4, 0x2, 0x2, 0x17, 0x18, 0x7, 0x4, 0x2, 0x2, 0x18, 0x1a, 0x5, 0x4, 
    0x3, 0x5, 0x19, 0x13, 0x3, 0x2, 0x2, 0x2, 0x19, 0x16, 0x3, 0x2, 0x2, 
    0x2, 0x1a, 0x1d, 0x3, 0x2, 0x2, 0x2, 0x1b, 0x19, 0x3, 0x2, 0x2, 0x2, 
    0x1b, 0x1c, 0x3, 0x2, 0x2, 0x2, 0x1c, 0x5, 0x3, 0x2, 0x2, 0x2, 0x1d, 
    0x1b, 0x3, 0x2, 0x2, 0x2, 0x1e, 0x1f, 0x7, 0xc, 0x2, 0x2, 0x1f, 0x7, 
    0x3, 0x2, 0x2, 0x2, 0x20, 0x21, 0x5, 0x6, 0x4, 0x2, 0x21, 0x22, 0x5, 
    0xa, 0x6, 0x2, 0x22, 0x23, 0x5, 0xc, 0x7, 0x2, 0x23, 0x9, 0x3, 0x2, 
    0x2, 0x2, 0x24, 0x25, 0x9, 0x2, 0x2, 0x2, 0x25, 0xb, 0x3, 0x2, 0x2, 
    0x2, 0x26, 0x27, 0x9, 0x3, 0x2, 0x2, 0x27, 0xd, 0x3, 0x2, 0x2, 0x2, 
    0x4, 0x19, 0x1b, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

QueryLangParser::Initializer QueryLangParser::_init;
