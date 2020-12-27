grammar Query;

AND         : 'and';
OR          : 'or';

// comparison operators
EQ          : '=';
CONTAINS    : '~';
GT          : '>';
GTE         : '>=';
LT          : '<';
LTE         : '<=';

WS          : [ \t\r\n]+ -> skip;
INT         : '-'? [0-9]+ ;
FLOAT       : '-'? [0-9]+'.'[0-9]+;
STRING      : '"' .*? '"';

UNIT_SIZE   : 'kb' | 'mb' | 'gb';
UNIT_SPEED  : 'kbps' | 'mbps' | 'gbps';

ID          : [a-zA-Z]+;

filter
    : expression
    ;

expression
    : expression AND expression #AndExpression
    | expression OR  expression #OrExpression
    | predicate                 #PredicateExpression
    ;

reference: ID;

predicate
    : reference oper value #OperatorPredicate
    ;

oper
    : EQ
    | CONTAINS
    | GT
    | GTE
    | LT
    | LTE
    ;

value
    : INT WS? UNIT_SIZE?
    | INT WS? UNIT_SPEED?
    | FLOAT WS? UNIT_SIZE?
    | FLOAT WS? UNIT_SPEED?
    | STRING
    ;
