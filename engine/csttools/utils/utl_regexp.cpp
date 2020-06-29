
//
// This class is based on the work of <a href="mailto:guy@wyrdrune.com">
// Guy Gascoigne - Piggford</a>. The original scource code is downloaded from
// <a href="http://www.codeguru.com/cpp/cpp/string/regex/article.php/c2779/">
// CodeGuru: An alternative Regular Expression Class</a>
//

// regcomp and regexec -- regsub and regerror are elsewhere
// @(#)regexp.c 1.3 of 18 April 87
//
//  Copyright (c) 1986 by University of Toronto.
//  Written by Henry Spencer.  Not derived from licensed software.
//
//  Permission is granted to anyone to use this software for any
//  purpose on any computer system, and to redistribute it freely,
//  subject to the following restrictions:
//
//  1.  The author is not responsible for the consequences of use of
//      this software, no matter how awful, even if they arise
//      from defects in it.
//
//  2.  The origin of this software must not be misrepresented, either
//      by explicit claim or by omission.
//
//  3.  Altered versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
// *** THIS IS AN ALTERED VERSION.  It was altered by John Gilmore,
// *** hoptoad!gnu, on 27 Dec 1986, to add \< and \> for word-matching
// *** as in BSD grep and ex.
// *** THIS IS AN ALTERED VERSION.  It was altered by John Gilmore,
// *** hoptoad!gnu, on 28 Dec 1986, to optimize characters quoted with \.
// *** THIS IS AN ALTERED VERSION.  It was altered by James A. Woods,
// *** ames!jaw, on 19 June 1987, to quash a regcomp() redundancy.
// *** THIS IS AN ALTERED VERSION.  It was altered by Geoffrey Noer,
// *** THIS IS AN ALTERED VERSION.  It was altered by Guy Gascoigne - Piggford
// *** guy@wyrdrune.com, on 15 March 1998, porting it to C++ and converting
// *** it to be the engine for the Regexp class
//
// Beware that some of this code is subtly aware of the way operator
// precedence is structured in regular expressions.  Serious changes in
// regular-expression syntax might require a total rethink.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utl_regexp.h"

// type definitions
#if defined(WIN32) || defined(WINCE)
#   include <tchar.h>
#else
#if defined(_UNICODE) || defined(UNICODE)
#   include <wchar.h>
#   define _tcschr         wcschr
#   define _tcsstr         wcsstr
#   define _tcslen         wcslen
#   define _tcsspn         wcsspn
#   define _tcscspn        wcscspn
#   define _tcsncicmp      wcsncasecmp
#else
#   define _tcschr         strchr
#   define _tcsstr         strstr
#   define _tcslen         strlen
#   define _tcsspn         strspn
#   define _tcscspn        strcspn
#   define _tcsncicmp      strncasecmp
#endif
#endif

namespace cst
{
    namespace RegExp
    {
        //////////////////////////////////////////////////////////////////////////
        //
        //  Error message definition for errorhandler
        //
        //////////////////////////////////////////////////////////////////////////

        /*const struct errorhandler::ErrorMsg errorhandler::errormsgs[] = {
        {REGERR_NO_ERROR,               "No error"},
        {REGERR_SENTINEL_VALUE,         "Unknown error"},
        {REGERR_NULL_ARG,               "NULL argument"},
        {REGERR_CORRUPTION,             "Regular expression corruption"},
        {REGERR_BAD_REGREPEAT,          "Internal error: bad call of repeat"},
        {REGERR_TOO_BIG,                "Regular expression too big"},
        {REGERR_TOO_MANY_PAREN,         "Too many () (sub-strings)"},
        {REGERR_UNTERMINATED_PAREN,     "Unterminated ()"},
        {REGERR_UNMATCHED_PAREN,        "Unmatched ()"},
        {REGERR_UNEXPECTED_JUNK,        "Expected junk on end"},
        {REGERR_OP_COULD_BE_EMPTY,      "*+ operand could be empty"},
        {REGERR_NESTED_OP,              "Nested *?+"},
        {REGERR_INVALID_RANGE,          "Invalid [] range"},
        {REGERR_UNMATCHED_BRACE,        "Unmatched []"},
        {REGERR_UNEXPECTED_CHAR,        "Unexpected char: \\0|)"},
        {REGERR_OP_FOLLOWS_NOTHING,     "?+* follows nothing"},
        {REGERR_TRAILING_ESC,           "Trailing \\"},
        {REGERR_NO_REGEXP,              "NULL regular expression"},
        };*/

        //////////////////////////////////////////////////////////////////////////
        //
        //  accessor
        //
        //////////////////////////////////////////////////////////////////////////

        /**
        *  @brief  Get the current OPERATOR for the program
        */
        inline TCHAR accessor::OPERATOR(TCHAR* p)
        {
            return (*(p));
        }
        /**
        *  @brief  Get the OPERAND of the current program OPERATOR
        */
        inline TCHAR* accessor::OPERAND(TCHAR* p)
        {
            return (p+3);
        }
        /**
        *  @brief  Get next regular expression program
        */
        inline TCHAR* accessor::REGNEXT(TCHAR* p)
        {
            const short &offset = *((short*)(p+1));
            if (offset==0)
                return NULL;
            else
                return (OPERATOR(p)==BACK)?p-offset:p+offset;
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  compilerbase
        //
        //////////////////////////////////////////////////////////////////////////

        compilerbase::compilerbase(const TCHAR* parse) : regstr((TCHAR*)parse), regnum(1)
        {
        }
        /**
        *  @brief  Parse the regular expression, i.e. main body or parenthesized thing
        *
        *  Caller must absorb opening parenthesis.
        *  Combining parenthesis handling with the base level of regular expression
        *  is a trifle forced, but the need to tie the tails of the branches to
        *  what follows makes it hard to avoid.
        */
        TCHAR* compilerbase::parse(bool hasparent, int& flag)
        {
            TCHAR *ret, *brch, *ender;
            int partflag, subno = 0;
            flag = HASWIDTH;        // tentatively
            if (hasparent)
            {
                // make an OPEN node
                if (regnum>=CLOSE-OPEN)     // maximum sub-string #
                {
                    seterror(REGERR_TOO_MANY_PAREN);
                    return NULL;
                }
                subno = regnum;
                regnum++;
                ret = emitnode(OPEN+subno);
            }
            // pick up the branches, linking them together
            brch = branch(partflag);
            if (brch==NULL)
                return NULL;
            if (hasparent)
                tail(ret, brch);    // OPEN -> first
            else
                ret = brch;
            flag &= ~(~partflag&HASWIDTH);  // clear bit if bit 0
            flag |= partflag&SPSTART;
            while (*regstr=='|')
            {
                regstr++;
                brch = branch(partflag);
                if (brch==NULL)
                    return NULL;
                tail(ret, brch);    // BRANCH -> BRANCH
                flag &= ~(~partflag&HASWIDTH);
                flag |= partflag&SPSTART;
            }
            // make a closing node, and hook it on the end
            ender = emitnode((hasparent)?CLOSE+subno:END);
            tail(ret, ender);
            // hook the tails of the branches to the closing node
            for (brch=ret; brch!=NULL; brch = REGNEXT(brch))
                optail(brch, ender);
            // check for proper termination
            if (hasparent && *regstr++!=')')
            {
                seterror(REGERR_UNTERMINATED_PAREN);
                return NULL;
            }
            else if (!hasparent && *regstr!='\0')
            {
                if (*regstr==')')
                {
                    seterror(REGERR_UNMATCHED_PAREN);
                    return NULL;
                }
                else
                {
                    seterror(REGERR_UNEXPECTED_JUNK);
                    return NULL;
                }
            }
            return ret;
        }
        /**
        *  @brief  Alternative of "|" operator, implements the concatenation operator.
        */
        TCHAR* compilerbase::branch(int& flag)
        {
            TCHAR *ret, *chain, *latest;
            int partflag, c;
            flag = WORST;
            ret  = emitnode(BRANCH);
            chain= NULL;
            while ((c=*regstr)!='\0' && c!='|' && c!=')')
            {
                latest = piece(partflag);
                if (latest==NULL)
                    return NULL;
                flag |= partflag&HASWIDTH;
                if (chain==NULL)    // first piece
                    flag |= partflag&SPSTART;
                else
                    tail(chain, latest);
                chain = latest;
            }
            if (chain==NULL)    // loop zero times
                emitnode(NOTHING);
            return ret;
        }
        /**
        *  @brief  Something followed by possible [*+?]
        *
        *  Note that the branching code sequences used for ? and the general cases
        *  of * and + are somewhat optimized: they use the same NOTHING node as
        *  both the end marker for their branch list and the body of the last branch.
        *  It might see that this node could be dispensed with entirely, but the
        *  end marker role is not redundant.
        */
        TCHAR* compilerbase::piece(int& flag)
        {
            TCHAR *ret, *next;
            TCHAR  op;
            int   partflag;
            ret = atom(partflag);
            if (ret==NULL)
                return NULL;
            op = *regstr;
            if (op!='*' && op!='+' && op!='?')
            {
                flag = partflag;
                return ret;
            }
            if (!(partflag&HASWIDTH) && op!='?')
            {
                seterror(REGERR_OP_COULD_BE_EMPTY);
                return NULL;
            }
            switch (op)
            {
            case '*':   flag = WORST|SPSTART;           break;
            case '+':   flag = WORST|SPSTART|HASWIDTH;  break;
            case '?':   flag = WORST;                   break;
            }
            if (op=='*' && (partflag&SIMPLE))
                insert(STAR, ret);
            else if (op=='*')
            {
                // emit x* as (x&|), where & means 'self'
                insert(BRANCH, ret);            // either x
                optail(ret, emitnode(BACK));    // and loop
                optail(ret, ret);               // back
                tail(ret, emitnode(BRANCH));    // or
                tail(ret, emitnode(NOTHING));   // null
            }
            else if (op=='+' && (partflag&SIMPLE))
                insert(PLUS, ret);
            else if (op=='+')
            {
                // emit x+ as x(&|), where & means 'self'
                next = emitnode(BRANCH);        // either
                tail(ret, next);
                tail(emitnode(BACK), ret);      // loop back
                tail(ret, emitnode(BRANCH));    // or
                tail(ret, emitnode(NOTHING));   // null
            }
            else if (op=='?')
            {
                // emit x? as (x|)
                insert(BRANCH, ret);            // either x
                tail(ret, emitnode(BRANCH));    // or
                next = emitnode(NOTHING);       // null
                tail(ret, next);
                optail(ret, next);
            }
            op = *(++regstr);
            if (op=='*' || op=='+' || op=='?')
            {
                seterror(REGERR_NESTED_OP);
                return NULL;
            }
            return ret;
        }
        /**
        *  @brief  The lowest level for the regexp
        *
        *  Optimization: gobbles an entire sequence of ordinary characters so that
        *  it can turn them into a single node, which is smaller to store and
        *  faster to run. Backslashed characters are exceptions, each becoming a
        *  separate node; the code is simpler that way and it's not worth fixing.
        */
        TCHAR* compilerbase::atom(int& flag)
        {
            int partflags, c;
            TCHAR* ret;
            flag = WORST;
            switch (*regstr++)
            {
            case '^':
                ret = emitnode(BOL);
                break;
            case '$':
                ret = emitnode(EOL);
                break;
            case '.':
                ret = emitnode(ANY);
                flag |= HASWIDTH|SIMPLE;
                break;
            case '[':
                // any of the characters
                if (*regstr!='^')
                    ret = emitnode(ANYOF);
                else
                {
                    // complement of the characters
                    ret = emitnode(ANYBUT);
                    regstr++;
                }
                if ((c=*regstr)==']' || c=='-')
                {
                    emitcode(c);
                    regstr++;
                }
                while ((c=*regstr++)!='\0' && c!=']')
                {
                    if (c!='-')
                        emitcode(c);
                    else if ((c=*regstr)==']' || c=='\0')
                        emitcode('-');
                    else
                    {
                        int rangebeg = *(regstr-2);
                        int rangeend  = c;
                        if (rangebeg>rangeend)
                        {
                            seterror(REGERR_INVALID_RANGE);
                            return NULL;
                        }
                        for (rangebeg++; rangebeg<=rangeend; rangebeg++)
                            emitcode(rangebeg);
                        regstr++;
                    }
                }
                emitcode('\0');
                if (c!=']')
                {
                    seterror(REGERR_UNMATCHED_BRACE);
                    return NULL;
                }
                flag |= HASWIDTH|SIMPLE;
                break;
            case '(':
                // sub-string
                ret = parse(true, partflags);
                if (ret==NULL)
                    return NULL;
                flag |= partflags&(HASWIDTH|SPSTART);
                break;
            case '\0':
            case '|':
            case ')':
                // supposed to be caught earlier
                seterror(REGERR_UNEXPECTED_CHAR);
                return NULL;
            case '*':
            case '+':
            case '?':
                seterror(REGERR_OP_FOLLOWS_NOTHING);
                return NULL;
            case '\\':
                switch (*regstr++)
                {
                case '\0':  seterror(REGERR_TRAILING_ESC);  return NULL;
                case '<':   ret = emitnode(WORDA);  break;
                case '>':   ret = emitnode(WORDZ);  break;
                    // TODO: handle \1, \2, ...
                default:    goto lbl_default;   // handle general quoted chars in exact-match routine
                }
                break;
lbl_default:
            default:
                // Encode a string of characters to be matched exactly.
                //
                // This is a bit tricky due to quoted chars and due to '*+?'
                // taking the SINGLE previous TCHAR as their operand.
                //
                // On entry, the TCHAR at 'regstr[-1]' is going to go into the
                // string, no matter what it is.
                // (It could be following a \ if we are entered from the '\' case.)
                // 
                // Basic idea is to pick up a good TCHAR in 'ch' and examine
                // the next TCHAR. If it is '*+?', then we twiddle.
                // If it is '\' then we frizzle. If it is other magic TCHAR
                // we push 'ch' and terminate the string. If none of the
                // above, we push 'ch' into the string and go around again.
                //
                // 'regprev' is used to remember where 'the current TCHAR' starts
                // in the string, if due to a '*+?' we need to back up and put
                // the current TCHAR in a separate, 1-TCHAR, string.
                // When 'regprev' is NULL, 'ch' is the only TCHAR in the string;
                // this is used in '*+?' handling, and in setting flags |= SIMPLE at the end.
                {
                    TCHAR* regprev;
                    register TCHAR ch;
                    regstr--;   // look at current TCHAR
                    ret = emitnode(EXACTLY);
                    for (regprev=0; ;)
                    {
                        ch = *regstr++;     // get current TCHAR
                        switch (*regstr)    // look at next TCHAR
                        {
                        case '.': case '[': case '(':
                        case ')': case '|': case '\n':
                        case '$': case '^': case '\0':
                            // TODO: $ and ^ should not always be magic
lbl_magic:
                            emitcode(ch);   // dump current TCHAR
                            goto lbl_done;  // and it is done
                        case '?': case '+': case '*':
                            if (regprev==NULL)  // if just ch in string, use it
                                goto lbl_magic;
                            regstr = regprev;   // end multi-TCHAR string one earlier
                            goto lbl_done;      // and back up current parsing result
                        case '\\':
                            emitcode(ch);       // current TCHAR OK
                            switch (regstr[1])  // look after
                            {
                            case '\0':
                            case '<':
                            case '>':
                                // TODO: handle \1, \2, ...
                                goto lbl_done;  // not quoted
                            default:
                                regprev = regstr;   // backup point is \, scan point after it
                                regstr++;
                                continue;
                            }
                        default:
                            emitcode(ch);   // add current TCHAR to string
                            break;
                        }
                        regprev = regstr;   // set backup point
                    }
lbl_done:
                    emitcode('\0');
                    flag |= HASWIDTH;
                    if (regprev==NULL)      // one TCHAR?
                        flag |= SIMPLE;
                }
                break;
            }
            return ret;
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  validator
        //
        //////////////////////////////////////////////////////////////////////////

        validator::validator(const TCHAR* parse) : compilerbase(parse), regsize(0)
        {
            emitcode(program::MAGIC);
            regdummy[0]=NOTHING;
            regdummy[1]=0;
            regdummy[2]=0;
        }


        //////////////////////////////////////////////////////////////////////////
        ///
        /// compiler
        ///
        //////////////////////////////////////////////////////////////////////////

        compiler::compiler (const TCHAR* parse, TCHAR* program) : compilerbase(parse), regcode(program)
        {
            emitcode(program::MAGIC);
        }
        /**
        *  @brief  Emit (if appropriate) a byte of code
        */
        void compiler::emitcode(int c)
        {
            *regcode++ = (TCHAR)c;
        }
        /**
        *  @brief  Emit a node
        */
        TCHAR* compiler::emitnode(int op)
        {
            TCHAR* const ret = regcode;
            TCHAR* ptr = ret;
            *ptr++ = (TCHAR)op;
            *ptr++ = '\0';  // NULL next pointer
            *ptr++ = '\0';
            regcode = ptr;
            return ret;
        }
        /**
        *  @brief  Insert an operator in front of already-emitted operand, relocating the operand
        */
        void compiler::insert(TCHAR op, TCHAR* opnd)
        {
            TCHAR* place;
            memmove(opnd+3, opnd, (regcode-opnd)*sizeof(TCHAR));
            regcode += 3;
            place = opnd;   // op node, where operand used to be
            *place++ = op;
            *place++ = '\0';
            *place++ = '\0';
        }
        /**
        *  @brief  Set the next-pointer at the end of a node chain
        */
        void compiler::tail(TCHAR* p, TCHAR* val)
        {
            TCHAR *scan, *temp;
            // find the last node
            for (scan=p; (temp=REGNEXT(scan))!=NULL; scan=temp);
            *((short*)(scan+1)) = (short)((OPERATOR(scan)==BACK)?scan-val:val-scan);
        }
        /**
        *  @brief  Tail on operand of first argument; NOP if no operand
        */
        void compiler::optail(TCHAR* p, TCHAR* val)
        {
            // "no operand" and "op!=BRANCH" are synonymous in practice
            if (OPERATOR(p)==BRANCH)
                tail(OPERAND(p), val);
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  executor
        //
        //////////////////////////////////////////////////////////////////////////

        executor::executor(program* prog, const TCHAR* inputstr)
            : strinput((TCHAR*)inputstr), strbegin((TCHAR*)inputstr), 
            regprog(prog), subbegp(prog->subbegp), subendp(prog->subendp)
        {
        }
        /**
        *  @brief  Try match at a specific point
        */
        bool executor::matchtry(TCHAR* str)
        {
            strinput    = str;
            // clear all beginning/ending indicator
            for (int i=0; i<CLOSE-OPEN; i++)
            {
                subbegp[i] = NULL;
                subendp[i] = NULL;
            }
            // set the begining/ending for completely matched string (sub-string 0)
            if (match(regprog->programchar+1))
            {
                subbegp[0] = str;
                subendp[0] = strinput;
                return true;
            }
            return false;
        }
        /**
        *  @brief  Main matching routine
        *
        *  Conceptually the strategy is simple:
        *  Check to see whether the current node matches, call self recursively
        *  to see whether the rest matches, and then act accordingly.
        *  In practice, we make some effort to avoid recursion, in particular by
        *  going through "ordinary" nodes (that do not need to know whether the
        *  rest of the match failed or not) by a loop instead by by recursion.
        */
        bool executor::match(TCHAR* program)
        {
            TCHAR *scan; // current program node
            TCHAR *next; // next program node
            TCHAR  op;
            for (scan=program; scan!=NULL; scan=next)
            {
                next = REGNEXT(scan);
                switch (op = OPERATOR(scan))
                {
                case BOL:
                    if (strinput!=strbegin)
                        return false;
                    break;
                case EOL:
                    if (*strinput!='\0')
                        return false;
                    break;
                case WORDA:
                    // must be looking at a letter, digit, or _
                    if (!isalnum(*strinput) && *strinput!='_')
                        return false;
                    // previous must be BOL or none-word
                    if (strinput>strbegin && (isalnum(strinput[-1]) || strinput[-1]=='_'))
                        return false;
                    break;
                case WORDZ:
                    // must be looking at none-letter, digit, or _
                    if (isalnum(*strinput) || *strinput=='_')
                        return false;
                    // do not care what the previous TCHAR is
                    break;
                case ANY:
                    if (*strinput=='\0')
                        return false;
                    strinput++;
                    break;
                case EXACTLY:
                    {
                        TCHAR* const opnd = OPERAND(scan);
                        // inline the first character, for speed
                        if (*opnd!=*strinput)
                            return false;
                        size_t len = _tcslen(opnd);
                        if (len>1 && _tcsncicmp(opnd, strinput, len)!=0)
                            return false;
                        strinput += len;                    
                    }
                    break;
                case ANYOF:
                    if (*strinput=='\0' || _tcschr(OPERAND(scan), *strinput)==NULL)
                        return false;
                    strinput++;
                    break;
                case ANYBUT:
                    if (*strinput=='\0' || _tcschr(OPERAND(scan), *strinput)!=NULL)
                        return false;
                    strinput++;
                    break;
                case NOTHING:
                    break;
                case BACK:
                    break;
                case BRANCH:
                    {
                        TCHAR* const saves = strinput;
                        if (OPERATOR(next)!=BRANCH)
                            next = OPERAND(scan);   // no choice, avoid recursion
                        else
                        {
                            while (OPERATOR(scan)==BRANCH)
                            {
                                if (match(OPERAND(scan)))
                                    return true;
                                strinput = saves;
                                scan = REGNEXT(scan);
                            }
                            return false;
                        }
                    }
                    break;
                case STAR:
                case PLUS:
                    {
                        const TCHAR nextch = (OPERATOR(next)==EXACTLY) ? *OPERAND(next) : '\0';
                        TCHAR* const saves = strinput;
                        const size_t  min = (OPERATOR(scan)==STAR) ? 0 : 1;
                        for (size_t no=repeat(OPERAND(scan))+1; no>min; no--)
                        {
                            strinput = saves + no-1;
                            // if it could work, try it
                            if (nextch=='\0' || *strinput==nextch)
                                if (match(next))
                                    return true;
                        }
                        return false;
                    }
                    break;
                case END:
                    return true;    // success!
                    break;
                default:
                    if ( (op>OPEN && op<CLOSE) || (op>CLOSE && op<CLOSE+(CLOSE-OPEN)) )
                    {
                        // sub-string \1, \2, ...
                        const int no = (op>OPEN && op<CLOSE) ? op-OPEN : op-CLOSE;
                        TCHAR* const inputs = strinput;
                        if (match(next))
                        {
                            // 1) do not set endp if some later invocation
                            //    of the same parenthesis already occurred
                            // 2) always set begp as the same parenthesis may
                            //    occur multiple times in "(.)+" or "(.)*"
                            if (op>OPEN && op<CLOSE)
                            {
                                //if (subbegp[no]==NULL)
                                subbegp[no] = inputs;
                            }
                            else
                            {
                                if (subendp[no]==NULL)
                                    subendp[no] = inputs;
                            }
                            return true;
                        }
                        else
                        {
                            // 3) always set begp even if regexp is not matched
                            //    for the condition when "(.)*" matches nothing
                            // 4) this is used to count the number of sub-strings
                            // 5) should set endp to be the same as begp if endp
                            //    is NULL while counting the number of sub-strings
                            // 6) see bprogram::match() function
                            if (op>OPEN && op<CLOSE)
                            {
                                //if (subbegp[no]==NULL)
                                subbegp[no] = inputs;
                            }
                            return false;
                        }
                    }
                    seterror(REGERR_CORRUPTION);
                    return false;
                    break;
                }
            }
            // we get here only if there is trouble
            // -- normally, "case END" is the terminating point
            seterror(REGERR_CORRUPTION);
            return false;
        }
        /**
        *  @brief  Report how many times something simple would match
        */
        size_t executor::repeat(TCHAR* prognode)
        {
            size_t count;
            TCHAR   ch, *scan;
            switch (OPERATOR(prognode))
            {
            case ANY:
                return _tcslen(strinput);
                break;
            case EXACTLY:
                ch = *OPERAND(prognode);
                for (count=0, scan=strinput; *scan==ch; scan++, count++);
                return count;
                break;
            case ANYOF:
                return _tcsspn(strinput, OPERAND(prognode));
                break;
            case ANYBUT:
                return _tcscspn(strinput, OPERAND(prognode));
                break;
            default:        // called inappropriately
                seterror(REGERR_BAD_REGREPEAT);
                return 0;   // best compromise
                break;
            }
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  reprogram
        //
        //////////////////////////////////////////////////////////////////////////

        program::program(const TCHAR* exp, bool icase)
            : programchar(0), programsize(0), compiledok(false), regstart(0), reganch(false), regmust(0), regmustlen(0)
        {
            strinput= 0;
            subbegp = new TCHAR*[CLOSE-OPEN];
            subendp = new TCHAR*[CLOSE-OPEN];
            subnum  = 0;
            if (icase==false)
                compiledok = compile(exp);
            else
            {
                TCHAR* out = new TCHAR[_tcslen(exp)*4+1];
                igncase(exp, out);
                compiledok = compile(out);
                delete []out;
            }
        }
        program::program(const program& right)
        {
            programsize = right.programsize;
            programchar = new TCHAR[programsize];
            memcpy(programchar, right.programchar, sizeof(TCHAR)*programsize);
            compiledok = right.compiledok;
            regstart = right.regstart;
            reganch  = right.reganch;
            regmust  = right.regmust;
            regmustlen=right.regmustlen;
            subbegp  = new TCHAR*[CLOSE-OPEN];
            subendp  = new TCHAR*[CLOSE-OPEN];
            subnum   = right.subnum;
            memcpy(subbegp, right.subbegp, sizeof(TCHAR*)*(CLOSE-OPEN));
            memcpy(subendp, right.subendp, sizeof(TCHAR*)*(CLOSE-OPEN));
            strinput = right.strinput;
        }
        /**
        *  @brief  clear the regular expression information
        */
        bool program::clear()
        {
            delete []programchar;
            delete []subbegp;
            delete []subendp;
            programchar = 0;
            programsize = 0;
            compiledok  = false;
            regstart = 0;
            reganch  = false;
            regmust  = 0;
            regmustlen = 0;
            subbegp  = 0;
            subendp  = 0;
            subnum   = 0;
            return true;
        }
        /**
        *  @brief  Prepare the ignore case expression,
        *          copy src to out making every top level character a [Aa] set
        */
        void program::igncase(const TCHAR* src, TCHAR* out)
        {
            bool inrange = false;
            while (*src)
            {
                if (*src=='[')
                    inrange = true;
                if (*src==']')
                    inrange = false;
                if (*src<0)
                {
                    *out ++ = *src++;   // Full-shape
                    *out ++ = *src;
                }
                else if (!inrange && isalpha(*src))
                {
                    *out++ = '[';
                    *out++ = toupper(*src);
                    *out++ = tolower(*src);
                    *out++ = ']';
                }
                else
                    *out++ = *src;
                src++;
            }
            *out = '\0';
        }
        /**
        *  @brief  Compile a regular expression into its internal representation
        *
        *  Compiling a regular expression might be time consuming,
        *  so the user should declare a static regexp object in his/her program,
        *  and just compile once in the construction of the regexp object,
        *  and use isCompiled to determined whether the regexp is compiled OK.
        *
        *  We can not allocate the space until we know how big the compiled form
        *  will be, but we can not compile it (and thus know how bit it is) until
        *  we have got a place to put the compiled code.
        *  So we cheat: we compile it twice, once with size counting but not
        *  generating the code, and another time for real code generation.
        *  This means that we do not allocate space until we are sure that the
        *  things will really compiled successfully.
        *
        *  Beware that the optimization-preparation code here knows about
        *  some of the structure of the compiled form of regexp.
        */
        bool program::compile(const TCHAR* expstr)
        {
            TCHAR* scan;
            int   flags;
            if (expstr==NULL)
            {
                seterror(REGERR_NULL_ARG);
                return false;
            }
            // first pass: check validation, and determine size
            validator tester(expstr);
            if (tester.parse(false, flags)==NULL)
            {
                seterror(tester.geterror());
                return false;
            }
            // smaller enough for storage?
            if (tester.size()>=0x7fffL) // probably could be 0xffffL
            {
                seterror(REGERR_TOO_BIG);
                return false;
            }
            // allocate program space
            programsize = tester.size();
            programchar = new TCHAR[programsize];
            if (programchar==NULL)
            {
                seterror(REGERR_TOO_BIG);
                clear();
                return false;
            }
            // second pass: emit code
            compiler comp(expstr, programchar);
            if (comp.parse(0, flags)==NULL)
            {
                seterror(comp.geterror());
                return false;
            }
            scan = programchar+1;   // first BRANCH
            if (OPERATOR(REGNEXT(scan))==END)
            {
                // only one top-level choice
                scan = OPERAND(scan);
                // starting-point information
                if (OPERATOR(scan)==EXACTLY)
                    regstart = *OPERAND(scan);
                else if (OPERATOR(scan)==BOL)
                    reganch = true;
                // If there is something expensive in the regexp, find the longest
                // literal string that must appear and make it the regmust.
                // Resolve ties in favor of later strings, since the regstart check works with the 
                // beginning of the regexp and avoiding duplication strengthens checking.
                // Not a strong reason, but sufficient in the absence of others.
                if (flags&SPSTART)
                {
                    TCHAR* longest = NULL;
                    size_t length = 0;
                    for (; scan!=NULL; scan=REGNEXT(scan))
                    {
                        if (OPERATOR(scan)==EXACTLY && _tcslen(OPERAND(scan))>=length)
                        {
                            longest = OPERAND(scan);
                            length  = _tcslen(longest);
                        }
                    }
                    regmust = longest;
                    regmustlen = (int)length;
                }
            }
            return true;
        }
        /**
        *  @brief  match a regular expression against an input string
        */
        bool program::match(const TCHAR* instr)
        {
            subnum = 0;
            strinput = (TCHAR*)instr;
            if (strinput==NULL)
            {
                seterror(REGERR_NULL_ARG);
                return false;
            }
            // check validity of the program
            if (*programchar!=MAGIC)
            {
                seterror(REGERR_CORRUPTION);
                return false;
            }
            // if there is a "must appear" string, look for it
            if (regmust!=NULL && _tcsstr(strinput, regmust)==NULL)
                return false;

            executor regexe(this, strinput);

            // simplest case: anchored match, need be tried only once
            if (reganch)
            {
                if (regexe.matchtry(strinput))
                {
                    // count the sub-strings
                    for (subnum=0; subnum<CLOSE-OPEN && subbegp[subnum]; subnum++)
                    {
                        if (subendp[subnum]==NULL)
                            subendp[subnum] = subbegp[subnum];
                    }
                    return true;
                }
                return false;
            }

            // messy cases: unanchored match
            if (regstart!='\0')
            {
                // we know what TCHAR it must start with
                for (TCHAR* str=strinput; str!=NULL; str=_tcschr(str+1, regstart))
                {
                    if (regexe.matchtry(str))
                    {
                        for (subnum=0; subnum<CLOSE-OPEN && subbegp[subnum]; subnum++)
                        {
                            if (subendp[subnum]==NULL)
                                subendp[subnum] = subbegp[subnum];
                        }
                        return true;
                    }
                }
                return false;
            }
            else
            {
                // we do not know - general case: match it one TCHAR by one TCHAR
                for (TCHAR* str=strinput; (*str)!='\0'; str++)
                {
                    if (regexe.matchtry(str))
                    {
                        for (subnum=0; subnum<CLOSE-OPEN && subbegp[subnum]; subnum++)
                        {
                            if (subendp[subnum]==NULL)
                                subendp[subnum] = subbegp[subnum];
                        }
                        return true;
                    }
                }
                return false;
            }
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  regexp
        //
        //////////////////////////////////////////////////////////////////////////

        /**
        *  @brief  Construct a regular expression given the expression string
        *
        *  ignorecase is used to indicate whether the match will be case sensitive.
        *  User should use compiled() to determine whether regexp is compiled ok.
        *
        *  @param  expstr     [in]  The string representation of the regular expression
        *  @param  ignorecase [in]  Whether the match operation will be case insensitive
        */
        regexp::regexp(const TCHAR* expstr, bool ignorecase)  : regprog(new program(expstr, ignorecase))
        {
        }
        regexp::regexp(const regexp& right) : regprog(new program(*(right.regprog)))
        {
        }
        regexp::~regexp()
        {
            delete regprog;
        }
        /**
        *  @brief  Assignment operator
        */
        const regexp& regexp::operator=(const regexp& right)
        {
            if (this!=&right)
            {
                delete regprog;
                regprog = new program(*(right.regprog));
            }
            return *this;
        }
        /**
        *  @brief  Get the error/status message for the most recent operation
        *
        *  Errors include, but not limited to, various forms of compilation errors,
        *  usually syntax errors, and match errors. 
        */
        /*const TCHAR* regexp::errormsg() const
        {
            if (regprog==NULL)
                return "NULL regular expression";
            else
                return regprog->geterrormsg();
        }*/
        /**
        *  @brief  Judge whether the regular expression is compiled OK or not
        */
        bool regexp::compiled() const
        {
            return (regprog==NULL)?false:regprog->compiled();
        }
        /**
        *  @brief  Match a string against the regular expression, return true if there is a match
        *
        *  Examine the string with this regular expression, return true if there is a match.
        *  After a successful match, the sub-strings of the match can be obtained.
        *  The 0th sub-string is the substring that matches the whole regular expression.
        *  The others are those who matches parenthesized expressions within the regular expression,
        *  which are numbered in left-to-right order of their opening parentheses.
        *  If a parenthesized expression does not participate in the match at all, its length is 0.
        *
        *  @param  str [in]  The string to be matched against the regular expression
        *  @return Whether there is a match between the string and the regular expression
        */
        bool regexp::match(const TCHAR* str) const
        {
            if (regprog==0)
                return false;
            else
                return regprog->match(str);
        }
        /**
        *  @brief  Return the number of sub-strings after a successful match. -1 for error.
        */
        int regexp::substrings() const
        {
            return (regprog==0)?-1:regprog->subnum;
        }
        /**
        *  @brief  Get the starting offset of the i-th matched sub-string from the
        *          beginning of the input string used in match(). -1 for error.
        */
        int regexp::substart(int idx) const
        {
            if (regprog==0)
                return -1;
            return (int)(regprog->subbegp[idx]-regprog->strinput);
        }
        /**
        *  @brief  Get the length of the i-th match sub-string. -1 for error.
        */
        int regexp::sublength(int idx) const
        {
            if (regprog==0)
                return -1;
            return (int)(regprog->subendp[idx]-regprog->subbegp[idx]);
        }
    } // End of namespace REGEXP
} // End of namespace cst
