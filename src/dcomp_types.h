#ifndef __DCOMP_TYPES_H__INCLUDED
#define __DCOMP_TYPES_H__INCLUDED

#ifndef null
#define null 0
#endif /* null */

typedef int CONST_VALUE_TYPE;

typedef enum
{
  INSTRUCTION_SKIP,
  INSTRUCTION_READ,
  INSTRUCTION_WRITE,
  INSTRUCTION_ASSIGN,
  INSTRUCTION_IF,
  INSTRUCTION_WHILE
} INSTRUCTION_TYPE;

typedef enum
{
  ARITHMETIC_ADD,
  ARITHMETIC_SUB,
  ARITHMETIC_MUL,
  ARITHMETIC_DIV,
  ARITHMETIC_MOD
} ARITHMETIC_OPERATION;

typedef enum
{
  LOGICAL_TRUE,
  LOGICAL_FALSE,
  LOGICAL_NOT,
  LOGICAL_AND,
  LOGICAL_OR
} LOGICAL_OPERATION;

typedef enum
{
  RELATIONAL_EQUALS,
  RELATIONAL_NOT_EQUALS,
  RELATIONAL_LESS_THAN_EQUALS,
  RELATIONAL_GREATER_THAN_EQUALS,
  RELATIONAL_LESS_THAN,
  RELATIONAL_GREATER_THAN
} RELATIONAL_OPERATION;

typedef enum
{
  CONST_VARIABLE,
  MUTABLE_VARIABLE
} VARIABLE_ENTRY_TYPE;

typedef struct
{
  VARIABLE_ENTRY_TYPE type;
  union
  {
	char* variable_name;
	CONST_VALUE_TYPE variable_value;
  };
} T_VARIABLE_TYPE;

typedef T_VARIABLE_TYPE* VARIABLE_TYPE;

typedef enum
{
  SIMPLE_ARITHMETIC_EXPRESSION,
  COMPOSITE_ARITHMETIC_EXPRESSION
} ARITHMETIC_EXPRESSION_ENTRY_TYPE;

typedef struct _T_ARITHMETIC_EXPRESSION
{
  ARITHMETIC_EXPRESSION_ENTRY_TYPE type;
  union
  {
	VARIABLE_TYPE variable;
	struct
	{
	  ARITHMETIC_OPERATION operation;
	  struct _T_ARITHMETIC_EXPRESSION* param1;
	  struct _T_ARITHMETIC_EXPRESSION* param2;
	};
  };
} T_ARITHMETIC_EXPRESSION_TYPE;

typedef T_ARITHMETIC_EXPRESSION_TYPE* ARITHMETIC_EXPRESSION_TYPE;

typedef enum
{
  SIMPLE_LOGICAL_EXPRESSION,
  COMPOSITE_LOGICAL_EXPRESSION
} LOGICAL_EXPRESSION_ENTRY_TYPE;

typedef struct _T_LOGICAL_EXPRESSION
{
  LOGICAL_EXPRESSION_ENTRY_TYPE type;
  union
  {
	VARIABLE_TYPE variable;
	struct
	{
	  LOGICAL_OPERATION operation;
	  struct _T_LOGICAL_EXPRESSION* param1;
	  struct _T_LOGICAL_EXPRESSION* param2;
	};
  };
} T_LOGICAL_EXPRESSION_TYPE;

typedef T_LOGICAL_EXPRESSION_TYPE* LOGICAL_EXPRESSION_TYPE;

typedef struct _T_INSTRUCTION_LIST T_INSTRUCTION_LIST;

typedef T_INSTRUCTION_LIST* INSTRUCTION_LIST;

typedef struct
{
  VARIABLE_TYPE variable;
} INSTRUCTION_READ_PARAMS;

typedef struct
{
  ARITHMETIC_EXPRESSION_TYPE expression;
} INSTRUCTION_WRITE_PARAMS;

typedef struct
{
  VARIABLE_TYPE variable;
  ARITHMETIC_EXPRESSION_TYPE expression;
} INSTRUCTION_ASSIGN_PARAMS;

typedef struct
{
  LOGICAL_EXPRESSION_TYPE expression;
  INSTRUCTION_LIST nested_true_code_block;
  INSTRUCTION_LIST nested_false_code_block;
} INSTRUCTION_IF_PARAMS;

typedef struct
{
  LOGICAL_EXPRESSION_TYPE expression;
  INSTRUCTION_LIST nested_code_block;
} INSTRUCTION_WHILE_PARAMS;

typedef struct
{
  INSTRUCTION_TYPE type;
  union
  {
	INSTRUCTION_READ_PARAMS read_params;
	INSTRUCTION_WRITE_PARAMS write_params;
	INSTRUCTION_ASSIGN_PARAMS assign_params;
	INSTRUCTION_IF_PARAMS if_params;
	INSTRUCTION_WHILE_PARAMS while_params;
  };
} T_INSTRUCTION;

typedef T_INSTRUCTION* INSTRUCTION;

typedef struct _T_INSTRUCTION_LIST_ENTRY
{
  INSTRUCTION instruction;
  struct _T_INSTRUCTION_LIST_ENTRY* next;
} T_INSTRUCTION_LIST_ENTRY;

typedef T_INSTRUCTION_LIST_ENTRY* INSTRUCTION_LIST_ENTRY;

struct _T_INSTRUCTION_LIST
{
  INSTRUCTION_LIST_ENTRY first;
  INSTRUCTION_LIST_ENTRY last;
};

typedef T_INSTRUCTION_LIST_ENTRY* INSTRUCTION_LIST_ITERATOR;

VARIABLE_TYPE createConstantVariable(CONST_VALUE_TYPE value);
VARIABLE_TYPE createVariable(const char* label);
void destroyVariable(VARIABLE_TYPE* variable_ptr);

ARITHMETIC_EXPRESSION_TYPE createSimpleExpression(VARIABLE_TYPE variable);
ARITHMETIC_EXPRESSION_TYPE 
    createCompositeExpression(ARITHMETIC_OPERATION operation,
                              ARITHMETIC_EXPRESSION_TYPE param1,
                              ARITHMETIC_EXPRESSION_TYPE param2);

/* TODO: Logical expressions */

INSTRUCTION createSkipInstruction();
INSTRUCTION createReadInstruction(VARIABLE_TYPE variable);
INSTRUCTION createWriteInstruction(ARITHMETIC_EXPRESSION_TYPE expression);
INSTRUCTION createAssignInstruction(VARIABLE_TYPE variable,
                                    ARITHMETIC_EXPRESSION_TYPE expression);
INSTRUCTION createIfInstruction(LOGICAL_EXPRESSION_TYPE expression,
                                INSTRUCTION_LIST nested_true_code_block,
                                INSTRUCTION_LIST nested_false_code_block);
INSTRUCTION createWhileInstruction(LOGICAL_EXPRESSION_TYPE expression,
                                   INSTRUCTION_LIST nested_code_block);
void destroyInstruction(INSTRUCTION* instruction_ptr);

INSTRUCTION_LIST_ENTRY createInstructionListEntry();
INSTRUCTION getInstruction(const INSTRUCTION_LIST_ENTRY entry);
void setInstruction(INSTRUCTION_LIST_ENTRY entry, const INSTRUCTION instruction);
INSTRUCTION_LIST_ENTRY getNextILEntry(const INSTRUCTION_LIST_ENTRY entry);
void setNextILEntry(INSTRUCTION_LIST_ENTRY entry, const INSTRUCTION_LIST_ENTRY next);
void destroyInstructionListEntry(INSTRUCTION_LIST_ENTRY* entry);

INSTRUCTION_LIST createInstructionList();
void addInstruction(INSTRUCTION_LIST list, const INSTRUCTION instruction);
INSTRUCTION_LIST_ITERATOR getILIterator(const INSTRUCTION_LIST list);
INSTRUCTION_LIST_ITERATOR getILNext(const INSTRUCTION_LIST_ITERATOR iterator);
void destroyInstructionList(INSTRUCTION_LIST* list);

#endif /* __DCOMP_TYPES_H__INCLUDED */