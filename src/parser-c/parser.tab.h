
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_INTEGER = 258,
     TOKEN_PROP_SHORT = 259,
     TOKEN_PROP_LONG = 260,
     TOKEN_STRING = 261,
     TOKEN_PATTERN = 262,
     TOKEN_NUMERIC = 263,
     TOKEN_GROUP_OPEN = 264,
     TOKEN_GROUP_CLOSE = 265,
     TOKEN_SYMBOL_ID = 266,
     TOKEN_OPFIL_HAS = 267,
     TOKEN_REFERENCE = 268,
     TOKEN_OPFIL_EQ = 269,
     TOKEN_OPFIL_MATCH = 270,
     TOKEN_OPFIL_LE = 271,
     TOKEN_OPFIL_GE = 272,
     TOKEN_OPFIL_LT = 273,
     TOKEN_OPFIL_GT = 274,
     TOKEN_OPSET_OR = 275,
     TOKEN_OPSET_AND = 276,
     TOKEN_OPSET_NOT = 277,
     TOKEN_IDSEQ_RANGE = 278,
     TOKEN_IDSEQ_SEP = 279,
     TOKEN_UNIVERSE = 280,
     TOKEN_ERR = 281
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 33 "/home/vdust/developpement/GIT/xm-coll-parser/src/parser.y"

	unsigned int ival;
	xm_boxed_t *coll;
	xm_string_t *xstr;
	xm_sequence_t *seq;



/* Line 1676 of yacc.c  */
#line 87 "parser.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif



#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



