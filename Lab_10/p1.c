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

struct file_header
{
    int bno;      /* total number of blocks */
    int bsize;    /* size of each block */
    int fbno;     /* number of free blocks */
    u_int8_t *fb; /* free block bit-pattern */
    int *chain;   /* array of intial block numbers of each chain */
};

/// @brief creates a file (if it does not exist) called fname of appropriate size (1024 + bsize*bno Bytes) in the folder
/// @return 0 on SUCCESS, -1 on FAILURE
int init_File_dd(const char *fname, struct file_header *fh)
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

    fh->fbno = fh->bno;
    fh->fb = (u_int8_t *)malloc(fh->bno / 8);
    memset(fh->fb, 0, fh->bno / 8);

    fh->chain = (int *)malloc(fh->bno * sizeof(int)); // Properly allocate memory for chain
    memset(fh->chain, -1, fh->bno * sizeof(int));     // Initialize all elements to -1

    write(file, fh, sizeof(struct file_header));

    // off_t lseek(int fd, off_t offset, int whence): repositions the file offset of the open file description
    // #define SEEK_SET	0	/* Seek from beginning of file.  */
    // Set file size to 1024 + bsize * bno
    lseek(file, FILE_HEADER_SIZE + fh->bno * fh->bsize - 1, SEEK_SET);
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
    printf("1. Press 1 to write to a specific block\n");
    printf("2. Press 2 to read from a specific block\n");
    printf("3. Press 3 to exit\n");
}

int main()
{
    const char *filename = "dd1";

    struct file_header fh;

    int choice;

    printf("Enter total number of blocks(<= 245): ");
    scanf("%d", &fh.bno);

    printf("Enter block size: ");
    scanf("%d", &fh.bsize);

    init_File_dd(filename, &fh);
    printf("total number of blocks = %d\nsize of each block = %d\ntotal number of free blocks = %d\n", fh.bno, fh.bsize, fh.fbno);
    printf("free block pattern: ");
    for (int i = 0; i < fh.bno; ++i)
    {
        printf("%c", fh.fb[i]);
    }
    printf("\nchains:\n");
    for (int i = 0; i < fh.bno; ++i)
    {
        printf("%d ", fh.chain[i]);
    }
    printf("\n");

    return EXIT_SUCCESS;
}