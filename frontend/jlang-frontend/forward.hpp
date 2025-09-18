#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once

#include <memory>

// This is the set of forward declarations for
// the classes that make up the strongly-typed
// syntax tree.  These are roughly organized in
// heirarchal order with the things that depend
// on other things indented and appearing below them.
// The heirarchy is imperfect because some
// nodes may appear in multiple contexts and
// some nodes are recursive, so this is only
// a rough guide for how the classes relate
// to one another.  For a more full picture
// of their relationships, see the Doxygen
// documents.
//
// When maintaining these declarations, please
// make sure to maintain both the class forward-declarations
// as well as the pointer typedefs associated with
// them so that users of these classes can refer
// to them without having a dependency on the
// classes themselves.  This is important because
// there is some recursiveness in the definitions
// and it's important that these forward declarations
// resolve that circularity.
namespace JLang::frontend::tree {

//----------------------------
// Class forward declarations.
//----------------------------
  class Terminal;

  class AccessQualifier;
  class AccessModifier;
  class UnsafeModifier;
  class TypeSpecifier;
    class TypeName;
    class TypeSpecifierCallArgs;
    class TypeSpecifierSimple;
    class TypeSpecifierTemplate;
    class TypeSpecifierFunctionPointer;
    class TypeSpecifierPointerTo;
    class TypeSpecifierReferenceTo;
  
  class FunctionDefinitionArgList;
    class FunctionDefinitionArg;

    class ClassDeclStart;
    class ClassArgumentList;
    class ClassMemberDeclarationList;
      class ClassMemberDeclaration;
        class ClassMemberDeclarationVariable;
        class ClassMemberDeclarationMethod;
        class ClassMemberDeclarationConstructor;
        class ClassMemberDeclarationDestructor;
  class ClassDefinition;
    
  class TypeDefinition;

  class EnumDefinition;
    class EnumDefinitionValueList;
      class EnumDefinitionValue;

  class Expression;
    class ExpressionPrimary;
      class ExpressionPrimaryIdentifier;
      class ExpressionPrimaryNested;
      class ExpressionPrimaryLiteralChar;
      class ExpressionPrimaryLiteralString;
      class ExpressionPrimaryLiteralInt;
      class ExpressionPrimaryLiteralFloat;
  
    class ExpressionPostfixArrayIndex;
    class ExpressionPostfixFunctionCall;
      class ArgumentExpressionList;
    class ExpressionPostfixDot;
    class ExpressionPostfixArrow;
    class ExpressionPostfixIncDec;
    class ExpressionUnaryPrefix;
    class ExpressionUnarySizeofType;
    class ExpressionCast;
    class ExpressionBinary;
    class ExpressionTrinary;

  class StatementList;
    class Statement;
      class StatementVariableDeclaration;
      class StatementBlock;
      class StatementExpression;
      class StatementIfElse;
      class StatementWhile;
      class StatementFor;
      class StatementSwitch;
        class StatementSwitchContent;
          class StatementSwitchBlock;
      class StatementLabel;
      class StatementGoto;
      class StatementContinue;
      class StatementBreak;
      class StatementReturn;
    
  class TranslationUnit;
    class FileStatementList;
      class FileStatement;
        class FileStatementFunctionDefinition;
          class ScopeBody;
        class FileStatementFunctionDeclaration;
          class ArrayLength;
        class FileStatementGlobalDefinition;
          class GlobalInitializer;
            class GlobalInitializerExpressionPrimary;
            class GlobalInitializerAddressofExpressionPrimary;
            class GlobalInitializerStructInitializerList;
              class StructInitializerList;
                class StructInitializer;
    
        class FileStatementNamespace;
          class NamespaceDeclaration;
        class FileStatementUsing;
          class UsingAs;
//----------------------------
// Collection of classes to
// include in the 'SyntaxNode'
// variant type.
//----------------------------
#define JLANG_SYNTAX_NODE_VARIANT_LIST               \
        JLang::frontend::tree::Terminal*,                                   \
        JLang::frontend::tree::AccessQualifier*,                            \
        JLang::frontend::tree::AccessModifier*,                             \
        JLang::frontend::tree::UnsafeModifier*,                             \
        JLang::frontend::tree::TypeSpecifier*,                              \
        JLang::frontend::tree::  TypeName*,                                 \
        JLang::frontend::tree::  TypeSpecifierCallArgs*,                    \
        JLang::frontend::tree::  TypeSpecifierSimple*,                      \
        JLang::frontend::tree::  TypeSpecifierTemplate*,                    \
        JLang::frontend::tree::  TypeSpecifierFunctionPointer*,             \
        JLang::frontend::tree::  TypeSpecifierPointerTo*,                   \
        JLang::frontend::tree::  TypeSpecifierReferenceTo*,                 \
        JLang::frontend::tree::FunctionDefinitionArgList*,                  \
        JLang::frontend::tree::  FunctionDefinitionArg*,                    \
        JLang::frontend::tree::  ClassDeclStart*,                           \
        JLang::frontend::tree::  ClassArgumentList*,                        \
        JLang::frontend::tree::      ClassMemberDeclarationVariable*,       \
        JLang::frontend::tree::      ClassMemberDeclarationMethod*,         \
        JLang::frontend::tree::      ClassMemberDeclarationConstructor*,    \
        JLang::frontend::tree::      ClassMemberDeclarationDestructor*,     \
        JLang::frontend::tree::    ClassMemberDeclaration*,                 \
        JLang::frontend::tree::  ClassMemberDeclarationList*,               \
        JLang::frontend::tree::ClassDefinition*,                            \
        JLang::frontend::tree::TypeDefinition*,                             \
        JLang::frontend::tree::    EnumDefinitionValue*,                    \
        JLang::frontend::tree::  EnumDefinitionValueList*,                  \
        JLang::frontend::tree::EnumDefinition*,                             \
        JLang::frontend::tree::Expression*,                                 \
        JLang::frontend::tree::  ExpressionPrimary*,                        \
        JLang::frontend::tree::    ExpressionPrimaryIdentifier*,            \
        JLang::frontend::tree::    ExpressionPrimaryNested*,                \
        JLang::frontend::tree::    ExpressionPrimaryLiteralChar*,           \
        JLang::frontend::tree::    ExpressionPrimaryLiteralString*,         \
        JLang::frontend::tree::    ExpressionPrimaryLiteralInt*,            \
        JLang::frontend::tree::    ExpressionPrimaryLiteralFloat*,          \
        JLang::frontend::tree::  ExpressionPostfixArrayIndex*,              \
        JLang::frontend::tree::  ExpressionPostfixFunctionCall*,            \
        JLang::frontend::tree::      ArgumentExpressionList*,               \
        JLang::frontend::tree::  ExpressionPostfixDot*,                     \
        JLang::frontend::tree::  ExpressionPostfixArrow*,                   \
        JLang::frontend::tree::  ExpressionPostfixIncDec*,                  \
        JLang::frontend::tree::  ExpressionUnaryPrefix*,                    \
        JLang::frontend::tree::  ExpressionUnarySizeofType*,                \
        JLang::frontend::tree::  ExpressionCast*,                           \
        JLang::frontend::tree::  ExpressionBinary*,                         \
        JLang::frontend::tree::  ExpressionTrinary*,                        \
        JLang::frontend::tree::StatementList*,                              \
        JLang::frontend::tree::  Statement*,                                \
        JLang::frontend::tree::    StatementVariableDeclaration*,           \
        JLang::frontend::tree::    StatementBlock*,                         \
        JLang::frontend::tree::    StatementExpression*,                    \
        JLang::frontend::tree::    StatementGoto*,                          \
        JLang::frontend::tree::    StatementIfElse*,                        \
        JLang::frontend::tree::    StatementWhile*,                         \
        JLang::frontend::tree::    StatementFor*,                           \
        JLang::frontend::tree::    StatementSwitch*,                        \
        JLang::frontend::tree::      StatementSwitchContent*,               \
        JLang::frontend::tree::        StatementSwitchBlock*,               \
        JLang::frontend::tree::    StatementReturn*,                        \
        JLang::frontend::tree::    StatementContinue*,                      \
        JLang::frontend::tree::    StatementBreak*,                         \
        JLang::frontend::tree::    StatementLabel*,                         \
        JLang::frontend::tree::TranslationUnit*,                            \
        JLang::frontend::tree::  FileStatementList*,                        \
        JLang::frontend::tree::    FileStatement*,                          \
        JLang::frontend::tree::      FileStatementFunctionDefinition*,        \
        JLang::frontend::tree::        ScopeBody *,                           \
        JLang::frontend::tree::      FileStatementFunctionDeclaration*,       \
        JLang::frontend::tree::        ArrayLength*,                          \
        JLang::frontend::tree::        GlobalInitializer*,                    \
        JLang::frontend::tree::          GlobalInitializerExpressionPrimary*, \
        JLang::frontend::tree::          GlobalInitializerAddressofExpressionPrimary*, \
        JLang::frontend::tree::          GlobalInitializerStructInitializerList*,      \
        JLang::frontend::tree::            StructInitializerList*,            \
        JLang::frontend::tree::              StructInitializer*,              \
        JLang::frontend::tree::      FileStatementGlobalDefinition*,          \
        JLang::frontend::tree::      FileStatementNamespace*,                 \
        JLang::frontend::tree::        NamespaceDeclaration*,                 \
        JLang::frontend::tree::      FileStatementUsing*,                     \
        JLang::frontend::tree::        UsingAs*

//----------------------------
// Unique pointer types to
// be used internally to enforce
// single ownership of these classes
// in the tree.
//----------------------------
  
  
  

  

    


  


  /**
   * Convenience definition
   */
  /**
   * Convenience definition
   */
    
  
};
