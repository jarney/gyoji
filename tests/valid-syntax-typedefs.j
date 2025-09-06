namespace std {
          class string {
              public class u8 int;    // Define u8 to be of type 'int'.
              
              public static u8 nstrings; // Define a static string in the namespace of 'std::string' to be the variable nstrings.
              u8 foo;
          };
};

// In this case, it's just a re-naming of string, not a composite type.
// We don't actually need a separate keyword here.
class mystring std::string; 

int do_something(char a)
{
        std::string::u8 ns = std::strings::nstrings;
        
        // Rule when parsing:

// Read the pattern.  If the pattern is a 'namespace' then
// push the namespace into a lex stack and continue lexing.
// otherwise, it's an identifier.

// identifier
//     : IDENTIFIER
//     | opt_namespace_qualifier IDENTIFIER // Lexer has not identified a type inside the namespace and therefore this must be an identifier.
//     ;
//
// type_name
//     : TYPE_NAME
//     | opt_namespace_qualifier TYPE_NAME  // Lexer has identified that this is the name of a type inside a known namespace.
//     ;
//
// opt_namespace_qualifier:
//     : /**/
//     | namespace_qualifier
//         namespace_clear()   // We've reached the end of the namespaces, the following type is a true TYPE_NAME or IDENTIFIER inside a namespace
//     ;

// namespace_qualifier
//     : NAMESPACE            // Lexer has identified the namespace as being the name of a (nested) namespace.
//        namespace_push($1);
//     | namespace_qualifier NAMESPACE  // Lexer has identified namespace as the next part of a namespace.
//        namespace_push($2);
//     ;

        // Each 'std ::' may be interpreted as
        // a namespace qualifier (if it resolves to a namespace)
        // and then the last component will either be an IDENTIFIER or a TYPE
        // depending on the leaf node.
}
