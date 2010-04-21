#include "Assembler.h"

Assembler::Assembler()
//opcode map
{
	opcode["load"] = 0;	
	opcode["loadi"] = 0;    
	opcode["store"] = 1;	
	opcode["add"] = 2;	
	opcode["addi"] = 2;	
	opcode["addc"] = 3;	
	opcode["addci"] = 3;	
	opcode["sub"] = 4;	
	opcode["subi"] = 4;	
	opcode["subc"] = 5;	
	opcode["subci"] = 5;	
	opcode["and"] = 6;	
	opcode["andi"] = 6;	
	opcode["xor"] = 7;      
	opcode["xori"] = 7;     
	opcode["compl"] = 8;   	
	opcode["shl"] = 9;     	
	opcode["shla"] = 10;    
	opcode["shr"] = 11;	
	opcode["shra"] = 12;	
	opcode["compr"] = 13;  	
	opcode["compri"] = 13;	
	opcode["getstat"] = 14;
	opcode["putstat"] = 15;
	opcode["jump"] = 16;
	opcode["jumpl"] = 17;
	opcode["jumpe"] = 18;
	opcode["jumpg"] = 19;
	opcode["call"] = 20;
	opcode["return"] = 21;
	opcode["read"] = 22;
	opcode["write"] = 23;
	opcode["halt"] = 24;
	opcode["noop"] = 25;  
}

void Assembler::assemble(string file)
{
	assemblyFile.open(file.c_str(), ios::in);
	
	//Check if file exists
	if (!assemblyFile)
	{
		cout << 	"Error opening file: " << file << endl;
		exit(1);
	}
	cout << "assembler started for " << file << endl;	
	//Remove file extension and add new extension
	outputName.assign(file,0,file.length()-2);
	outputName += ".o";
	cout << outputName << " created" << endl;
	objectFile.open(outputName.c_str(),ios::out);
	
	getline(assemblyFile, line);
	cout << "reading instructions" << endl;
    while(!assemblyFile.eof()){
        int rd=-1, rs=-1; //initialize to invalid values

        istringstream str(line.c_str());
        str >> op >> rd >> rs;
		
		//skip all comments and blank lines
		if (line[0] == '!' || line.empty())
		{
			goto next;
		}
		
		//handles opcodes which have no parameters		
		else if(rd < 0 && rs < 0)
		{
			instrNum = opcode[op] << 11;
		}


		//evaluate parameters for immediate opcodes
		else if ( op == "loadi" || op == "addci" || op == "subi" || op == "subci" || op == "addi" || 
			op == "xori" || op == "compri" || op == "andi" )
		{
						// 0 <= RD < 4 and 0 =< CONST (RT) < 128
                        if ( rd >= 0 && rd < 4 && rs >= 0 && rs < 128 )
                        {
                                instrNum = opcode[op] << 11;
                                instrNum += rd << 9;
                                instrNum += 1 << 8;  //set I bit = 1
                                instrNum += rs;
                        }
						// 0 =< RD < 4 and -128 =< CONST (RT) < 0
                        else if ( rd >= 0 && rd < 4 && rs < 0 && rs >= -128 )
                        {
                                instrNum = opcode[op] << 11;
                                instrNum += rd << 9;
                                instrNum += 1 << 9; // set I bit = 1
                                instrNum += rs;
                        }
		}
		//sets instruction for reference opcodes
		else if ( rd >= 0 && rd < 256 &&		// 0 =< ADDR < 256 -- ADDR is valid
				 (op == "call" || op == "jumpg" || op == "jumpe" || op == "jumpl" || op == "jump") )
                {        
			instrNum = (opcode[op] << 11) + rd;
		}

		else if ( rd >= 0 && rd < 4 &&  // RD is a valid register
				(op == "compl" || op == "shl" || op == "shla" || op == "shr" || op == "shra" ||
				 op == "getstat" || op == "putstat" || op == "read" || op == "write" ))
                {
		  	instrNum = opcode[op] << 11;
           	instrNum += rd << 9;
		}

		else if ( rd >= 0 && rd < 4 && //RD is a valid register
				 rs >= 0 && rs < 256 && // 0 =< ADDR < 256 -- ADDR is valid
				 (op == "load" || op == "store") )
                {
			instrNum = opcode[op] << 11;
            		instrNum += rd << 9;
            		instrNum += rs;
		}

		else if ( rd >= 0 && rd < 4 && rs >= 0 && rs < 4 && // RD and RS are valid registers
				 (op == "add" || op == "addc" || op == "sub" || op == "subc" || op == "and" || 
				 op == "xor" || op == "compr"))
		{
       	        	instrNum = opcode[op] << 11;
        	        instrNum += rd << 9;
                	instrNum += rs << 6;
		}
		

                //evaluate failed instructions
		else
		{
                        cout << "Failed to assemble the following instruction:   ";
						if (rd != -1 && rs != -11)
                                 cout << op << " " << rd << " " << rs << endl;
                        else
                                cout << op << endl;
                        if(rd != -1 && rs == -1)
							cout << op << " " << rd << endl;

                        cout << "Assembler is now exiting!\n";
                        exit(2);
                }
		
        objectFile << instrNum << endl; //writes the instruction number to our .o file
		next: //reference point for skipping lines
        getline(assemblyFile, line);

	}
	cout << "instructions assembled, continuing to VM" << endl;
	assemblyFile.close();
	objectFile.close();
}
