#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <gyoji-misc/pointers.hpp>

namespace Gyoji::misc::cmdline {

    class OptionValue {
    public:
	OptionValue(
	    std::string _string_value,
	    bool _bool_value,
	    bool _specified
	    );
	OptionValue(const OptionValue & other);
	~OptionValue();

	void set_string(std::string _string_value);

	void set_specified(bool _specified);

	void set_bool(bool _bool_value);

	/**
	 * Return the string associated with this option
	 * for positional or named options.
	 */
	std::string get_string_value() const;
	
	/**
	 * Return true if this option should be turned on.
	 */
	bool get_bool_value() const;

	/**
	 * Return true if this option was specified by the user
	 * and false if it was the default value by the system.
	 * This lets us know whether the user overrode the
	 * default setting and actually specified this behavior.
	 */
	bool is_specified() const;
    private:
	std::string string_value;
	bool bool_value;
	bool specified;
    };
    

    /**
     * These are the result of selecting the
     * options available.  All possible options
     * are present here and the is_set indicates
     * whether the value was set by the user.
     */
    class OptionValues {
    public:
	OptionValues(
	    const std::map<std::string, bool> & _bool_arguments,
	    const std::map<std::string, std::string> & _str_arguments,
	    const std::map<std::string, std::vector<std::string>> & _named_arguments,
	    const std::vector<std::string> & _positional_arguments
	    );
	
	~OptionValues();
	/**
	 * Return a boolean value depending on whether a boolean
	 * option is turned on or off.  This would be 'true' if the '--compile' argument
	 * is passed or 'false' if '--no-compile' is passed.
	 */
	bool get_boolean(std::string boolean_option_id) const;

	std::string get_string(std::string option_id) const;
	    
	/**
	 * Return the list of 'string' arguments in the order they were specified.
	 * This is used for things like -I or -l or -L where a number of string
	 * arguments can be provided, each providing one element of a list like
	 * include paths or link libraries.  If the '-I' option is named 'include-path'
	 * then this would return -I <path1> would return 'std::pair("include-path", "<path1>");
	 */
	const std::map<std::string, std::vector<std::string>> & get_named_arguments() const;

	/**
	 * Return the list of positional arguments, for example,
	 * if the command wants <program> <options> <arg1> <arg2>,
	 * this returns <arg1> and <arg2> as elements in the order
	 * they were given on the command-line.
	 */
	const std::vector<std::string> & get_positional_arguments() const;
	
    private:
	std::map<std::string, bool> bool_arguments;
	std::map<std::string, std::string> str_arguments;
	std::map<std::string, std::vector<std::string>> named_arguments;
	std::vector<std::string> positional_arguments;
	
    };
    
    /**
     * This models a particular option the user might select.
     */
    class Option {
    public:
	typedef enum {
	    OPTION_BOOLEAN,
	    OPTION_SINGLE_STRING,
	    OPTION_STRING_LIST
	} OptionType;
	Option(const Option & other);
	virtual ~Option();

	std::string get_id() const;
	OptionType get_type() const;
	std::string get_shortname() const;
	std::string get_longname() const;

	std::string get_default_string() const;
	bool get_default_bool() const;

	std::string get_help_text() const;
	
	static Option create_boolean(
	    std::string _option_id,
	    std::string _shortname,
	    std::string _longname,
	    std::string _help_text
	    );
	
	static Option create_string(
	    std::string _option_id,
	    std::string _shortname,
	    std::string _longname,
	    std::string _help_text
	    );

	static Option create_string_list(
	    std::string _option_id,
	    std::string _shortname,
	    std::string _longname,
	    std::string _help_text
	    );
	
    protected:
	Option(
	    std::string _option_id,
	    OptionType _type,
	    std::string _shortname,
	    std::string _longname,
	    std::string _help_text
	    );
    private:
	std::string option_id;
	OptionType type;
	std::string shortname;
	std::string longname;
	std::string help_text;
    };
    
    class GetOptions {
    public:
	GetOptions(
	    std::vector<Option> _options,
	    std::vector<std::pair<std::string, std::string>> _positional_options
	    );
	~GetOptions();
	
	void print_help(std::string command, FILE *out);

	/**
	 * This parses the command-line and returns all of the options
	 * that should be in force.  For boolean options,
	 * they are always present in the output, but may be default
	 * values.  For string values, they may be a single string or
	 * there may be multiples selected (as in the case of -I or -l, for example)
	 */
	Gyoji::owned<OptionValues> getopt(int argc, char **argv);
    private:
	// This is the set of options the user might want to select.
	std::vector<Option> options;
	std::vector<std::pair<std::string, std::string>> positional_options;
    };
};

