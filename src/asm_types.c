#include <malloc.h>
#include <string.h>
#include "asm_types.h"
#include "trace.h"

typedef struct _T_LABEL_LIST_ENTRY
{
  LABEL_TYPE label;
  struct _T_LABEL_LIST_ENTRY* next;
} T_LABEL_LIST_ENTRY;

typedef T_LABEL_LIST_ENTRY* LABEL_LIST_ENTRY;

static LABEL_LIST_ENTRY pool=null;

void destroyLabelPool()
{
  while(pool != null)
  {
    if (pool->label != null)
    {
      if (pool->label->label_name != null)
      {
        free(pool->label->label_name);
      }
      free(pool->label);
    }
    pool=pool->next;
  }
}

LABEL_TYPE createLabel(const char* label)
{
  LABEL_TYPE result;
  LABEL_LIST_ENTRY iterator = pool;
  TRACE(TRACE_DEBUG, "Creating label %s", label);

  while (iterator != null)
  {
    if (!strcmp(getLabelString(iterator->label), label))
    {
      TRACE(TRACE_DEBUG, "Label %s already created", label);
      return iterator->label;
    }
    iterator = iterator->next;
  }

  iterator = (LABEL_LIST_ENTRY)malloc(sizeof(T_LABEL_LIST_ENTRY));
  if (iterator == null)
  {
    TRACE(TRACE_ERROR, "Memory allocation failed");
    return null;
  }
  
  result = (LABEL_TYPE)malloc(sizeof(T_LABEL_TYPE));
  if (result == null)
  {
    TRACE(TRACE_ERROR, "Memory allocation failed");
    free(iterator);
    return null;
  }
  result->label_name=(char*)malloc(strlen(label)*sizeof(char));
  if (result->label_name == null)
  {
    TRACE(TRACE_ERROR, "Memory allocation failed");
    free(iterator);
    free(result);
    return null;
  }
  strcpy(result->label_name, label);
  result->label_value=-1;
  iterator->label=result;
  iterator->next=pool;
  pool=iterator;
  TRACE(TRACE_DEBUG, "Label %s created", label);
  return result;
}
  
char* getLabelString(const LABEL_TYPE label)
{
  if (label == null)
  {
    TRACE(TRACE_ERROR, "Illegal access to null label");
    return null;
  }
  return label->label_name;
}

LABEL_VALUE_TYPE getLabelValue(const LABEL_TYPE label)
{
  if (label == null)
  {
    TRACE(TRACE_ERROR, "Illegal access to null label");
    return 0;
  }
  return label->label_value;
}

void setLabelValue(LABEL_TYPE label, const LABEL_VALUE_TYPE value)
{
  if (label == null)
  {
    TRACE(TRACE_ERROR, "Illegal access to null label");
    return;
  }
  label->label_value=value;
}
PARAMETER createValueParameter(const PARAMETER_VALUE_TYPE value)
{
  PARAMETER result;
  TRACE(TRACE_DEBUG, "Creating parameter with value %d", value);
  result = (PARAMETER)malloc(sizeof(T_PARAMETER));
  if (result == null)
  {
    TRACE(TRACE_ERROR, "Memory allocation failed");
    return null;
  }
  result->type=PARAMETER_VALUE;
  result->value=value;
  return result;
}
  
PARAMETER createLabelParameter(const LABEL_TYPE label)
{
  PARAMETER result;
  TRACE(TRACE_DEBUG, "Creating parameter with label %s", getLabelString(label));
  result = (PARAMETER)malloc(sizeof(T_PARAMETER));
  if (result == null)
  {
    TRACE(TRACE_ERROR, "Memory allocation failed");
    return null;
  }
  result->type=PARAMETER_LABEL;
  result->label=label;
  return result;
}
  
PARAMETER_TYPE getParameterType(const PARAMETER parameter)
{
  return parameter->type;
}

PARAMETER_VALUE_TYPE getParameterValue(const PARAMETER parameter)
{
  if (parameter->type == PARAMETER_VALUE)
  {
    return parameter->value;
  }
  else if (parameter->type == PARAMETER_LABEL)
  {
    if (getLabelValue(parameter->label)==-1)
    {
      TRACE(TRACE_ERROR, "Illegal access to undefined label [%s]", getLabelString(parameter->label));
    }
    return getLabelValue(parameter->label);
  }
  else
  {
    TRACE(TRACE_ERROR, "Illegal access to uninitialized parameter");
  }
  return 0;
}

unsigned short int getValue(const PARAMETER_VALUE_TYPE value)
{
  return value;
}

LABEL_TYPE getParameterLabel(const PARAMETER parameter)
{
  if (parameter->type == PARAMETER_LABEL)
  {
    return parameter->label;
  }
  else if (parameter->type == PARAMETER_VALUE)
  {
    TRACE(TRACE_ERROR, "Illegal access to parameter label, parameter value is: [%d]", getValue(parameter->value));
  }
  else
  {
    TRACE(TRACE_ERROR, "Illegal access to uninitialized parameter");
  }
  return null;
}

void destroyParameter(PARAMETER* parameter)
{
  free(*parameter);
  parameter = null;
}

COMMAND createCommand(const LABEL_TYPE label, const COMMAND_TYPE type, const PARAMETER parameter)
{
  COMMAND result;
  TRACE(TRACE_DEBUG, "Creating command %s", getCommandTypeString(type));
  result = (COMMAND)malloc(sizeof(T_COMMAND));
  if (result == null)
  {
    TRACE(TRACE_ERROR, "Memory allocation failed");
    return null;
  }
  if (label == null)
  {
    result->label = null;
  }
  else
  {
    result->label = label;
    if (result->label == null)
    {
      free(result);
      return null;
    }
  }
  result->type = type;
  result->parameter = parameter;
  return result;
}
  
LABEL_TYPE getCommandLabel(const COMMAND command)
{
  if (command == null)
  {
    TRACE(TRACE_ERROR, "Illegal access to null command");
    return null;
  }
  return command->label;
}

void setCommandLabel(COMMAND command, const LABEL_TYPE label)
{
  if (command == null)
  {
    TRACE(TRACE_ERROR, "Illegal access to null command");
    return;
  }
  command->label = label;
}

char getCommandRepresentation(const COMMAND_TYPE type)
{
  switch(type)
  {
    case COMMAND_NOP:
    case COMMAND_DATA:
      return '0';
    case COMMAND_LOAD:
      return '1';
    case COMMAND_STORE:
      return '2';
    case COMMAND_READ:
      return '3';
    case COMMAND_WRITE:
      return '4';
    case COMMAND_SWAPA:
      return '5';
    case COMMAND_SWAPD:
      return '6';
    case COMMAND_JUMP:
      return '7';
    case COMMAND_BRANCHZ:
      return '8';
    case COMMAND_BRANCHP:
      return '9';
    case COMMAND_CONST:
      return 'A';
    case COMMAND_ADD:
      return 'B';
    case COMMAND_SUB:
      return 'C';
    case COMMAND_MUL:
      return 'D';
    case COMMAND_DIV:
      return 'E';
    case COMMAND_HALT:
      return 'F';
  }
  TRACE(TRACE_ERROR, "Illegal translation of command type %d", type);
  return 0;
}

const char* getCommandTypeString(const COMMAND_TYPE type)
{
  switch (type)
  {
    case COMMAND_NOP:
      return "NOP";
      break;
    case COMMAND_LOAD:
      return "LOAD";
      break;
    case COMMAND_STORE:
      return "STORE";
      break;
    case COMMAND_READ:
      return "READ";
      break;
    case COMMAND_WRITE:
      return "WRITE";
      break;
    case COMMAND_SWAPA:
      return "SWAPA";
      break;
    case COMMAND_SWAPD:
      return "SWAPD";
      break;
    case COMMAND_JUMP:
      return "JUMP";
      break;
    case COMMAND_BRANCHZ:
      return "BRANCHZ";
      break;
    case COMMAND_BRANCHP:
      return "BRANCHP";
      break;
    case COMMAND_CONST:
      return "CONST";
      break;
    case COMMAND_ADD:
      return "ADD";
      break;
    case COMMAND_SUB:
      return "SUB";
      break;
    case COMMAND_MUL:
      return "MUL";
      break;
    case COMMAND_DIV:
      return "DIV";
      break;
    case COMMAND_HALT:
      return "HALT";
      break;
    case COMMAND_DATA:
      return "DATA";
      break;
  }
  TRACE(TRACE_ERROR, "Illegal command type passed: %d", type);
  return null;
}

COMMAND_TYPE getCommandType(const COMMAND command)
{
  if (command == null)
  {
    TRACE(TRACE_ERROR, "Illegal access to null command");
    return COMMAND_NOP;
  }
  return command->type;
}

PARAMETER getCommandParameter(const COMMAND command)
{
  if (command == null)
  {
    TRACE(TRACE_ERROR, "Illegal access to null command");
    return null;
  }
  return command->parameter;
}

void destroyCommand(COMMAND* command)
{
  if (*command == null)
  {
    TRACE(TRACE_ERROR, "Illegal attempt to destroy null command");
    return;
  }
  if ((*command)->parameter != null)
  {
    destroyParameter(&((*command)->parameter));
  }
  free(*command);
  command=null;
}

COMMAND_LIST_ENTRY createCommandListEntry()
{
  COMMAND_LIST_ENTRY result;
  TRACE(TRACE_DEBUG, "Creating command list entry");
  result=(COMMAND_LIST_ENTRY)malloc(sizeof(T_COMMAND_LIST_ENTRY));
  if (result == null)
  {
    TRACE(TRACE_ERROR, "Memory allocation failed");
    return null;
  }
  result->command=null;
  result->next=null;
  return result;
}

COMMAND getCommand(const COMMAND_LIST_ENTRY entry)
{
  if (entry == null)
  {
    TRACE(TRACE_ERROR, "Illegal access to null entry");
    return null;
  }
  return entry->command;
}

void setCommand(COMMAND_LIST_ENTRY entry, const COMMAND command)
{
  if (entry == null)
  {
    TRACE(TRACE_ERROR, "Illegal access to null entry");
    return;
  }
  entry->command=command;
}

COMMAND_LIST_ENTRY getNextEntry(const COMMAND_LIST_ENTRY entry)
{
  if (entry == null)
  {
    TRACE(TRACE_ERROR, "Illegal access to null entry");
    return null;
  }
  return entry->next;
}

void setNextEntry(COMMAND_LIST_ENTRY entry, const COMMAND_LIST_ENTRY next)
{
  if (entry == null)
  {
    TRACE(TRACE_ERROR, "Illegal access to null entry");
    return;
  }
  entry->next=next;
}

void destroyCommandListEntry(COMMAND_LIST_ENTRY* entry)
{
  TRACE(TRACE_DEBUG, "Destroying command list entry");
  if (*entry == null)
  {
    TRACE(TRACE_ERROR, "Illegal attempt to destroy null command list entry");
    return;
  }
  if ((*entry)->command != null)
  {
    destroyCommand(&((*entry)->command));
  }
  free(*entry);
  entry=null;
}

COMMAND_LIST createCommandList()
{
  COMMAND_LIST result;
  TRACE(TRACE_DEBUG, "Creating command list");
  result=(COMMAND_LIST)malloc(sizeof(T_COMMAND_LIST));
  if (result == null)
  {
    TRACE(TRACE_ERROR, "Memory allocation failed");
    return null;
  }
  result->first=null;
  result->last=null;
}

void addCommand(COMMAND_LIST list, const COMMAND command)
{
  if (list == null)
  {
    TRACE(TRACE_ERROR, "Illegal attempt to access null list");
    return;
  }
  TRACE(TRACE_DEBUG, "Adding new entry to the list");
  COMMAND_LIST_ENTRY command_list_entry = createCommandListEntry();
  if (command_list_entry == null)
  {
    return;
  }
  command_list_entry->command=command;
  command_list_entry->next=null;
  if (list->first == null)
  {
    list->first=command_list_entry;
  }
  else
  {
    setNextEntry(list->last, command_list_entry);
  }
  list->last=command_list_entry;
  TRACE(TRACE_DEBUG, "New entry added");
}

void insertCommandAfter(COMMAND_LIST list, COMMAND_LIST_ITERATOR iterator, const COMMAND command)
{
  if (list == null)
  {
    TRACE(TRACE_ERROR, "Illegal attempt to access null list");
    return;
  }
  if (iterator == null)
  {
    TRACE(TRACE_ERROR, "Illegal attempt to access null iterator");
    return;
  }
  COMMAND_LIST_ENTRY command_list_entry = createCommandListEntry();
  if (command_list_entry == null)
  {
    return;
  }
  command_list_entry->command=command;
  command_list_entry->next=iterator->next;
  iterator->next=command_list_entry;
  if (list->last == iterator)
  {
    list->last == command_list_entry;
  }
}

COMMAND_LIST_ITERATOR getIterator(const COMMAND_LIST list)
{
  if (list == null)
  {
    TRACE(TRACE_ERROR, "Illegal attempt to access null list");
    return null;
  }
  return list->first;
}
  
COMMAND_LIST_ITERATOR getNext(const COMMAND_LIST_ITERATOR iterator)
{
  if (iterator == null)
  {
    TRACE(TRACE_ERROR, "Illegal access to null iterator");
    return null;
  }
  return iterator->next;
}

void destroyCommandList(COMMAND_LIST* list)
{
  if (list == null || *list == null)
  {
    TRACE(TRACE_ERROR, "Illegal attempt to destroy null list");
    return;
  }
  COMMAND_LIST_ITERATOR iterator = getIterator(*list);
  while(iterator != null)
  {
    COMMAND_LIST_ENTRY temp = iterator->next;
    destroyCommandListEntry(&iterator);
    iterator=temp;
  }
  free(*list);
  *list=null;
}


