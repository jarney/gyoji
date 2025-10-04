#pragma once

namespace JLang::misc {

    /**
     * @brief Input Source used by lexer
     *
     * @details
     * This interface is used to provide the lexer/parser with a method
     * of reading the input data.  This can be implemented as a
     * file reader to read data from a file and can also be implemented
     * as a string buffer reader to read from memory.
     */
    class InputSource {
    public:
	/**
	 * @brief Constructor just provides an interface
	 *
	 * @details
	 * This class has no concrete implementation, but is just an
	 * interface that other classes may provide implementations for.
	 */
	InputSource();
	
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~InputSource();
	
	/**
	 * Sub-classes must implement this method to provide input to
	 * the parser.
	 * @param buf This is the buffer provided by the parser.  This method
	 *            should place the result of the read into the buffer provided.
	 * @param result This is the number of bytes actually read by the input source.
	 * @param max_size This is the maximum number of bytes to be read
	 *                 from the input source.  Note that result must
	 *                 always be less than or equal to max_size in order
	 *                 to ensure that the buffer is not overflowed.
	 */
	virtual void read(char *buf, int &result, int max_size) = 0;
    };
  
};
