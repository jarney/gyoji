#include <stdio.h>
#include <string>
#include <vector>
#include <gyoji-misc/pointers.hpp>

namespace Gyoji::misc::cmdline {


    /**
     * These are the result of selecting the
     * options available.  All possible options
     * are present here and the is_set indicates
     * whether the value was set by the user.
     */
    class OptionValues {
    public:
	OptionValues();
	~OptionValues();
	bool is_set();
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
	virtual ~Option();
	
	size_t get_position() const;
	std::string get_shortname() const;
	std::string get_longname() const;
	
    protected:
	Option(OptionType _type, std::string _shortname, std::string _longname);
	Option(size_t position);
    private:
	OptionType type;
	std::string shortname;
	std::string longname;
	size_t position;
    };
    
    class OptionPositional : public Option {
    public:
	OptionPositional(size_t position);
	~OptionPositional();
    };
    
    class OptionBoolean : public Option {
    public:
	OptionBoolean(std::string shortname, std::string longname);
	~OptionBoolean();
    private:
    };

    class OptionString : public Option {
    public:
	OptionString(std::string shortname, std::string longname);
	~OptionString();
    };
    
    class GetOptions {
    public:
	GetOptions();
	~GetOptions();
	
	Gyoji::owned<OptionValues> getopt(int argc, char **argv);
    private:
	// This is the set of options the user might want to select.
	std::vector<Option> possible_options;
    };


};

