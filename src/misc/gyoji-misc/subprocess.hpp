/* Copyright 2025 Jonathan S. Arney
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://github.com/jarney/gyoji/blob/master/LICENSE
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <gyoji-misc/pointers.hpp>

namespace Gyoji::misc::subprocess {

    class SubProcessReader {
    public:
	SubProcessReader();
	virtual ~SubProcessReader();
	virtual int write(char *buffer, size_t bytes) = 0;
    };
    class SubProcessWriter {
    public:
	SubProcessWriter();
	virtual ~SubProcessWriter();

	/**
	 * Used by the parent process to write data into a buffer
	 * so that the subprocess has it available for read.
	 */
	virtual int read(const char *buffer, size_t bytes) = 0;
	virtual bool is_eof() const = 0;
    };

    class SubProcessReaderFile : public SubProcessReader {
    public:
	SubProcessReaderFile(int _fd);
	virtual ~SubProcessReaderFile();
	int write(char *buffer, size_t bytes);
    private:
	int fd;
    };

    class SubProcessWriterEmpty : public SubProcessWriter {
    public:
	SubProcessWriterEmpty();
	virtual ~SubProcessWriterEmpty();
	int read(const char *buffer, size_t bytes);
	bool is_eof() const;
    private:
    };
    
    class SubProcess {
    public:
	SubProcess(
	    Gyoji::owned<SubProcessReader> _stdout_reader,
	    Gyoji::owned<SubProcessReader> _stderr_reader,
	    Gyoji::owned<SubProcessWriter> _stdin_writer
	    );
	~SubProcess();

	int invoke(
	    std::string command_name,
	    std::vector<std::string> arguments,
	    std::map<std::string, std::string> environment
	    ) const;
    private:
	Gyoji::owned<SubProcessReader> stdout_reader;
	Gyoji::owned<SubProcessReader> stderr_reader;
	Gyoji::owned<SubProcessWriter> stdin_writer;
    };
};
