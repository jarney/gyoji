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
	    std::map<std::string, OptionValue> _values
	    );
	
	~OptionValues();
	
	const OptionValue * get_value(std::string option_id) const;

	const std::map<std::string, OptionValue> & get_values() const;
	
    private:
	std::map<std::string, OptionValue> values;
    };
    
    /**
     * This models a particular option the user might select.
     */
    class Option {
    public:
	typedef enum {
	    OPTION_BOOLEAN,
	    OPTION_STRING,
	    OPTION_POSITIONAL
	} OptionType;
	Option(const Option & other);
	virtual ~Option();

	std::string get_id() const;
	OptionType get_type() const;
	size_t get_position() const;
	std::string get_shortname() const;
	std::string get_longname() const;

	std::string get_default_string() const;
	bool get_default_bool() const;

	std::string get_help_text() const;
	
	static Option create_boolean(
	    std::string _option_id,
	    std::string _shortname,
	    std::string _longname,
	    bool _default_bool,
	    std::string _help_text
	    );
	
	static Option create_string(
	    std::string _option_id,
	    std::string _shortname,
	    std::string _longname,
	    std::string _default_string,
	    bool _default_bool,
	    std::string _help_text
	    );
	
	static Option create_positional(
	    std::string _option_id,
	    size_t position,
	    std::string _default_string,
	    std::string _help_text
	    );
	
    protected:
	Option(
	    std::string _option_id,
	    OptionType _type,
	    std::string _shortname,
	    std::string _longname,
	    size_t position,
	    std::string _default_string,
	    bool _default_bool,
	    std::string _help_text
	    );
    private:
	std::string option_id;
	OptionType type;
	std::string shortname;
	std::string longname;
	size_t position;
	std::string default_string;
	bool default_bool;
	std::string help_text;
    };
    
    class GetOptions {
    public:
	GetOptions(std::vector<Option> _options);
	~GetOptions();
	
	void print_help(FILE *out);

	Gyoji::owned<OptionValues> getopt(int argc, char **argv);
    private:
	// This is the set of options the user might want to select.
	std::vector<Option> options;
    };


};

