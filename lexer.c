#include "lexer.h"

// LEXER macros
#define NEXT					lexer->buffer[lexer->offset++]; ++lexer->position; INC_COL
#define PEEK_CURRENT			lexer->buffer[lexer->offset]
#define PEEK_NEXT				((lexer->offset < lexer->length) ? lexer->buffer[lexer->offset+1] : 0)
#define INC_LINE				++lexer->line; RESET_COL
#define INC_COL					++lexer->col
#define DEC_COL					--lexer->col
#define RESET_COL				lexer->col = 1
#define IS_EOF					(lexer->offset >= lexer->length)
#define DEC_OFFSET				--lexer->offset; DEC_COL
#define DEC_POSITION			--lexer->position
#define DEC_OFFSET_POSITION		DEC_OFFSET; DEC_POSITION
#define INC_OFFSET				++lexer->offset; INC_COL
#define INC_POSITION			++lexer->position
#define INC_OFFSET_POSITION		INC_OFFSET; INC_POSITION

// TKN macros
#define TKN_RESET				lexer->token = NO_TOKEN; lexer->token.position = lexer->position; \
								lexer->token.line = lexer->line; lexer->token.col = lexer->col
#define SET_TKNESCAPED(value)	lexer->token.escaped = value
#define SET_TKNTYPE(t)			lexer->token.type = t

typedef enum {
    NUMBER_INTEGER,
    NUMBER_HEX,
    NUMBER_BIN,
    NUMBER_OCT
} number_type;

static inline bool is_newline(struct lexer* lexer, int c){
    // many more cases
    return (c == '\n');
}

static inline bool is_identifier (int c) {
    return ((isalpha(c)) || (isdigit(c)) || (c == '_') || (c == '(') || (c == ')'));
}

static inline bool is_alpha (int c) {
    if (c == '_') return true;
    return isalpha(c);
}

static inline bool is_digit (int c, number_type ntype) {
    if (ntype == NUMBER_BIN) return (c == '0' || (c == '1'));
    if (ntype == NUMBER_OCT) return (c >= '0' && (c <= '7'));
    if ((ntype == NUMBER_HEX) && ((toupper(c) >= 'A' && toupper(c) <= 'F'))) return true;
    return isdigit(c);
}

static inline bool is_string (int c) {
    return ((c == '"') || (c == '\''));
}

uint32_t utf8_charbytes (const char *s, uint32_t i) {
    unsigned char c = s[i];

    // determine bytes needed for character, based on RFC 3629
    if ((c > 0) && (c <= 127)) return 1;
    if ((c >= 194) && (c <= 223)) return 2;
    if ((c >= 224) && (c <= 239)) return 3;
    if ((c >= 240) && (c <= 244)) return 4;

    // means error
    return 0;
}

int lexer_error(struct lexer* lexer, char* error){
    return -1;
}

struct lexer* lexer_create(const char* src, size_t len){
    struct lexer* lexer = malloc(sizeof(struct lexer));
	lexer->buffer = src;
	lexer->offset = 0;
	lexer->line = 0;
	lexer->col = 0;
	lexer->position = 0;
	lexer->length = len;
	lexer->peeking = false;
    return lexer;
}

void lexer_free(struct lexer* lexer){
    free(lexer);
}

static inline int next_utf8(struct lexer* lexer) {
	printf("next utf8\n");
    int c = NEXT;
	lexer->token.end++;
    uint32_t len = utf8_charbytes((const char *)&c, 0);
    if (len == 1) {
        return c;
    }
    switch(len) {
        case 0: lexer_error(lexer, "Unknown character inside a string literal"); return 0;
        case 2: INC_OFFSET; break;
    	case 3: INC_OFFSET; INC_OFFSET; break;
        case 4: INC_OFFSET; INC_OFFSET; INC_OFFSET; INC_POSITION;  break;
    }
    return c;
}

void lexer_scan_string(struct lexer* lexer) {
	printf("Scanning string\n");
    int c, c2;

	printf("set token start to offset %d\n", lexer->offset);
	lexer->token.start = lexer->offset;
	lexer->token.end = lexer->offset;

    // no memory allocation here
    c = NEXT;					// save escaped character
    TKN_RESET;				// save offset
    SET_TKNESCAPED(false);		// set escaped flag to false

    while ((c2 = (unsigned char)PEEK_CURRENT) != c) {
        if (IS_EOF) {lexer_error(lexer, "Unexpected EOF inside a string literal");}
        if (is_newline(lexer, c2)) INC_LINE;

        // handle escaped characters
        if (c2 == '\\') {
            SET_TKNESCAPED(true);
            lexer->offset+=2;
			lexer->token.end+=2;
            continue;
        }
        // scan next
        next_utf8(lexer);
    }

    // skip last escape character
    INC_OFFSET_POSITION;

	printf("string with start %d and end %d\n", lexer->token.start, lexer->token.end);
    // string is from buffer->[nseek] and it is nlen length
    lexer->token.type = TKN_STRING;
	printf("Found string: %.*s\n", lexer->token.end - lexer->token.start,
		lexer->buffer + lexer->token.start);
}

int is_operator(int c){
  return ((c == '=') || (c == ',') || (c == '{') || (c == '}') ||
      (c == '[') || (c == ']') || (c == '(') || (c == ')') || (c == '!'));
}

// straight from Gravity
void lexer_scan_number(struct lexer *lexer) {
	printf("Scanning number\n");
    int			c;
    int			minusSign = '-';

    number_type	ntype = NUMBER_INTEGER;

    TKN_RESET;

    loop:
    c = PEEK_CURRENT;

    // explicitly list all accepted cases
    if (IS_EOF) goto report_token;
    if (is_digit(c, ntype)) goto accept_char;
    if (is_whitespace(c)) goto report_token;
    if (is_newline(lexer, c)) goto report_token;

    if (floatAllowed) {
        if ((c == floatChar) && (!is_digit(PEEK_NEXT, ntype))) {
            goto report_token;
        }
        if ((c == floatChar) && (!dotFound))  {
            dotFound = true;
            goto accept_char;
        }
    }
    if (signAllowed) {
        if (c == minusSign) {
            signAllowed = false;
            goto accept_char;
        }
    }
    if (is_operator(c)) {
        goto report_token;
    }

    // any other case is an error
    goto report_error;

    accept_char:
        lexer->token.end++;
        INC_OFFSET_POSITION;
        goto loop;

    report_token:
        // number is from buffer->[nseek] and it is bytes length
        lexer->token.type = TKN_NUMBER;
		return;

        //return TKN_NUMBER;

    report_error:
        lexer_error(lexer, "Malformed number expression.");
}

void lexer_scan_operator(struct lexer* lexer){
	printf("Scanning operator\n");
  	int c = lexer->buffer[lexer->offset++];
	// current only handles one character oprators
	printf("Found operator: %c\n", c);
	switch (c) {
	case '=':
		lexer->token.type = TKN_ASSIGN;break;
	case ',':
	  	lexer->token.type = TKN_COMMA;break;
	case '{':
	  	lexer->token.type = TKN_OPEN_BRACE;break;
	case '}':
	  	lexer->token.type = TKN_CLOSE_BRACE;break;
	case '[':
	  	lexer->token.type = TKN_OPEN_SQUARE;break;
	case ']':
	  	lexer->token.type = TKN_CLOSE_SQUARE; break;
	case '(':
		lexer->token.type = TKN_OPEN_BRACKET; break;
	case ')':
		lexer->token.type = TKN_CLOSE_BRACKET; break;
	case '!':
		lexer->token.type = TKN_EXCLAMATION; break;
	case ':':
		lexer->token.type = TKN_COLON; break;
  	default:
		lexer->token.type = TKN_ERROR;
	}
}

void lexer_scan_identifier(struct lexer* lexer) {
	printf("scanning identifier\n");
    TKN_RESET;

	lexer->token.start = lexer->offset;
	lexer->token.end = lexer->offset;

    while (is_identifier(PEEK_CURRENT)) {
        lexer->offset++;
    	lexer->token.end++;
    }
    lexer->token.type = TKN_IDENTIFIER;
	printf("Found identifier: %.*s\n", lexer->token.end - lexer->token.start,
		lexer->buffer + lexer->token.start);
}

struct token* lexer_tokenize(struct lexer* lexer, size_t* num_tokens){
	size_t tokens_alloc = 1, tokens_used = 0;
	struct token* tokens = calloc(tokens_alloc, sizeof(struct token));
	while (lexer->offset < lexer->length){
		int c = lexer->buffer[lexer->offset];
		if (is_whitespace(c)){
			lexer->offset++;
			continue;
		} else if (is_newline(lexer, c)){
			lexer->offset++;
			continue;
		} else if (is_operator(c)){
			lexer_scan_operator(lexer);
		} else if (is_alpha(c)){
			lexer_scan_identifier(lexer);
		} else if (is_digit(c, false)){
			lexer_scan_number(lexer);
		} else if (is_string(c)){
			lexer_scan_string(lexer);
		}
		if (tokens_used == tokens_alloc){
			tokens_alloc *= 2;
			tokens = realloc(tokens, tokens_alloc * sizeof(struct token));
		}
		tokens[tokens_used++] = lexer->token;
	}
	if (tokens_used == 0) {
        free(tokens);
        tokens = NULL;
    } else {
        tokens = realloc(tokens, tokens_used * sizeof(char*));
    }
	*num_tokens = tokens_used;
	return tokens;
}
