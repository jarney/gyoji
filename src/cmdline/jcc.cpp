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
#include <gyoji-frontend.hpp>
#include <gyoji-misc/input-source-file.hpp>
#include <gyoji-misc/getopt.hpp>
#include <gyoji-misc/subprocess.hpp>
#include <gyoji-analysis.hpp>
#include <gyoji-codegen.hpp>
#include <cstring>

using namespace Gyoji::codegen;
using namespace Gyoji::context;
using namespace Gyoji::frontend;
using namespace Gyoji::mir;
using namespace Gyoji::analysis;
using namespace Gyoji::misc::cmdline;
using namespace Gyoji::misc::subprocess;

class JCCOptions {
public:
    JCCOptions();
    ~JCCOptions();

    /**
     * Name of the source file to compile.
     */
    const std::string & get_source_filename() const;
    void set_source_filename(const std::string & _filename);

    const std::string & get_output_filename() const;
    void set_output_filename(const std::string & _filename);
    
    /**
     * Whether to just compile the file or continue
     * to the link stage.
     */
    void set_compile_only(bool _compile_only);
    bool get_compile_only() const;

    /**
     * Whether to dump the MIR representation.
     */
    bool get_output_mir() const;
    void set_output_mir(bool _output_mir);

    bool get_verbose() const;
    void set_verbose(bool _verbose);
    
    /**
     * Whether to dump the LLVM IR representation.
     */
    bool get_output_llvm_ir() const;
    void set_output_llvm_ir(bool _output_llvm_ir);

    int get_optimization_level() const;
    void set_optimization_level(int level);

    const std::vector<std::string> & get_include_directories() const;
    void set_include_directories(std::vector<std::string> _include_directories);
    
private:
    std::string source_filename;
    std::string output_filename;
    bool compile_only;
    bool output_mir;
    bool verbose;
    bool output_llvm_ir;
    int optimization_level; // 0,1,2,3
    std::vector<std::string> include_directories;
};

class JCCGetopt {
public:
    static const std::string JCC_OPTION_COMPILE_ONLY;
    static const std::string JCC_OPTION_OUTPUT_MIR;
    static const std::string JCC_OPTION_OUTPUT_LLVM_IR;
    static const std::string JCC_OPTION_OPTIMIZATION_LEVEL;
    static const std::string JCC_OPTION_OUTPUT_FILENAME;
    static const std::string JCC_OPTION_INCLUDE_DIRECTORY;
    static const std::string JCC_OPTION_VERBOSE;

    static Gyoji::owned<JCCOptions> getopt(int argc, char **argv);
};

const std::string JCCGetopt::JCC_OPTION_COMPILE_ONLY = "compile-only";
const std::string JCCGetopt::JCC_OPTION_OUTPUT_MIR = "output-mir";
const std::string JCCGetopt::JCC_OPTION_OUTPUT_LLVM_IR = "output-llvm-ir";
const std::string JCCGetopt::JCC_OPTION_OPTIMIZATION_LEVEL = "optimization-level";
const std::string JCCGetopt::JCC_OPTION_OUTPUT_FILENAME = "output-filename";
const std::string JCCGetopt::JCC_OPTION_INCLUDE_DIRECTORY = "include-directory";
const std::string JCCGetopt::JCC_OPTION_VERBOSE = "verbose";

JCCOptions::JCCOptions()
{}

JCCOptions::~JCCOptions()
{}

const std::string &
JCCOptions::get_source_filename() const
{ return source_filename; }

void
JCCOptions::set_source_filename(const std::string & _filename)
{ source_filename = _filename; }

const std::string &
JCCOptions::get_output_filename() const
{ return output_filename; }

void
JCCOptions::set_output_filename(const std::string & _filename)
{ output_filename = _filename; }

bool
JCCOptions::get_compile_only() const
{ return compile_only; }

void
JCCOptions::set_compile_only(bool _compile_only)
{ compile_only = _compile_only; }

void
JCCOptions::set_output_mir(bool _output_mir)
{ output_mir = _output_mir; }

bool
JCCOptions::get_output_mir() const
{ return output_mir; }

bool
JCCOptions::get_verbose() const
{ return verbose; }

void
JCCOptions::set_verbose(bool _verbose)
{ verbose = _verbose; }

bool
JCCOptions::get_output_llvm_ir() const
{ return output_llvm_ir; }

void
JCCOptions::set_output_llvm_ir(bool _output_llvm_ir)
{ output_llvm_ir = _output_llvm_ir; }

int
JCCOptions::get_optimization_level() const
{ return optimization_level; }

void
JCCOptions::set_optimization_level(int level)
{ optimization_level = level; }

const std::vector<std::string> &
JCCOptions::get_include_directories() const
{ return include_directories; }

void
JCCOptions::set_include_directories(std::vector<std::string> _include_directories)
{ include_directories = _include_directories; }

Gyoji::owned<JCCOptions>
JCCGetopt::getopt(int argc, char **argv)
{
    std::vector<Option> options;
    options.push_back(
	Option::create_boolean(
	    JCC_OPTION_COMPILE_ONLY,
	    "c",
	    "compile",
	    "Compile the source file to a .o object file"
	    )
	);
    options.push_back(
	Option::create_boolean(
	    JCC_OPTION_OUTPUT_MIR,
	    "",
	    "output-mir",
	    "Output the MIR representation of the program"
	    )
	);
    options.push_back(
	Option::create_boolean(
	    JCC_OPTION_OUTPUT_LLVM_IR,
	    "",
	    "output-llvm-ir",
	    "Output the LLVM IR representation of the program"
	    )
	);
    options.push_back(
	Option::create_boolean(
	    JCC_OPTION_VERBOSE,
	    "v",
	    "verbose",
	    "Verbose output for debugging compiler"
	    )
	);
    options.push_back(
	Option::create_string(
	    JCC_OPTION_OPTIMIZATION_LEVEL,
	    "O",
	    "optimization-level",
	    "0=None, 1=Less, 2=Default, 3=Aggressive"
	    )
	);
    options.push_back(
	Option::create_string(
	    JCC_OPTION_OUTPUT_FILENAME,
	    "o",
	    "output",
	    "Name of the output file to produce"
	    )
	);
    options.push_back(
	Option::create_string_list(
	    JCC_OPTION_INCLUDE_DIRECTORY,
	    "I",
	    "include",
	    "Include directory"
	    )
	);
    std::vector<std::pair<std::string, std::string>> positional_options;
    positional_options.push_back(std::pair(
				     "filename", "Name of a source file to compile"
				     )
	);
    
    GetOptions get_options(
	options,
	positional_options
	);

    auto selected_options = get_options.getopt(argc, argv);
    if (selected_options == nullptr) {
	get_options.print_help("jcc", stderr);
	return nullptr;
    }

    const std::vector<std::string> & positional_arguments = selected_options->get_positional_arguments();
    if (positional_arguments.size() < 1) {
	fprintf(stderr, "JCC requires a file to compile\n");
	get_options.print_help("jcc", stderr);
	return nullptr;
    }
    
    Gyoji::owned<JCCOptions> jcc_options = Gyoji::owned_new<JCCOptions>();
    
    const auto & named_arguments = selected_options->get_named_arguments();
    
    jcc_options->set_source_filename(positional_arguments.at(0));
    jcc_options->set_compile_only(selected_options->get_boolean(JCC_OPTION_COMPILE_ONLY));
    jcc_options->set_output_mir(selected_options->get_boolean(JCC_OPTION_OUTPUT_MIR));
    jcc_options->set_verbose(selected_options->get_boolean(JCC_OPTION_VERBOSE));
    jcc_options->set_output_llvm_ir(selected_options->get_boolean(JCC_OPTION_OUTPUT_LLVM_IR));

    if (selected_options->get_boolean(JCC_OPTION_OPTIMIZATION_LEVEL)) {
	const std::string & level = selected_options->get_string(JCC_OPTION_OPTIMIZATION_LEVEL);
	if (!strcmp(level.c_str(), "0")) {
	    fprintf(stderr, "O0\n");
	    jcc_options->set_optimization_level(0);
	}
	else if (!strcmp(level.c_str(), "1")) {
	    fprintf(stderr, "O1\n");
	    jcc_options->set_optimization_level(1);
	}
	else if (!strcmp(level.c_str(), "2")) {
	    fprintf(stderr, "O2\n");
	    jcc_options->set_optimization_level(2);
	}
	else if (!strcmp(level.c_str(), "3")) {
	    fprintf(stderr, "O3\n");
	    jcc_options->set_optimization_level(3);
	}
	else {
	    fprintf(stderr, "No such optimization level %s\n", level.c_str());
	    get_options.print_help("jcc", stderr);
	    return nullptr;
	}
    }
    else {
	jcc_options->set_optimization_level(2);
    }
    
    if (selected_options->get_boolean(JCC_OPTION_OUTPUT_FILENAME)) {
	jcc_options->set_output_filename(selected_options->get_string(JCC_OPTION_OUTPUT_FILENAME));
    }
    else {
	jcc_options->set_output_filename("a.out");
    }

    const auto & include_it = named_arguments.find(JCC_OPTION_INCLUDE_DIRECTORY);
    if (include_it != named_arguments.end()) {
        jcc_options->set_include_directories(include_it->second);
    }
    
    return jcc_options;
}

int main(int argc, char **argv)
{

    Gyoji::owned<JCCOptions> options = JCCGetopt::getopt(argc, argv);
    if (options == nullptr) {
	return -1;
    }

    const std::string & input_filename = options->get_source_filename();
    const std::string & output_filename = options->get_output_filename();

    std::string preprocessed_filename = input_filename + std::string(".preproc");
    int preprocessed_fd = ::open(preprocessed_filename.c_str(), O_CREAT | O_WRONLY, 0777);
    
    // Call the preprocessor
    SubProcess preprocessor(
	std::move(Gyoji::owned_new<SubProcessReaderFile>(preprocessed_fd)),
	std::move(Gyoji::owned_new<SubProcessReaderFile>(STDERR_FILENO)),
	std::move(Gyoji::owned_new<SubProcessWriterEmpty>())
	);

    std::vector<std::string> arguments;
    std::map<std::string, std::string> environment;

    // Treat input as 'c'
    // otherwise the .g suffix
    // will imply that it's something for the linker.
    arguments.push_back("--language");
    arguments.push_back("c");

    // Tell it we want to preprocess the file.
    arguments.push_back("-E");

    // TODO: wire this up to our command-line so
    // we can specify include dirs directly.
    for (const auto & include_dir : options->get_include_directories()) {
	arguments.push_back("-I");
	arguments.push_back(include_dir);
    }

    arguments.push_back(input_filename);
    
    int rc = preprocessor.invoke(
	"clang",
	arguments,
	environment
	);
    if (rc != 0) {
	fprintf(stderr, "Preprocessor failed\n");
	return rc;
    }
	
    
//    int input = open(input_filename.c_str(), O_RDONLY);
    int input = open(preprocessed_filename.c_str(), O_RDONLY);
    if (input == -1) {
	fprintf(stderr, "Cannot open file %s\n", input_filename.c_str());
	return -1;
    }
    
    CompilerContext context(input_filename);
    Gyoji::misc::InputSourceFile input_source(input);
    
    Gyoji::owned<MIR> mir =
	Parser::parse_to_mir(
	    context,
	    input_source,
	    options->get_verbose()
	    );
    close(input);

    // Remove the preprocessor temporary file.
    unlink(preprocessed_filename.c_str());

    // Dump our MIR
    // for debugging/review purposes
    // before any analysis or code-generation
    // passes.
    if (options->get_output_mir()) {
	std::string mir_filename = output_filename + std::string(".mir");
	FILE *mir_output = fopen(mir_filename.c_str(), "w");
	mir->dump(mir_output);
	fclose(mir_output);
    }

    // If we had errors at the MIR construction
    // phase, it is likely we'll have an unsuitable
    // MIR for analysis, so don't bother.
    if (context.has_errors()) {
	context.get_errors().print();
	return -1;
    }

    // Make sure that all types that are used in functions
    // actually have 'complete' definitions.
    std::vector<Gyoji::owned<AnalysisPass>> analysis_passes;
    
    analysis_passes.push_back(Gyoji::owned_new<AnalysisPassTypeResolution>(context));
    analysis_passes.push_back(Gyoji::owned_new<AnalysisPassUnreachable>(context));
    analysis_passes.push_back(Gyoji::owned_new<AnalysisPassReturnValues>(context));
    analysis_passes.push_back(Gyoji::owned_new<AnalysisPassUseBeforeAssignment>(context));
    analysis_passes.push_back(Gyoji::owned_new<AnalysisPassBorrowChecker>(context));

    for (const auto & analysis_pass : analysis_passes) {
	if (options->get_verbose()) {
	    fprintf(stderr, "============================\n");
	    fprintf(stderr, "Analysis pass %s\n", analysis_pass->get_name().c_str());
	    fprintf(stderr, "============================\n");
	}
	analysis_pass->check(*mir);
    }

    if (context.has_errors()) {
	context.get_errors().print();
	return -1;
    }

    if (options->get_verbose()) {
	fprintf(stderr, "============================\n");
	fprintf(stderr, "Code Generation Pass\n");
	fprintf(stderr, "============================\n");
    }
    // This leaks memory. The code-generation
    // stage is a bit problematic
    // because we're not really cleaning up the
    // LLVM stuff at the moment.

    CodeGeneratorLLVMOptions llvm_options;
    llvm_options.set_output_llvm_ir(options->get_output_llvm_ir());
    llvm_options.set_output_filename(output_filename);
    llvm_options.set_optimization_level(options->get_optimization_level());
    llvm_options.set_verbose(options->get_verbose());
    
    generate_code(context, *mir, llvm_options);
    
    if (context.has_errors()) {
	context.get_errors().print();
	return -1;
    }

    /**
     * TODO: Invoke the linker.
     */
    
    return 0;
}
