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
  
typedef std::unique_ptr<UsingAs> UsingAs_owned_ptr;
  
  
  typedef std::unique_ptr<Terminal> Terminal_owned_ptr;

  typedef std::unique_ptr<AccessQualifier> AccessQualifier_owned_ptr;
  typedef std::unique_ptr<AccessModifier> AccessModifier_owned_ptr;
  typedef std::unique_ptr<UnsafeModifier> UnsafeModifier_owned_ptr;
  typedef std::unique_ptr<TypeSpecifier> TypeSpecifier_owned_ptr;
  typedef std::unique_ptr<TypeName> TypeName_owned_ptr;
    typedef std::unique_ptr<TypeSpecifierCallArgs> TypeSpecifierCallArgs_owned_ptr;
    typedef std::unique_ptr<TypeSpecifierSimple> TypeSpecifierSimple_owned_ptr;
    typedef std::unique_ptr<TypeSpecifierTemplate> TypeSpecifierTemplate_owned_ptr;
      typedef std::unique_ptr<TypeSpecifierFunctionPointer> TypeSpecifierFunctionPointer_owned_ptr;
    typedef std::unique_ptr<TypeSpecifierPointerTo> TypeSpecifierPointerTo_owned_ptr;
    typedef std::unique_ptr<TypeSpecifierReferenceTo> TypeSpecifierReferenceTo_owned_ptr;
  
  typedef std::unique_ptr<FunctionDefinitionArgList> FunctionDefinitionArgList_owned_ptr;
    typedef std::unique_ptr<FunctionDefinitionArg> FunctionDefinitionArg_owned_ptr;

    typedef std::unique_ptr<ClassDeclStart> ClassDeclStart_owned_ptr;
    typedef std::unique_ptr<ClassArgumentList> ClassArgumentList_owned_ptr;
    typedef std::unique_ptr<ClassMemberDeclarationList> ClassMemberDeclarationList_owned_ptr;
      typedef std::unique_ptr<ClassMemberDeclaration> ClassMemberDeclaration_owned_ptr;
        typedef std::unique_ptr<ClassMemberDeclarationVariable> ClassMemberDeclarationVariable_owned_ptr;
        typedef std::unique_ptr<ClassMemberDeclarationMethod> ClassMemberDeclarationMethod_owned_ptr;
        typedef std::unique_ptr<ClassMemberDeclarationConstructor> ClassMemberDeclarationConstructor_owned_ptr;
        typedef std::unique_ptr<ClassMemberDeclarationDestructor> ClassMemberDeclarationDestructor_owned_ptr;
  typedef std::unique_ptr<ClassDefinition> ClassDefinition_owned_ptr;
    
  typedef std::unique_ptr<TypeDefinition> TypeDefinition_owned_ptr;

  typedef std::unique_ptr<EnumDefinition> EnumDefinition_owned_ptr;
    typedef std::unique_ptr<EnumDefinitionValueList> EnumDefinitionValueList_owned_ptr;
      typedef std::unique_ptr<EnumDefinitionValue> EnumDefinitionValue_owned_ptr;

  typedef std::unique_ptr<Expression> Expression_owned_ptr;
    typedef std::unique_ptr<ExpressionPrimary> ExpressionPrimary_owned_ptr;
      typedef std::unique_ptr<ExpressionPrimaryIdentifier> ExpressionPrimaryIdentifier_owned_ptr;
      typedef std::unique_ptr<ExpressionPrimaryNested> ExpressionPrimaryNested_owned_ptr;
      typedef std::unique_ptr<ExpressionPrimaryLiteralChar> ExpressionPrimaryLiteralChar_owned_ptr;
      typedef std::unique_ptr<ExpressionPrimaryLiteralString> ExpressionPrimaryLiteralString_owned_ptr;
      typedef std::unique_ptr<ExpressionPrimaryLiteralInt> ExpressionPrimaryLiteralInt_owned_ptr;
      typedef std::unique_ptr<ExpressionPrimaryLiteralFloat> ExpressionPrimaryLiteralFloat_owned_ptr;
  
    typedef std::unique_ptr<ExpressionPostfixArrayIndex> ExpressionPostfixArrayIndex_owned_ptr;
    typedef std::unique_ptr<ExpressionPostfixFunctionCall> ExpressionPostfixFunctionCall_owned_ptr;
      typedef std::unique_ptr<ArgumentExpressionList> ArgumentExpressionList_owned_ptr;
    typedef std::unique_ptr<ExpressionPostfixDot> ExpressionPostfixDot_owned_ptr;
    typedef std::unique_ptr<ExpressionPostfixArrow> ExpressionPostfixArrow_owned_ptr;
    typedef std::unique_ptr<ExpressionPostfixIncDec> ExpressionPostfixIncDec_owned_ptr;
    typedef std::unique_ptr<ExpressionUnaryPrefix> ExpressionUnaryPrefix_owned_ptr;
    typedef std::unique_ptr<ExpressionUnarySizeofType> ExpressionUnarySizeofType_owned_ptr;
    typedef std::unique_ptr<ExpressionCast> ExpressionCast_owned_ptr;
    typedef std::unique_ptr<ExpressionBinary> ExpressionBinary_owned_ptr;
    typedef std::unique_ptr<ExpressionTrinary> ExpressionTrinary_owned_ptr;

  typedef std::unique_ptr<StatementList> StatementList_owned_ptr;
    typedef std::unique_ptr<Statement> Statement_owned_ptr;
      typedef std::unique_ptr<StatementVariableDeclaration> StatementVariableDeclaration_owned_ptr;
      typedef std::unique_ptr<StatementBlock> StatementBlock_owned_ptr;
      typedef std::unique_ptr<StatementExpression> StatementExpression_owned_ptr;
      typedef std::unique_ptr<StatementIfElse> StatementIfElse_owned_ptr;
      typedef std::unique_ptr<StatementWhile> StatementWhile_owned_ptr;
      typedef std::unique_ptr<StatementFor> StatementFor_owned_ptr;
      typedef std::unique_ptr<StatementSwitch> StatementSwitch_owned_ptr;
        typedef std::unique_ptr<StatementSwitchContent> StatementSwitchContent_owned_ptr;
          typedef std::unique_ptr<StatementSwitchBlock> StatementSwitchBlock_owned_ptr;
      typedef std::unique_ptr<StatementLabel> StatementLabel_owned_ptr;
      typedef std::unique_ptr<StatementGoto> StatementGoto_owned_ptr;
      typedef std::unique_ptr<StatementContinue> StatementContinue_owned_ptr;
      typedef std::unique_ptr<StatementBreak> StatementBreak_owned_ptr;
      typedef std::unique_ptr<StatementReturn> StatementReturn_owned_ptr;
    
  typedef std::unique_ptr<TranslationUnit> TranslationUnit_owned_ptr;
    typedef std::unique_ptr<FileStatementList> FileStatementList_owned_ptr;
      typedef std::unique_ptr<FileStatement> FileStatement_owned_ptr;
        typedef std::unique_ptr<FileStatementFunctionDefinition> FileStatementFunctionDefinition_owned_ptr;
          typedef std::unique_ptr<ScopeBody> ScopeBody_owned_ptr;
        typedef std::unique_ptr<FileStatementFunctionDeclaration> FileStatementFunctionDeclaration_owned_ptr;
          typedef std::unique_ptr<ArrayLength> ArrayLength_owned_ptr;
        typedef std::unique_ptr<FileStatementGlobalDefinition> FileStatementGlobalDefinition_owned_ptr;
          typedef std::unique_ptr<GlobalInitializer> GlobalInitializer_owned_ptr;
            typedef std::unique_ptr<GlobalInitializerExpressionPrimary> GlobalInitializerExpressionPrimary_owned_ptr;
            typedef std::unique_ptr<GlobalInitializerAddressofExpressionPrimary> GlobalInitializerAddressofExpressionPrimary_owned_ptr;
            typedef std::unique_ptr<GlobalInitializerStructInitializerList> GlobalInitializerStructInitializerList_owned_ptr;
              typedef std::unique_ptr<StructInitializerList> StructInitializerList_owned_ptr;
                typedef std::unique_ptr<StructInitializer> StructInitializer_owned_ptr;
    
        typedef std::unique_ptr<FileStatementNamespace> FileStatementNamespace_owned_ptr;
          typedef std::unique_ptr<NamespaceDeclaration> NamespaceDeclaration_owned_ptr;
        typedef std::unique_ptr<FileStatementUsing> FileStatementUsing_owned_ptr;
          typedef std::unique_ptr<UsingAs> UsingAs_owned_ptr;
  
};
