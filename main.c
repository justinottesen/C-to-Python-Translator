#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

int get_token(char** token, unsigned int* len) {
  char* temp = *token;
  temp++;
  // Variable name
  if (isalpha(**token) || **token == '_') {
    while (isalnum(*temp) || *temp == '_') { temp++; }
    if (!strncmp(*token, "char", 4) && temp - *token == 4) { temp++; }
  }
  // Number
  else if (isdigit(**token)) {
    while (isdigit(*temp)) { temp++; }
  }
  // String Literal
  else if (**token == '"') {
    while (*temp != '"') { temp++; }
    temp++;
  }
  *len = temp-*token;
  if (**token == '\0') return 0;
  return 1;
}

void inc_token(char** token) {
  if (**token == '\0') { return; }
  // Get out of current token
  if (isalpha(**token) || **token == '_') {
    int temp = strncmp("char", *token, 4);
    while (isalnum(**token) || **token == '_') { ++*token; }
    if (!temp) { ++*token; }
  } else if (isdigit(**token)) {
    while (isdigit(**token)) { ++*token; }
  } else if (**token == '"') {
    ++*token;
    while (**token != '"') { ++*token; }  
    ++*token;
  } else {
    ++*token;
  }
  // Find next token
  while (isspace(**token) && **token != '\0') { ++*token; }
}

void print_token(char* token, int len) {
  for (int i = 0; i < len; i++) {
    printf("%c", token[i]);
  }
}

typedef enum {UNKNOWN, MAIN, PRINT, RETURN, TYPE, NAME, NUM, LITERAL, OPEN_P, CLOSE_P, OPEN_B, CLOSE_B, SEMICOLON, COLON, COMMA, EQUATE, ASSIGN} Token_T;

typedef struct {
  char** data;
  Token_T* types;
  unsigned int size;
  unsigned int alloc;
} Tokens;

void T_init(Tokens* T, unsigned int size) {
  T->data = (char**)malloc(sizeof(char*)*size);
  T->size = 0;
  T->alloc = size;
}

void T_resize(Tokens* T) {
  T->alloc *= 2;
  T->data = realloc(T->data, sizeof(char*)*T->alloc);
}

void T_push_back(Tokens* T, char* token, unsigned int len) {
  if (T->size == T->alloc) { T_resize(T); }
  T->data[T->size] = (char*)malloc(sizeof(char)*(len+1));
  for (unsigned int i = 0; i < len; i++) {
    T->data[T->size][i] = token[i];
  }
  T->data[T->size][len] = '\0';
  ++T->size;
}

void T_destruct(Tokens* T) {
  for (unsigned int i = 0; i < T->size; i++) {
    free(T->data[i]);
  }
  free(T->data);
}

void T_get_types(Tokens* T) {
  T->types = (Token_T*)malloc(sizeof(Token_T*)*T->alloc);
  Token_T type;
  for (unsigned int i = 0; i < T->size; i++) {
    if (isdigit(T->data[i][0]))             { type = NUM;     }
    else if (T->data[i][0] == '"')          { type = LITERAL; }
    else if (T->data[i][0] == '(')          { type = OPEN_P;  }
    else if (T->data[i][0] == ')')          { type = CLOSE_P; }
    else if (T->data[i][0] == '{')          { type = OPEN_B;  }
    else if (T->data[i][0] == '}')          { type = CLOSE_B; }
    else if (T->data[i][0] == ';')          { type = SEMICOLON; }
    else if (T->data[i][0] == ':')          { type = COLON; }
    else if (T->data[i][0] == ',')          { type = COMMA; }
    else if (!strcmp("==", T->data[i]))     { type = EQUATE; }
    else if (T->data[i][0] == '=')          { type = ASSIGN; }
    else if (!strcmp("main", T->data[i]))   { type = MAIN; }
    else if (!strcmp("printf", T->data[i])) { type = PRINT; }
    else if (!strcmp("return", T->data[i])) { type = RETURN; }
    else if (!(strcmp("int", T->data[i])   && strcmp("char*", T->data[i])    &&
               strcmp("char", T->data[i])  && strcmp("unsigned", T->data[i]) &&
               strcmp("short", T->data[i]) && strcmp("long", T->data[i])     &&
               strcmp("float", T->data[i]) && strcmp("double", T->data[i])   &&
               strcmp("void", T->data[i]))) { type = TYPE; }
    else if (isalpha(T->data[i][0]) || T->data[i][0] == '_') { type = NAME; }
    else { type = UNKNOWN; }
    T->types[i] = type;
  }
}

void T_read_file(Tokens* T, FILE* c_file) {
  char buff[256];
  char* token;
  unsigned int len;
  while(fgets(buff, 256, c_file)) {
    char* temp = buff;
    while (*temp != '\n' && *temp != '\0') { temp++; }
    *temp = '\0';
    if (*buff == '#') continue;
    token = buff;
    if (isspace(*token)) { inc_token(&token); }
    while (get_token(&token, &len)) {
      T_push_back(T, token, len);
      inc_token(&token);
    }
  }
  T_get_types(T);
}

void T_write_file(Tokens* T, FILE* py_file) {
  int indent_level = 0;
  int EOL = 0;
  int in_print = 0;
  for (unsigned int i = 0; i < T->size; i++) {
    if (T->types[i] == TYPE) { continue; }
    if (T->types[i] == MAIN) {
      EOL = 1;
      fprintf(py_file, "if __name__ == \"__main__\":");
      indent_level++;
      i+= 3;
    } else if (T->types[i] == SEMICOLON) {
      EOL = 1;
    } else if (T->types[i] == OPEN_B) {
      fprintf(py_file, ":");
      EOL = 1;
      indent_level++;
    } else if (T->types[i] == CLOSE_B) {
      EOL = 1;
      indent_level--;
    } else if (T->types[i] == PRINT) {
      in_print = 1;
      fprintf(py_file, "print");
    } else if (T->types[i] == LITERAL) {
      if (in_print) {
        fprintf(py_file, "f");
        int offset = 0;
        for (unsigned int j = 0; j < strlen(T->data[i]); j++) {
          if (T->data[i][j] != '%') { fprintf(py_file, "%c", T->data[i][j]); continue; }
          if (T->data[i][j+1] == '%') { fprintf(py_file, "%%"); continue; }
          offset += 2;
          fprintf(py_file, "{%s}", T->data[i+offset]);
          j++;
          if (T->data[i][j] == 'h' || tolower(T->data[i][j]) == 'l') { j++; }
        }
        i += offset;
      } else {
        fprintf(py_file, "%s", T->data[i]);
      }
    } else if (T->types[i] == CLOSE_P) {
      if (in_print) { in_print = 0; fprintf(py_file, ", end=\"\""); }
      fprintf(py_file, ")");
    } else if (T->types[i] == NAME) {
      if (i > 0 && T->types[i-1] == TYPE && i+1 < T->size && T->types[i+1] == OPEN_P) { fprintf(py_file, "def "); }
      fprintf(py_file, "%s", T->data[i]);
    } else if (T->types[i] == RETURN) {
      fprintf(py_file, "return ");
    } else {
      fprintf(py_file, "%s", T->data[i]);
    }
    if (EOL) { EOL = 0; fprintf(py_file, "\n"); for (int j = 0; j < indent_level; j++) { fprintf(py_file, "  "); } }
  }
}

int main(int argc, char* argv[]) {
  assert(argc == 3);

  FILE* c_file = fopen(argv[1], "r");
  assert(c_file);

  FILE* py_file = fopen(argv[2], "w");
  assert(py_file);

  Tokens tokens;
  T_init(&tokens, 4);
  T_read_file(&tokens, c_file);
  fclose(c_file);
  
  T_write_file(&tokens, py_file);
  fclose(py_file);

  T_destruct(&tokens);
  return 0;
}