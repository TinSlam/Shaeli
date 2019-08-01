#include "Utils.h"

extern void parseFile(std::string fileName);

namespace Shaeli{
	extern std::string shaeliOutputDirectory;
	extern std::map<std::string, bool> referencedFiles;
	
	void closeParentheses(){
		fprintf(output, ")");
	}
	
	void openCurlyBracket(){
		fprintf(output, "{\n");
	}
	
	void closeCurlyBracket(){
		fprintf(output, "}\n\n");
	}
	
	void addSemiColon(){
		fprintf(output, ";\n");
	}
	
	void setOutputFile(FILE* outputFile){
		output = outputFile;
	}
	
	void referenceFile(std::string fileName){
		if(referencedFiles[fileName]) return;
		parseFile(fileName);
		referencedFiles[fileName] = true;
	}
	
	void generatePrecode(){
		fopen_s(&headersFile, (shaeliOutputDirectory + "/ShaeliReferences.h").c_str(), "w");
		fopen_s(&functionHeadersFiles, (shaeliOutputDirectory + "/FunctionHeaders.h").c_str(), "w");
		fprintf(output, "#include <stdio.h>\n#include \"ShaeliReferences.h\"\n\n");
	}
	
	void generateMainFunction(){
		fprintf(output, "int main(){\n");
	}

	void wrapUpFile(){
		fprintf(headersFile, "#include \"FunctionHeaders.h\"");
		fclose(headersFile);
		fclose(functionHeadersFiles);
		fprintf(output, "\t_main();\n\treturn 0;\n}\n");
	}
}