#include "lexer.h"

#define POS_ADD(n) lexer->position += n
#define COL_ADD(n) lexer->column += n
#define LINE_ADD(n) lexer->line += n; RESET_COL
#define RESET_COL lexer->column = 0
#define IS_EOF (lexer->offset >= lexer->length)
#define OFFSET_ADD(n) lexer->offset += n;
#define INC_OFFSET_POSITION(n) OFFSET_ADD(n); POS_ADD(n)
#define PEEK(n) ((lexer->offset < lexer->length + 1) ? lexer->buffer[lexer->offset+n] : 0)
#define NEXT(n) lexer->buffer[lexer->offset+=n]; POS_ADD(n); COL_ADD(n);

// TKN macros
#define TKN_RESET				lexer->token = NO_TOKEN; lexer->token.position = lexer->position; \
								lexer->token.line = lexer->line; lexer->token.column = lexer->column
#define SET_TKNESCAPED(value)	lexer->token.escaped = value
#define SET_TKNTYPE(t)			lexer->token.type = t

static inline bool is_newline(int c){
    // many more cases
    return (c == '\n');
}

int token_int(struct lexer* lexer, struct token token){
	// get as string first --> probably a much better way
	char* value = malloc((token.end - token.start + 1)*(sizeof(char)));
    strncpy(value, lexer->buffer + token.start, token.end - token.start);
    value[token.end - token.start] = '\0';
	int x = atoi(value); // guaranteed to be int
	free(value);
	return x;
}

char* token_string(struct lexer* lexer, struct token token){
	char* value = malloc((token.end - token.start + 1)*(sizeof(char)));
    strncpy(value, lexer->buffer + token.start, token.end - token.start);
    value[token.end - token.start] = '\0';
	return value;
}

static inline bool is_identifier (int c) {
    return ((isalpha(c)) || (isdigit(c)) || (c == '_'));
}

static inline bool is_alpha (int c) {
    if (c == '_') return true;
    return isalpha(c);
}

static inline bool is_string (int c) {
    return ((c == '"') || (c == '\''));
}

uint32_t utf8_charbytes (const char *s, uint32_t i) {
    unsigned char c = s[i];

    // req. bytes (RFC 3629)
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
	lexer->column = 0;
	lexer->position = 0;
	lexer->length = len;
    return lexer;
}

void lexer_free(struct lexer* lexer){
    free(lexer);
}

static inline int next_utf8(struct lexer* lexer) {
	printf("next utf8\n");
    int c = NEXT(1);
	lexer->token.end++;
    uint32_t len = utf8_charbytes((const char *)&c, 0);
    if (len == 1) {
        return c;
    }
    switch(len) {
        case 0: lexer_error(lexer, "Unknown character inside a string literal"); return 0;
        case 2: OFFSET_ADD(1) COL_ADD(1); break;
    	case 3: OFFSET_ADD(2); COL_ADD(1); break;
        case 4: OFFSET_ADD(3); COL_ADD(1); POS_ADD(1);  break;
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
    c = NEXT(1);					// save escaped character
    TKN_RESET;				// save offset
    SET_TKNESCAPED(false);		// set escaped flag to false

    while ((c2 = (unsigned char)PEEK(0)) != c) {
        if (IS_EOF) {lexer_error(lexer, "Unexpected EOF inside a string literal");}
        if (is_newline(lexer, c2)) LINE_ADD(1);

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
    ADD_OFFSET_POSITION(1);

	printf("string with start %d and end %d\n", lexer->token.start, lexer->token.end);
    // string is from buffer->[nseek] and it is nlen length
    lexer->token.type = TKN_STRING;
	printf("Found string: %.*s\n", lexer->token.end - lexer->token.start,
		lexer->buffer + lexer->token.start);
}

int is_operator(int c){
  return ((c == '=') || (c == ',') || (c == '{') || (c == '}') ||
      (c == '[') || (c == ']') || (c == '(') ||
	  (c == ')') || (c == '!') || (c == ':') || (c == '|'));
}

// straight from Gravity
void lexer_scan_number(struct lexer *lexer) {
	printf("Scanning number\n");
    int			c;

    TKN_RESET;

    loop:
    c = PEEK(0);

	    // explicitly list all accepted cases
	    if (IS_EOF) goto report_token;
	    if (is_digit(c)) goto accept_char;
	    if (is_whitespace(c)) goto report_token;
	    if (is_newline(lexer, c)) goto report_token;

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
  	int c = PEEK_CURRENT(0);
	// current only handles one character oprators
	printf("Found operator: %c\n", c);
	switch (c) {
	case '=':
		lexer->token.type = TKN_ASSIGN; break;
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
	case '|':
		lexer->token.type = TKN_DIVIDER; break;
  	default:
		lexer->token.type = TKN_ERROR;
	}
}

void lexer_scan_identifier(struct lexer* lexer) {
	printf("scanning identifier\n");
    TKN_RESET;

	lexer->token.start = lexer->offset;
	lexer->token.end = lexer->offset;

    while (is_identifier(PEEK(0))) {
        OFFSET_ADD(1);
		COLUMN_ADD(1);
    	lexer->token.end++;
    }
    SET_TKNTYPE(TKN_IDENTIFIER);
	printf("Found identifier: %.*s\n", lexer->token.end - lexer->token.start,
		lexer->buffer + lexer->token.start);
}

struct token lexer_next(struct lexer* lexer){
	int c;
	struct token token;
	while (1) {
		if (IS_EOF) { return TKN_EOF; }
    	c = PEEK(0);
		if (is_whitespace(c)){
			ADD_OFFSET_POSITION(1);
		} else if (is_newline(c)){
			ADD_OFFSET_POSITION(1);
		} else if (is_operator(c)){
			lexer_scan_operator(lexer);
		} else if (is_alpha(c)) {
			return lexer_scan_identifier(lexer);
		} else if (is_digit(c)) {
			return lexer_scan_number(lexer);
		} else if (is_string(c)){
			return lexer_scan_string(lexer);
		} else {
			lexer_error(lexer, "Unrecognised token.");
		}
	}
	return token;
}
