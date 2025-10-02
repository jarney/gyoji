
#include <jlang-context.hpp>

using namespace JLang::context;



SourceReference::SourceReference(
    std::string _filename,
    size_t _line,
    size_t _column,
    size_t _length
    )
    : filename(_filename)
    , line(_line)
    , column(_column)
    , length(_length)
{}
SourceReference::SourceReference(const SourceReference & _other)
    : filename(_other.filename)
    , line(_other.line)
    , column(_other.column)
    , length(_other.length)
{}
SourceReference::~SourceReference()
{}
const std::string &
SourceReference::get_filename() const
{ return filename; }

size_t SourceReference::get_line() const
{ return line; }

size_t SourceReference::get_column() const
{ return column; }

size_t SourceReference::get_length() const
{ return length; }

