#pragma once

#include <string>

namespace JLang::context {
    /**
     * This is a reference to a point in the source
     * file and is used as a way to mark portions
     * of the code so that the error handler has
     * a way to bring the source code to the context
     * of an error message.
     */
    class SourceReference {
    public:
	SourceReference(
	    const std::string & _filename,
	    size_t _line,
	    size_t _column,
	    size_t _length
	    );
	SourceReference(const SourceReference & _other);
	~SourceReference();
	const std::string & get_filename() const;
	size_t get_line() const;
	size_t get_column() const;
	size_t get_length() const;
    private:
	const std::string & filename;
	size_t line;
	size_t column;
	size_t length;
    };
};
