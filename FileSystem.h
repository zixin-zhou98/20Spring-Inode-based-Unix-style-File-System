#pragma once
#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#define BLOCK_SIZE 1024 //1KB
#define BLOCK_NUM 1024*16  //16MB in total
#define DATA_BLOCK_NUM BLOCK_NUM-25
#define INODE_NUM 60//
#define MAX_FILE_SIZE 351//351kb or 351 blocks
#define MAX_ADDR_PER_BLOCK 341
#define MAX_FILE_NUM 45
#define I_OFF	sizeof(Superblock)//i-node offset
#define B_OFF	I_OFF+INODE_NUM*sizeof(Inode)//datablock offset
#define INODE_PER_BLOCK 20
#define MAX_FILENAME_LEN 32 
#define MAX_FILE_PER_DIR 20
#define INITIAL_USE_BLK 25
#include<string>
#include<vector>
struct Superblock{
	unsigned int block_num;//blocks that store files
	unsigned int inode_num;//number of inodes 60
	char inode_map[INODE_NUM];//bitmap of inodes
	char block_map[BLOCK_NUM];//bitmap of blocks
	int inode_used;//the number of inodes being used
	int blk_used;//the number of blocks being used
};

struct Inode {
	char name[MAX_FILENAME_LEN];
	int i_mode;//File:0 Dir:1
	unsigned int i_num;//idenfity the inode to be unique
	char i_addr[33];//11 addresses (10 direct, 1 indirect)
	int i_size;//number of blocks allocated
	float file_size;
	char time[64];
	int dir_parent;
};

struct Dir_rec {
	int i_number;
	char name[MAX_FILENAME_LEN];
};

struct Dir {
	Dir_rec records[MAX_FILE_PER_DIR];
};

class FileSystem {

public:
	int byte_to_int(/*char num_in_byte[3]*/);
	void int_to_byte(int i);
	int init();
	void init_inode(int i_number);
	void util();//debug helper
	int file_create(char* name, int parent, float f_size);
	int file_delete(int i_number);
	int dir_create(int mode,char* name, int parent);
	int dir_delete( int i_number);
	int changeDir(int i_number);
	void inode_update(int i_number,int indirect, int count);
	void write(int b_number,const char* data,int num_byte);
	int getInodeFromName(int parent, char* n);
	int getAvailableInode();
	int getParent(int inum);
	int readContent(int i_number);
	int copy(char* name, int i_number);
	int checkExistence(char* name, int parent);
	int dir_create_recursive(char* name,int parent);
	void ls(int i_number);
	int getMode(int inum);
	void sum();
	void printContent();
	Dir_rec currDir;
	void readFromDisk();
	void writeToDisk();
	int getParentDir(int parent, char* n);
	void printDir();
	std::vector<std::string> dirstring;
private:
	int file_num = 0;
	Superblock* sb;
	char* p;
	Inode* inode_list;
	char* temp = new char[4];
	char* temp_3 = new char[3];
	char* temp_2 = new char[4];
	char* temp_2_3 = new char[3];
	std::string content="";
};

#endif
