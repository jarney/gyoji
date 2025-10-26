#include <gyoji-misc/getopt.hpp>

using namespace Gyoji::misc::cmdline;

// Here, we need to give the set of possible
// selectable options to parse from.
// There are several types of option available.
//   * Named boolean values (e.g. -g, just a simple option that can be selected on or off)
//     These can be single character options like -g or can be more verbose like --debug
//
//   * Named string values (e.g. -o <filename> or -llibrary).
//     These can be of the form -l<string> or --library <string>
//     Each option takes a mandatory string value that can be set
//     and there can be multiples of them.
//
//   * Un-named positional arguments.  These can appear anywhere in the
//     argument stream, but are options that must be present and must be at a particular
//     position.  For example, in jcc -c -o foo.o foo.j,
//     the positional argument is 'foo.j' and is the position 0 argument.
//     It can appear in any order, so all of these are equivalent with position '0' argument foo.j
//         jcc foo.j -c -o foo.o
//         jcc -c foo.j -o foo.o
//         jcc -c -o foo.o foo.j
//


int main(int argc, char **argv)
{

    std::vector<Option> options;
    options.push_back(
	Option::create_boolean(
	    "do-compile-only",
	    "c",
	    "compile",
	    false, // Default boolean value
	    "Compile the source file to a .o object file"
	    )
	);
    options.push_back(
	Option::create_string(
	    "do-include-dirs",
	    "I",
	    "include-dir",
	    "",
	    false, // Default boolean value
	    "Include an include directory"
	    )
	);
    options.push_back(
	Option::create_string(
	    "do-link-libs",
	    "l",
	    "link",
	    "",
	    false, // Default boolean value
	    "Include an include directory"
	    )
	);
    GetOptions get_options(options);

    auto selected_options = get_options.getopt(argc, argv);
    if (selected_options == nullptr) {
	get_options.print_help(stderr);
	return 1;
    }
    
    fprintf(stderr, "Compile only: %s\n",
	    (selected_options->get_boolean("do-compile-only") ? "true" : "false")
	);
    for (const auto & namearg : selected_options->get_named_arguments()) {
	fprintf(stderr, "Named argument %s = %s\n",
		namearg.first.c_str(),
		namearg.second.c_str()
	    );
    }
    for (const auto & arg : selected_options->get_positional_arguments()) {
	fprintf(stderr, "Positional argument %s\n",
		arg.c_str());
    }
    
    return 0;
}
