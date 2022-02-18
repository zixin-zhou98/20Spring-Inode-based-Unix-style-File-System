#include"FileSystem.h"
#include<iostream>
#include <math.h>
#include<time.h>
#include<string>
#include<iomanip>

using namespace std;

int FileSystem::init()
{
	p = (char*)malloc(BLOCK_NUM*1024);//Allocate memory of 16MB
	if (NULL == p)
	{
		return 0;
	}
	else
	{
		cout << "Memory allocated.\n";
		//-------------initialize superblock-------------
		sb = (Superblock*)p;
		sb->block_num = BLOCK_NUM;//1024
		sb->inode_num = INODE_NUM;//40
		for (int i = 0; i < INODE_NUM; i++)
			sb->inode_map[i] = '0';
		for (int i = 0; i < BLOCK_NUM; i++)
			sb->block_map[i] = '0';

		sb->inode_used = 0;
		sb->blk_used = INITIAL_USE_BLK;
		//initialize current directory 
		currDir.i_number = 0;
		char root[] = "root";
		memcpy(currDir.name, root, 4);
		//-------------initialize inodes----------------
		inode_list = (Inode*)(p+I_OFF);
		Inode* temp_i_list = new Inode[INODE_NUM];
		//inode_list = temp_i_list;
		memcpy(inode_list, temp_i_list, sizeof(Inode) * INODE_NUM);
		
		delete[]temp_i_list;
		dir_create(1, root, -1);
		for (int i = 0; i < INITIAL_USE_BLK; i++)
			sb->block_map[i] = '1';
		for (int i = INITIAL_USE_BLK; i < BLOCK_NUM; i++)
			sb->block_map[i] = '0';
		dirstring.push_back("root");
		return 1;
	}
	
}

int FileSystem::getParent(int inum)
{
	if (inum >= 0 && inum < INODE_NUM)
		return inode_list[inum].dir_parent;
	else
		return -1;
}
void FileSystem::util()
{
	cout << "Data field------\n";
	cout << "---------Memory: Superblock\n";
	for (int i = 0; i < 8; i++)
	{
		cout << *(p + i) << "";
	}
	cout << "---------inode map\n";
	for (int i = 8; i < 8+ INODE_NUM; i++)
	{
		cout << *(p + i) << " ";
		if (i % 50 == 0)
			cout << "\n";
	}
	cout << "---------end of inode map\n";

	cout << "---------block map\n";
	for (int i = 8+ INODE_NUM; i < 8 + INODE_NUM+ BLOCK_NUM; i++)
	{
		cout << *(p + i) << " ";
		if (i % 50 == 0)
			cout << "\n";
	}
	cout << "---------end of block map\n";
	
	for (int i = 8 + INODE_NUM + BLOCK_NUM+3; i < I_OFF; i++)
	{
		cout << *(p + i) << " ";
	}
cout << "---------finish superblock"<<endl;

	cout << "---------Inode\n";
	for (int i = I_OFF; i < B_OFF; i++)
	{
		//cout << *(p + i);
		//cout <<p[i]<<endl;
		if ((i - I_OFF+1) % sizeof(Inode) == 0)
			cout << "\n";
	}
	cout << "---------end of Inode\n";



	/**/
	cout << "---------Data Block\n";
	for (int i = 0; i < 13*1024; i++)//TODO:change field of blocks
	{
		cout << *(p +B_OFF+ i) << "";
		if ((i+1) % 1024 == 0)
			cout << "\n\n";
	}
	cout << "---------end of data block\n";

	cout << "\n";



}
int FileSystem::file_delete(int i_number)
{
	int blk_release = inode_list[i_number].i_size;
	sb->inode_used -= 1;
	sb->blk_used -= blk_release;
	sb->inode_map[i_number] = '0';
	int bound = 10;
	//delete direct
	if (inode_list[i_number].i_size <= 10)
		bound = inode_list[i_number].i_size;
	for (int i = 0; i < bound; i++)
	{
		temp_3[0] = inode_list[i_number].i_addr[i * 3];
		temp_3[1] = inode_list[i_number].i_addr[i * 3 + 1];
		temp_3[2] = inode_list[i_number].i_addr[i * 3 + 2]; 
		int block_addr = byte_to_int();
		sb->block_map[block_addr] = '0';
		memset(p + B_OFF + block_addr * 1024, 0, 1024);
	}
	if (inode_list[i_number].file_size > 10)
	{
		
		//delete indirect
		temp_3[0] = inode_list[i_number].i_addr[30];
		temp_3[1] = inode_list[i_number].i_addr[31];
		temp_3[2] = inode_list[i_number].i_addr[32];
		int block_addr = byte_to_int();
		sb->block_map[block_addr] = '0';
		sb->blk_used -= 1;
		//delete indirect
		//the block that stores addresses
		int j = 0;
		while (p[B_OFF + block_addr * 1024 + j] != NULL)
		{
			temp_3[0] = *(p + B_OFF + block_addr * 1024 + j * 3);
			temp_3[1] = *(p + B_OFF + block_addr * 1024 + j * 3 + 1);
			temp_3[2] = *(p + B_OFF + block_addr * 1024 + j * 3 + 2);
			int block_addr_2 = byte_to_int();
			sb->block_map[block_addr_2] = '0';
			memset(p + B_OFF + block_addr_2 * 1024, 0, 1024);
			j = j + 1;
		}
	}
	init_inode(i_number);
	return 0;
}
int FileSystem::checkExistence(char* name, int parent)
{
	for (int i = 0; i < INODE_NUM; i++)
	{
		if (inode_list[i].dir_parent == parent)
		{
			//same: return 
			if (!strcmp(name, inode_list[i].name))
			{
				return i;
			}
		}
	}
	return -1;
}
void FileSystem::init_inode(int i_number)
{
	memset(p + I_OFF + i_number * sizeof(Inode), 0, sizeof(Inode));
	inode_list[i_number].dir_parent = -2;
}

int FileSystem::file_create(char* name,int parent,float f_size)
 {//mode 0:file,1:dir
	if (file_num > MAX_FILE_NUM)
	{
		cout << "Reach maxinum number of files!" << endl;
		return -1;
	}
	int f_size_byte = 0;
	f_size_byte = ceil(f_size * 1024);//KB to byte
	int blk_occupy = (int)ceil((double)(f_size_byte)/1024);//f_size:in KB
	int inode_number = -1;
	int indirect_number = -1;
	int name_len = 0;
	while (*name++)
	{
		name_len++;
	}
	if (name_len > MAX_FILENAME_LEN)
		return -1;
	if (blk_occupy > MAX_FILE_SIZE)
	{
		cout << "File size should be smaller than 351KB!\n";
		return -1;
	}
	if (sb->inode_used == sb->inode_num)
	{
		cout << "No available inode!\n";
		return -1;
	}


	for (int i = 0; i < INODE_NUM; i++)
	{
		if (sb->inode_map[i] == '0')//available inode
		{
			//update inode
			inode_number = i;
			//inode_update((double)(f_size_byte) / 1024, i);//f_size is the f_size of inode
			sb->inode_map[i] = ' 1';
			sb->inode_used += 1;
			for (int k = 0; k < MAX_FILENAME_LEN; k++)
			{
				inode_list[i].name[k] = '\0';
			}
			for (int j = 0; j <= name_len; j++)
			{
				*name--;
			}
			memcpy(inode_list[i].name, name, name_len);
			inode_list[i].i_mode = 0;
			inode_list[i].i_num = i;
			inode_list[i].file_size = (double)(f_size_byte) / 1024;
			inode_list[i].i_size = blk_occupy;
			inode_list[i].dir_parent = parent;
			time_t timep;
			time(&timep);
			char tmp[64];
			struct tm nowTime;
			localtime_s(&nowTime, &timep);
			strftime(inode_list[i].time, sizeof(inode_list[i].time) - 1, "%Y-%m-%d %H:%M:%S", &nowTime);
			break;
		}
	}
	
	int count = 0;
	//generate file randomly
	srand((unsigned)time(NULL));
	char data[MAX_FILE_SIZE*1024] = {0};
	for (int i = 0; i < f_size_byte; i++)
	{
		int flag = rand() % 3;
		switch (flag)
		{
		case 0:
			data[i] = 'A' + rand() % 26;
			break;
		case 1:
			data[i] = 'a' + rand() % 26;
			break;
		case 2:
			data[i] = '0' + rand() % 10;
			break;
		}
	}
	int write_size = 1024;
	//direct address:10
	int flag = 0;//the 11th block is not used
	int flag_2 = 0;
	if (blk_occupy == 10)
		flag_2 = 1;
	for (int i = 20; i < BLOCK_NUM&&(count < blk_occupy||flag_2); i++)//if 10 blocks, we allow count==blk_occupy
	{
		//the last one
		if (count == blk_occupy-1 && f_size > 1)
			write_size = f_size_byte - count * 1024;
		if (sb->block_map[i] == '0')
		{
			if (count < 10)
			{
				int_to_byte(i);
				inode_list[inode_number].i_addr[count*3] = temp_3[0];
				inode_list[inode_number].i_addr[count*3+1] = temp_3[1];
				inode_list[inode_number].i_addr[count*3+2] = temp_3[2];
				sb->block_map[i] = '1';
				
				memcpy(p + B_OFF + i * 1024, data + count * 1024, write_size);
				count++;
			}
			else if (count == 10 && !flag&&blk_occupy!=10)
			{//the address of block that stores address of data block
				int_to_byte(i);
				inode_list[inode_number].i_addr[30] = temp_3[0];
				inode_list[inode_number].i_addr[31] = temp_3[1];
				inode_list[inode_number].i_addr[32] = temp_3[2];

				sb->block_map[i] = '1';
				flag = 1;
				indirect_number = i;
			}
			//countinue searching
			else if(blk_occupy != 10) //indirect
			{
				flag_2 = 0;
				int_to_byte(i);
				memcpy(p + B_OFF + 1024 * indirect_number+(count-10)*3, temp_3, 3);
				//write address into block
				memcpy(p + B_OFF + i * 1024, data + count * 1024, write_size);
				//write data
				sb->block_map[i] = '1';
				count++;
				
			}
		}
	}
	file_num++;
	sb->blk_used += blk_occupy;
	int indirect = 0;
	if (blk_occupy > 10)
	{
		sb->blk_used++;
		indirect = 1;
	}
	return 0;
}
void FileSystem::inode_update(int i_number,int indirect,int count)
{
	memcpy(p + I_OFF+i_number * (sizeof(Inode) - 11 * 4 + 11 * 3 - sizeof(int)*MAX_ADDR_PER_BLOCK), &inode_list[i_number].i_mode, sizeof(int));
	memcpy(p + I_OFF + i_number * (sizeof(Inode) - 11 * 4 + 11 * 3 - sizeof(int)*MAX_ADDR_PER_BLOCK) + sizeof(int), &inode_list[i_number].i_num, sizeof(int));
	for (int i = 0; i < 11; i++)
	{
		char output[40];
		int_to_byte(inode_list[i_number].i_addr[i]);
		memcpy(p + I_OFF + i_number * sizeof(Inode) + sizeof(int) * (2)+i*3, temp_3, 3);
		memset(output, 0,1);
		memcpy(output, p + i_number * sizeof(Inode) + I_OFF + sizeof(int) * (2) + i * 3, 4);
		
	}
	memcpy(p + I_OFF + i_number * sizeof(Inode) + sizeof(int) * 13, &inode_list[i_number].i_size, sizeof(int));
	memcpy(p + I_OFF + i_number * sizeof(Inode) + sizeof(int) * 14, &inode_list[i_number].file_size, sizeof(float));
	if (indirect)
	{
		for (int j = 0; j < count; j++)
		{
			//int_to_byte(inode_list[i_number].address_list[j]);
			//which block? inode_list[i_number].i_addr[10]
			//which byte? inode_list[i_number].i_addr[10]*1024
			memcpy(p + B_OFF + inode_list[i_number].i_addr[10] * 1024+j*3, temp_3, 3);
		}
	}
}
int FileSystem::dir_create(int mode, char* name,int parent)
{
	//store i_number:name pair
	int len = 0;
	int inode_number = -1;
	int blk_occupy = 1;

	while (*name++)
	{
		len++;
	}
	if (len > MAX_FILENAME_LEN)
	{

		return -1;
	}
	if (sb->inode_used == sb->inode_num)
		return -1;
	//f_size_byte = len + 3;
	
	for (int i = 0; i < INODE_NUM; i++)
	{
		if (sb->inode_map[i] == '0')//available inode
		{
			//update inode
			inode_number = i;
			//inode_update((double)(f_size_byte) / 1024, i);//f_size is the f_size of inode
			sb->inode_map[i] = ' 1';
			sb->inode_used += 1;
			time_t timep;
			time(&timep);
			char tmp[64];
			struct tm nowTime;
			localtime_s(&nowTime, &timep);
			strftime(inode_list[i].time, sizeof(inode_list[i].time) - 1, "%Y-%m-%d %H:%M:%S", &nowTime);
			for (int k = 0; k < MAX_FILENAME_LEN; k++)
			{
				inode_list[i].name[k] = '\0';
			}
			for (int j = 0; j <= len; j++)
			{
				*name--;
			}
			
			memcpy(inode_list[i].name, name, len);
			inode_list[i].i_mode = mode;
			inode_list[i].i_num = i;
			inode_list[i].file_size = 0;
			inode_list[i].i_size = blk_occupy;
			inode_list[i].dir_parent = parent;
			break;


		}
	}
	return inode_number;
}
int FileSystem::dir_create_recursive(char* name,int parent)
{
	char *token;
	char *next_token = NULL;
	token = strtok_s(name, "/", &next_token);
	if (token == NULL)
		return 0;
	int flag = 0;
	if (inode_list[parent].i_mode == 1)
	{
		flag = 1;
	}
	if (next_token == NULL)
	{
		//not exist
		int i_num = checkExistence(token, parent);
		if (i_num<0 && inode_list[parent].i_mode == 1)
		{
			dir_create(1, token, parent);
		}
		//else: do nothing
		return 0;
	}
	else//nested
	{
		int i_num = checkExistence(token, parent);
		//not exit
		if (i_num<0&&inode_list[parent].i_mode == 1)
		{
			int new_num=dir_create(1, token, parent);
			dir_create_recursive(next_token, new_num);

		}
		else
		{
			if (flag)
			{
				dir_create_recursive(next_token, i_num);
			}
		}
	}
	return 0;
}
int FileSystem::dir_delete(int i_number)
{
	for (int i = 0; i < INODE_NUM; i++)
	{
		if (inode_list[i].dir_parent == i_number)
		{
			if (inode_list[i].i_mode == 0)//file
				file_delete(i);
			else if (inode_list[i].i_mode == 1)
			{
				dir_delete(inode_list[i].i_num);
				sb->inode_map[i] = '0';
				//init_inode(i);
			}
		}
	}
	for (int i = 0; i < INODE_NUM; i++)
	{
		if (i == i_number)
		{
			sb->inode_map[i] = '0';
			sb->inode_used--;
			init_inode(i);
		}
	}
	return 0;
}
void FileSystem::ls(int i_number)
{
	
	for (int i = 0; i < INODE_NUM; i++)
	{
		if (inode_list[i].dir_parent == i_number)
		{
			if (getMode(i) == 0)//file
				cout << left << setw(15) << inode_list[i].name << setw(10) << fixed<<setprecision(2)<< inode_list[i].file_size << setw(30) << inode_list[i].time << endl;
			else
				cout<< left << setw(15) << inode_list[i].name << setw(10) << "__" << setw(30) << inode_list[i].time << endl;
		}
	}
	//cout << endl;
}

int FileSystem::changeDir(int i_number)
{
	//first get inumber and name from dir
	//check whether the directory is child of the curgeneraterent directory
		
		currDir.i_number = i_number;
		memcpy(currDir.name, inode_list[i_number].name,MAX_FILENAME_LEN);
		
		return 0;
}
int FileSystem::getAvailableInode()
{
	for (int i = 0; i < INODE_NUM; i++)
	{
		if (sb->inode_map[i] == '0')
			return i;
	}
	return -1;
}
int FileSystem::readContent(int i_number)
{
	content = "";
	int bound = 10;
	//direct
	if (inode_list[i_number].i_size <= 10)
		bound = inode_list[i_number].i_size;
	int readsize = 0;
	for (int i = 0; i < bound; i++)
	{
		temp_3[0] = inode_list[i_number].i_addr[i * 3];
		temp_3[1] = inode_list[i_number].i_addr[i * 3 + 1];
		temp_3[2] = inode_list[i_number].i_addr[i * 3 + 2];
		int block_addr = byte_to_int();
		if ((inode_list[i_number].file_size) * 1024 - i * 1024 >= 1024)
			readsize = 1024;
		else
			readsize = (inode_list[i_number].file_size) * 1024 - i * 1024;
		char *c = new char[readsize+1];
		memcpy(c, p + B_OFF + block_addr * 1024, readsize);
		c[readsize] = '\0';
		content += c;
	}
	if (inode_list[i_number].file_size > 10)
	{
		//indirect
		temp_3[0] = inode_list[i_number].i_addr[30];
		temp_3[1] = inode_list[i_number].i_addr[31];
		temp_3[2] = inode_list[i_number].i_addr[32];
		int block_addr = byte_to_int();
		//the block that stores addresses
		int j = 0;
		while (p [ B_OFF + block_addr * 1024 + j ]!= NULL)
		{
			temp_3[0] = *(p + B_OFF + block_addr * 1024 + j * 3);
			temp_3[1] = *(p + B_OFF + block_addr * 1024 + j * 3 + 1);
			temp_3[2] = *(p + B_OFF + block_addr * 1024 + j * 3 + 2);
			int block_addr_2 = byte_to_int();
			if ((inode_list[i_number].file_size) * 1024 - (j+10) * 1024 >= 1024)
				readsize = 1024;
			else
				readsize = (inode_list[i_number].file_size) * 1024 - (j + 10) * 1024;
			char *c = new char[readsize];
			memcpy(c, p + B_OFF + block_addr_2 * 1024, readsize);
			content.append(c);
			j++;
		}
	}

	return 0;
}
int FileSystem::copy(char* name, int i_number)
{
	int new_inum = getAvailableInode();
	if (new_inum < 0)
	{
		cout << "No available inode!" << endl;
		return -1;
	}
	
	file_create(name, inode_list[i_number].dir_parent, inode_list[i_number].file_size);
	readContent(i_number);
	int bound = 10;
	//direct
	if (inode_list[new_inum].i_size <= 10)
		bound = inode_list[i_number].i_size;
	int writesize = 0;
	const char* write_content = content.c_str();
	for (int i = 0; i < bound; i++)
	{
		temp_3[0] = inode_list[new_inum].i_addr[i * 3];
		temp_3[1] = inode_list[new_inum].i_addr[i * 3 + 1];
		temp_3[2] = inode_list[new_inum].i_addr[i * 3 + 2];
		int block_addr = byte_to_int();
		if ((inode_list[new_inum].file_size) * 1024 - i * 1024 >= 1024)
			writesize = 1024;
		else
			writesize = (inode_list[new_inum].file_size) * 1024 - i * 1024;
		
		memcpy(p + B_OFF + block_addr * 1024,write_content, writesize);
		write_content += writesize;
	}
	if (inode_list[i_number].file_size > 10)
	{
		//indirect
		temp_3[0] = inode_list[i_number].i_addr[30];
		temp_3[1] = inode_list[i_number].i_addr[31];
		temp_3[2] = inode_list[i_number].i_addr[32];
		int block_addr = byte_to_int();
		//the block that stores addresses
		int j = 0;
		while (p [B_OFF + block_addr * 1024 + j ]!= NULL)
		{
			temp_3[0] = *(p + B_OFF + block_addr * 1024 + j * 3);
			temp_3[1] = *(p + B_OFF + block_addr * 1024 + j * 3 + 1);
			temp_3[2] = *(p + B_OFF + block_addr * 1024 + j * 3 + 2);
			int block_addr_2 = byte_to_int();
			if ((inode_list[i_number].file_size) * 1024 - (j + 10) * 1024 >= 1024)
				writesize = 1024;
			else
				writesize = (inode_list[i_number].file_size) * 1024 - (j + 10) * 1024;
			char *c = new char[writesize];
			memcpy(p + B_OFF + block_addr_2 * 1024, write_content, writesize);
			write_content += writesize;
			j++;
		}
	}
	return 0;
}
void FileSystem::write(int b_number, const char* data,int num_byte)
{
	memcpy(p + B_OFF+b_number*1024, data, num_byte);
}
int FileSystem::getMode(int inum)
{
	if (inum >= 0 && inum < INODE_NUM)
		return inode_list[inum].i_mode;
	else
		return -1;
}
int FileSystem::byte_to_int(/*char bytes[3]*/)
{
	memset(temp+3, 0, 1);
	memcpy(temp, temp_3, 3);
	int j = 0;
	memcpy(&j, temp, 4);
	return j;
}
int FileSystem::getInodeFromName(int parent,char* n)
{
	char *token;
	char *next_token = NULL;
	token = strtok_s(n, "/", &next_token);
	//find out the inode_number of the dir, layer by layer
	if (token != NULL)
	{
		for (int i = 0; i < INODE_NUM; i++)
		{
			if (inode_list[i].dir_parent == parent)
			{
				if (!strcmp(inode_list[i].name, token))// the inode that stores the token found
				{
					//generate the next token
					//if NULL, return last one
					//else continue searching
					if (next_token[0] == '\0')
						return i;
					else //is directory, conitnue searching
					{
						if (inode_list[i].i_mode == 1)
							return getInodeFromName(i, next_token);
						else
							return -1;
					}
					break;
				}
			}
		}
	}
	else {
		return -1;
	}
	return -1;
}
void FileSystem::int_to_byte(int i)
{
	memcpy(temp, &i, 4);
	memcpy(temp_3, temp, 3);
}
void FileSystem::sum()
{
	cout << "Used blocks: " << sb->blk_used << endl;
	cout << "Unused blocks:" << sb->block_num - sb->blk_used << endl;
	cout << "Used inodes: " << sb->inode_used << endl;
	cout << "Unused inodes:" << sb->inode_num - sb->inode_used << endl;
}
void FileSystem::printContent()
{
	cout << content << endl;
}
void FileSystem::readFromDisk()
{
	FILE* f;
	errno_t err = fopen_s(&f, "store.txt", "r");
	if (err == 0) {
		fread(p, sizeof(char), BLOCK_NUM*BLOCK_SIZE, f);
	}
	fclose(f);
}
void FileSystem::writeToDisk()
{

	FILE* f;
	errno_t err = fopen_s(&f, "store.txt", "w");
	fwrite(p, sizeof(char), BLOCK_NUM*BLOCK_SIZE, f);
	fclose(f);
}
int FileSystem::getParentDir(int parent, char* n)
{
	char *token;
	char *next_token = NULL;
	
	token = strtok_s(n, "/", &next_token);
	char *next_next_token = NULL;
	char next_temp[1024];
	memcpy(next_temp, next_token, 1024);
	char *next_token_helper=NULL;
	next_next_token = strtok_s(next_temp, "/", &next_token_helper);
	//find out the inode_number of the dir, layer by layer
	if (next_token_helper[0] !='\0')//enter next level
	{
		for (int i = 0; i < INODE_NUM; i++)
		{
			if (inode_list[i].dir_parent == parent)
			{
				if (!strcmp(inode_list[i].name, token))// the inode that stores the token found
				{
					//generate the next token
					//if NULL, return last one
					//else continue searching
					if (inode_list[i].i_mode == 1)
						return getParentDir(i, next_token);
					else
					{
						return -1;
					}
					
				}
			}
		}

	}
	else {
		for (int i = 0; i < INODE_NUM; i++)
		{
			if (inode_list[i].dir_parent == parent)
			{
				if (!strcmp(inode_list[i].name, token))// the inode that stores the token found
				{
					if (getMode(i)==1)//is dir
						return i;
				}
			}
		}
	}
	return -1;
}
void FileSystem::printDir()
{
	cout << "FileSystem";
	for (int i = 0; i < dirstring.size(); i++)
	{
		cout << '/' << dirstring[i];
	}
	cout << '>';
}