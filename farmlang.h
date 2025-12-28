#ifndef FARMLANG_H
#define FARMLANG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// ---------------------
// Estruturas e utilitários de ambiente
// ---------------------

typedef enum { FL_STRING, FL_INT } FLType;

typedef struct {
    char name[64];
    FLType type;
    union {
        char s[256];
        int i;
    } val;
} FLEntry;

typedef struct {
    FLEntry *entries;
    size_t len;
} FLEnv;

static void fl_env_init(FLEnv *env) {
    env->entries = NULL;
    env->len = 0;
}

static void fl_env_free(FLEnv *env) {
    free(env->entries);
    env->entries = NULL;
    env->len = 0;
}

__attribute__((unused))static char *fl_strdup(const char *s) {
    size_t n = strlen(s);
    char *copy = (char *)malloc(n + 1);
    if (copy) memcpy(copy, s, n + 1);
    return copy;
}

static FLEntry *fl_lookup(FLEnv *env, const char *name) {
    size_t i;
    for (i = 0; i < env->len; i++) {
        if (strcmp(env->entries[i].name, name) == 0) {
            return &env->entries[i];
        }
    }
    return NULL;
}

// ---------------------
// Funções dos animais
// ---------------------

static void pig(FLEnv *env, const char *name, const char *literal_in) {
    char buf[256];
    strncpy(buf, literal_in, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';

    FLEntry *found = fl_lookup(env, name);
    if (!found) {
        env->entries = realloc(env->entries, (env->len + 1) * sizeof(FLEntry));
        found = &env->entries[env->len++];
        strncpy(found->name, name, 63);
        found->name[63] = '\0';
    }

    // 5% chance de ignorar e setar "oink"
    if (rand() % 20 == 0) {
        found->type = FL_STRING;
        strncpy(found->val.s, "oink", 255);
        found->val.s[255] = '\0';
        return;
    }
    // tenta converter para inteiro
    char *end = NULL;
    long val = strtol(buf, &end, 10);
    if (buf[0] != '\0' && end && *end == '\0') {
        found->type = FL_INT;
        found->val.i = (int)val;
    } else {
        // remove aspas se houver
        size_t n = strlen(buf);
        if (n >= 2 && buf[0] == '"' && buf[n-1] == '"') {
            memmove(buf, buf+1, n-2);
            buf[n-2] = '\0';
        }
        found->type = FL_STRING;
        strncpy(found->val.s, buf, 255);
        found->val.s[255] = '\0';
    }
}

static void cow(FLEnv *env, const char *name) {
    FLEntry *e = fl_lookup(env, name);
    if (!e) return;

    if (rand() % 5 == 0) { // 20% falha local
        printf("The cow refuses to moo today.\n");
        return;
    }

    if (e->type == FL_STRING) printf("%s\n", e->val.s);
    else printf("%d\n", e->val.i);
}

static int cat_value(const char *token) {
    if (strcmp(token, "meow") == 0) return 1;
    if (strcmp(token, "purr") == 0) return 5;
    if (strcmp(token, "hiss") == 0) return 10;
    return 0;
}

static void cat(FLEnv *env, const char *name, const char *literal_in) {
    FLEntry *found = fl_lookup(env, name);
    if (!found) {
        env->entries = realloc(env->entries, (env->len + 1) * sizeof(FLEntry));
        found = &env->entries[env->len++];
        strncpy(found->name, name, 63);
        found->name[63] = '\0';
    }

    // 10% chance de mau humor
    if (rand() % 10 == 0) {
        found->type = FL_INT;
        found->val.i = 0;
        return;
    }

    int total = 0;
    char buf[256];
    strncpy(buf, literal_in, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';

    // cada token separado por espaço é somado
    char *tok = strtok(buf, " ");
    while (tok) {
        int meow_sum = 0;
        int product = 1;
        int pos = 0;

        // percorre sons dentro do token
        while (tok[pos]) {
            if (strncmp(tok + pos, "meow", 4) == 0) {
                meow_sum += cat_value("meow");
                pos += 4;
            } else if (strncmp(tok + pos, "purr", 4) == 0) {
                product *= cat_value("purr");
                pos += 4;
            } else if (strncmp(tok + pos, "hiss", 4) == 0) {
                product *= cat_value("hiss");
                pos += 4;
            } else {
                pos++; // ignora caracteres inválidos
            }
        }

        if (meow_sum > 0) {
            product *= meow_sum; // soma dos meows entra como fator
        }

        total += product;
        tok = strtok(NULL, " ");
    }

    found->type = FL_INT;
    found->val.i = total;
}


static void dog(FLEnv *env, const char *name, int delta) {
    FLEntry *e = fl_lookup(env, name);
    if (e && e->type == FL_INT) e->val.i += delta;
}

static void horse(FLEnv *env, const char *res, const char *a, const char *b, char op) {
    if (rand() % 10 == 0) { // 10% chance de falha
        printf("The horse refuses to carry the numbers.\n");
        return;
    }
    int va = 0, vb = 0;
    FLEntry *ea = fl_lookup(env, a);
    FLEntry *eb = fl_lookup(env, b);
    if (ea && ea->type == FL_INT) va = ea->val.i;
    if (eb && eb->type == FL_INT) vb = eb->val.i;

    int r = 0;
    if (op == '+') r = va + vb;
    else if (op == '-') r = va - vb;
    else if (op == '*') r = va * vb;
    else if (op == '/') r = (vb != 0) ? va / vb : 0;

    FLEntry *er = fl_lookup(env, res);
    if (!er) {
        env->entries = (FLEntry *)realloc(env->entries, (env->len + 1) * sizeof(FLEntry));
        er = &env->entries[env->len++];
        strncpy(er->name, res, 63);
        er->name[63] = '\0';
    }
    er->type = FL_INT;
    er->val.i = r;
}

static void chicken(FLEnv *env, const char *name) {
    // 10% chance de não fazer nada (chocando)
    if (rand() % 10 == 0) {
        printf("The chicken is busy hatching eggs...\n");
        return;
    }

    char buf[256];

    // 10% chance de ignorar input e usar onomatopeia
    if (rand() % 10 == 0) {
        // escolhe aleatoriamente uma onomatopeia de galinha
        const char *sounds[] = {"cluck cluck", "cackle", "squawk"};
        int idx = rand() % 3;
        strncpy(buf, sounds[idx], sizeof(buf)-1);
        buf[sizeof(buf)-1] = '\0';
    } else {
        if (!fgets(buf, sizeof(buf), stdin)) return;
        buf[strcspn(buf, "\n")] = '\0';
    }

    FLEntry *e = fl_lookup(env, name);
    if (!e) {
        env->entries = realloc(env->entries, (env->len + 1) * sizeof(FLEntry));
        e = &env->entries[env->len++];
        strncpy(e->name, name, 63);
        e->name[63] = '\0';
    }

    char *end = NULL;
    long val = strtol(buf, &end, 10);
    if (buf[0] != '\0' && end && *end == '\0') {
        e->type = FL_INT;
        e->val.i = (int)val;
    } else {
        e->type = FL_STRING;
        strncpy(e->val.s, buf, 255);
        e->val.s[255] = '\0';
    }
}

// ---------------------
// Declaração dos interpretadores
// ---------------------

static int fl_run_block(FLEnv *env, const char *source);
static int fl_run_source(const char *source);

// ---------------------
// Owl completo (blocos)
// ---------------------

    static void owl(FLEnv *env, int condition, const char *block) {
        if (rand() % 100 < 15) { // 15% chance de falha
            printf("The owl is too wise to act today.\n");
            return;
    }
        if (condition && block && *block) {
        // executa o bloco no mesmo ambiente
        fl_run_block(env, block);
    }
}

// ---------------------
// Interpretador top-level
// ---------------------

static int fl_run_source(const char *source) {
    // 1. Verifica se começa com "barn {"
    const char *barn_kw = "barn {";
    if (strncmp(source, barn_kw, strlen(barn_kw)) != 0) {
        printf("Animals outside a barn won't do the work.\n");
        return 1;
    }

    // 2. Verifica se tem fechamento de chave
    const char *block_start = strchr(source, '{');
    const char *block_end   = strrchr(source, '}');
    if (!block_start || !block_end || block_end <= block_start) {
        printf("The barn had a breach in its fences, so the animals escaped.\n");
        return 1;
    }

    // 3. Chance de falha global (25%)
    if (rand() % 4 == 0) {
        printf("The animals are sleeping...\n");
        return 1;
    }

    // Extrai conteúdo dentro de barn { ... }
    size_t len = block_end - block_start - 1;
    char *block = malloc(len + 1);
    strncpy(block, block_start + 1, len);
    block[len] = '\0';

    FLEnv env;
    fl_env_init(&env);

    int rc = fl_run_block(&env, block);

    fl_env_free(&env);
    free(block);
    return rc;
}

// ---------------------
// Interpretador de bloco (sem falha global, no mesmo env)
// Parsing robusto sem strtok_r: varre char a char
// ---------------------

// Helpers de trim
    __attribute__((unused))static const char *skip_ws(const char *p) {
    while (p && *p && isspace((unsigned char)*p)) p++;
    return p;
}
    static const char *skip_until_eol(const char *p) {
    while (p && *p && *p != '\n') p++;
    return p;
}

    static int fl_run_block(FLEnv *env, const char *source) {
    const char *p = source ? source : "";
    char linebuf[512];

    while (*p) {
        // pega uma linha (ou continua fluxo dentro de bloco)
        const char *line_start = p;
        const char *line_end = skip_until_eol(p);
        size_t len = (size_t)(line_end - line_start);
        if (len >= sizeof(linebuf)) len = sizeof(linebuf) - 1;
        memcpy(linebuf, line_start, len);
        linebuf[len] = '\0';
        p = (*line_end == '\n') ? line_end + 1 : line_end;

        // trim left
        char *line = linebuf;
        while (*line && isspace((unsigned char)*line)) line++;
        if (*line == '\0') continue;

        // pig
        if (strncmp(line, "pig", 3) == 0) {
            char name[64], literal[256];
            if (sscanf(line, "pig %63s = %255[^\n]", name, literal) == 2) {
                pig(env, name, literal);
            }
            continue;
        }

        // cow
        if (strncmp(line, "cow", 3) == 0) {
            char name[64];
            if (sscanf(line, "cow(%63[^)])", name) == 1) {
                cow(env, name);
            }
            continue;
        }

        // cat
        if (strncmp(line, "cat", 3) == 0) {
            char name[64], literal[256];
            if (sscanf(line, "cat %63s = %255[^\n]", name, literal) == 2) {
                cat(env, name, literal);
            }
            continue;
        }


        // dog
        if (strncmp(line, "dog", 3) == 0) {
            char name[64]; int delta;
            if (sscanf(line, "dog(%63[^,], %d)", name, &delta) == 2) {
                dog(env, name, delta);
            }
            continue;
        }

        // horse
        if (strncmp(line, "horse", 5) == 0) {
            char res[64], a[64], b[64], op;
            if (sscanf(line, "horse(%63[^,], %63[^,], %63[^,], %c)", res, a, b, &op) == 4) {
                horse(env, res, a, b, op);
            }
            continue;
        }

        // chicken
        if (strncmp(line, "chicken", 7) == 0) {
            char name[64];
            if (sscanf(line, "chicken %63s", name) == 1) {
                chicken(env, name);
            }
            continue;
        }

// owl com bloco
if (strncmp(line, "owl", 3) == 0) {
    // encontra '(' e ')' na linha para extrair a condição (ok ficar na linha)
    const char *lp = strchr(line, '(');
    const char *rp = lp ? strchr(lp, ')') : NULL;
    if (!lp || !rp) continue;

    // extrai condição
    char condbuf[128];
    size_t clen = (size_t)(rp - lp - 1);
    if (clen > sizeof(condbuf) - 1) clen = sizeof(condbuf) - 1;
    memcpy(condbuf, lp + 1, clen);
    condbuf[clen] = '\0';

    char var[64] = {0};
    char op[3] = {0};
    int num = 0;
    if (sscanf(condbuf, " %63s %2s %d ", var, op, &num) != 3) continue;

    // IMPORTANTE: encontrar '{' e '}' no SOURCE, não na linha temporária
    // mapeia posição da linha para a posição correspondente em source
    const char *src_at_line = line_start;                 // início da linha no source
    size_t off_after_rp = (size_t)(rp - line);            // offset de ')' dentro da linha
    const char *src_after_rp = src_at_line + off_after_rp;

    // avança em source até achar '{' (pode estar na mesma linha ou em linhas seguintes)
    const char *brace_src = src_after_rp;
    while (*brace_src && *brace_src != '{') brace_src++;
    if (!*brace_src) continue; // não achou '{'

    // captura bloco a partir de source com contagem de profundidade
    const char *cur_src = brace_src + 1;
    int depth = 1;
    char block[2048];
    size_t blen = 0;

    while (*cur_src && depth > 0) {
        if (*cur_src == '{') {
            depth++;
            cur_src++;
        } else if (*cur_src == '}') {
            depth--;
            if (depth == 0) { cur_src++; break; }
            cur_src++;
        } else {
            if (blen < sizeof(block) - 1) {
                block[blen++] = *cur_src;
            }
            cur_src++;
        }
    }
    block[blen] = '\0';

    // avalia condição
    int val = 0;
    FLEntry *ev = fl_lookup(env, var);
    if (ev && ev->type == FL_INT) val = ev->val.i;

    int cond = 0;
    if (strcmp(op, ">=") == 0) cond = (val >= num);
    else if (strcmp(op, "<=") == 0) cond = (val <= num);
    else if (strcmp(op, ">") == 0) cond = (val > num);
    else if (strcmp(op, "<") == 0) cond = (val < num);
    else if (strcmp(op, "==") == 0) cond = (val == num);

    owl(env, cond, block);

    // AVANÇAR p NO SOURCE: para depois do '}' correspondente
    p = cur_src;
    continue;

        }

        // qualquer outra linha é ignorada
    }

    return 0;
}

#endif // FARMLANG_H
