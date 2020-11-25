
// Generated from .\Query.g4 by ANTLR 4.9

#pragma once


#include "antlr4-runtime.h"


namespace pt::PQL {


class  QueryLexer : public antlr4::Lexer {
public:
  enum {
    AND = 1, OR = 2, EQ = 3, CONTAINS = 4, GT = 5, GTE = 6, LT = 7, LTE = 8, 
    WS = 9, TYPE_INT = 10, TYPE_FLOAT = 11, TYPE_STRING = 12, UNIT_SIZE = 13, 
    UNIT_SPEED = 14, ID = 15
  };

  explicit QueryLexer(antlr4::CharStream *input);
  ~QueryLexer();

  virtual std::string getGrammarFileName() const override;
  virtual const std::vector<std::string>& getRuleNames() const override;

  virtual const std::vector<std::string>& getChannelNames() const override;
  virtual const std::vector<std::string>& getModeNames() const override;
  virtual const std::vector<std::string>& getTokenNames() const override; // deprecated, use vocabulary instead
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;

  virtual const std::vector<uint16_t> getSerializedATN() const override;
  virtual const antlr4::atn::ATN& getATN() const override;

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;
  static std::vector<std::string> _channelNames;
  static std::vector<std::string> _modeNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

}  // namespace pt::PQL
