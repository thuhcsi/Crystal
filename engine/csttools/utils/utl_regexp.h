
#ifndef _CST_TTS_BASE_CMN_REGEXP_H_
#define _CST_TTS_BASE_CMN_REGEXP_H_

#ifndef _TCHAR_DEFINED
    #ifdef UNICODE
        typedef wchar_t TCHAR;
    #else
        typedef char TCHAR;
    #endif
    #define _TCHAR_DEFINED
#endif


namespace cst
{
    namespace RegExp
    {

        /**
        *  @brief  A simple regular expression parser
        */
        class program;

        /**
        *  @brief  A simple class for regular expression operations
        *
        *  @note   Compiling a regular expression might be time consuming
        *
        *  So the user should declare a STATIC regexp object in his/her program,
        *  and just compile once in the construction of the regexp object,
        *  and use compiled() to determine whether the regexp is compiled OK.
        */
        class regexp
        {
        public:
            regexp(const TCHAR* expstr, bool ignorecase=false);
            regexp(const regexp& right);
            ~regexp();
            const regexp& operator=(const regexp& right);

        public:
            /*const TCHAR* errormsg() const;*/
            bool compiled() const;
            bool match(const TCHAR* str) const;
            int  substrings() const;
            int  substart(int idx) const;
            int  sublength(int idx) const;

        private:
            program* regprog;
        };

        /**
        *  @brief  The class for error information manipulations
        */
        class errorhandler
        {
        public:
            errorhandler() : errid(0) {}
            void seterror(int id) {errid = id;}
            int  geterror() const {return errid;}
            /*const TCHAR* geterrormsg() const {return errormsgs[errid].errormsg;}*/
        private:
            int errid;
        protected:
            typedef enum ErrorID {
                REGERR_NO_ERROR = 0,
                REGERR_SENTINEL_VALUE = 1,
                REGERR_NULL_ARG,
                REGERR_CORRUPTION,
                REGERR_BAD_REGREPEAT,
                REGERR_TOO_BIG,
                REGERR_TOO_MANY_PAREN,
                REGERR_UNTERMINATED_PAREN,
                REGERR_UNMATCHED_PAREN,
                REGERR_UNEXPECTED_JUNK,
                REGERR_OP_COULD_BE_EMPTY,
                REGERR_NESTED_OP,
                REGERR_INVALID_RANGE,
                REGERR_UNMATCHED_BRACE,
                REGERR_UNEXPECTED_CHAR,
                REGERR_OP_FOLLOWS_NOTHING,
                REGERR_TRAILING_ESC,
                REGERR_NO_REGEXP,
            } ErrorID;
 /*           static const struct ErrorMsg {
                int   errorid;
                TCHAR* errormsg;
            } errormsgs[];*/
        };

        /**
        *  @brief  The class for the "program" accessing
        *
        *  A "program" is an internal representation of the compiled regular expressions,
        *  which is stored in the class regexp as TCHAR string (TCHAR*).
        */
        class accessor : public errorhandler
        {
        public:
            static inline TCHAR OPERATOR(TCHAR* p);   ///< Get the current OPERATOR for the program
            static inline TCHAR* OPERAND(TCHAR* p);   ///< Get the OPERAND of the current program OPERATOR
            static inline TCHAR* REGNEXT(TCHAR* p);   ///< Get next regular expression program
        protected:
            typedef enum REStatus {
                //def    number  opnd?  meaning
                END     =   0,  // no   end of program.
                BOL     =   1,  // no   match beginning of line.
                EOL     =   2,  // no   match end of line.
                ANY     =   3,  // no   match any character.
                ANYOF   =   4,  // str  match any of these.
                ANYBUT  =   5,  // str  match any but one of these.
                BRANCH  =   6,  // node match this, or the next..\&.
                BACK    =   7,  // no   next pinter points backward.
                EXACTLY =   8,  // str  match this string.
                NOTHING =   9,  // no   match empty string.
                STAR    =   10, // node match this 0 or more times.
                PLUS    =   11, // node match this 1 or more times.
                WORDA   =   12, // no   match "" at word TCHAR, where prev is none-word
                WORDZ   =   13, // no   match "" at none-word TCHAR, where prev is word
                OPEN    =   20, // no   sub-RE starts here. OPEN+1 is number 1, etc.
                CLOSE   =   99, // no   analogous to OPEN. the range between OPEN and CLOSE defines maximum sub-RE.
            } REStatus;
            typedef enum REFlag {
                HASWIDTH=   01, // known never to match null string
                SIMPLE  =   02, // simple enough to be STAR/PLUS operand
                SPSTART =   04, // starts with * or +
                WORST   =   00, // worst case
            } REFlag;
        };

        /**
        *  @brief  The basic class for regular expression compiler, 
        *          which also checks the validation of the regular expression.
        */
        class compilerbase : public accessor
        {
        public:
            compilerbase(const TCHAR* parse);
            TCHAR* parse(bool hasparent, int& flag);
        protected:
            virtual void emitcode(int c ) = 0;           ///< emit (if appropriate) a byte of code
            virtual TCHAR*emitnode(int op) = 0;           ///< emit a node
            virtual void insert(TCHAR op, TCHAR* opnd) = 0;///< insert an operator in front of already-emitted operand
            virtual void tail(TCHAR* p, TCHAR* val) = 0;   ///< set the next-pointer at the end of a node chain
            virtual void optail(TCHAR* p, TCHAR* val) = 0; ///< tail on operand of first argument; NOP if no operand
        protected:
            TCHAR* atom(int& flag);
            TCHAR* branch(int& flag);
            TCHAR* piece(int& flag);
            TCHAR* regstr;   // current regexp in parsing
            int   regnum;   // number of sub-string
        };
        /**
        *  @brief  The class to check validation of a regexp,
        *          which also stores the size of the compiled regexp if valid.
        */
        class validator : public compilerbase
        {
        public:
            validator(const TCHAR* parse);
            /// get the size of the compiled regexp
            inline long size() const {return regsize;}
        protected:
            virtual void emitcode(int c)  {regsize++;}
            virtual TCHAR*emitnode(int op) {regsize+=3; return regdummy;}
            virtual void insert(TCHAR op, TCHAR* opnd) {regsize+=3;}
            virtual void tail(TCHAR* p, TCHAR* val) {}
            virtual void optail(TCHAR* p, TCHAR* val) {}
        private:
            long regsize;     // code size
            TCHAR regdummy[3]; // NOTHING, next pointer is 0
        };

        /**
        *  @brief  The class to compile a regular expression into internal "program" representation.
        */
        class compiler : public compilerbase
        {
        public:
            compiler (const TCHAR* parse, TCHAR* program);
        protected:
            virtual void emitcode(int c);
            virtual TCHAR*emitnode(int op);
            virtual void insert(TCHAR op, TCHAR* opnd);
            virtual void tail(TCHAR* p, TCHAR* val);
            virtual void optail(TCHAR* p, TCHAR* val);
        private:
            TCHAR* regcode;  // regular expression program pointer
        };

        /**
        *  @brief  The class to execute the regular expression matching, replacing etc.
        */
        class executor : public accessor
        {
        public:
            executor(program* prog, const TCHAR* inputstr);
            bool matchtry(TCHAR* str);
        protected:
            bool match(TCHAR* program);
            size_t repeat(TCHAR* prognode);
        private:
            TCHAR*  strinput;    // pointer to the string to be matched
            TCHAR*  strbegin;    // beginning of the string, for ^ check
            TCHAR** subbegp;     // pointer to the sub-string beginning
            TCHAR** subendp;     // pointer to the sub-string ending
            program* regprog;   // pointer to the regexp being executed
        };

        /**
        *  @brief  The class where regular expression is really compiled and stored
        */
        class program : public accessor
        {
        public:
            program(const TCHAR* exp, bool icase);
            program(const program& right);
            ~program() {clear();}
        protected:
            bool compiled() const {return compiledok;}
            bool match(const TCHAR* instr);
            void igncase(const TCHAR* src, TCHAR* out);
            bool compile(const TCHAR* expstr);
            bool clear();
        public:
            // The first byte of the regexp internal "program" is actually this magic number;
            // the start node begins in the second byte.
            static const TCHAR MAGIC = '\234';
        private:
            friend class regexp;
            friend class executor;
            bool  compiledok;   // whether the regular expression is compiled OK or not
            TCHAR* programchar;  // the compiled regular expression "program"
            int   programsize;  // the size of the compiled regular expression
            TCHAR**subbegp;      // pointers to the sub-string beginning, \0 is the whole matched string
            TCHAR**subendp;      // pointers to the sub-string ending
            int   subnum;       // numbers  of the sub-strings
            TCHAR* strinput;     // keeps pointer to the input string for the last matching
        private:
            TCHAR  regstart;     // TCHAR that must begin a match, '\0' if none obvious
            bool  reganch;      // is the match anchored (at beginning-of-line only)?
            TCHAR* regmust;      // string (pointer into program) that match must include, or NULL
            int   regmustlen;   // length of the regmust string
            //
            // The above 4 fields are present to pass information from compiler to executor
            // that permits the execute phase to run faster on simple cases.
            //
            //     regstart and reganch: permit very fast decisions on suitable starting points
            // for a match, cutting down the work a lost. \n
            //     regmust: permits fast rejection of lines that can not possibly match. The regmust
            // tests are so costly that compile() supplies a regmust only if the regexp contains
            // something potentially expensive (at present, the only such thing detected is * or +
            // at the start of the regexp, which can involve a lot of backups). \n
            //     regmustlen: is supplied because the test in match() needs it.
            //
        };
    } //RegExp
} //cst


namespace cst
{
    namespace cmn
    {
        class CRegexp:public RegExp::regexp
        {
        public:
            CRegexp(const TCHAR* expstr, bool ignorecase=false):regexp(expstr,ignorecase){}
            CRegexp(const CRegexp& right):regexp(right){}
        };
    }
}

#endif//_CST_TTS_BASE_CMN_REGEXP_H_
