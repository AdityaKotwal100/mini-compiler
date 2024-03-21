#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "lexer.h"
#include <iostream>
#include <algorithm>
#include <map>
#include "execute.h"
using namespace std;

LexicalAnalyzer lexer; // NOTE: even though this file does not peek() at or get() any
                       // tokens, the very fact that a lexer is declared
                       // requires that you provide input to the program
                       // when you execute it even if you don't add any code
                       // to execute just run ./a.out < test, where test is any
                       // test case

struct InstructionNode *parseStatementList();

void syntax_error()
{
    cout << "SYNTAX ERROR !!!\n";
    exit(1);
}

map<string, int> location_table;

int location(string s)
{
    return next_available;
}
void add_to_location_table(string s, int location)
{
    location_table[s] = next_available;
    next_available++;
}
int idx = 0;

int initialize_new_location(string s)
{

    int new_location = location(s);
    mem[idx] = 0;
    idx++;
    add_to_location_table(s, new_location);
    return new_location;
}

int allocate_constant(string s)
{

    int new_location = location(s);
    mem[idx] = stoi(s);
    idx++;
    add_to_location_table(s, new_location);
    return new_location;
}

int get_variable_location(string s)
{
    if (location_table.find(s) != location_table.end())
    {
        return location_table[s];
    }
    return -1;
}

Token expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

struct InstructionNode *parseInputStatement()
{
    InstructionNode *node = new InstructionNode;

    expect(INPUT);
    Token t = lexer.peek(1);
    node->input_inst.var_index = get_variable_location(t.lexeme);

    expect(ID);
    node->type = IN;
    expect(SEMICOLON);
    return node;
}

int convert_string_to_integer(string s)
{
    int i = 0;
    for (int n = 0; n < s.length(); n++)
    {
        i = i * 10;
        i += (s[n] - '0');
    }
    return i;
}

int parsePrimary()
{
    Token t = lexer.peek(1);
    string s = t.lexeme;
    if (t.token_type == ID)
        expect(ID);
    else if (t.token_type == NUM)
    {
        expect(NUM);
        return allocate_constant(t.lexeme);
    }
    return get_variable_location(s);
}

ArithmeticOperatorType get_arithmetic_operator(TokenType t)
{
    switch (t)
    {
    case PLUS:
        return OPERATOR_PLUS;
    case MINUS:
        return OPERATOR_MINUS;
    case DIV:
        return OPERATOR_DIV;
    case MULT:
        return OPERATOR_MULT;
    default:
        return OPERATOR_NONE;
    }
}

ArithmeticOperatorType parseOperator()
{
    Token t = lexer.peek(1);
    string s = t.lexeme;
    TokenType t_type = t.token_type;
    if (t.token_type == PLUS)
        expect(PLUS);
    else if (t.token_type == MINUS)
        expect(MINUS);
    else if (t.token_type == MULT)
        expect(MULT);
    else if (t.token_type == DIV)
        expect(DIV);
    return get_arithmetic_operator(t_type);
}

void parseExpression(InstructionNode &node)
{
    node.assign_inst.opernd1_index = parsePrimary();
    node.assign_inst.op = parseOperator();
    node.assign_inst.opernd2_index = parsePrimary();
}

struct InstructionNode *parseAssignmentStatement()
{

    Token t = lexer.peek(1);
    InstructionNode *node = new InstructionNode;
    node->type = ASSIGN;

    node->assign_inst.left_hand_side_index = get_variable_location(t.lexeme);
    expect(ID);
    expect(EQUAL);
    Token t1 = lexer.peek(1);
    Token t2 = lexer.peek(2);
    if ((t1.token_type == ID && t2.token_type == SEMICOLON) || (t1.token_type == NUM && t2.token_type == SEMICOLON))
    {

        node->assign_inst.op = OPERATOR_NONE;
        node->assign_inst.opernd1_index = parsePrimary();
    }
    else if ((t1.token_type == ID) || (t1.token_type == NUM))
    {
        if (t2.token_type == PLUS || t2.token_type == MINUS || t2.token_type == MULT || t2.token_type == DIV)
        {
            parseExpression(*node);
        }
    }
    expect(SEMICOLON);
    // node->assign_inst = assignment;
    node->next = nullptr;
    return node;
}

ConditionalOperatorType parseRelop()
{

    Token t = lexer.peek(1);

    if (t.token_type == GREATER)
    {
        expect(GREATER);
        return CONDITION_GREATER;
    }

    else if (t.token_type == LESS)
    {
        expect(LESS);
        return CONDITION_LESS;
    }

    else if (t.token_type == NOTEQUAL)
    {
        expect(NOTEQUAL);
        return CONDITION_NOTEQUAL;
    }
}

void *parseCondition(InstructionNode &inst)
{
    inst.cjmp_inst.opernd1_index = parsePrimary();
    inst.cjmp_inst.condition_op = parseRelop();
    inst.cjmp_inst.opernd2_index = parsePrimary();
    inst.cjmp_inst.target = nullptr;
}

struct InstructionNode *parseBody()
{
    struct InstructionNode *inst;
    expect(LBRACE);

    inst = parseStatementList();

    expect(RBRACE);
    return inst;
}

InstructionNode *createNoOpNode()
{
    InstructionNode *inst = new InstructionNode;
    inst->next = nullptr;
    inst->type = NOOP;
    return inst;
}

InstructionNode *parseWhileStatement()
{
    InstructionNode *inst = new InstructionNode;
    InstructionNode *temp = inst;
    InstructionNode *no_op_node = createNoOpNode();
    // create jmp node of type JMP
    InstructionNode *jump_node = new InstructionNode;
    jump_node->type = JMP;
    inst->type = CJMP;
    expect(WHILE);
    parseCondition(*inst);
    inst->next = parseBody();
    // set jmp->jmp_inst.target to inst
    jump_node->jmp_inst.target = inst;
    jump_node->next = no_op_node;
    // append jmp node to end of body of while
    while (inst->next)
        inst = inst->next;
    inst->next = jump_node;
    temp->cjmp_inst.target = no_op_node;

    return temp;
}

void parseCase()
{
    expect(CASE);
    expect(NUM);
    expect(COLON);
    parseBody();
}
void parseCaseList()
{
    parseCase();
    Token t = lexer.peek(1);
    if (t.token_type == CASE)
        parseCaseList();
}

void parseDefaultCase()
{
    expect(DEFAULT);
    expect(COLON);
    parseBody();
}

struct InstructionNode *parseOutputStatement()
{
    struct InstructionNode *inst = new InstructionNode;
    expect(OUTPUT);
    Token t = lexer.peek(1);
    inst->output_inst.var_index = get_variable_location(t.lexeme);
    inst->type = OUT;
    inst->next = nullptr;
    expect(ID);
    expect(SEMICOLON);
    return inst;
}

struct InstructionNode *parseSwitchStatement()
{
    expect(SWITCH);
    expect(ID);
    expect(LBRACE);
    parseCaseList();
    Token t = lexer.peek(1);
    if (t.token_type == RBRACE)
        expect(RBRACE);
    else if (t.token_type == DEFAULT)
        parseDefaultCase();
}

struct InstructionNode *parseForStatement()
{
    InstructionNode *inst_1 = new InstructionNode;
    InstructionNode *temp_1 = inst_1;

    InstructionNode *no_op_node = createNoOpNode();
    // create jmp node of type JMP
    InstructionNode *jump_node = new InstructionNode;
    jump_node->type = JMP;

    expect(FOR);
    expect(LPAREN);
    inst_1 = parseAssignmentStatement();

    InstructionNode *inst = new InstructionNode;
    InstructionNode *temp = inst;
    inst->type = CJMP;
    // inst->cjmp_inst = new ConditionalInstruction;
    parseCondition(*inst);

    expect(SEMICOLON);
    while (inst->next)
        inst = inst->next;

    InstructionNode *second_assignment = new InstructionNode;
    // inst->next = parseAssignmentStatement();
    second_assignment = parseAssignmentStatement();
    expect(RPAREN);

    inst->next = parseBody();
    while (inst->next)
        inst = inst->next;

    inst->next = second_assignment;

    // jump_node->jmp_inst = new JumpInstruction;
    // set jmp->jmp_inst.target to inst
    jump_node->jmp_inst.target = temp;
    jump_node->next = no_op_node;
    // append jmp node to end of body of while
    while (inst->next)
        inst = inst->next;
    inst->next = jump_node;
    temp->cjmp_inst.target = no_op_node;
    inst_1->next = temp;

    return inst_1;
}

struct InstructionNode *parseIfStatement()
{
    InstructionNode *inst = new InstructionNode;
    InstructionNode *temp = inst;
    InstructionNode *no_op_node = createNoOpNode();
    inst->type = CJMP;
    expect(IF);
    parseCondition(*inst);
    inst->cjmp_inst.target = no_op_node;
    inst->next = parseBody();

    // Append to the end
    while (inst->next)
    {
        inst = inst->next;
    }
    inst->next = no_op_node;
    return temp;
}

struct InstructionNode *parseStatement()
{
    Token t = lexer.peek(1);
    if (t.token_type == ID)
        return parseAssignmentStatement();
    else if (t.token_type == WHILE)
        return parseWhileStatement();
    // else if (t.token_type == SWITCH)
    //     return parseSwitchStatement();
    else if (t.token_type == FOR)
        return parseForStatement();
    else if (t.token_type == OUTPUT)
        return parseOutputStatement();
    else if (t.token_type == INPUT)
        return parseInputStatement();
    else if (t.token_type == IF)
        return parseIfStatement();
}

struct InstructionNode *parseStatementList()
{
    struct InstructionNode *inst; // instruction for one statement
    struct InstructionNode *temp;

    struct InstructionNode *instList; // instruction list for statement list
    inst = parseStatement();
    temp = inst;
    Token t = lexer.peek(1);
    if (t.token_type == ID ||
        t.token_type == IF ||
        t.token_type == WHILE ||
        t.token_type == SWITCH ||
        t.token_type == FOR ||
        t.token_type == OUTPUT ||
        t.token_type == INPUT)
    {

        instList = parseStatementList();
        while (inst->next != nullptr)
            inst = inst->next;
        inst->next = instList;
    }

    return temp;
}

void parseNumList()
{
    Token t = lexer.peek(1);
    inputs.push_back(convert_string_to_integer(t.lexeme));
    expect(NUM);
    t = lexer.peek(1);
    if (t.token_type == NUM)
        parseNumList();
}

void parseInputs()
{
    parseNumList();
}

void parseIdList()
{
    Token t = lexer.peek(1);
    string current_variable = t.lexeme;
    initialize_new_location(current_variable);
    expect(ID);

    t = lexer.peek(1);
    if (t.token_type == COMMA)
    {
        expect(COMMA);
        parseIdList();
    }
}

void parseVarSection()
{
    parseIdList();
    expect(SEMICOLON);
}

struct InstructionNode *parse_Generate_Intermediate_Representation()
{
    parseVarSection();
    InstructionNode *code = parseBody();
    InstructionNode *temp = code;
    // printInstructionNode(temp, 100);
    parseInputs();
    return code;
}