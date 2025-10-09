#pragma once

#include <gyoji-misc/pointers.hpp>
#include <gyoji-mir.hpp>

namespace Gyoji::codegen {
    class CodeGeneratorLLVMContext;
    
    /**
     * This is the public interface to the code generator.
     * It is careful not to pollute the namespace
     * by requiring inclusion of LLVM headers
     * so that the caller need know nothing at all
     * about how the code is actually generated.
     */
    class CodeGeneratorLLVM {
    public:
	CodeGeneratorLLVM(
	    const Gyoji::context::CompilerContext & _compiler_context,
	    const Gyoji::mir::MIR & _mir
	    );
	~CodeGeneratorLLVM();
	void initialize();
	void generate();
	int output(const std::string & filename);
    private:
	Gyoji::owned<CodeGeneratorLLVMContext> context;
	
    };
    
    void generate_code(const Gyoji::context::CompilerContext & _context, const Gyoji::mir::MIR & _mir, const std::string & _filename);
};
