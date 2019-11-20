#include "astexec.h"
#include "astgen.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


void make_id(int var_name, TYPE varType, int varIndex) {
        var_map[var_name][0] = stackSize;
        var_map[var_name][1] = varIndex;
        for(int i = 0; i < varIndex; i++) {
                mem_stack[stackSize][0].i = varType;
                mem_stack[stackSize][0].f = 0;
                stackSize++;
        }
}

float get_var_val(int var_name, int varIndex) {
        if(mem_stack[var_map[var_name][0]][0].i == INT) return (float)mem_stack[var_map[var_name][0] + varIndex][1].i;
        else return (float)mem_stack[var_map[var_name][0] + varIndex][1].f;
}

void assign_var(int var_name, float val, int varIndex) {
        if(mem_stack[var_map[var_name][0]][0].i == INT) mem_stack[var_map[var_name][0] + varIndex][1].i = (int)val;
        else mem_stack[var_map[var_name][0] + varIndex][1].f = (float)val;
}

void print_val(float num) {
        printf("%.4f", num);
}


struct ExecEnviron
{
    float x; /* The value of the x variable, a real language would have some name->value lookup table instead */
};

static float execTermExpression(struct ExecEnviron* e, struct AstElement* a);
static float execBinExp(struct ExecEnviron* e, struct AstElement* a);
static void execAssign(struct ExecEnviron* e, struct AstElement* a);
static void execWhile(struct ExecEnviron* e, struct AstElement* a);
static void execCall(struct ExecEnviron* e, struct AstElement* a);
static void execStmt(struct ExecEnviron* e, struct AstElement* a);

/* Lookup Array for AST elements which yields values */
static float(*valExecs[])(struct ExecEnviron* e, struct AstElement* a) =
{
    execTermExpression,
    execTermExpression,
    execBinExp,
    NULL,
    NULL,
    NULL,
    NULL
};

/* lookup array for non-value AST elements */
static void(*runExecs[])(struct ExecEnviron* e, struct AstElement* a) =
{
    NULL, /* ID and numbers are canonical and */
    NULL, /* don't need to be executed */
    NULL, /* a binary expression is not executed */
    execAssign,
    execWhile,
    execCall,
    execStmt,
};

/* Dispatches any value expression */
static float dispatchExpression(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(valExecs[a->kind]);
    return valExecs[a->kind](e, a);
}

static void dispatchStatement(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(runExecs[a->kind]);
    runExecs[a->kind](e, a);
}

static void onlyName(const char* name, const char* reference, const char* kind)
{
    if(strcmp(reference, name))
    {
        fprintf(stderr,
            "This language knows only the %s '%s', not '%s'\n",
            kind, reference, name);
        exit(1);
    }
}

static void onlyPrint(const char* name)
{
    onlyName(name, "print", "function");
}

static float execTermExpression(struct ExecEnviron* e, struct AstElement* a)
{
    /* This function looks ugly because it handles two different kinds of
     * AstElement. I would refactor it to an execNameExp and execVal
     * function to get rid of this two if statements. */
    assert(a);
    if(ekNumber == a->kind)
    {
        return a->data.val;
    }
    else
    {
        if(ekId == a->kind)
        {
            assert(e);
            return get_var_val(a->data.name, a->data.index);
        }
    }
    fprintf(stderr, "OOPS: tried to get the value of a non-expression(%d)\n", a->kind);
    exit(1);
}

static float execBinExp(struct ExecEnviron* e, struct AstElement* a)
{
    assert(ekBinExpression == a->kind);
    const float left = dispatchExpression(e, a->data.expression.left);
    const float right = dispatchExpression(e, a->data.expression.right);
    switch(a->data.expression.op)
    {
        case '+':
            return left + right;
        case '-':
            return left - right;
        case '*':
            return left * right;
        case '<':
            return left < right;
        case '>':
            return left > right;
        default:
            fprintf(stderr,  "OOPS: Unknown operator:%c\n", a->data.expression.op);
            exit(1);
    }
    /* no return here, since every switch case returns some value (or bails out) */
}

static void execAssign(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(ekAssignment == a->kind);
    assert(e);
    struct AstElement* r = a->data.assignment.right;
    assign_var(a->data.assignment.name, dispatchExpression(e, r), a->data.assignment.index);
}

static void execWhile(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(ekWhile == a->kind);
    struct AstElement* const c = a->data.whileStmt.cond;
    struct AstElement* const s = a->data.whileStmt.statements;
    assert(c);
    assert(s);
    while(dispatchExpression(e, c))
    {
        dispatchStatement(e, s);
    }
}

static void execCall(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(ekCall == a->kind);
    onlyPrint(a->data.call.name);
    printf("%f\n", dispatchExpression(e, a->data.call.param));
}

static void execStmt(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(ekStatements == a->kind);
    int i;
    for(i=0; i<a->data.statements.count; i++)
    {
        dispatchStatement(e, a->data.statements.statements[i]);
    }
}

void execAst(struct ExecEnviron* e, struct AstElement* a)
{
    dispatchStatement(e, a);
}

struct ExecEnviron* createEnv()
{
    assert(ekLastElement == (sizeof(valExecs)/sizeof(*valExecs)));
    assert(ekLastElement == (sizeof(runExecs)/sizeof(*runExecs)));
    return calloc(1, sizeof(struct ExecEnviron));
}

void freeEnv(struct ExecEnviron* e)
{
    free(e);
}