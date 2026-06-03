#include <assert.h>
#include <stdio.h>
#include <string.h>

#define STB_C_LEXER_IMPLEMENTATION
#include "stb_c_lexer.h"

#define NOB_STRIP_PREFIX
#define NOB_IMPLEMENTATION
#include "nob.h"

typedef struct {
   long kind;
   double real_number;
   long   int_number;
   char *string;
} Token;

typedef struct {
    Token *items;
    size_t count;
    size_t capacity;
    size_t i;
} Stream;

typedef struct {
    float *items;
    size_t count;
    size_t capacity;
} Args;

void print_token(Token *token)
{
   switch (token->kind) {
      case CLEX_id        : printf("CLEX_id        _%s", token->string); break;
      case CLEX_eq        : printf("CLEX_eq        =="); break;
      case CLEX_noteq     : printf("CLEX_noteq     !="); break;
      case CLEX_lesseq    : printf("CLEX_lesseq    <="); break;
      case CLEX_greatereq : printf("CLEX_greatereq >="); break;
      case CLEX_andand    : printf("CLEX_andand    &&"); break;
      case CLEX_oror      : printf("CLEX_oror      ||"); break;
      case CLEX_shl       : printf("CLEX_shl       <<"); break;
      case CLEX_shr       : printf("CLEX_shr       >>"); break;
      case CLEX_plusplus  : printf("CLEX_plusplus  ++"); break;
      case CLEX_minusminus: printf("CLEX_minusminus--"); break;
      case CLEX_arrow     : printf("CLEX_arrow     ->"); break;
      case CLEX_andeq     : printf("CLEX_andeq     &="); break;
      case CLEX_oreq      : printf("CLEX_oreq      |="); break;
      case CLEX_xoreq     : printf("CLEX_xoreq     ^="); break;
      case CLEX_pluseq    : printf("CLEX_pluseq    +="); break;
      case CLEX_minuseq   : printf("CLEX_minuseq   -="); break;
      case CLEX_muleq     : printf("CLEX_muleq     *="); break;
      case CLEX_diveq     : printf("CLEX_diveq     /="); break;
      case CLEX_modeq     : printf("CLEX_modeq     %%="); break;
      case CLEX_shleq     : printf("CLEX_shleq     <<="); break;
      case CLEX_shreq     : printf("CLEX_shreq     >>="); break;
      case CLEX_eqarrow   : printf("CLEX_eqarrow   =>"); break;
      case CLEX_dqstring  : printf("CLEX_dqstring  \"%s\"", token->string); break;
      case CLEX_sqstring  : printf("CLEX_sqstring  '\"%s\"'", token->string); break;
      case CLEX_charlit   : printf("CLEX_charlit   '%s'", token->string); break;
      #if defined(STB__clex_int_as_double) && !defined(STB__CLEX_use_stdlib)
      case CLEX_intlit    : printf("CLEX_intlit #%g", token->real_number); break;
      #else
      case CLEX_intlit    : printf("CLEX_intlit #%ld", token->int_number); break;
      #endif
      case CLEX_floatlit  : printf("CLEX_floatlit %g", token->real_number); break;
      default:
         if (token->kind >= 0 && token->kind < 256)
            printf("%c", (int) token->kind);
         else {
            printf("<<<UNKNOWN TOKEN %ld >>>\n", token->kind);
         }
         break;
   }
}

#define s_get(s) ((s->items)[(s)->i++])
#define s_putback(s) ((s)->i--)

float expression(Stream *stream);

float primary(Stream *stream)
{
    Token t = s_get(stream);
    switch (t.kind) {
    case '(': {
        float f = expression(stream);
        Token t = s_get(stream);
        if (t.kind != ')')
            printf("ERROR: `)` expected\n");
        return f;
    } break;
    case CLEX_intlit: return (float)t.int_number;
    case CLEX_floatlit: return t.real_number;
    case '-': {
        return -primary(stream);
    }
    default: {
        printf("\nERROR: primary expected\n");
        return -99.0f;
    }
    }
}

float term(Stream *stream)
{
    float left = primary(stream);
    Token t = s_get(stream);
    for (;;) {
        switch (t.kind) {
        case '*': {
            left *= primary(stream);
            t = s_get(stream);
        } break;
        case '/': {
            float f = primary(stream);
            if (f == 0) {
                assert(false && "TODO: handle zero division");
            }
            left /= f;
            t = s_get(stream);
        } break;
        default:
            s_putback(stream);
            return left;
        }
    }
}


float expression(Stream *stream)
{
    float left = term(stream);
    Token t = s_get(stream);
    for (;;) {
        switch (t.kind) {
        case '+': {
            left += term(stream);
            t = s_get(stream);
        } break;
        case '-': {
            left -= term(stream);
            t = s_get(stream);
        } break;
        default:
            s_putback(stream);
            return left;
        }
    }
}

void parse_id(stb_lexer *lex, Engine *engine)
{
    const char* id_str = lex->string;
    stb_c_lexer_get_token(lex);
    if (lex->token != '(') {
        printf("Expected `(`, after vector");
        return;
    }
    size_t nivel = 1;

    Stream stream = {0};
    Args args = {0};

    stb_c_lexer_get_token(lex);
    while (lex->token != CLEX_eof && nivel > 0) {
        while (lex->token != CLEX_eof && nivel > 0 && lex->token != ',') {
            if ((lex->token) == '(') nivel++;
            if ((lex->token) == ')') nivel--;
            if (nivel == 0) break;
            Token t = {
                .kind = lex->token,
                .real_number = lex->real_number,
                .int_number = lex->int_number,
                .string = lex->string
            };
            da_append(&stream, t);
            stb_c_lexer_get_token(lex);
        }

        for (size_t i = 0; i < stream.count; ++i) {
            print_token(&(stream.items[i]));
            printf("  ");
        }
        printf("\n");

        if (stream.count > 0) {
            da_append(&args, expression(&stream));
            stream.count = 0;
            stream.i = 0;
        }
        stb_c_lexer_get_token(lex);
    }
    if (stream.count > 0) {
        da_free(stream);
    }
    if (strcmp(id_str, "vector") == 0) {
        if (args.count == 4) {
            Vec2 v = new_vector((Vector2) { args.items[0], args.items[1] }, args.items[2], args.items[3], MAROON);
            da_append(engine->v_ar, v);
        }

    } else if (strcmp(id_str, "quit") == 0) {
        engine->should_close = true;

    }
}

void parse_input (Engine *engine)
{
    char *input_stream = engine->input_chars;
    stb_lexer lex;
    // char *input_stream = "vector(0+1, 0, 11.3, degree_to_rad(33.3))";
    int store_length = 100;
    char string_store[store_length+1];

    stb_c_lexer_init(&lex, input_stream, input_stream+strlen(input_stream), string_store, store_length);

    while (stb_c_lexer_get_token(&lex)) {
        switch (lex.token) {
        case CLEX_parse_error: {
            printf("<<<PARSE ERROR>>>");
            return;
        }
        case CLEX_id: {
            parse_id(&lex, engine);
        } break;
        default: assert(false && "TODO: token type not implemented\n");
        }

    }
}

// TODO: mostrar uma mensagem de erro.
// TODO: na macro s_get é preciso verificar se a array não acabou, talvez fazer com uma função seja mais confiável

