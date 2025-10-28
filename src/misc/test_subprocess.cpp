#include <gyoji-misc/subprocess.hpp>
#include <unistd.h>

using namespace Gyoji::misc::subprocess;

int main(int argc, char **argv)
{
    SubProcess lsproc(
	std::move(Gyoji::owned_new<SubProcessReaderFile>(STDOUT_FILENO)),
	std::move(Gyoji::owned_new<SubProcessReaderFile>(STDERR_FILENO)),
	std::move(Gyoji::owned_new<SubProcessWriterEmpty>())
	);

    std::vector<std::string> arguments;
    std::map<std::string, std::string> environment;
//    int rc = lsproc.invoke("/home/jona/NetBeansProjects/jos/compiler/jlang2/test-shell.sh",
    int rc = lsproc.invoke("ls",
		  arguments,
		  environment
	);
    fprintf(stderr, "Child process exited with %d\n", rc);
    return 0;
}
