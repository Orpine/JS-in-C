# JS-in-C

---

#Lex
##Usage

    Lex l(const string& str)
or

    Lex l;
    l.getLex();

after that, directly use l.tokens:
    
    vector<Token> tokens;

    class Token{
    public:
        Token() {};
        Token(TOKEN_TYPES _type, const string& _value): type(_type), value(_value) {};
        void setToken(TOKEN_TYPES _type, const string& _value){
            type = _type;
            value = _value;
        }
        TOKEN_TYPES type;
        string value;
    };
    


##SUPPORT
* " " is not required, tokens can be adjacent to each other.
* tokens defined in TOKEN_TYPES

Numbers:
	signed decimal;

    positive octal(prefix '0'), hex(prefix '0x', '0X');
    
    signed float:

	"^[\+-]?((([1-9]\d*)?\.\d+|[1-9]\d*(\.\d*)?)[eE][\+-]?[1-9]\d*|([1-9]\d*)?\.\d+)"

* Comments removed.
To remove the Comments correctly, you need to add '\n' to every line so that I can judge the range of '\\'-type comments.

And if the script contains multi-line comments, you need to read the whole text, then send to Lex object.

    ifstream input(filePath, std::ios::in);
   
    while (input.getline(buffer, 1024)) {
        string line(buffer);
        wholeText += line + endline;
    }


##RESTRICTION

1. the tokens that can appear before numbers with '+/-' prefix:
    
    set<TOKEN_TYPES> tokenBeforePrefix{TK_L_BRACKET, TK_ASSIGN, TK_PLUS_EQUAL, TK_MINUS_EQUAL, TK_MULTI_EQUAL, TK_DIV_EQUAL, TK_MOD_EQUAL, TK_AND_EQUAL,
        TK_OR_EQUAL,
        TK_L_EQUAL,
        TK_G_EQUAL, };

2. do not support operators '>>>' and '>>>='

---

