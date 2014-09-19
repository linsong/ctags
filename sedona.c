/*
*   Copyright (c) 2014, Vincent Wang
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for parsing and scanning C, C++ and Java
*   source files.
*/

/* INCLUDE FILES */
#include <stdio.h>
#include "general.h"    /* always include first */

#include "parse.h"      /* always include */
#include "read.h"       /* to define file fileReadLine() */

/* DATA DEFINITIONS */
typedef enum eSedonaKinds {
    K_METHOD=0,
    K_CLASS,
} sedonaKind;

static kindOption SedonaKinds[] = {
    { TRUE, 'm', "method", "Sedona Methods" },
    { TRUE, 'c', "class", "Sedona Classes" }
};

/* FUNCTION DEFINITIONS */
static vString * groupVal(int index, const char *const line,
    const int start, const int length)
{
  vString *const name = vStringNew();
  vStringNCopyS(name, line + start, length);
  return name;
};

static void handleMethod(const char *const line, const regexMatch *const matches,
                       const unsigned int count)
{
  if (count > 5)
  {
    vString * name = 0;
    if (matches[5].start == -1)
      return;

    if (matches[4].start != -1)
    {
      vString *const returnType = groupVal(4, line, matches[4].start, matches[4].length);
      if (!returnType)
        return;

      if (strcmp(returnType->buffer, "return") == 0)
      { 
        vStringDelete(returnType);
        return;
      }
      else
        vStringDelete(returnType);
    }

    name = groupVal(5, line, matches[5].start, matches[5].length);
    makeSimpleTag(name, SedonaKinds, K_METHOD);
  }
}

static void handleClass(const char *const line, const regexMatch *const matches,
                       const unsigned int count)
{
  if (count > 3)    /* should always be true per regex */
  {
    vString * name = 0; 
    if (matches[3].start == -1)
      return;

    name = groupVal(3, line, matches[3].start, matches[3].length);
    makeSimpleTag(name, SedonaKinds, K_CLASS);
  }
}

static void findSedonaTags(void)
{
  while (fileReadLine() != NULL)
    ;  /* don't need to do anything here since callback is sufficient */
}

static void installSedona(const langType language)
{
  addCallbackRegex(language,
    "^[ \t]*(@[A-Za-z0-9]+[ \t]+)?((static|virtual|abstract|override|native|action|internal)[ \t]+)*([A-Za-z0-9]+)[ \t]+([A-Za-z0-9]+)\\([^)]*\\)[ \t]*(\\{[^}]*\\}?[ \t]*)?.?$",
    NULL, handleMethod);

  addCallbackRegex(language, 
      "^[ \t]*((public|internal|abstract|final)[ \t]+)?class[ \t]*([A-Za-z0-9]+)",
      NULL, handleClass);
}

/* Create parser definition stucture */
extern parserDefinition* SedonaParser (void)
{
  static const char *const extensions [] = { "sedona", NULL };
  parserDefinition* def = parserNew ("Sedona");
  def->kinds      = SedonaKinds;
  def->kindCount  = KIND_COUNT (SedonaKinds);
  def->extensions = extensions;
  def->parser     = findSedonaTags;
  def->initialize = installSedona;
  return def;
}

