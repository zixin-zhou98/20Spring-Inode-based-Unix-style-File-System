# 1. Overveiw
In this project, a simplified Unix-style file system based on inode representation is implemented.


# 2. Use cases
This file system supports the following commands:

createDir /dirname  ---  create a new file
![createDir](<https://github.com/zixin-zhou98/Inode-based-Unix-style-File-System/blob/main/pictures/createDir.png>)
deleteDir /dirname   ---  delete a directory
![deleteDir](<https://github.com/zixin-zhou98/Inode-based-Unix-style-File-System/blob/main/pictures/deleteDir.png>)
cd /dirname ---  change working directory
createFile filename filesize  ---  create a new file
![createFile](<https://github.com/zixin-zhou98/Inode-based-Unix-style-File-System/blob/main/pictures/createFile.png>)
deleteFile filename ---  delete a file
![deleteFile](<https://github.com/zixin-zhou98/Inode-based-Unix-style-File-System/blob/main/pictures/deleteFile.png>)
cp original_file_name new_file_name   ---  cp a directory file to another directory or file
![cp](<https://github.com/zixin-zhou98/Inode-based-Unix-style-File-System/blob/main/pictures/cp.png>)
cat filename      ---  read a file
![cat](<https://github.com/zixin-zhou98/Inode-based-Unix-style-File-System/blob/main/pictures/cat.png>)
sum ---  display memory usage
exit --- exit the program, release the memory occupied and store all data into the disk
reload --- get the data from the disk
![reload](<https://github.com/zixin-zhou98/Inode-based-Unix-style-File-System/blob/main/pictures/reload.png>)


# 3. Structure
![structure](<https://github.com/zixin-zhou98/Inode-based-Unix-style-File-System/blob/main/pictures/structure.png>)
First, we divide the memory into blocks, which is the unit in which we read from and write into memory. In practice, the memory is usually sector-addressable. For simplicity, we make it block-addressable in this implementation.

Apart from storing files and directories, we need some space to store data that are used for maintenance. Therefore, we split the whole system into three parts: superblock, inode blocks and data blocks.

All the files and direcotries are uniquely identified by the inode nubmer. For convenience in implementation, we put the inode map and block map into superblock, which help us keep track of usage of memory.

# 4. Workflow
The workflow of our file system is shown as below.
![workflow](<https://github.com/zixin-zhou98/Inode-based-Unix-style-File-System/blob/main/pictures/workflow.png>)
The main.cpp works as a shell program, which takes in and interprets the user input, matching it with the corresponding action. The action calls functions in FileSystem.cpp and finishes its operations on memory. The disk manipulation is not necessarily done, unless load or exit command is input.
