
// Generated from .\Query.g4 by ANTLR 4.9


#include "QueryVisitor.h"

#include "QueryParser.h"


using namespace antlrcpp;
using namespace pt::PQL;
using namespace antlr4;

QueryParser::QueryParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

QueryParser::~QueryParser() {
  delete _interpreter;
}

std::string QueryParser::getGrammarFileName() const {
  return "Query.g4";
}

const std::vector<std::string>& QueryParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& QueryParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- FilterContext ------------------------------------------------------------------

QueryParser::FilterContext::FilterContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

QueryParser::ExpressionContext* QueryParser::FilterContext::expression() {
  return getRuleContext<QueryParser::ExpressionContext>(0);
}


size_t QueryParser::FilterContext::getRuleIndex() const {
  return QueryParser::RuleFilter;
}


antlrcpp::Any QueryParser::FilterContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryVisitor*>(visitor))
    return parserVisitor->visitFilter(this);
  else
    return visitor->visitChildren(this);
}

QueryParser::FilterContext* QueryParser::filter() {
  FilterContext *_localctx = _tracker.createInstance<FilterContext>(_ctx, getState());
  enterRule(_localctx, 0, QueryParser::RuleFilter);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
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

QueryParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t QueryParser::ExpressionContext::getRuleIndex() const {
  return QueryParser::RuleExpression;
}

void QueryParser::ExpressionContext::copyFrom(ExpressionContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- AndExpressionContext ------------------------------------------------------------------

std::vector<QueryParser::ExpressionContext *> QueryParser::AndExpressionContext::expression() {
  return getRuleContexts<QueryParser::ExpressionContext>();
}

QueryParser::ExpressionContext* QueryParser::AndExpressionContext::expression(size_t i) {
  return getRuleContext<QueryParser::ExpressionContext>(i);
}

tree::TerminalNode* QueryParser::AndExpressionContext::AND() {
  return getToken(QueryParser::AND, 0);
}

QueryParser::AndExpressionContext::AndExpressionContext(ExpressionContext *ctx) { copyFrom(ctx); }


antlrcpp::Any QueryParser::AndExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryVisitor*>(visitor))
    return parserVisitor->visitAndExpression(this);
  else
    return visitor->visitChildren(this);
}
//----------------- PredicateExpressionContext ------------------------------------------------------------------

QueryParser::PredicateContext* QueryParser::PredicateExpressionContext::predicate() {
  return getRuleContext<QueryParser::PredicateContext>(0);
}

QueryParser::PredicateExpressionContext::PredicateExpressionContext(ExpressionContext *ctx) { copyFrom(ctx); }


antlrcpp::Any QueryParser::PredicateExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryVisitor*>(visitor))
    return parserVisitor->visitPredicateExpression(this);
  else
    return visitor->visitChildren(this);
}
//----------------- OrExpressionContext ------------------------------------------------------------------

std::vector<QueryParser::ExpressionContext *> QueryParser::OrExpressionContext::expression() {
  return getRuleContexts<QueryParser::ExpressionContext>();
}

QueryParser::ExpressionContext* QueryParser::OrExpressionContext::expression(size_t i) {
  return getRuleContext<QueryParser::ExpressionContext>(i);
}

tree::TerminalNode* QueryParser::OrExpressionContext::OR() {
  return getToken(QueryParser::OR, 0);
}

QueryParser::OrExpressionContext::OrExpressionContext(ExpressionContext *ctx) { copyFrom(ctx); }


antlrcpp::Any QueryParser::OrExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryVisitor*>(visitor))
    return parserVisitor->visitOrExpression(this);
  else
    return visitor->visitChildren(this);
}

QueryParser::ExpressionContext* QueryParser::expression() {
   return expression(0);
}

QueryParser::ExpressionContext* QueryParser::expression(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  QueryParser::ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, parentState);
  QueryParser::ExpressionContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 2;
  enterRecursionRule(_localctx, 2, QueryParser::RuleExpression, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
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
          match(QueryParser::AND);
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
          match(QueryParser::OR);
          setState(22);
          expression(3);
          break;
        }

        default:
          break;
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

QueryParser::ReferenceContext::ReferenceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QueryParser::ReferenceContext::ID() {
  return getToken(QueryParser::ID, 0);
}


size_t QueryParser::ReferenceContext::getRuleIndex() const {
  return QueryParser::RuleReference;
}


antlrcpp::Any QueryParser::ReferenceContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryVisitor*>(visitor))
    return parserVisitor->visitReference(this);
  else
    return visitor->visitChildren(this);
}

QueryParser::ReferenceContext* QueryParser::reference() {
  ReferenceContext *_localctx = _tracker.createInstance<ReferenceContext>(_ctx, getState());
  enterRule(_localctx, 4, QueryParser::RuleReference);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(28);
    match(QueryParser::ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PredicateContext ------------------------------------------------------------------

QueryParser::PredicateContext::PredicateContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t QueryParser::PredicateContext::getRuleIndex() const {
  return QueryParser::RulePredicate;
}

void QueryParser::PredicateContext::copyFrom(PredicateContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- OperatorPredicateContext ------------------------------------------------------------------

QueryParser::ReferenceContext* QueryParser::OperatorPredicateContext::reference() {
  return getRuleContext<QueryParser::ReferenceContext>(0);
}

QueryParser::OperContext* QueryParser::OperatorPredicateContext::oper() {
  return getRuleContext<QueryParser::OperContext>(0);
}

QueryParser::ValueContext* QueryParser::OperatorPredicateContext::value() {
  return getRuleContext<QueryParser::ValueContext>(0);
}

QueryParser::OperatorPredicateContext::OperatorPredicateContext(PredicateContext *ctx) { copyFrom(ctx); }


antlrcpp::Any QueryParser::OperatorPredicateContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryVisitor*>(visitor))
    return parserVisitor->visitOperatorPredicate(this);
  else
    return visitor->visitChildren(this);
}
QueryParser::PredicateContext* QueryParser::predicate() {
  PredicateContext *_localctx = _tracker.createInstance<PredicateContext>(_ctx, getState());
  enterRule(_localctx, 6, QueryParser::RulePredicate);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    _localctx = dynamic_cast<PredicateContext *>(_tracker.createInstance<QueryParser::OperatorPredicateContext>(_localctx));
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

QueryParser::OperContext::OperContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QueryParser::OperContext::EQ() {
  return getToken(QueryParser::EQ, 0);
}

tree::TerminalNode* QueryParser::OperContext::CONTAINS() {
  return getToken(QueryParser::CONTAINS, 0);
}

tree::TerminalNode* QueryParser::OperContext::GT() {
  return getToken(QueryParser::GT, 0);
}

tree::TerminalNode* QueryParser::OperContext::GTE() {
  return getToken(QueryParser::GTE, 0);
}

tree::TerminalNode* QueryParser::OperContext::LT() {
  return getToken(QueryParser::LT, 0);
}

tree::TerminalNode* QueryParser::OperContext::LTE() {
  return getToken(QueryParser::LTE, 0);
}


size_t QueryParser::OperContext::getRuleIndex() const {
  return QueryParser::RuleOper;
}


antlrcpp::Any QueryParser::OperContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryVisitor*>(visitor))
    return parserVisitor->visitOper(this);
  else
    return visitor->visitChildren(this);
}

QueryParser::OperContext* QueryParser::oper() {
  OperContext *_localctx = _tracker.createInstance<OperContext>(_ctx, getState());
  enterRule(_localctx, 8, QueryParser::RuleOper);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(34);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << QueryParser::EQ)
      | (1ULL << QueryParser::CONTAINS)
      | (1ULL << QueryParser::GT)
      | (1ULL << QueryParser::GTE)
      | (1ULL << QueryParser::LT)
      | (1ULL << QueryParser::LTE))) != 0))) {
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

QueryParser::ValueContext::ValueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QueryParser::ValueContext::TYPE_INT() {
  return getToken(QueryParser::TYPE_INT, 0);
}

tree::TerminalNode* QueryParser::ValueContext::WS() {
  return getToken(QueryParser::WS, 0);
}

tree::TerminalNode* QueryParser::ValueContext::UNIT_SIZE() {
  return getToken(QueryParser::UNIT_SIZE, 0);
}

tree::TerminalNode* QueryParser::ValueContext::UNIT_SPEED() {
  return getToken(QueryParser::UNIT_SPEED, 0);
}

tree::TerminalNode* QueryParser::ValueContext::TYPE_FLOAT() {
  return getToken(QueryParser::TYPE_FLOAT, 0);
}

tree::TerminalNode* QueryParser::ValueContext::TYPE_STRING() {
  return getToken(QueryParser::TYPE_STRING, 0);
}


size_t QueryParser::ValueContext::getRuleIndex() const {
  return QueryParser::RuleValue;
}


antlrcpp::Any QueryParser::ValueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<QueryVisitor*>(visitor))
    return parserVisitor->visitValue(this);
  else
    return visitor->visitChildren(this);
}

QueryParser::ValueContext* QueryParser::value() {
  ValueContext *_localctx = _tracker.createInstance<ValueContext>(_ctx, getState());
  enterRule(_localctx, 10, QueryParser::RuleValue);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(65);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 10, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(36);
      match(QueryParser::TYPE_INT);
      setState(38);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx)) {
      case 1: {
        setState(37);
        match(QueryParser::WS);
        break;
      }

      default:
        break;
      }
      setState(41);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx)) {
      case 1: {
        setState(40);
        match(QueryParser::UNIT_SIZE);
        break;
      }

      default:
        break;
      }
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(43);
      match(QueryParser::TYPE_INT);
      setState(45);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
      case 1: {
        setState(44);
        match(QueryParser::WS);
        break;
      }

      default:
        break;
      }
      setState(48);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx)) {
      case 1: {
        setState(47);
        match(QueryParser::UNIT_SPEED);
        break;
      }

      default:
        break;
      }
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(50);
      match(QueryParser::TYPE_FLOAT);
      setState(52);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx)) {
      case 1: {
        setState(51);
        match(QueryParser::WS);
        break;
      }

      default:
        break;
      }
      setState(55);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
      case 1: {
        setState(54);
        match(QueryParser::UNIT_SIZE);
        break;
      }

      default:
        break;
      }
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(57);
      match(QueryParser::TYPE_FLOAT);
      setState(59);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx)) {
      case 1: {
        setState(58);
        match(QueryParser::WS);
        break;
      }

      default:
        break;
      }
      setState(62);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
      case 1: {
        setState(61);
        match(QueryParser::UNIT_SPEED);
        break;
      }

      default:
        break;
      }
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(64);
      match(QueryParser::TYPE_STRING);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool QueryParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 1: return expressionSempred(dynamic_cast<ExpressionContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool QueryParser::expressionSempred(ExpressionContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 3);
    case 1: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> QueryParser::_decisionToDFA;
atn::PredictionContextCache QueryParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN QueryParser::_atn;
std::vector<uint16_t> QueryParser::_serializedATN;

std::vector<std::string> QueryParser::_ruleNames = {
  "filter", "expression", "reference", "predicate", "oper", "value"
};

std::vector<std::string> QueryParser::_literalNames = {
  "", "'and'", "'or'", "'='", "'~'", "'>'", "'>='", "'<'", "'<='"
};

std::vector<std::string> QueryParser::_symbolicNames = {
  "", "AND", "OR", "EQ", "CONTAINS", "GT", "GTE", "LT", "LTE", "WS", "TYPE_INT", 
  "TYPE_FLOAT", "TYPE_STRING", "UNIT_SIZE", "UNIT_SPEED", "ID"
};

dfa::Vocabulary QueryParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> QueryParser::_tokenNames;

QueryParser::Initializer::Initializer() {
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
    0x3, 0x11, 0x46, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x3, 
    0x2, 0x3, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x7, 0x3, 0x1a, 0xa, 0x3, 0xc, 0x3, 
    0xe, 0x3, 0x1d, 0xb, 0x3, 0x3, 0x4, 0x3, 0x4, 0x3, 0x5, 0x3, 0x5, 0x3, 
    0x5, 0x3, 0x5, 0x3, 0x6, 0x3, 0x6, 0x3, 0x7, 0x3, 0x7, 0x5, 0x7, 0x29, 
    0xa, 0x7, 0x3, 0x7, 0x5, 0x7, 0x2c, 0xa, 0x7, 0x3, 0x7, 0x3, 0x7, 0x5, 
    0x7, 0x30, 0xa, 0x7, 0x3, 0x7, 0x5, 0x7, 0x33, 0xa, 0x7, 0x3, 0x7, 0x3, 
    0x7, 0x5, 0x7, 0x37, 0xa, 0x7, 0x3, 0x7, 0x5, 0x7, 0x3a, 0xa, 0x7, 0x3, 
    0x7, 0x3, 0x7, 0x5, 0x7, 0x3e, 0xa, 0x7, 0x3, 0x7, 0x5, 0x7, 0x41, 0xa, 
    0x7, 0x3, 0x7, 0x5, 0x7, 0x44, 0xa, 0x7, 0x3, 0x7, 0x2, 0x3, 0x4, 0x8, 
    0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0x2, 0x3, 0x3, 0x2, 0x5, 0xa, 0x2, 0x4d, 
    0x2, 0xe, 0x3, 0x2, 0x2, 0x2, 0x4, 0x10, 0x3, 0x2, 0x2, 0x2, 0x6, 0x1e, 
    0x3, 0x2, 0x2, 0x2, 0x8, 0x20, 0x3, 0x2, 0x2, 0x2, 0xa, 0x24, 0x3, 0x2, 
    0x2, 0x2, 0xc, 0x43, 0x3, 0x2, 0x2, 0x2, 0xe, 0xf, 0x5, 0x4, 0x3, 0x2, 
    0xf, 0x3, 0x3, 0x2, 0x2, 0x2, 0x10, 0x11, 0x8, 0x3, 0x1, 0x2, 0x11, 
    0x12, 0x5, 0x8, 0x5, 0x2, 0x12, 0x1b, 0x3, 0x2, 0x2, 0x2, 0x13, 0x14, 
    0xc, 0x5, 0x2, 0x2, 0x14, 0x15, 0x7, 0x3, 0x2, 0x2, 0x15, 0x1a, 0x5, 
    0x4, 0x3, 0x6, 0x16, 0x17, 0xc, 0x4, 0x2, 0x2, 0x17, 0x18, 0x7, 0x4, 
    0x2, 0x2, 0x18, 0x1a, 0x5, 0x4, 0x3, 0x5, 0x19, 0x13, 0x3, 0x2, 0x2, 
    0x2, 0x19, 0x16, 0x3, 0x2, 0x2, 0x2, 0x1a, 0x1d, 0x3, 0x2, 0x2, 0x2, 
    0x1b, 0x19, 0x3, 0x2, 0x2, 0x2, 0x1b, 0x1c, 0x3, 0x2, 0x2, 0x2, 0x1c, 
    0x5, 0x3, 0x2, 0x2, 0x2, 0x1d, 0x1b, 0x3, 0x2, 0x2, 0x2, 0x1e, 0x1f, 
    0x7, 0x11, 0x2, 0x2, 0x1f, 0x7, 0x3, 0x2, 0x2, 0x2, 0x20, 0x21, 0x5, 
    0x6, 0x4, 0x2, 0x21, 0x22, 0x5, 0xa, 0x6, 0x2, 0x22, 0x23, 0x5, 0xc, 
    0x7, 0x2, 0x23, 0x9, 0x3, 0x2, 0x2, 0x2, 0x24, 0x25, 0x9, 0x2, 0x2, 
    0x2, 0x25, 0xb, 0x3, 0x2, 0x2, 0x2, 0x26, 0x28, 0x7, 0xc, 0x2, 0x2, 
    0x27, 0x29, 0x7, 0xb, 0x2, 0x2, 0x28, 0x27, 0x3, 0x2, 0x2, 0x2, 0x28, 
    0x29, 0x3, 0x2, 0x2, 0x2, 0x29, 0x2b, 0x3, 0x2, 0x2, 0x2, 0x2a, 0x2c, 
    0x7, 0xf, 0x2, 0x2, 0x2b, 0x2a, 0x3, 0x2, 0x2, 0x2, 0x2b, 0x2c, 0x3, 
    0x2, 0x2, 0x2, 0x2c, 0x44, 0x3, 0x2, 0x2, 0x2, 0x2d, 0x2f, 0x7, 0xc, 
    0x2, 0x2, 0x2e, 0x30, 0x7, 0xb, 0x2, 0x2, 0x2f, 0x2e, 0x3, 0x2, 0x2, 
    0x2, 0x2f, 0x30, 0x3, 0x2, 0x2, 0x2, 0x30, 0x32, 0x3, 0x2, 0x2, 0x2, 
    0x31, 0x33, 0x7, 0x10, 0x2, 0x2, 0x32, 0x31, 0x3, 0x2, 0x2, 0x2, 0x32, 
    0x33, 0x3, 0x2, 0x2, 0x2, 0x33, 0x44, 0x3, 0x2, 0x2, 0x2, 0x34, 0x36, 
    0x7, 0xd, 0x2, 0x2, 0x35, 0x37, 0x7, 0xb, 0x2, 0x2, 0x36, 0x35, 0x3, 
    0x2, 0x2, 0x2, 0x36, 0x37, 0x3, 0x2, 0x2, 0x2, 0x37, 0x39, 0x3, 0x2, 
    0x2, 0x2, 0x38, 0x3a, 0x7, 0xf, 0x2, 0x2, 0x39, 0x38, 0x3, 0x2, 0x2, 
    0x2, 0x39, 0x3a, 0x3, 0x2, 0x2, 0x2, 0x3a, 0x44, 0x3, 0x2, 0x2, 0x2, 
    0x3b, 0x3d, 0x7, 0xd, 0x2, 0x2, 0x3c, 0x3e, 0x7, 0xb, 0x2, 0x2, 0x3d, 
    0x3c, 0x3, 0x2, 0x2, 0x2, 0x3d, 0x3e, 0x3, 0x2, 0x2, 0x2, 0x3e, 0x40, 
    0x3, 0x2, 0x2, 0x2, 0x3f, 0x41, 0x7, 0x10, 0x2, 0x2, 0x40, 0x3f, 0x3, 
    0x2, 0x2, 0x2, 0x40, 0x41, 0x3, 0x2, 0x2, 0x2, 0x41, 0x44, 0x3, 0x2, 
    0x2, 0x2, 0x42, 0x44, 0x7, 0xe, 0x2, 0x2, 0x43, 0x26, 0x3, 0x2, 0x2, 
    0x2, 0x43, 0x2d, 0x3, 0x2, 0x2, 0x2, 0x43, 0x34, 0x3, 0x2, 0x2, 0x2, 
    0x43, 0x3b, 0x3, 0x2, 0x2, 0x2, 0x43, 0x42, 0x3, 0x2, 0x2, 0x2, 0x44, 
    0xd, 0x3, 0x2, 0x2, 0x2, 0xd, 0x19, 0x1b, 0x28, 0x2b, 0x2f, 0x32, 0x36, 
    0x39, 0x3d, 0x40, 0x43, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

QueryParser::Initializer QueryParser::_init;
