#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#define FILE_HEADER_SIZE 1024

/// @brief creates a file (if it does not exist) called fname of appropriate size (1024 + bsize*bno Bytes) in the folder
/// @param fname name of file
/// @param bsize block size
/// @param bno block no
/// @return 0 on SUCCESS, -1 on FAILURE
int init_File_dd(const char *fname, int bsize, int bno)
{
    // int open(const char *pathname, int flags, mode_t mode);
    // flags:   O_RDWR -> to open in read/write
    //          O_CREAT -> if pathname does not exist, create it as a regular file
    // modes:   S_IRUSR -> user has read permission
    //          S_IWUSR -> user has write permission

    int file = open(fname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (file == -1)
    {
        perror("Error opening or creating file");
        return -1;
    }

    int info[2] = {bno, bsize};
    write(file, info, sizeof(int) * 2);

    // off_t lseek(int fd, off_t offset, int whence): repositions the file offset of the open file description
    // #define SEEK_SET	0	/* Seek from beginning of file.  */
    // Set file size to 1024 + bsize * bno
    lseek(file, FILE_HEADER_SIZE + bsize * bno - 1, SEEK_SET);
    write(file, "\0", 1);

    close(file);
    return 0;
}

/// @brief reads the bnoth block from the file fname and puts the block in buffer
/// @param fname name of file
/// @param bno block no
/// @param buffer place to put the block
/// @return 0 on SUCCESS, -1 on FAILURE
int read_block(const char *fname, int bno, char *buffer)
{
    // int open(const char *pathname, int flags, mode_t mode);
    int file = open(fname, O_RDONLY);
    if (file == -1)
    {
        perror("Error opening file for reading");
        return -1;
    }

    // Read block size and block number information from the file header
    int info[2];
    read(file, info, sizeof(int) * 2);

    int stored_bno = info[0];
    int bsize = info[1];

    if (stored_bno < bno)
    {
        close(file);
        perror("Error: Block number exceeds file size");
        return -1;
    }

    // Move to the start of the desired block
    lseek(file, FILE_HEADER_SIZE + bsize * bno, SEEK_SET);

    // Read block data into the buffer
    read(file, buffer, bsize);

    close(file);
    return 0;
}

/// @brief reads entire file
/// @param fname name of file
/// @return 0 on SUCCESS, -1 on FAILURE
int read_entire_file(const char *fname)
{
    // int open(const char *pathname, int flags, mode_t mode);
    int file = open(fname, O_RDONLY);
    if (file == -1)
    {
        perror("Error opening file for reading");
        return -1;
    }

    // Read block size and block number information from the file header
    int info[2];
    read(file, info, sizeof(int) * 2);

    int bno = info[0];
    int bsize = info[1];

    char *buffer = (char *)malloc(bno * bsize * sizeof(char));

    lseek(file, FILE_HEADER_SIZE, SEEK_SET);
    if (read(file, buffer, bno * bsize) == -1)
        printf("\nNope\n");

    printf("File contents: ");
    for (int i = 0; i < bno * bsize; i++)
    {
        if (i % bsize == 0)
            printf("\nBlock %d: ", i / bsize + 1);
        printf("%c", buffer[i]);
    }
    printf("\n *** EOF *** \n");

    close(file);
    free(buffer);

    return 0;
}

/// @brief overwrites the bnoth block in the file fname and with the data in buffer
/// @param fname name of file
/// @param bno block no
/// @param buffer contains data that needs to be written
/// @return 0 on SUCCESS, -1 on FAILURE
int write_block(const char *fname, int bno, char *buffer)
{
    int file = open(fname, O_RDWR);
    if (file == -1)
    {
        perror("Error opening file for writing");
        return -1;
    }

    // Read block size and block number information from the file header
    int info[2];
    read(file, info, sizeof(int) * 2);
    int stored_bno = info[0];
    int bsize = info[1];

    if (stored_bno < bno)
    {
        close(file);
        perror("Error: Block number exceeds file size");
        return -1;
    }

    // Move to the start of the desired block
    lseek(file, FILE_HEADER_SIZE + bsize * bno, SEEK_SET);

    // Write block data from the buffer to the file
    write(file, buffer, bsize);

    close(file);
    return 0;
}

// Each of the functions get the information n and s from the 1024 File header

void message()
{
    printf("\nMenu\n");
    printf("1. Press 1 to initialize\n");
    printf("2. Press 2 to write to a specific block\n");
    printf("3. Press 3 to read from a specific block\n");
    printf("4. Press 4 to read entire file contents\n");
    printf("5. Press 5 to exit\n");
}

int main()
{
    const char *filename = "dd1";

    int block_size;
    int block_number;

    int choice;

    printf("Enter block size: ");
    scanf("%d", &block_size);

    while (1)
    {
        message();
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            // Initialize or create the file
            printf("No of blocks: ");
            scanf("%d", &block_number);

            if (init_File_dd(filename, block_size, block_number) == -1)
            {
                fprintf(stderr, "Failed to initialize or create the file.\n");
                // return EXIT_FAILURE;
            }
            else
                printf("Initialized\n");
            break;

        case 2:
            // Write data to block

            printf("Block no to write to: ");
            scanf("%d", &block_number);

            char *write_data;
            write_data = (char *)malloc(block_size * sizeof(char));
            strcpy(write_data, "Hello, this is written to the file!");

            if (write_block(filename, block_number, write_data) == -1)
            {
                fprintf(stderr, "Failed to write data to the block.\n");
                // return EXIT_FAILURE;
            }
            else
                printf("Written successfully\n");

            free(write_data);
            break;

        case 3:
            // Read block from block

            printf("Block no to read from: ");
            scanf("%d", &block_number);

            char *read_data;
            read_data = (char *)malloc(block_size * sizeof(char));
            if (read_block(filename, block_number, read_data) == -1)
            {
                fprintf(stderr, "Failed to read data from the block.\n");
                // return EXIT_FAILURE;
            }
            else
                printf("Data read from block %d: %s\n", block_number, read_data);

            free(read_data);
            break;

        case 4:
            // Read all blocks i.e. entire file

            if (read_entire_file(filename) == -1)
            {
                fprintf(stderr, "Failed to read data from the block.\n");
                // return EXIT_FAILURE;
            }

            break;

        case 5:
            printf("Exited gracefully\n");
            exit(0);

            break;

        default:
            exit(0);
        }
    }
    return EXIT_SUCCESS;
}