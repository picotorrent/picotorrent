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
TYPE_INT    : '-'? [0-9]+ ;
TYPE_FLOAT  : '-'? [0-9]+'.'[0-9]+;
TYPE_STRING : '"' .*? '"';

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
    : TYPE_INT WS? UNIT_SIZE?
    | TYPE_INT WS? UNIT_SPEED?
    | TYPE_FLOAT WS? UNIT_SIZE?
    | TYPE_FLOAT WS? UNIT_SPEED?
    | TYPE_STRING
    ;
