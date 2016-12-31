/*
 * Copyright (c) 2005 Palmsource, Inc.
 *
 * This software is licensed as described in the file LICENSE, which
 * you should have received as part of this distribution. The terms
 * are also available at http://www.openbinder.org/license.html.
 *
 * This software consists of voluntary contributions made by many
 * individuals. For the exact contribution history, see the revision
 * history and logs, available at http://www.openbinder.org
 */

#ifndef AST_H
#define AST_H

#include <utils/RefBase.h>
#include <utils/String.h>
#include <binder/TextOutput.h>
#include <utils/Vector.h>

using namespace android;

class Expression : public virtual RefBase
{
public:
                    Expression(bool statement_requires_semicolon);
    virtual void	Output(TextOutput &stream, int32_t flags=0) const = 0;
    virtual bool	StatementRequiresSemicolon();
private:
    bool m_statement_requires_semicolon;
};

class Comment : public Expression
{
public:
                    Comment(const String &str);
                    Comment(const char *str);
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
private:
    String m_str;
};

class Literal : public Expression
{
public:
                    Literal(bool requires_semi, const char *fmt, ...);
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
private:
    String m_value;
};

class StringLiteral : public Expression
{
public:
                    StringLiteral(const String &value, bool requires_semi = true);
                    StringLiteral(const char *value, bool requires_semi = true);
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
private:
    String m_value;
};

class IntLiteral : public Expression
{
public:
                    IntLiteral(int value);
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
private:
    int m_value;
};

class ArrayInitializer : public Expression
{
public:
                    ArrayInitializer();
    void			AddItem(const sp<Expression> &value);
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
private:
    Vector<sp<Expression> > m_list;
};

enum {
    STATIC		= 0x00000001,
    CONST		= 0x00000002,
    EXTERN		= 0x00000004,
    VIRTUAL		= 0x00000008
};

class VariableDefinition : public Expression
{
public:
                    // array is 0 for an unsized array and -1 for not an array
                    VariableDefinition(const String &type, const String name,
                            uint32_t cv = 0, const sp<Expression> &initial = NULL,
                            int array = -1, int pointer_indirection = 0);
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
private:
    String m_type;
    String m_name;
    uint32_t m_cv;
    sp<Expression> m_initial;
    int m_array;
    int m_pointer;
};

class StatementList : public Expression
{
public:
                    StatementList();
    void			AddItem(const sp<Expression> &item);
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
private:
    Vector<sp<Expression> > m_statements;
};

class FunctionPrototype : public Expression
{
public:
                    FunctionPrototype(const String &return_type, const String &name, uint32_t linkage, uint32_t cv = 0, const String & clas = String(""));
    void			AddParameter(const String &type, const String &name);
    enum {
        nolinkage = 1,
        classname = 2
    };
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
private:
    String m_return_type;
    String m_classname;
    String m_name;
    uint32_t m_linkage;
    uint32_t m_cv;
    struct Param {
        String type;
        String name;
    };
    Vector<Param> m_params;
};

class Function : public StatementList
{
public:
                    Function(const sp<FunctionPrototype> &prototype);
    void			AddStatement(const sp<Expression> &item);
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
private:
    sp<FunctionPrototype> m_prototype;
    Vector<sp<Expression> > m_statements;
};

class Return : public Expression
{
public:
                    Return(const sp<Expression> &value);
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
private:
    sp<Expression> m_value;
};

class StaticCast : public Expression
{
public:
                    StaticCast(const String &type, int indirection,
                                const sp<Expression> value);
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
private:
    String m_type;
    int m_indirection;
    sp<Expression> m_value;
};

class FunctionCall : public Expression
{
public:
                    FunctionCall(const String &name);
                    FunctionCall(const String &namespac, const String &name);
                    FunctionCall(const String &object, const String &namespac, const String &name);
    void			AddArgument(const sp<Expression> &item);
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
private:
    String m_object;
    String m_namespace;
    String m_name;
    Vector<sp<Expression> > m_arguments;
};

class Assignment : public Expression
{
public:
                    Assignment(const sp<Expression> &lvalue, const sp<Expression> &rvalue);
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
private:
    sp<Expression> m_lvalue;
    sp<Expression> m_rvalue;
};

class Optional : public Expression
{
public:
                    Optional(const sp<Expression> &expr, bool initial, bool requires_semi = true);
    void			SetOutput(bool will_output);
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
    virtual bool	StatementRequiresSemicolon();
private:
    sp<Expression> m_expr;
    bool m_will_output;
};

class ClassDeclaration : public StatementList
{
public:
                    ClassDeclaration(const String &name);
    void			AddBaseClass(const String &name, const String &scope = String("public"));
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
private:
    String m_name;
    struct base_class {
        String name;
        String scope;
    };
    Vector<base_class> m_base_classes;
};

class ParameterUse : public Expression
{
public:
                    ParameterUse();
    void			AddParameter(const String &name);
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
private:
    Vector<String> m_expr;
};

class Null : public Expression
{
public:
                    Null();
    virtual void	Output(TextOutput &stream, int32_t flags=0) const;
};

#endif // AST_H
