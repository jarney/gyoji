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
#include <gyoji-misc/subprocess.hpp>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <ctime>

using namespace Gyoji::misc::subprocess;

#define READ_END 0
#define WRITE_END 1

#define BUFFER_LENGTH (512)

class Buffer {
public:
    Buffer(
        SubProcessWriter & _writer
	);
    ~Buffer();

    size_t read(char *data, size_t maxlen);
    void consume(size_t bytes);
    bool is_eof() const;
private:
    char buffer[BUFFER_LENGTH];
    size_t position;
    size_t length;
    bool eof;
    SubProcessWriter & writer;
};

Buffer::Buffer(
        SubProcessWriter & _writer
    )
    : position(0)
    , length(0)
    , eof(false)
    , writer(_writer)
{}
Buffer::~Buffer()
{}

bool
Buffer::is_eof() const
{ return eof; }

size_t
Buffer::read(char *data, size_t maxlen)
{
    // We've read to the end of
    // the buffer, so we want to refill the buffer
    if (position == length) {
	// Refill buffer.
	position = 0;
	length = 0;

	// Read until we get something.
	// If we get nothing, check for EOF
	// otherwise keep spinning.
	while (true) {
	    int bytes_read = writer.read(buffer, sizeof(buffer));
	    if (bytes_read == 0 && writer.is_eof()) {
		eof = true;
		return 0;
	    }
	    length += bytes_read;
	    if (length != 0) {
		break;
	    }
	    else {
		// Keep spinning on the read
		usleep(100*1000);
	    }
	}
	// At this point, length != 0, so we definitely
	// have some data to handle.  If it was EOF,
	// we would have returned already.
    }
    
    size_t bytes = std::min(maxlen, length-position);
    memcpy(data, &buffer[position], bytes);
    return bytes;
}

void
Buffer::consume(size_t bytes)
{
    position += bytes;
}
//////////////////////////////////////////////
// SubProcessReader
//////////////////////////////////////////////

SubProcessReader::SubProcessReader()
{}

SubProcessReader::~SubProcessReader()
{}

//////////////////////////////////////////////
// SubProcessReaderFile
//////////////////////////////////////////////
SubProcessReaderFile::SubProcessReaderFile(int _fd)
    : SubProcessReader()
    , fd(_fd)
{}

SubProcessReaderFile::~SubProcessReaderFile()
{}

int
SubProcessReaderFile::write(char *buffer, size_t bytes)
{
    return ::write(fd, buffer, bytes);
}


//////////////////////////////////////////////
// SubProcessWriter
//////////////////////////////////////////////

SubProcessWriter::SubProcessWriter()
{}

SubProcessWriter::~SubProcessWriter()
{}



//////////////////////////////////////////////
// SubProcessWriterEmpty
//////////////////////////////////////////////

SubProcessWriterEmpty::SubProcessWriterEmpty()
    : SubProcessWriter()
{}

SubProcessWriterEmpty::~SubProcessWriterEmpty()
{}

int
SubProcessWriterEmpty::read(const char *buffer, size_t bytes)
{
    return 0;
}

bool
SubProcessWriterEmpty::is_eof() const
{
    // Signal EOF immediately
    // because we have nothing to read.
    return true;
}

//////////////////////////////////////////////
// SubProcess
//////////////////////////////////////////////

SubProcess::SubProcess(
    Gyoji::owned<SubProcessReader> _stdout_reader,
    Gyoji::owned<SubProcessReader> _stderr_reader,
    Gyoji::owned<SubProcessWriter> _stdin_writer
    )
    : stdout_reader(std::move(_stdout_reader))
    , stderr_reader(std::move(_stderr_reader))
    , stdin_writer(std::move(_stdin_writer))
{}

SubProcess::~SubProcess()
{}

int
SubProcess::invoke(
    std::string command_name,
    std::vector<std::string> arguments,
    std::map<std::string, std::string> environment
    ) const
{
    Buffer reader(*stdin_writer);
    
    int stdin_fds[2];
    int stdout_fds[2];
    int stderr_fds[2];
    
    ::pipe(stdin_fds);
    ::pipe(stdout_fds);
    ::pipe(stderr_fds);

    // fork the process
    pid_t pid = ::fork();
    if (pid == -1) {
	// Fork failed, we're out of memory
	// or the OS is in a bad state.
	return -1;
    }
    else if (pid != 0) { // Parent
	// Close write ends of stdout and stderr for write
	// Close read end of stdin
	::close(stdout_fds[WRITE_END]);
	::close(stderr_fds[WRITE_END]);
	::close(stdin_fds[READ_END]);

	// Parent now has access to child read/write pipes.
	int child_stdin = stdin_fds[WRITE_END];
	bool child_stdin_open = true;
	
	int child_stdout = stdout_fds[READ_END];
	bool child_stdout_open = true;
	
	int child_stderr = stderr_fds[READ_END];
	bool child_stderr_open = true;

	// TODO: This should be a giant loop.  Probably move this
	// into another method that specifically handles the child I/O.
	// We need to set up the 'select' and fd_set based on what
	// file descriptors are still open.  This is because the 'stdin'
	// descriptor is expected to close early in the process and the
	// corresponding 'stdout/stderr' will close whenever the child is
	// done writing their data, so we don't know when that's going to be
	// either.  Bottom line, any of these descriptors might close at
	// any time, so we need to handle all cases.
	
	// Parent now writes any required content to child
	// and closes the FD to signal to the child
	// that all writing has finished.
	// Simultaneously, parent attempts to read
	// from stdout, stderr, capturing any output
	// from the child.

	while (child_stdin_open ||
	       child_stdout_open ||
	       child_stderr_open) {
	
	    fd_set rd;
	    fd_set wr;

	    int max_fd = -1;
	    FD_ZERO(&wr);
	    if (child_stdin_open) {
		max_fd = std::max(max_fd, child_stdin);
		FD_SET(child_stdin, &wr);
	    }
	    
	    FD_ZERO(&rd);
	    if (child_stdout_open) {
		max_fd = std::max(max_fd, child_stdout);
		FD_SET(child_stdout, &rd);
	    }
	    if (child_stderr_open) {
		max_fd = std::max(max_fd, child_stderr);
		FD_SET(child_stderr, &rd);
	    }
	    // Max fd must be >-1 here because
	    // otherwise, at least one of child_stdout,err,in
	    // is true.

	    // Timeout in 100ms for read.
	    struct timeval timeout;
	    timeout.tv_sec = 0;
	    timeout.tv_usec = 100*1000;
	    
	    // See what's available to read and/or write.
	    int available = ::select(
		max_fd+1,
		&rd,
		&wr,
		nullptr,
		&timeout
		);
	    if (FD_ISSET(child_stdin, &wr)) {
		// This is a non-blocking pipe
		// according to POSIX.
		// https://pubs.opengroup.org/onlinepubs/9699919799/functions/pipe.html
		//
		// We will just keep writing chunks of data
		// until we've reached the end of the stream
		// and then we'll close the FD.
		
		// TODO: We need to only read from
		// the reader when the read buffer is empty.  Probably
		// should marshal this through an intermediary,
		// maybe the writer itself.
		char buffer[BUFFER_LENGTH];
		int got_from_writer = reader.read(buffer, BUFFER_LENGTH);
		if (got_from_writer == 0 && reader.is_eof()) {
		    // on EOF, close it.
		    // Register a callback to do the write
		    // or at least grab a chunk of data to write.
		    close(child_stdin);
		    child_stdin_open = false;
		}
		else {
		    size_t wrote_to_subprocess = write(child_stdin, buffer, got_from_writer);
		    // We successfully wrote to the subprocess,
		    // so we can safely mark that portion of the input as
		    // consumed.
		    reader.consume(wrote_to_subprocess);
		}
	    }
	    if (FD_ISSET(child_stdout, &rd)) {
		// Reads child's stdout and
		// put it somewhere.
		// Register a callback to receive it.
		char buffer[BUFFER_LENGTH];
		errno = 0;
		int read_from_stdout = ::read(child_stdout, buffer, BUFFER_LENGTH);
		if (read_from_stdout == 0 && errno != EAGAIN) {
		    ::close(child_stdout);
		    child_stdout_open = false;
		}
		stdout_reader->write(buffer, read_from_stdout);
	    }
	    if (FD_ISSET(child_stderr, &rd)) {
		// Read child's stderr and
		// put it somewhere.
		char buffer[BUFFER_LENGTH];
		errno = 0;
		int read_from_stderr = ::read(child_stderr, buffer, BUFFER_LENGTH);
		if (read_from_stderr == 0 && errno != EAGAIN) {
		    ::close(child_stderr);
		    child_stderr_open = false;
		}
		stderr_reader->write(buffer, read_from_stderr);
	    }
	}
	int exit_status;
	::waitpid(pid, &exit_status, 0);

	return WEXITSTATUS(exit_status);
    }
    else { // Child, pid == 0
	// Close read ends of stdout and stderr
	// Close write end of stdin
	close(stdout_fds[READ_END]);	
	close(stderr_fds[READ_END]);	
	close(stdin_fds[WRITE_END]);

	// Assign the pipes to the
	// actual file descriptors stdin,out,err
	// respectively.
	dup2(stdout_fds[WRITE_END], STDOUT_FILENO);
	dup2(stderr_fds[WRITE_END], STDERR_FILENO);
	dup2(stdin_fds[READ_END], STDIN_FILENO);
	
	// Exec the process we want to run.
	const char *cmd = command_name.c_str();
	const char * args[arguments.size()+1];

	size_t ai = 0;
	for (const auto & arg : arguments) {
	    args[ai++] = arg.c_str();
	}
	args[ai] = nullptr;

	const char *env[environment.size()+1];
	std::vector<std::string> env_list;
	size_t ei = 0;
	for (const auto & env_var : environment) {
	    env_list.push_back(env_var.first + std::string("=") + std::string(env_var.second));
	    env[ei++] = env_list.back().c_str();
	}
	env[ei] = nullptr;

	// I strongly suspect that the declaration of execvpe is wrong, but
	// this seems to satisfy it.
	int rc = execvpe(cmd, (char *const*)&args[0], (char *const*)&env[0]);

	// If we have reached here at all, it must be
	// an error regardless of what the value of rc is.
	fprintf(stderr, "Process would not be launched\n");

	return rc;
    }
}
