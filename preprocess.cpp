//Author - Cameron Liddell
//Purpose - To build and optimise a .C preprocessor

/*
░░░░█████╗░  ██████╗░██████╗░███████╗██████╗░██████╗░░█████╗░░█████╗░███████╗░██████╗░██████╗░█████╗░██████╗░
░░░██╔══██╗  ██╔══██╗██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔══██╗██╔══██╗██╔════╝██╔════╝██╔════╝██╔══██╗██╔══██╗
░░░██║░░╚═╝  ██████╔╝██████╔╝█████╗░░██████╔╝██████╔╝██║░░██║██║░░╚═╝█████╗░░╚█████╗░╚█████╗░██║░░██║██████╔╝
░░░██║░░██╗  ██╔═══╝░██╔══██╗██╔══╝░░██╔═══╝░██╔══██╗██║░░██║██║░░██╗██╔══╝░░░╚═══██╗░╚═══██╗██║░░██║██╔══██╗
██╗╚█████╔╝  ██║░░░░░██║░░██║███████╗██║░░░░░██║░░██║╚█████╔╝╚█████╔╝███████╗██████╔╝██████╔╝╚█████╔╝██║░░██║
╚═╝░╚════╝░  ╚═╝░░░░░╚═╝░░╚═╝╚══════╝╚═╝░░░░░╚═╝░░╚═╝░╚════╝░░╚════╝░╚══════╝╚═════╝░╚═════╝░░╚════╝░╚═╝░░╚═╝
*/

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

class pp{

    public:
        
        string inputFile;
        ofstream outputFile;
        string fileDirectory =  inputFile.substr(0, (inputFile.find_last_of("\\") + 1));

        int removeComments(string path){ // Removes comments from the codes and accepts the input file path as an argument
            
            string tempLine;
            int commentStart;

            filesystem::path fspath = path; // Stores argv[2] (the input file path)
            fspath.replace_extension(".o");

            ifstream iFile("constants_"); //Opens inputfile
            outputFile.open(fspath); //Creates outputfile

            while (getline(iFile, tempLine)){ //Finding comments

                if (tempLine[0] == '/' && tempLine[1] == '/' && tempLine[2] == ' '){ // Checks if line starts with "// " aka comments
                    continue;

                } else if ((commentStart = tempLine.find("// ")) != string::npos){ // Checks if the line has an inline comment
                    outputFile << tempLine.substr(0, commentStart) << "\n";

                } else {
                    outputFile << tempLine << "\n"; // Writes to the output file

                }
            }

            outputFile.close(); // Closes and saves edits to output file
            return 0;
        }

        int replaceHeaders(string headerFilePath){ // Replaces headers with their file contents and accepts each header file path as a parameter

            ifstream headerFile; 
            string tempLine;
            string importLine;
            bool foundHeaders;

            ifstream iFile(headerFilePath);

            //cout << "\nScanning file\n"; //Debug purposes 

            while (getline(iFile, tempLine)){

                outputFile << tempLine << "\n"; //Writes to the output file
                //cout << "SourceFile: " << tempLine << "\n"; // Debug purposes

                outputFile.open("headers_", ios_base::app); //Creates outputfile
                if ((tempLine.find("#include \"") != string::npos)){
                    //cout << "\nFound header\n" + tempLine + "\n"; //Debug purposes 
                    foundHeaders = true;

                    headerFilePath = fileDirectory + tempLine.substr(10, (tempLine.find_last_of("\"")) - 10); // Locates the header file path

                    replaceHeaders(headerFilePath);

                }   else {             
                    outputFile << tempLine << "\n"; //Writes to the output file
                    //cout << "SourceFile: " << tempLine << "\n"; // Debug purposes
                     
                }
                outputFile.close();
            }
            return 0;
        }

        vector<string> splitString(string tempLine, string delimiter = " "){ // Splits a line up into tokenized strings separated by a delimiter. 
                                                                            //Accepts an input line and a delimeter as a parameter
            int start = 0;
            int end = tempLine.find(delimiter);
            vector<string> token;

            while (end != -1) {
                //cout << tempLine.substr(start, end - start) << endl; // Debug purposes
                token.push_back(tempLine.substr(start, end - start)); // Pushes each word onto the back of the vector
                start = end + delimiter.size();
                end = tempLine.find(delimiter, start);
            }
            
            //cout << tempLine.substr(start, end - start); // Debug purposes
            token.push_back(tempLine.substr(start, end - start));
            return token;

        }

        int removeConstants(){ // Replaces all constants in the file with their corresponding values

            string tempLine;
            string pendingLine;
            string constantValue;
            map<string, string> defineDict;
            vector<string> token;
            int tokenCounter;
            bool foundConstant = false;

            ifstream iFile("conditional_");
            outputFile.open("constants_");

            while (getline(iFile, tempLine)){
                token = splitString(tempLine, " ");

                if (tempLine.find("#define ") != string::npos){ // Searches for "#define" within line
                    defineDict.insert({token[1], token[2]});                 

                } else if (any_of(tempLine.begin(), tempLine.end(), ::isupper)){ 
                    tokenCounter = 0;
                    pendingLine = "";

                    for(string i: token){
                        tokenCounter += 1;
                        
                        if (defineDict.find(i) != defineDict.end()){
                            //cout << i << "\n"; // Debug purposes
                            //cout << defineDict[i] << "\n"; // Debug purposes
                            constantValue = defineDict[i];
                            foundConstant = true;
                            pendingLine.append(constantValue + " ");
                            
                        } else {
                            pendingLine.append(i + " ");

                        }
                    } 
                    pendingLine.append("\n");

                    if (!foundConstant){
                        //cout << tempLine << "\n"; // Debug purposes
                        outputFile << tempLine << "\n";
                            
                    } else if (foundConstant){
                        outputFile << pendingLine;

                    } 
                } else{
                    outputFile << tempLine << "\n";

                }
            }
            outputFile.close();
            return 0;

        }

        int conditionalCompilation(int argc, char **argv){ // Checks if a constant has been specified as an argument and outputs the corresponding lines to the file
                                                            // Accepts the number of arguments and the character array of arguments as parameters
            string tempLine;
            string constant;
            vector<string> constants(argv, argv + argc);
            vector<string> token;
            bool found = false;

            ifstream iFile("headers_");
            outputFile.open("conditional_");

            while (getline(iFile, tempLine)){
                token = splitString(tempLine, " ");

                if (found && tempLine.find("#else")){
                    getline(iFile, tempLine);
                    found = false;

                } else if (found && tempLine.find("#endif")){
                    getline(iFile, tempLine);
                    found = false;

                } else if (!found && tempLine.find("#ifdef ") != string::npos){ // Searches for "#ifdef" within line
                    getline(iFile, tempLine);
                    
                    for (auto i : token){
                        if (any_of(i.begin(), i.end(), ::isupper)){
                            constant = i; // Assigns the consant after "ifdef" to a variable
                            //cout << "Constant is: " << constant << "\n"; // Debug purposes
                        }
                    }
                    if (count(constants.begin(), constants.end(), constant)){
                        //cout << "got in here" << "\n"; // Debug purposes
                        outputFile << tempLine << "\n";
                        found = true;
                        
                    } 
                    //cout << tempLine << "\n"; // Debug purposes
                } else {
                    //cout << "first else " << tempLine << "\n";
                    if (tempLine.find("#else") == string::npos && tempLine.find("#endif") == string::npos){
                        outputFile << tempLine << "\n";
                    }
                    
                }
            }
            outputFile.close();
            return 0;
        }
};

int errorHandler(int code){ // Used to handle error messages
        
    if (code == 0){
        cout << "\033[1;31mERROR:\033[0m\n";
        cout << "No or invalid argument provided!\n";
        cout << "Usage:\nFirst argument:\n    -i The name of the .c input file to preprocess\nSecond argument:\n    -d Defines constant name for conditional compilation\n";
    
    } else if (code == 1){
        cout << "\033[1;31mERROR:\033[0m\n";
        cout << "File Does Not Exist\n";

    } else if (code == 2){
        cout << "\033[1;31mERROR:\033[0m\n";
        cout << "File does not have extension .c\n";

    }
    return code;    
}

void running(int argc, char **argv){ // Used to handle the running order of functions

    //cout << argv[2] << "\n"; // Debug purposes
    pp obj;
    obj.inputFile = argv[2];
  
    if (argv[3] != NULL && argv[3][1] == 'd'){
        

    } else if (argv[3] != NULL && argv[3][1] != 'd'){
        errorHandler(0);

    }

    obj.replaceHeaders(argv[2]);
    obj.conditionalCompilation(argc, argv);
    obj.removeConstants();
    obj.removeComments(argv[2]);

    remove("headers_");
    remove("conditional_");
    remove("constants_");
}

int main(int argc, char **argv){ // Main function - Handles the command line input

    cout <<"\n"; // To neaten up the output to console 

    if (argv[1] != NULL && argv[1][0] != '-'){
        errorHandler(0);

    } else if (argv[1] != NULL && argv[1][0] == '-'){
        if (argv[1][1] == 'i'){
            
            if (argv[2] == NULL){
                errorHandler(1);

            } else if (filesystem::exists(argv[2])){ // Checks if the file exists
                if (filesystem::path(argv[2]).extension() == ".c"){
                    cout << "File " << argv[2] << " has been opened successfully\n";
                    running(argc, argv);
                    return 0;
                    
                } else{
                    errorHandler(2);

                }
            } else {
                errorHandler(1);
                
            }
        } else {
            errorHandler(0);

        }
    } else{
        errorHandler(0);
        
    }  
}