#include <gyoji-misc/getopt.hpp>
#include <gyoji-misc/jstring.hpp>

using namespace Gyoji::misc::cmdline;

//////////////////////////////////////////////
// OptionValues
//////////////////////////////////////////////

OptionValues::OptionValues(
	    std::map<std::string, OptionValue> _values
    )
    : values(_values)
{}
OptionValues::~OptionValues()
{}


const OptionValue *
OptionValues::get_value(std::string option_id) const
{
    const auto & it = values.find(option_id);
    if (it == values.end()) {
	return nullptr;
    }
    return &it->second;
}
const std::map<std::string, OptionValue> &
OptionValues::get_values() const
{ return values; }

//////////////////////////////////////////////
// OptionValue
//////////////////////////////////////////////
OptionValue::OptionValue(
    std::string _string_value,
    bool _bool_value,
    bool _specified
    )
    : string_value(_string_value)
    , bool_value(_bool_value)
    , specified(_specified)
{}
OptionValue::OptionValue(const OptionValue & other)
    : string_value(other.string_value)
    , bool_value(other.bool_value)
    , specified(other.specified)
{}
OptionValue::~OptionValue()
{}

std::string
OptionValue::get_string_value() const
{ return string_value; }

void
OptionValue::set_string(std::string _string_value)
{ string_value = _string_value; }

void
OptionValue::set_specified(bool _specified)
{ specified = _specified; }

void
OptionValue::set_bool(bool _bool_value)
{ bool_value = _bool_value; }

bool
OptionValue::get_bool_value() const
{ return bool_value; }

bool
OptionValue::is_specified() const
{ return specified; }

//////////////////////////////////////////////
// Option
//////////////////////////////////////////////

Option::Option(
    std::string _option_id,
    OptionType _type,
    std::string _shortname,
    std::string _longname,
    size_t _position,
    std::string _default_string,
    bool _default_bool,
    std::string _help_text
    )
    : option_id(_option_id)
    , type(_type)
    , shortname(_shortname)
    , longname(_longname)
    , position(_position)
    , default_string(_default_string)
    , default_bool(_default_bool)
    , help_text(_help_text)
{}
Option::Option(const Option & other)
    : option_id(other.option_id)
    , type(other.type)
    , shortname(other.shortname)
    , longname(other.longname)
    , position(other.position)
    , default_string(other.default_string)
    , default_bool(other.default_bool)
    , help_text(other.help_text)
{}

Option::~Option()
{}

Option
Option::create_string(
    std::string _option_id,
    std::string _shortname,
    std::string _longname,
    std::string _default_string,
    bool _default_bool,
    std::string _help_text
    )
{
    return Option(
	_option_id,
	Option::OPTION_STRING,
	_shortname,
	_longname,
	0,
	_default_string,
	_default_bool,
	_help_text
	);
}
Option
Option::create_boolean(
    std::string _option_id,
    std::string _shortname,
    std::string _longname,
    bool _default_bool,
    std::string _help_text
    )
{
    return Option(
	_option_id,
	Option::OPTION_BOOLEAN,
	_shortname,
	_longname,
	0,
	std::string(),
	_default_bool,
	_help_text
	);
}
Option
Option::create_positional(
    std::string _option_id,
    size_t _position,
    std::string _default_string,
    std::string _help_text
    )
{
   return Option(
	_option_id,
	Option::OPTION_POSITIONAL,
	std::string(),
	std::string(),
	_position,
	_default_string,
	false,
	_help_text
	);
}

std::string
Option::get_id() const
{ return option_id; }

Option::OptionType
Option::get_type() const
{ return type; }

size_t
Option::get_position() const
{ return position; }

std::string
Option::get_shortname() const
{ return shortname; }

std::string
Option::get_longname() const
{ return longname; }

std::string
Option::get_default_string() const
{ return default_string; }

bool
Option::get_default_bool() const
{ return default_bool; }

std::string
Option::get_help_text() const
{ return help_text; }

//////////////////////////////////////////////
// GetOptions
//////////////////////////////////////////////
GetOptions::GetOptions(
    std::vector<Option> _options
    )
    : options(_options)
{}
GetOptions::~GetOptions()
{}

void
GetOptions::print_help(FILE *out)
{
    fprintf(out, "Usage:\n");
    for (const auto & option : options) {
	switch (option.get_type()) {
	case Option::OPTION_BOOLEAN:
            fprintf(out, "    -%s | --%s\n",
		    option.get_shortname().c_str(),
		    option.get_longname().c_str()
		);
	    break;
	case Option::OPTION_STRING:
            fprintf(out, "    -%s <%s> | --%s <%s>\n",
		    option.get_shortname().c_str(),
		    option.get_id().c_str(),
		    option.get_longname().c_str(),
		    option.get_id().c_str()
		);
	    break;
	case Option::OPTION_POSITIONAL:
	    fprintf(out, "    <%s>\n", option.get_id().c_str());
	    break;
	}
	std::string help_wrapped = Gyoji::misc::wrap_text((size_t)70, option.get_help_text());
	std::string help_indented = Gyoji::misc::indent_text(20, help_wrapped);
	fprintf(out, "%s\n", help_indented.c_str());
    }
	    
}

Gyoji::owned<OptionValues>
GetOptions::getopt(int argc, char **argv)
{
    std::map<std::string, OptionValue> values;

    std::map<std::string, const Option*> options_by_shortname;
    std::map<std::string, const Option*> options_by_longname;
    std::map<size_t, const Option*> options_by_position;
    
    for (const auto & option : options) {
	// For each option, prepare the 'default' value
	// of that option.
	values.insert(
	    std::pair(
		option.get_id(),
		OptionValue(
		    option.get_default_string(),
		    option.get_default_bool(),
		    false // This option was not specified, it was defaulted.
		    )
		)
	    );
	switch (option.get_type()) {
	case Option::OPTION_BOOLEAN:
	case Option::OPTION_STRING:
	{
	    std::string longname = option.get_longname();
	    std::string shortname = option.get_shortname();
	    if (longname.size() != 0) {
		options_by_longname.insert(
		    std::pair(longname, &option)
		    );
	    }
	    if (shortname.size() != 0) {
		options_by_shortname.insert(
		    std::pair(shortname, &option)
		    );
	    }
	}
	    break;
	case Option::OPTION_POSITIONAL:
	{
	    options_by_position.insert(
		std::pair(option.get_position(), &option)
		);
	}
	    break;
	}
    }

    size_t len = (size_t) argc;
    size_t pos = 1;
    size_t position_id = 0;
    while (pos < len) {
	// Start parsing the options:
	std::string arg(argv[pos]);
	const Option *opt = nullptr;
	
	if (startswith(arg, std::string("--"))) {
	    std::string longname = arg.substr(2);
	    const auto & it = options_by_longname.find(longname);
	    if (it == options_by_longname.end()) {
		fprintf(stderr, "No such long option %s\n", arg.c_str());
		return nullptr;
	    }
	    opt = it->second;
	}
	else if (startswith(arg, std::string("-"))) {
	    std::string shortname = arg.substr(1);
	    const auto & it = options_by_shortname.find(shortname);
	    if (it == options_by_shortname.end()) {
		fprintf(stderr, "No such short option %s\n", shortname.c_str());
		return nullptr;
	    }
	    opt = it->second;
	}
	else {
	    const auto & it = options_by_position.find(position_id);
	    if (it == options_by_position.end()) {
		fprintf(stderr, "No such positional option %ld\n", position_id);
		return nullptr;
	    }
	    opt = it->second;
	}
	if (opt == nullptr) {
	    break;
	}
	const auto & it_val = values.find(opt->get_id());
	if (it_val == values.end()) {
	    fprintf(stderr, "No such string argument %s\n", opt->get_id().c_str());
	    break;
	}
	OptionValue & value = it_val->second;
	    
	switch (opt->get_type()) {
	case Option::OPTION_STRING:
	{
	    if (pos+1 >= len) {
		fprintf(stderr, "String option %s requires a value\n", opt->get_id().c_str());
		return nullptr;
	    }
	    std::string avalue(argv[pos+1]);

	    value.set_specified(true);
	    value.set_string(avalue);
	    value.set_bool(true);
	    
	    // Skip past the argument and its value.
	    pos += 2;
	}
	    break;
	case Option::OPTION_BOOLEAN:
	    value.set_specified(true);
	    value.set_bool(true);
	    pos++;
	    break;
	case Option::OPTION_POSITIONAL:
	    value.set_specified(true);
	    value.set_string(argv[pos]);
	    value.set_bool(true);
	    pos++;
	    position_id++;
	    break;
	}
    }

    auto option_values = Gyoji::owned_new<OptionValues>(values);

    return option_values;
}
