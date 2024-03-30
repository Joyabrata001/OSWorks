#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

int recursive_flg = 1;
mode_t targetPermission;
time_t targetModificationTime;

void checkIfRecursive(char *arg);
void listDir(char *dirName);
void searchFile(char *fileName);

int matchPermissions(mode_t actualPermission);
void parseModificationTime(char *timeArg);
int matchModificationTime(time_t modTime);

int main(int argc, char *argv[])
{
    char *dirName;

    if (argc == 5)
    {
        dirName = argv[1];
        checkIfRecursive(argv[2]);
        parseModificationTime(argv[3]);

        // Parse permission argument
        if (sscanf(argv[4], "%o", &targetPermission) != 1)
        {
            fprintf(stderr, "\nInvalid permission format. Please use octal representation (e.g., 755 [0 for no permission specification]).\n");
            exit(1);
        }

        printf("\nThe contents of the directory with matching permissions and modification time are:\n\n");

        // Check if dirName is a file or a directory
        struct stat statBuf;
        if (lstat(dirName, &statBuf) == 0 && S_ISREG(statBuf.st_mode))
        {
            // If dirName is a file, search in the current directory
            searchFile(dirName);
        }
        else
        {
            // If dirName is a directory, list its contents
            listDir(dirName);
        }
    }
    else
    {
        fprintf(stderr, "\nUsage: %s <directory or file> <recursive-flag> <modification-time> <permission>\n", argv[0]);
        fprintf(stderr, "Example: %s /path/to/dir -y '2024-03-12 12:00:00' 755\n", argv[0]);
        exit(1);
    }
}

void checkIfRecursive(char *arg)
{
    if (strcmp(arg, "-n") == 0 || strcmp(arg, "-N") == 0)
        recursive_flg = 0;
    else if (strcmp(arg, "-y") == 0 || strcmp(arg, "-Y") == 0)
        recursive_flg = 1;
    else
    {
        fprintf(stderr, "\nPlease specify '-y' or '-n' for recursive or non-recursive search.\n");
        exit(1);
    }
}

void listDir(char *dirName)
{
    DIR *dir;
    struct dirent *dirEntry;
    struct stat inode;
    char name[1000];

    dir = opendir(dirName);
    if (dir == NULL)
    {
        perror("Error opening file or directory: ");
        exit(1);
    }

    while ((dirEntry = readdir(dir)) != NULL)
    {
        if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0)
            continue;

        sprintf(name, "%s/%s", dirName, dirEntry->d_name);
        lstat(name, &inode);

        if (matchPermissions(inode.st_mode) || targetPermission == 0)
            printf("%s\n", name);

        if (S_ISDIR(inode.st_mode) && recursive_flg)
            listDir(name);
    }

    closedir(dir);
}

int matchPermissions(mode_t actualPermission)
{
    return ((actualPermission & targetPermission) == targetPermission);
}

void searchFile(char *fileName)
{
    struct dirent *dirEntry;
    struct stat inode;
    DIR *dir = opendir(".");

    if (dir == NULL)
    {
        perror("Error opening current directory: ");
        exit(1);
    }

    while ((dirEntry = readdir(dir)) != NULL)
    {
        if (strcmp(dirEntry->d_name, fileName) == 0)
        {
            lstat(fileName, &inode);
            if (matchPermissions(inode.st_mode) || targetPermission == 0)
            {
                printf("%s\n", fileName);
                break;
            }
        }
    }

    closedir(dir);
}

void parseModificationTime(char *timeArg)
{
    struct tm tm_time;
    if (strptime(timeArg, "%Y-%m-%d %H:%M:%S", &tm_time) == NULL)
    {
        fprintf(stderr, "\nInvalid modification time format. Please use 'YYYY-MM-DD HH:MM:SS'.\n");
        exit(1);
    }

    // Convert struct tm to time_t
    targetModificationTime = mktime(&tm_time);
}

int matchModificationTime(time_t modTime)
{
    // Check if modification time is before or after the target modification time
    // Modify this condition according to your specific requirement
    return modTime < targetModificationTime; // Change to > for after
}