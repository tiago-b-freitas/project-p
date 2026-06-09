#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define STB_C_LEXER_IMPLEMENTATION
#include "stb_c_lexer.h"

#define NOB_STRIP_PREFIX
#include "nob.h"

#define MAX_ARGS 10
#define MAX_LABELS 10

#include "parser.h"
#include "vec.h"

typedef struct {
   long kind;
   double real_number;
   long   int_number;
   char *string;
} Token;

void print_token(Token *token)
{
   switch (token->kind) {
      case CLEX_id        : printf("_%s", token->string); break;
      case CLEX_eq        : printf("=="); break;
      case CLEX_noteq     : printf("!="); break;
      case CLEX_lesseq    : printf("<="); break;
      case CLEX_greatereq : printf(">="); break;
      case CLEX_andand    : printf("&&"); break;
      case CLEX_oror      : printf("||"); break;
      case CLEX_shl       : printf("<<"); break;
      case CLEX_shr       : printf(">>"); break;
      case CLEX_plusplus  : printf("++"); break;
      case CLEX_minusminus: printf("--"); break;
      case CLEX_arrow     : printf("->"); break;
      case CLEX_andeq     : printf("&="); break;
      case CLEX_oreq      : printf("|="); break;
      case CLEX_xoreq     : printf("^="); break;
      case CLEX_pluseq    : printf("+="); break;
      case CLEX_minuseq   : printf("-="); break;
      case CLEX_muleq     : printf("*="); break;
      case CLEX_diveq     : printf("/="); break;
      case CLEX_modeq     : printf("%%="); break;
      case CLEX_shleq     : printf("<<="); break;
      case CLEX_shreq     : printf(">>="); break;
      case CLEX_eqarrow   : printf("=>"); break;
      case CLEX_dqstring  : printf("\"%s\"", token->string); break;
      case CLEX_sqstring  : printf("'\"%s\"'", token->string); break;
      case CLEX_charlit   : printf("'%s'", token->string); break;
      #if defined(STB__clex_int_as_double) && !defined(STB__CLEX_use_stdlib)
      case CLEX_intlit    : printf("#%g", token->real_number); break;
      #else
      case CLEX_intlit    : printf("#%ld", token->int_number); break;
      #endif
      case CLEX_floatlit  : printf("%g", token->real_number); break;
      default:
         if (token->kind >= 0 && token->kind < 256)
            printf("%c", (int) token->kind);
         else {
            printf("<<<UNKNOWN TOKEN %ld >>>\n", token->kind);
         }
         break;
   }
}

typedef struct {
    Token *items;
    size_t count;
    size_t capacity;
    size_t i;
} Stream;

#define s_get(s) (((s)->items)[(s)->i++])
#define s_putback(s) ((s)->i--)

float expression(Stream *stream);
float parse_id(Stream *stream, Engine *engine, const char *id_str);

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
    case CLEX_id: {
        return parse_id(stream, NULL, t.string);
    } break;
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

float parse_id(Stream *stream, Engine *engine, const char *id_str)
{
    Token t = s_get(stream);
    if (t.kind != '(') {
        printf("Expected `(`, after vector, get `%ld`\n", t.kind);
        return -1.0f;
    }
    float args[MAX_ARGS] = {0};
    char labels[MAX_LABELS][MAX_SIZE_LABEL+1] = {0};
    size_t i_args = 0;
    size_t i_labels = 0;

    for (;;) {
        // debug
        /* print_token(&stream->items[stream->i]); */
        /* printf("\n"); */

        if (stream->i >= stream->count) break;
        if (stream->items[stream->i].kind == ')') break;
        if (stream->items[stream->i].kind ==  CLEX_dqstring) {
            strcpy(labels[i_labels++], stream->items[stream->i].string);
            stream->i++;
            continue;
        }
        args[i_args++] = expression(stream);
        if (stream->items[stream->i].kind == ',') stream->i++;
    }
    if (strcmp(id_str, "quit") == 0) {
        engine->should_close = true;
    } else if (strcmp(id_str, "vector") == 0) {
        if (i_args == 4 && i_labels == 1) {
            Vec2 v = new_vector((Vector2) { args[0], args[1] }, args[2], args[3], RAYWHITE, labels[0]);
            da_append(engine->v_ar, v);
        }
    } else if (strcmp(id_str, "sqrt") == 0) {
        if (i_args == 1) {
            stream->i++;
            return sqrtf(args[0]);
        }
    } else if (strcmp(id_str, "deg2rad") == 0) {
        if (i_args == 1) {
            stream->i++;
            return args[0] * DEG2RAD;
        }
    } else if (strcmp(id_str, "print_vec") == 0) {
        if (i_labels == 1) {
            for (size_t i = 0; i < engine->v_ar->count; ++i) {
                if (strcmp(engine->v_ar->items[i].label, labels[0]) == 0) {
                    print_vec(engine->v_ar->items[i]);
                }
            }
            stream->i++;
        }
    }
    return -1.0f;
}

void parse_input (Engine *engine)
{
    char *input_stream = engine->input_chars;
    stb_lexer lex = {0};
    const int store_length = 200;
    char string_store[store_length+1];

    stb_c_lexer_init(&lex, input_stream, input_stream+strlen(input_stream), string_store, store_length);
    Stream stream = {0};
    while (stb_c_lexer_get_token(&lex)) {
        Token t = {
            .kind = lex.token,
            .real_number = lex.real_number,
            .int_number = lex.int_number,
            .string = strdup(lex.string)
        };
        da_append(&stream, t);
    }

    for (; stream.i < stream.count; ++stream.i) {
        Token t = s_get(&stream);
        switch (t.kind) {
        case CLEX_parse_error: {
            printf("<<<PARSE ERROR>>>");
            return;
        }
        case CLEX_id: {
            parse_id(&stream, engine, t.string);
        } break;
        default: {
            printf("%ld\n", t.kind);
            assert(false && "TODO: token type not implemented\n");
        }
        }

    }
}

// TODO: mostrar uma mensagem de erro.
// TODO: na macro s_get é preciso verificar se a array não acabou, talvez fazer com uma função seja mais confiável

