#ifndef ASTGEN_H
#define ASTGEN_H


struct AstElement
{
    enum {ekId, ekNumber, ekBinExpression, ekAssignment, ekWhile, ekCall, ekStatements, ekIf, ekNop, ekVar, ekProcedure, 
    ekParameter, ekAssignAddress, ekCompound, ekExpAddress, ekForCondition, ekFor, ekLastElement} kind;
    union
    {
        float val;
        int name[2]; // name, index
        struct
        {
            struct AstElement *left, *right;
            char op;
        }expression;
        struct
        {
            int name;
            int index;
            struct AstElement* right;
        }assignment;
        struct
        {
            int count;
            struct AstElement** statements;
        }statements;
        struct
        {
            struct AstElement* cond;
            struct AstElement* statements;
        } whileStmt;
        struct
        {
            char* name;
            struct AstElement* param;
        }call;
        struct
        {
            struct AstElement* cond;
            struct AstElement* if_statement;
            struct AstElement* else_statement;
        } ifStmt;
        struct
        {
            int name;
            int type;
            int index;
        } variable;
        struct 
        {
            int name;
            struct AstElement* parameter;
        } procedure;
        struct
        {
            int count;
            struct AstElement** expressions;   
        } parameter;
        struct
        {
            struct AstElement* expression;
        } assignment_by_address;
        struct
        {
            struct AstElement* statement;
        } compound;
        struct
        {
            struct AstElement* procedure;
        } expAddr;
        struct
        {
            int left_var;
            int right_var;
            int num_call;
        } for_condition;
    } data;
};

struct AstElement* makeAssignment(int name, int index,struct AstElement* val);
struct AstElement* makeExpByNum(float val);
struct AstElement* makeExpByName(int name, int index);
struct AstElement* makeExp(struct AstElement* left, struct AstElement* right, char op);
struct AstElement* makeStatement(struct AstElement* dest, struct AstElement* toAppend);
struct AstElement* combineStatement(struct AstElement* dest, struct AstElement* toAppend);
struct AstElement* makeWhile(struct AstElement* cond, struct AstElement* exec);
struct AstElement* makeCall(char* name, struct AstElement* param);
struct AstElement* makeIfElse(struct AstElement* cond, struct AstElement* ifStmt, struct AstElement* elseStmt);
struct AstElement* makeNop();
struct AstElement* makeVariable(int name, int type, int index);
// struct AstElement* makeProcedure(int name, struct AstElement* parameter);
struct AstElement* makeProcedure();
struct AstElement* makeParameters(struct AstElement* dest, struct AstElement* toAppend);
struct AstElement* makeAssignmentByAddress(struct AstElement* val);
struct AstElement* makeCompoundStmt(struct AstElement* stmt);
struct AstElement* makeExpByAddress(struct AstElement* procedure);
struct AstElement* makeForCondition(int left, int right);
struct AstElement* makeFor(struct AstElement* cond, struct AstElement* exec);
struct AstElement* makeSemicolonError();
struct AstElement* makeMainProgError();
#endif