#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <optional>
#include <memory>
#include <stdexcept>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#endif /* __PROGTEST__ */

struct CObjectFile { // Holds information about .o files
    std::string fileName; 
    std::map<std::string, uint32_t> exports; // All exports from .o file

    std::map<std::string, std::vector<uint32_t>> imports; // All imports from .o file

    std::vector<uint8_t> compiledCode; // Rest of the code with their definitions
    uint32_t bodySize; // Length of the rest of the code
};

class CLinker { // Main class
    struct FunctionInfo { // Holds information about individual functions
        std::string name; 
        std::string sourceFile; // Which .o file it comes from
        uint32_t orgOffset; // Where in the definition body it is defined
        uint32_t newOffset; // Where it will be defined in the resulting code
        uint32_t size; // Length of the definition
    };

    bool loadObjectFile(const std::string& objFileName, CObjectFile& objFile) { // Helper function to load values into CObjectFile struct
        std::ifstream file(objFileName, std::ios::binary); // Read .o from folder
        if (!file) return false; // Check if .o file is empty

        // Extract header (number of exports, imports and length)
        uint32_t exportCount, importCount, bodySize; 
        file.read(reinterpret_cast<char*>(&exportCount), sizeof(exportCount));
        file.read(reinterpret_cast<char*>(&importCount), sizeof(importCount));
        file.read(reinterpret_cast<char*>(&bodySize), sizeof(bodySize));
        objFile.bodySize = bodySize;

        // Load all exports
        for (uint32_t i = 0; i < exportCount; ++i) {
            uint8_t nameLen;
            file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen)); // Read name length

            std::string functionName(nameLen, '\0'); 
            file.read(functionName.data(), nameLen); // Read name

            uint32_t offset;
            file.read(reinterpret_cast<char*>(&offset), sizeof(offset)); // Get offset
            if (!file) return false;
            
            if (objFile.exports.find(functionName) != objFile.exports.end()) // Check if duplicate already exists
                throw std::runtime_error("Found duplicate function " + functionName);
            objFile.exports[functionName] = offset; // Load into map
        }

        // Load all functions that are imported
        for (uint32_t i = 0; i < importCount; ++i) {
            uint8_t nameLen;
            file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen)); // Determine name length

            std::string functionName(nameLen, '\0');
            file.read(functionName.data(), nameLen); // Read function name

            uint32_t use_count;
            file.read(reinterpret_cast<char*>(&use_count), sizeof(use_count));  // Find out how many times the function is imported
            if (!file) return false;            

            // Load all imports of the given function
            for (uint32_t j = 0; j < use_count; ++j) {
                uint32_t offset;
                file.read(reinterpret_cast<char*>(&offset), sizeof(offset)); // Read offsets where the function is used

                if (!file) return false;
                objFile.imports[functionName].push_back(offset); // Final loading of imported function into struct
            }
        }

        // Load the rest of the definitions code body
        objFile.compiledCode.resize(bodySize); 
        file.read(reinterpret_cast<char*>(objFile.compiledCode.data()), bodySize);
        return true;     
    };

    void c_exported_f_map(std::map<std::string, FunctionInfo>& exported_functions) { // Creates one container with all exported functions across .o files
    
        for(const auto& file : files) { // Process all files
            for (const auto& [funcName, offset] : file.exports) { // Process all exports
                if (exported_functions.find(funcName) != exported_functions.end()) { // Check for duplicates across .o files
                    throw std::runtime_error("Duplicity function: " + funcName);
                }

                uint32_t size = 0;
                uint32_t nextOffset = file.bodySize;

                // Find where the function ends by looking for the next function's start
                for(const auto& [otherName, otherOffset] : file.exports) {
                    if (otherOffset > offset && otherOffset < nextOffset) 
                        nextOffset = otherOffset;
                }
                size = nextOffset - offset;

                // Add function information to the map
                exported_functions[funcName] = {
                    funcName, 
                    file.fileName,
                    offset,
                    0,
                    size
                };
            }
        }
    }

    void find_needed_functions(std::set<std::string>& neededFucntions, std::map<std::string,
                               FunctionInfo>& exported_functions,
                               const std::string& entryPoint) { // Finds all functions that will be needed
        std::queue<std::string> functionQueue; // Queue to process all dependencies on other functions

        // Start with the entry point
        functionQueue.push(entryPoint);
        neededFucntions.insert(entryPoint);

        // BFS algorithm to find all functions needed from the entry point
        while(!functionQueue.empty()) {
            std::string currentFunc = functionQueue.front();
            functionQueue.pop();

            // Check if function exists in exported functions
            if (exported_functions.find(currentFunc) == exported_functions.end())
                throw std::runtime_error("No definition was founded " + currentFunc);
            const FunctionInfo& funcInfo = exported_functions.at(currentFunc);

            // Find the source file for this function
            for (const auto& file : files) {
                if (file.fileName != funcInfo.sourceFile) continue;
                uint32_t funcStart = funcInfo.orgOffset; // Prepare variables for better orientation
                uint32_t funcEnd = funcStart + funcInfo.size;

                // Check all imports in this file
                for (const auto& [importName, importOffsets] : file.imports) {
                    for (const auto& importOffset : importOffsets) {
                        // Check if this import belongs to the current function
                        if (importOffset >= funcStart && importOffset < funcEnd) {
                            // Verify that the imported function exists
                            if (exported_functions.find(importName) == exported_functions.end())
                                throw std::runtime_error("Undefined symbol (export for import missing) + importName");
                            
                            // Add to the needed functions if not already present
                            if (neededFucntions.find(importName) == neededFucntions.end()) {
                                neededFucntions.insert(importName);
                                functionQueue.push(importName);
                            }
                        }
                    }
                }
            }
        }
    }

    void create_output_file(const std::string& fileName, 
                           uint32_t currentOffset,
                           const std::vector<FunctionInfo*>& orderedFunctions,
                           const std::map<std::string, FunctionInfo>& exported_functions) {
        // Prepare data for the output file
        std::vector<uint8_t> outputData(currentOffset);
        
        // For each function in the output order
        for (const FunctionInfo* func : orderedFunctions) {
            // Find the source file
            for (const auto& file : files) {
                if (file.fileName == func->sourceFile) {
                    // Copy the function body
                    std::memcpy(
                        outputData.data() + func->newOffset,
                        file.compiledCode.data() + func->orgOffset,
                        func->size
                    );
                    
                    // Process all imports in this file
                    for (const auto& [importName, importOffsets] : file.imports) {
                        // Check if any import falls within the range of the current function
                        for (uint32_t offset : importOffsets) {
                            if (offset >= func->orgOffset && offset < func->orgOffset + func->size) {
                                // Replace the address in the output file
                                uint32_t newAddr = exported_functions.at(importName).newOffset;
                                uint32_t outputOffset = func->newOffset + (offset - func->orgOffset);
                                
                                // Write the new address (little-endian)
                                std::memcpy(
                                    outputData.data() + outputOffset,
                                    &newAddr,
                                    sizeof(uint32_t)
                                );
                            }
                        }
                    }
                    break;
                }
            }
        }
        
        // Write the result to file
        std::ofstream outFile(fileName, std::ios::binary);
        if (!outFile) {
            throw std::runtime_error("Cannot create output file: " + fileName);
        }
        
        outFile.write(reinterpret_cast<const char*>(outputData.data()), outputData.size());
        if (!outFile) {
            throw std::runtime_error("Failed to write output file: " + fileName);
        }
    }

  public:

    CLinker() = default;  // Explicitly allow default constructor
    ~CLinker() = default;  // Explicit default destructor
    
    CLinker(const CLinker&) = delete;  // Disable copy constructor
    CLinker& operator=(const CLinker&) = delete;  // Disable assignment operator

    CLinker & addFile(const std::string & fileName) { // Main method for loading .o files
        CObjectFile objFile;
        objFile.fileName = fileName;
        // Try to load the object file, throw exception if it fails
        if (!loadObjectFile(fileName, objFile)) {
            throw std::runtime_error("Failed to load object file: " + fileName);
        }

        files.push_back(objFile);
        return *this;
    }


void linkOutput(const std::string& fileName, const std::string& entryPoint) {
    // Check if any files were added
    if (files.empty()) {
        throw std::runtime_error("No files were added");
    }
    // Create a map of all exported functions
    std::map<std::string, FunctionInfo> exported_functions;
    c_exported_f_map(exported_functions);

    // Verify that the entry point exists
    if (exported_functions.find(entryPoint) == exported_functions.end()) {
        throw std::runtime_error("Undefined entryPoint "+ entryPoint);
    }
    
    // Find all functions needed from the entry point
    std::set<std::string> neededFunctions;
    find_needed_functions(neededFunctions, exported_functions, entryPoint);

    // Create an ordered list of functions for the output
    std::vector<FunctionInfo*> orderedFunctions;
    uint32_t currentOffset = 0;
    
    // Place the entry point function first
    FunctionInfo* entryPointFunc = &exported_functions[entryPoint];
    entryPointFunc->newOffset = currentOffset;  // Entry point starts at offset 0
    currentOffset += entryPointFunc->size;  // Now increment the offset
    orderedFunctions.push_back(entryPointFunc);

    // Add all other needed functions
    for (const auto& funcName : neededFunctions) {
        if (funcName != entryPoint) {  // Skip the entry point which is already added
            FunctionInfo* func = &exported_functions[funcName];
            func->newOffset = currentOffset;
            currentOffset += func->size;
            orderedFunctions.push_back(func);
        }
    }

    // Create the output file
    create_output_file(fileName, currentOffset, orderedFunctions, exported_functions);
}

void printWhole() const { // Prints first file for debug
    // Print filename
    std::cout << "File: " << files[0].fileName << std::endl;

    // Print exports
    std::cout << "Exports:" << std::endl;
    for (const auto& [name, offset] : files[0].exports) {
        std::cout << "  " << name << " @ 0x" << std::hex << offset << std::dec << std::endl;
    }

    // Print imports
    std::cout << "Imports:" << std::endl;
    for (const auto& [name, offsets] : files[0].imports) {
        std::cout << "  " << name << " used at:";
        for (uint32_t off : offsets) {
            std::cout << " 0x" << std::hex << off << std::dec;
        }
        std::cout << std::endl;
    }

    // Print compiled code (as hex dump)
    std::cout << "Compiled Code (" << files[0].bodySize << " bytes):" << std::endl;
    for (size_t i = 0; i < files[0].compiledCode.size(); ++i) {
        if (i % 16 == 0) std::cout << std::setw(4) << std::setfill('0') << i << ": ";
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)files[0].compiledCode[i] << " ";
        if (i % 16 == 15 || i + 1 == files[0].compiledCode.size()) std::cout << std::endl;
    }
}
  private:
    std::vector<CObjectFile> files;  // Collection of all loaded object files
};


#ifndef __PROGTEST__
int main ()
{
  CLinker () . addFile ( "0in0.o" ) . linkOutput ( "0out", "strlen" );

  CLinker () . addFile ( "1in0.o" ) . linkOutput ( "1out", "main" );

  CLinker () . addFile ( "2in0.o" ) . addFile ( "2in1.o" ) . linkOutput ( "2out", "main" );

  CLinker () . addFile ( "3in0.o" ) . addFile ( "3in1.o" ) . linkOutput ( "3out", "towersOfHanoi" );

  try
  {
    CLinker () . addFile ( "4in0.o" ) . addFile ( "4in1.o" ) . linkOutput ( "4out", "unusedFunc" );
    assert ( "missing an exception" == nullptr );
  }
  catch ( const std::runtime_error & e )
  {
    // e . what (): Undefined symbol qsort
  }
  catch ( ... )
  {
    assert ( "invalid exception" == nullptr );
  }

  try
  {
    CLinker () . addFile ( "5in0.o" ) . linkOutput ( "5out", "main" );
    assert ( "missing an exception" == nullptr );
  }
  catch ( const std::runtime_error & e )
  {
    // e . what (): Duplicate symbol: printf
  }
  catch ( ... )
  {
    assert ( "invalid exception" == nullptr );
  }

  try
  {
    CLinker () . addFile ( "6in0.o" ) . linkOutput ( "6out", "strlen" );
    assert ( "missing an exception" == nullptr );
  }
  catch ( const std::runtime_error & e )
  {
    // e . what (): Cannot read input file
  }
  catch ( ... )
  {
    assert ( "invalid exception" == nullptr );
  }

  try
  {
    CLinker () . addFile ( "7in0.o" ) . linkOutput ( "7out", "strlen2" );
    assert ( "missing an exception" == nullptr );
  }
  catch ( const std::runtime_error & e )
  {
    // e . what (): Undefined symbol strlen2
  }
  catch ( ... )
  {
    assert ( "invalid exception" == nullptr );
  }

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */