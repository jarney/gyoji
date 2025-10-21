/* Copyright 2025 Jonathan S. Arney
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://github.com/jarney/gyoji/blob/master/LICENSE
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#ifndef _GYOJI_INTERNAL
#error "This header is intended to be used internally as a part of the Gyoji front-end.  Please include frontend.hpp instead."
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
namespace Gyoji::frontend::tree {

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
    class TypeSpecifierArray;
  
  class FunctionDefinitionArgList;
    class FunctionDefinitionArg;

    class ClassDeclStart;
    class ClassArgumentList;
    class ClassMemberDeclarationList;
      class ClassMemberDeclaration;
        class ClassMemberDeclarationVariable;
        class ClassMemberDeclarationMethod;
        class ClassMemberDeclarationMethodStatic;
        class ClassMemberDeclarationConstructor;
        class ClassMemberDeclarationDestructor;
  class ClassDefinition;
  class ClassDeclaration;
    
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
      class ExpressionPrimaryLiteralBool;
      class ExpressionPrimaryLiteralNull;
  
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
        class InitializerExpression;
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
          class FileStatementFunctionDeclStart;
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
#define GYOJI_SYNTAX_NODE_VARIANT_LIST               \
        Gyoji::frontend::tree::Terminal*,                                   \
        Gyoji::frontend::tree::AccessQualifier*,                            \
        Gyoji::frontend::tree::AccessModifier*,                             \
        Gyoji::frontend::tree::UnsafeModifier*,                             \
        Gyoji::frontend::tree::TypeSpecifier*,                              \
        Gyoji::frontend::tree::  TypeName*,                                 \
        Gyoji::frontend::tree::  TypeSpecifierCallArgs*,                    \
        Gyoji::frontend::tree::  TypeSpecifierSimple*,                      \
        Gyoji::frontend::tree::  TypeSpecifierTemplate*,                    \
        Gyoji::frontend::tree::  TypeSpecifierFunctionPointer*,             \
        Gyoji::frontend::tree::  TypeSpecifierPointerTo*,                   \
        Gyoji::frontend::tree::  TypeSpecifierReferenceTo*,                 \
        Gyoji::frontend::tree::  TypeSpecifierArray*,                       \
        Gyoji::frontend::tree::FunctionDefinitionArgList*,                  \
        Gyoji::frontend::tree::  FunctionDefinitionArg*,                    \
        Gyoji::frontend::tree::  ClassDeclStart*,                           \
        Gyoji::frontend::tree::  ClassArgumentList*,                        \
        Gyoji::frontend::tree::      ClassMemberDeclarationVariable*,       \
        Gyoji::frontend::tree::      ClassMemberDeclarationMethod*,         \
        Gyoji::frontend::tree::      ClassMemberDeclarationMethodStatic*,   \
        Gyoji::frontend::tree::      ClassMemberDeclarationConstructor*,    \
        Gyoji::frontend::tree::      ClassMemberDeclarationDestructor*,     \
        Gyoji::frontend::tree::    ClassMemberDeclaration*,                 \
        Gyoji::frontend::tree::  ClassMemberDeclarationList*,               \
        Gyoji::frontend::tree::ClassDefinition*,                            \
        Gyoji::frontend::tree::ClassDeclaration*,                           \
        Gyoji::frontend::tree::TypeDefinition*,                             \
        Gyoji::frontend::tree::    EnumDefinitionValue*,                    \
        Gyoji::frontend::tree::  EnumDefinitionValueList*,                  \
        Gyoji::frontend::tree::EnumDefinition*,                             \
        Gyoji::frontend::tree::Expression*,                                 \
        Gyoji::frontend::tree::  ExpressionPrimary*,                        \
        Gyoji::frontend::tree::    ExpressionPrimaryIdentifier*,            \
        Gyoji::frontend::tree::    ExpressionPrimaryNested*,                \
        Gyoji::frontend::tree::    ExpressionPrimaryLiteralChar*,           \
        Gyoji::frontend::tree::    ExpressionPrimaryLiteralString*,         \
        Gyoji::frontend::tree::    ExpressionPrimaryLiteralInt*,            \
        Gyoji::frontend::tree::    ExpressionPrimaryLiteralFloat*,          \
        Gyoji::frontend::tree::    ExpressionPrimaryLiteralBool*,           \
        Gyoji::frontend::tree::    ExpressionPrimaryLiteralNull*,           \
        Gyoji::frontend::tree::  ExpressionPostfixArrayIndex*,              \
        Gyoji::frontend::tree::  ExpressionPostfixFunctionCall*,            \
        Gyoji::frontend::tree::      ArgumentExpressionList*,               \
        Gyoji::frontend::tree::  ExpressionPostfixDot*,                     \
        Gyoji::frontend::tree::  ExpressionPostfixArrow*,                   \
        Gyoji::frontend::tree::  ExpressionPostfixIncDec*,                  \
        Gyoji::frontend::tree::  ExpressionUnaryPrefix*,                    \
        Gyoji::frontend::tree::  ExpressionUnarySizeofType*,                \
        Gyoji::frontend::tree::  ExpressionCast*,                           \
        Gyoji::frontend::tree::  ExpressionBinary*,                         \
        Gyoji::frontend::tree::  ExpressionTrinary*,                        \
        Gyoji::frontend::tree::StatementList*,                              \
        Gyoji::frontend::tree::  Statement*,                                \
        Gyoji::frontend::tree::      InitializerExpression*,                \
        Gyoji::frontend::tree::    StatementVariableDeclaration*,           \
        Gyoji::frontend::tree::    StatementBlock*,                         \
        Gyoji::frontend::tree::    StatementExpression*,                    \
        Gyoji::frontend::tree::    StatementGoto*,                          \
        Gyoji::frontend::tree::    StatementIfElse*,                        \
        Gyoji::frontend::tree::    StatementWhile*,                         \
        Gyoji::frontend::tree::    StatementFor*,                           \
        Gyoji::frontend::tree::    StatementSwitch*,                        \
        Gyoji::frontend::tree::      StatementSwitchContent*,               \
        Gyoji::frontend::tree::        StatementSwitchBlock*,               \
        Gyoji::frontend::tree::    StatementReturn*,                        \
        Gyoji::frontend::tree::    StatementContinue*,                      \
        Gyoji::frontend::tree::    StatementBreak*,                         \
        Gyoji::frontend::tree::    StatementLabel*,                         \
        Gyoji::frontend::tree::TranslationUnit*,                            \
        Gyoji::frontend::tree::  FileStatementList*,                        \
        Gyoji::frontend::tree::    FileStatement*,                          \
        Gyoji::frontend::tree::      FileStatementFunctionDeclStart*,        \
        Gyoji::frontend::tree::      FileStatementFunctionDefinition*,        \
        Gyoji::frontend::tree::        ScopeBody *,                           \
        Gyoji::frontend::tree::      FileStatementFunctionDeclaration*,       \
        Gyoji::frontend::tree::        ArrayLength*,                          \
        Gyoji::frontend::tree::        GlobalInitializer*,                    \
        Gyoji::frontend::tree::          GlobalInitializerExpressionPrimary*, \
        Gyoji::frontend::tree::          GlobalInitializerAddressofExpressionPrimary*, \
        Gyoji::frontend::tree::          GlobalInitializerStructInitializerList*,      \
        Gyoji::frontend::tree::            StructInitializerList*,            \
        Gyoji::frontend::tree::              StructInitializer*,              \
        Gyoji::frontend::tree::      FileStatementGlobalDefinition*,          \
        Gyoji::frontend::tree::      FileStatementNamespace*,                 \
        Gyoji::frontend::tree::        NamespaceDeclaration*,                 \
        Gyoji::frontend::tree::      FileStatementUsing*,                     \
        Gyoji::frontend::tree::        UsingAs*  
};
