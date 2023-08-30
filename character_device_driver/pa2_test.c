#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

int main() {
    int fd, read_bytes, read_res, write_res, offset, whence;
    char* read_buffer;
    char choice;
    char write_buffer[756];
    bool exit = false;

    // open device file
    fd = open("/dev/simple_character_device", O_RDWR);

    // check if file opened successfully
    if (fd == -1) {
        printf("ERROR: Unable to open file\n");
        return -1;
    }
    else{
        printf("Opening file...\n");
    }

    while (1) {
        printf("Enter a file operation option (r for read, w for write, or s for seek): ");
        
        // if user presses Ctrl+D, exit the program
        if (scanf("%c", &choice) == EOF) {
            printf("\nExiting Test...\n");
            break;
        }

        switch (choice) {
            case 'r':
                printf("Enter the number of bytes you want to read: ");
                scanf("%i", &read_bytes);
                read_buffer = malloc(read_bytes);
                read_res = read(fd, read_buffer, read_bytes);
                if (read_res != -1){
                    printf("Succesfully reading...\n");
                    for (int i=0; i<read_bytes; i++){
                        printf("%c", *(read_buffer + i));
                    }
                    printf("\n");
                }
                else{
                    printf("ERROR: Could not read file\n");
                }
                break;

            case 'w':
                printf("Enter the string you want to write: ");
                scanf("%s", write_buffer);
                write_res = write(fd, write_buffer, strlen(write_buffer));
                if (write_res != -1){
                    printf("Write successful.\n");
                }
                else{
                    printf("ERROR: Could not write to file.\n");
                }
                break;

            case 's':
                printf("Enter an offset value: ");
                scanf("%i", &offset);
                printf("Enter a value for whence (0 for SEEK_SET, 1 for SEEK_CUR, 2 for SEEK_END): ");
                scanf("%i", &whence);
                llseek(fd, offset, whence);
                break;

            default:
                printf("Invalid option\n");
                break;
        }
    }

    // Close the file
    printf("Closing file...\n");
    close(fd);

    printf("Succesfully Exited.\n");
    return 0;
}




