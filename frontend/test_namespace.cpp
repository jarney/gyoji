#include <jlang-frontend/namespace.hpp>
#include <jlang-misc/test.hpp>

using namespace JLang::frontend;

int main(int argc, char **argv)
{
  printf("Testing namespace search functionality\n");
  NamespaceContext ctx;

  ctx.namespace_new("std", Namespace::TYPE_NAMESPACE, Namespace::VISIBILITY_PUBLIC);
  ctx.namespace_push("std");

  ctx.namespace_new("string", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
  ctx.namespace_new("string_protected", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PROTECTED);
  ctx.namespace_new("string_private", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PRIVATE);
  
  // Check that in the context of this namespace,
  // we can resolve public, private, and protected
  // without fully qualifying the name.
  
  {
    NamespaceFoundReason::ptr string_public = ctx.namespace_lookup("string");
    ASSERT_BOOL(true, string_public->reason == NamespaceFoundReason::REASON_FOUND, "Resolved string as public");
        
    NamespaceFoundReason::ptr string_protected = ctx.namespace_lookup("string_protected");
    ASSERT_BOOL(true, string_protected->reason == NamespaceFoundReason::REASON_FOUND, "Resolved string as protected");
    
    NamespaceFoundReason::ptr string_private = ctx.namespace_lookup("string_private");
    ASSERT_BOOL(true, string_private->reason == NamespaceFoundReason::REASON_FOUND, "Resolved string as private");
  }

  // Look things up using a fully-qualified path to
  // force resolution through the root.

  
  {
    NamespaceFoundReason::ptr string_public = ctx.namespace_lookup("::std::string");
    ASSERT_BOOL(true, string_public->reason == NamespaceFoundReason::REASON_FOUND, "Resolved string as public");
    
    NamespaceFoundReason::ptr string_protected = ctx.namespace_lookup("::std::string_protected");
    ASSERT_BOOL(true, string_protected->reason == NamespaceFoundReason::REASON_FOUND, "Resolved string as protected");
    
    NamespaceFoundReason::ptr string_private = ctx.namespace_lookup("::std::string_private");
    ASSERT_BOOL(true, string_private->reason == NamespaceFoundReason::REASON_FOUND, "Resolved string as private");
  }

  // Introduce a new set of datatypes in a nested namespace.
  ctx.namespace_new("datatypes", Namespace::TYPE_NAMESPACE, Namespace::VISIBILITY_PUBLIC);
  ctx.namespace_push("datatypes");
  
  ctx.namespace_new("int", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
  ctx.namespace_new("int_protected", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PROTECTED);
  ctx.namespace_new("int_private", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PRIVATE);

  // In this context, we should be able
  // to resolve everything in our parent namespaces directly.

  
  {
    NamespaceFoundReason::ptr string_public = ctx.namespace_lookup("string");
    ASSERT_BOOL(true, string_public->reason == NamespaceFoundReason::REASON_FOUND, "Resolved string as public because we're in a nested namespace.");
    
    NamespaceFoundReason::ptr string_protected = ctx.namespace_lookup("string_protected");
    ASSERT_BOOL(true, string_protected->reason == NamespaceFoundReason::REASON_FOUND, "We can see protected because we're in a nested namespace.");
    
    NamespaceFoundReason::ptr string_private = ctx.namespace_lookup("string_private");
    ASSERT_BOOL(true, string_private->reason == NamespaceFoundReason::REASON_FOUND, "We can see private because we're in a nested namespace.");
  }

  // In this context, we should be able
  // to resolve everything in our parent namespaces directly.

  
  {
    NamespaceFoundReason::ptr int_public = ctx.namespace_lookup("int");
    ASSERT_BOOL(true, int_public->reason == NamespaceFoundReason::REASON_FOUND, "Resolved string as public");
    
    NamespaceFoundReason::ptr int_protected = ctx.namespace_lookup("int_protected");
    ASSERT_BOOL(true, int_protected->reason == NamespaceFoundReason::REASON_FOUND, "Protected should be visible");
    
    NamespaceFoundReason::ptr int_private = ctx.namespace_lookup("int_private");
    ASSERT_BOOL(true, int_private->reason == NamespaceFoundReason::REASON_FOUND, "Private should not be visible outside that scope");
  }

  ctx.namespace_pop();

  {
    NamespaceFoundReason::ptr int_public = ctx.namespace_lookup("int");
    ASSERT_BOOL(true, int_public->reason == NamespaceFoundReason::REASON_NOT_FOUND, "It's not in our current namespace because it's nested");
    
    NamespaceFoundReason::ptr int_dt_public = ctx.namespace_lookup("datatypes::int");
    ASSERT_BOOL(true, int_dt_public->reason == NamespaceFoundReason::REASON_FOUND, "Resolved string as public");

    NamespaceFoundReason::ptr int_dt_protected = ctx.namespace_lookup("datatypes::int_protected");
    ASSERT_BOOL(true, int_dt_protected->reason == NamespaceFoundReason::REASON_FOUND, "Protected should be visible");
    
    NamespaceFoundReason::ptr int_dt_private = ctx.namespace_lookup("datatypes::int_private");
    ASSERT_BOOL(true, int_dt_private->reason == NamespaceFoundReason::REASON_NOT_FOUND_PRIVATE, "Private should not be visible outside that scope");
  }

  ctx.namespace_pop();
  
  // Now that we're outside of that namespace, we should no longer be able to
  // resolve protected or private types.

  {
    NamespaceFoundReason::ptr string_public = ctx.namespace_lookup("::std::string");
    ASSERT_BOOL(true, string_public->reason == NamespaceFoundReason::REASON_FOUND, "Resolved string as public");
    
    NamespaceFoundReason::ptr string_protected = ctx.namespace_lookup("::std::string_protected");
    ASSERT_BOOL(true, string_protected->reason == NamespaceFoundReason::REASON_FOUND, "Resolved string as protected");
    
    NamespaceFoundReason::ptr string_private = ctx.namespace_lookup("::std::string_private");
    ASSERT_BOOL(true, string_private->reason == NamespaceFoundReason::REASON_NOT_FOUND_PRIVATE, "Resolved string as private");

  }

  // Now that we're outside of that namespace, we should no longer be able to
  // resolve protected or private types.

  
  {
    NamespaceFoundReason::ptr string_public = ctx.namespace_lookup("std::string");
    ASSERT_BOOL(true, string_public->reason == NamespaceFoundReason::REASON_FOUND, "Resolved string as public");
    
    NamespaceFoundReason::ptr string_protected = ctx.namespace_lookup("std::string_protected");
    ASSERT_BOOL(true, string_protected->reason == NamespaceFoundReason::REASON_FOUND, "Resolved string as protected");
    
    NamespaceFoundReason::ptr string_private = ctx.namespace_lookup("std::string_private");
    ASSERT_BOOL(true, string_private->reason == NamespaceFoundReason::REASON_NOT_FOUND_PRIVATE, "Resolved string as private");

  }
  // Now let's add a 'using'
  // so that 'int' becomes a part of our
  // scope implicitly.
  
  {
    NamespaceFoundReason::ptr string_public = ctx.namespace_lookup("string");
    ASSERT_BOOL(true, string_public->reason == NamespaceFoundReason::REASON_NOT_FOUND, "String not visible because we're outside the std namespace");

  }
  
  NamespaceFoundReason::ptr found_std = ctx.namespace_lookup("::std");
  ASSERT_BOOL(true, found_std->reason == NamespaceFoundReason::REASON_FOUND, "Found the std namespace");
  ctx.namespace_using("foo", found_std->location);
  ctx.namespace_using("", ctx.namespace_lookup("::std::datatypes")->location);
  
  {
    NamespaceFoundReason::ptr string_public = ctx.namespace_lookup("foo::string");
    ASSERT_BOOL(true, string_public->reason == NamespaceFoundReason::REASON_FOUND, "String is visible because we're 'using' the std namespace");
    
    NamespaceFoundReason::ptr int_public = ctx.namespace_lookup("int");
    ASSERT_BOOL(true, int_public->reason == NamespaceFoundReason::REASON_FOUND, "Int is visible because we're 'using' the std::datatypes namespace");
  }

  printf("    PASSED\n");

  
}
// When can we find something
// that is protected but not private?
// Only when we're in the same namespace?  But the same namespace also means protected, so
// is there really any difference?

