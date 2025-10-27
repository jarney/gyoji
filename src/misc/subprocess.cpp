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
#include <sys/select.h>
#include <ctime>

using namespace Gyoji::misc::subprocess;

#define READ_END 0
#define WRITE_END 1

#define BUFFER_LENGTH (512)

SubProcessReader::SubProcessReader()
{}

SubProcessReader::~SubProcessReader()
{}

SubProcessWriter::SubProcessWriter()
{}

SubProcessWriter::~SubProcessWriter()
{}

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
    int stdin_fds[2];
    int stdout_fds[2];
    int stderr_fds[2];
    
    pipe(stdin_fds);
    pipe(stdout_fds);
    pipe(stderr_fds);
    
    // fork the process
    pid_t pid = fork();
    if (pid == -1) {
	// Fork failed, we're out of memory
	// or the OS is in a bad state.
	return -1;
    }
    else if (pid != 0) { // Parent
	// Close write ends of stdout and stderr for write
	// Close read end of stdin
	close(stdout_fds[WRITE_END]);
	close(stderr_fds[WRITE_END]);
	close(stdin_fds[READ_END]);

	// Parent now has access to child read/write pipes.
	int child_stdin = stdin_fds[WRITE_END];
	int child_stdout = stdout_fds[READ_END];
	int child_stderr = stderr_fds[READ_END];

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
	fd_set rd;
	fd_set wr;

	FD_ZERO(&wr);
	FD_SET(child_stdin, &wr);

	FD_ZERO(&rd);
	FD_SET(child_stdout, &rd);
	FD_SET(child_stderr, &rd);

	int maxfd = child_stdin;
	maxfd = child_stdout > maxfd ? child_stdout : maxfd;
	maxfd = child_stderr > maxfd ? child_stderr : maxfd;

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 100*1000;
	
	// See what's available to read and/or write.
	int available = select(
	    maxfd+1,
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
	    size_t got_from_writer = stdin_writer->write(buffer, BUFFER_LENGTH);
	    size_t remaining_to_write = BUFFER_LENGTH;
	    
	    size_t wrote_to_subprocess = write(child_stdin, &buffer[BUFFER_LENGTH - remaining_to_write], got_from_writer);
	    
	    // on EOF, close it.
	    // Register a callback to do the write
	    // or at least grab a chunk of data to write.
	    close(child_stdin);
	}
	if (FD_ISSET(child_stdout, &rd)) {
	    // Reads child's stdout and
	    // put it somewhere.
	    // Register a callback to receive it.
	    char buffer[BUFFER_LENGTH];
	    size_t read_from_stdout = read(child_stdout, buffer, BUFFER_LENGTH);
	    stdout_reader->read(buffer, read_from_stdout);
	}
	if (FD_ISSET(child_stderr, &rd)) {
	    // Read child's stderr and
	    // put it somewhere.
	    char buffer[BUFFER_LENGTH];
	    size_t read_from_stderr = read(child_stderr, buffer, BUFFER_LENGTH);
	    stderr_reader->read(buffer, read_from_stderr);
	}
	
	// Keep looping like this
	// and eventually reach EOF on all of the
	// streams.

	// Finally, once all of the data has been
	// transmitted/received, we waitpid for the child process
	// and we can return.
	
	// Note that if we reach EOF, we should
	// close the corresponding fd and
	// stop selecting on it.
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
