#ifndef __ASM_TYPES_H__INCLUDED
#define __ASM_TYPES_H__INCLUDED

#ifndef null
#define null 0
#endif /* null */

typedef unsigned short int PARAMETER_VALUE_TYPE;
typedef int LABEL_VALUE_TYPE;
typedef struct
{
  char* label_name;
  LABEL_VALUE_TYPE label_value;
} T_LABEL_TYPE;

typedef T_LABEL_TYPE* LABEL_TYPE;

typedef enum
{
  COMMAND_NOP,
  COMMAND_LOAD,
  COMMAND_STORE,
  COMMAND_READ,
  COMMAND_WRITE,
  COMMAND_SWAPA,
  COMMAND_SWAPD,
  COMMAND_JUMP,
  COMMAND_BRANCHZ,
  COMMAND_BRANCHP,
  COMMAND_CONST,
  COMMAND_ADD,
  COMMAND_SUB,
  COMMAND_MUL,
  COMMAND_DIV,
  COMMAND_HALT,
  COMMAND_DATA
} COMMAND_TYPE;

typedef enum
{
  PARAMETER_VALUE,
  PARAMETER_LABEL
} PARAMETER_TYPE;

typedef struct
{
  PARAMETER_TYPE type;
  union
  {
    PARAMETER_VALUE_TYPE value;
    LABEL_TYPE label;
  };
} T_PARAMETER;

typedef T_PARAMETER* PARAMETER;

typedef struct
{
  LABEL_TYPE label;
  COMMAND_TYPE type;
  PARAMETER parameter;
} T_COMMAND;

typedef T_COMMAND* COMMAND;

typedef struct _T_COMMAND_LIST_ENTRY
{
  COMMAND command;
  struct _T_COMMAND_LIST_ENTRY* next;
} T_COMMAND_LIST_ENTRY;

typedef T_COMMAND_LIST_ENTRY* COMMAND_LIST_ENTRY;

typedef struct
{
  COMMAND_LIST_ENTRY first;
  COMMAND_LIST_ENTRY last;
} T_COMMAND_LIST;

typedef T_COMMAND_LIST* COMMAND_LIST;
typedef T_COMMAND_LIST_ENTRY* COMMAND_LIST_ITERATOR;

LABEL_TYPE createLabel(const char* label);
char* getLabelString(const LABEL_TYPE label);
LABEL_VALUE_TYPE getLabelValue(const LABEL_TYPE label);
void setLabelValue(LABEL_TYPE label, const LABEL_VALUE_TYPE value);
void destroyLabelPool();

PARAMETER createValueParameter(const PARAMETER_VALUE_TYPE value);
PARAMETER createLabelParameter(const LABEL_TYPE label);
PARAMETER_TYPE getParameterType(const PARAMETER parameter);
PARAMETER_VALUE_TYPE getParameterValue(const PARAMETER parameter);
unsigned short int getValue(const PARAMETER_VALUE_TYPE value);
LABEL_TYPE getParameterLabel(const PARAMETER);
void destroyParameter(PARAMETER* parameter);

COMMAND createCommand(const LABEL_TYPE label, const COMMAND_TYPE type, const PARAMETER parameter);
LABEL_TYPE getCommandLabel(const COMMAND command);
void setCommandLabel(COMMAND command, LABEL_TYPE label);
COMMAND_TYPE getCommandType(const COMMAND command);
char getCommandRepresentation(const COMMAND_TYPE type);
const char* getCommandTypeString(const COMMAND_TYPE type);
PARAMETER getCommandParameter(const COMMAND command);
void destroyCommand(COMMAND* command);

COMMAND_LIST_ENTRY createCommandListEntry();
COMMAND getCommand(const COMMAND_LIST_ENTRY entry);
void setCommand(COMMAND_LIST_ENTRY entry, const COMMAND command);
COMMAND_LIST_ENTRY getNextEntry(const COMMAND_LIST_ENTRY entry);
void setNextEntry(COMMAND_LIST_ENTRY entry, const COMMAND_LIST_ENTRY next);
void destroyCommandListEntry(COMMAND_LIST_ENTRY* entry);

COMMAND_LIST createCommandList();
void addCommand(COMMAND_LIST list, const COMMAND command);
COMMAND_LIST_ITERATOR getIterator(const COMMAND_LIST list);
COMMAND_LIST_ITERATOR getNext(const COMMAND_LIST_ITERATOR iterator);
void destroyCommandList(COMMAND_LIST* list);

#endif /* __ASM_TYPES_H__INCLUDED */
