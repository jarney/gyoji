#pragma once

//----------------------------
// Unique pointer types to
// be used internally to enforce
// single ownership of these classes
// in the tree.
//----------------------------
namespace JLang {
  /**
   * This is a convenience definition for a unique pointer.
   * Pointers defined with this type are owned by the class
   * in which they are declared, so their life-span should
   * be at least as long as the structure they contain.
   *
   * The owning class may safely return references to
   * the owned type as long as the caller agrees not
   * to let the reference live longer than the object
   * that gave it up.
   *
   * For example, this is the typical pattern of usage for
   * an owned pointer.
   *
   * <pre>
   * class Foo {
   * };
   * class Bar {
   * public:
   *    const Foo & get() const {
   *        return *foo;
   *    };
   * private:
   *   JLang::owned<Foo> foo;
   * };
   * </pre>
   *
   * Given this setup, this is an acceptable use:
   * <pre>
   * void some_function()
   * {
   *     Bar b;
   *     const Foo & foo = b.get();
   * }
   * </pre>
   *
   * However, the following is an unacceptable use
   * because the reference lives beyond the scope of
   * the owning pointer.  Note that the J language
   * is designed so that this usage will be
   * disallowed by the compiler, but since the bootstrap
   * is written in C++, this is still possible
   * even if it is unsafe.
   *
   * <pre>
   * const Foo & some_function()
   * {
   *     Bar b;
   *     return b.get();
   * }
   * </pre>   
   */
  template <class T> using owned = std::unique_ptr<T>;
};
  
