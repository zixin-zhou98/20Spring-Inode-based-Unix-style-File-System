#include<iostream>
#include<bitset>
#include<cstdlib>
#include"FileSystem.h"
using namespace std;

int main()
{
	FileSystem sys;
	sys.init();
	cout << "			 ����� 201730601080" << endl;
	cout << "			 ������ 201730613496" << endl;
	cout << "			 ����� 201730600489" << endl;
	cout << "--------------------------v5: Copyright reserved--------------------------" << endl;
	cout << "Helper:" << endl;
	cout << "create file: createFile filename filesize (most support nested directory)" << endl;
	cout << "delete file: deleteFile filename" << endl;
	cout << "show file content: cat filename" << endl;
	cout << "copy file: cp original_file_name new_file_name" << endl;
	cout << "create directory: createDir /dirname" << endl;
	cout<< "delete directory: deleteDir /dirname" << endl;
	cout << "change directory: cd /dirname" << endl;
	cout << "show usage: sum" << endl;
	cout << "exit and store: exit" << endl;
	cout << "load stored information: reload" << endl;
	cout << "--------------------------------Input--------------------------------" << endl;

	char line[1024];
	char cmd[1024];
	char arg1[1024] ;
	char arg2[1024];
	int args;
	
	while (true)
	{
		sys.printDir();
		if (!fgets(line, sizeof(line), stdin)) break;
		if (line[0] == '\n') 
			continue;
		line[strlen(line) - 1] = 0;
		args = sscanf_s(line, "%s %s %s", cmd,1024, arg1, 1024,arg2,1024);
		if (args == 0) 
			continue;
		if (!strcmp(cmd, "createFile"))
		{
			if (args == 3)
			{
				float f_size = atof(arg2);
				if (f_size == 0)
				{
					cout << "Invalid file size!" << endl;
					continue;
				}
				if (f_size > MAX_FILE_SIZE)
				{
					cout << "File size should be less than 351 KB!" << endl;
					continue;
				}

				if (arg1[0] != '/')
				{

					int inum = sys.getInodeFromName(sys.currDir.i_number,arg1);
					if (inum < 0)
					{
						//changed
						sys.file_create(arg1, sys.currDir.i_number, f_size);
					}
					else
					{
						cout << "File already exists!"<<endl;
						continue;
					}
				}
				//First change directory to the 
				//Notice difference between file and directory
				else
				{
					//need to find directory first
					char *token;
					char *next_token = NULL;
					char *temp = NULL;
					char *tempname = NULL;
					char arg1temp[1024];
					memcpy(arg1temp, arg1, 1024);
					char arg1temp_2[1024];
					memcpy(arg1temp_2, arg1, 1024);
					token = strtok_s(arg1, "/", &next_token);
					tempname = next_token;
					if (next_token == NULL)
					{
						cout << "Please enter right name of fine!" << endl;
						continue;
					}
					else
					{//find the last directory
						while (next_token[0]!= '\0')
						{
							temp = token;
							tempname = next_token;
							token = strtok_s(NULL, "/", &next_token);
							
						}
						int parent = sys.getParentDir(sys.currDir.i_number, arg1temp);
						
						int num = sys.getInodeFromName(sys.currDir.i_number, arg1temp_2);
						if (num > 0)
						{
							cout << "File already exists!" << endl;
							continue;
						}
						else {
								sys.file_create(token,parent, f_size);
							
						}
					}
					
				}
			}
			else
			{
				cout << "Please enter name and file size!" << endl;
			}
	
		}
		if (!strcmp(cmd, "deleteFile"))
		{
			if (args == 2)
			{
				if (arg1[0] != '/')
				{
					int inum = sys.checkExistence(arg1,sys.currDir.i_number);
					if (inum > 0)
					{
						if (sys.getMode(inum) == 0)
						{
							sys.file_delete(inum);
						}
					}

				}
				else
				{
					//need to find directory first
					char *token;
					char *next_token = NULL;
					char *temp = NULL;
					char arg1temp[1024];
					memcpy(arg1temp, arg1, 1024);
					char arg1temp_2[1024];
					memcpy(arg1temp_2, arg1, 1024);
					token = strtok_s(arg1, "/", &next_token);
					if (next_token == NULL)
					{
						cout << "Please enter right name of fine!" << endl;
						continue;
					}
					else
					{
						//find the last directory
						while (next_token[0]!= '\0')
						{
							temp = token;
							token = strtok_s(arg1, "/", &next_token);

						}
						int num = sys.getInodeFromName(sys.currDir.i_number, arg1temp_2);
						if (num < 0)
						{
							cout << "File does not exist!" << endl;
							continue;
						}
						else {
							sys.file_delete(num);
						}
					}
				}
			}
			else
			{
				cout << "Invalid command!" << endl;
				continue;
			}
		}
		if (!strcmp(cmd, "createDir"))
		{
			//search for directory, check whether exist
			int inumber = 0;
			if (args == 2)
			{
				sys.dir_create_recursive(arg1, sys.currDir.i_number);
			}
			else
			{
				cout << "Invalid command!" << endl;
				continue;
			}
		}
		if (!strcmp(cmd, "deleteDir"))
		{
			//search for directory, check whether exist
			int inum = 0;
			if (args == 2)
			{
				if (arg1[0] != '/')
				{
					cout << "Must enter a valid directory" << endl;
					continue;
				}
				else //is in directory format, check for existence
				{
					char *token=NULL;
					char *next_token = NULL;
					char arg1temp[1024];
					memcpy(arg1temp, arg1, 1024);
					token = strtok_s(arg1, "/", &next_token);
					while (next_token[0]!= '\0')
					{
						token = strtok_s(arg1, "/", &next_token);
					}
					if (!strcmp(token, "root"))
					{
						cout << "Unable to delete root directory!" << endl;
						continue;
					}
					int num = sys.getInodeFromName(sys.currDir.i_number, arg1temp);
					if (num < 0) 
					{
						cout << "No such directory!" << endl;
						continue;
					}
					

					else {
						if (sys.getMode(num) == 1)//is a directory
						{
							sys.dir_delete(num);
						}
					}

				}
					
				}
			else
			{
				cout << "Invalid command!" << endl;
				continue;
			}
		}
		if (!strcmp(cmd, "dir"))
		{
			if (args == 1)
			{
				sys.ls(sys.currDir.i_number);
			}
			else {
				continue;
			}
		}
		if (!strcmp(cmd, "cd"))
		{
			
			if (args == 2)
			{
				if (arg1[0] == '.'&&arg1[1] == '.')
				{
					if (sys.currDir.i_number == 0)
					{
						cout << "At root directory!" << endl;
						continue;
					}
					else
					{
						int newnum = sys.getParent(sys.currDir.i_number);
						sys.dirstring.pop_back();
						sys.changeDir(newnum);
					}
					
				}
				else if (arg1[0] != '/')
				{
					cout << "Must enter a valid directory";
					continue;
				}
				else
				{
					char arg1temp[1024];
					memcpy(arg1temp,arg1,1024);
					
					//found last token
					int num = sys.getInodeFromName(sys.currDir.i_number, arg1temp);
					if (num == sys.currDir.i_number)
					{
						cout << "Already in this directory!" << endl;
						continue;
					}
					if (num > 0 && sys.getMode(num) == 1)//exists and is directory
					{
						char* token = NULL;
						char* next_token = NULL;
						token = strtok_s(arg1, "/", &next_token);
						while (token!=NULL)
						{
							sys.dirstring.push_back(token);
							token = strtok_s(NULL, "/", &next_token);
						}

						sys.changeDir(num);
					}
					else
					{
						cout << "Please enter a valid directory!" << endl;
						continue;
					}
				}
			}
			else
			{
				cout << "Invalid command!" << endl;
				continue;
			}
		}
		if (!strcmp(cmd, "sum"))
		{
			if (args == 1)
				sys.sum();
			else
			{
				cout << "Invalid command!" << endl;
				continue;
			}
		}
		if (!strcmp(cmd, "cat"))
		{
			if (args == 2)
			{
				char arg1temp[1024];
				memcpy(arg1temp, arg1, 1024);
				char *token;
				char *next_token = NULL;
				char *temp = NULL;
				token = strtok_s(arg1, "/", &next_token);
				temp = token;
				while (token != NULL)
				{
					temp = token;
					token = strtok_s(NULL, "/", &next_token);
				}
			
	//found last token
				int num = sys.getInodeFromName(sys.currDir.i_number, arg1temp);
				if (num > 0 && sys.getMode(num) == 0)//exists and is file
				{
					sys.readContent(num);
					sys.printContent();
				}
				else
				{
					cout << "Please enter a valid file name!" << endl;
					continue;
				}
			}
			else
			{
				cout << "Invalid command!" << endl;
				continue;
			}
		}
		if (!strcmp(cmd, "cp"))
		{
			if (args == 3)
			{
				char arg2temp[1024];
				memcpy(arg2temp, arg2, 1024);
				int num2 = sys.getInodeFromName(sys.currDir.i_number, arg2temp);
				int num = sys.getInodeFromName(sys.currDir.i_number,arg1);
				
				if (num2 > 0)
				{
					cout << "Duplicated file name!" << endl;
					continue;
				}
				if (num > 0&&sys.getMode(num)==0)//exists and is file
				{
					sys.copy(arg2, num);
				}
				
			}
			else
			{
				cout << "Invalid command!" << endl;
				continue;
			}
		}
		if (!strcmp(cmd, "exit"))
		{
			if (args == 1)
			{
				sys.writeToDisk();
				return 0;
			}
			else
			{
				cout << "Invalid command!" << endl;
				continue;
			}
		}
		if (!strcmp(cmd, "reload"))
		{
			if (args == 1)
			{
				sys.readFromDisk();
			}
			else
			{
				cout << "Invalid command!" << endl;
				continue;
			}
		}
	}
	system("pause");
}