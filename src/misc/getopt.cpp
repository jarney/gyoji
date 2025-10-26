#include <gyoji-misc/getopt.hpp>

using namespace Gyoji::misc::cmdline;

//////////////////////////////////////////////
// OptionValues
//////////////////////////////////////////////

OptionValues::OptionValues()
{}
OptionValues::~OptionValues()
{}


bool
OptionValues::is_set()
{
    return true;
}

//////////////////////////////////////////////
// Option
//////////////////////////////////////////////

Option::Option(OptionType _type, std::string _shortname, std::string _longname)
    : type(_type)
    , shortname(_shortname)
    , longname(_longname)
    , position(0)
{}

Option::Option(size_t _position)
    : type(OPTION_POSITIONAL)
    , shortname()
    , longname()
    , position(_position)
{}

Option::~Option()
{}


//////////////////////////////////////////////
// OptionString
//////////////////////////////////////////////
OptionString::OptionString(std::string shortname, std::string longname)
    : Option(Option::OPTION_STRING, shortname, longname)
{}
OptionString::~OptionString()
{}

//////////////////////////////////////////////
// OptionBoolean
//////////////////////////////////////////////
OptionBoolean::OptionBoolean(std::string shortname, std::string longname)
    : Option(Option::OPTION_BOOLEAN, shortname, longname)
{}
OptionBoolean::~OptionBoolean()
{}

//////////////////////////////////////////////
// OptionPositional
//////////////////////////////////////////////
OptionPositional::OptionPositional(size_t position)
    : Option(position)
{}
OptionPositional::~OptionPositional()
{}

//////////////////////////////////////////////
// GetOptions
//////////////////////////////////////////////
GetOptions::GetOptions()
{}
GetOptions::~GetOptions()
{}

Gyoji::owned<OptionValues>
GetOptions::getopt(int argc, char **argv)
{
    auto option_values = Gyoji::owned_new<OptionValues>();

    return option_values;
}

