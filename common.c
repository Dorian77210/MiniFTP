#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "common.h"
#include "tea.h"

void configure_criteria(struct addrinfo *criteria)
{
    // configuration of the criteria
    memset(criteria, 0, sizeof(struct addrinfo));
    criteria->ai_protocol = 0x0;
    criteria->ai_socktype = SOCK_STREAM;
    criteria->ai_flags = AI_PASSIVE | AI_NUMERICSERV;
    criteria->ai_family = AF_UNSPEC;
    criteria->ai_addr = NULL;
    criteria->ai_canonname = NULL;
    criteria->ai_next = NULL;
}

// returns 1, if architecture
//    is little endian, 0 in case of big endian.
int is_little_endian()
{
    unsigned int x = 1;
    char *c = (char *)&x;
    return (int)*c;
}

int store_file(client_session session, const char* filename, int filesize)
{
    // open the file
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 00666);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }


    int blocks_count = filesize / BLOCK_SIZE, n, m;
    block_t block;
    int size = sizeof(block);
    char *buffer = NULL;

    for (int i = 0; i < blocks_count; i++)
    {
        memset(&block, 0, size);
        n = recv(session.sfd, &block, size, 0x0);
        if (n == -1)
        {
            fprintf(stderr, "Error when receiving data \n");
            perror("recv");
            close(fd);
            return 0;
        }

        decrypt_block(&block, session.session_key);
        buffer = (char *)&block;

        if (i == (blocks_count - 1))
        {
            // last block, retrieve the padding
            int padding = block & 0xFF;
            int bytes_to_write = BLOCK_SIZE - padding;
            if (is_little_endian())
            {
                if (bytes_to_write != 0)
                {
                    printf("Write extra bytes by padding \n");
                    m = write(fd, buffer + padding, bytes_to_write);
                    if (m == -1)
                    {
                        perror("write");
                        close(fd);
                        return 0;
                    }
                }
            }
            else
            {
                // Not implemented for big endian
            }
        }
        else
        {
            m = write(fd, &block, n);
            if (m == -1)
            {
                perror("write");
                close(fd);
                return 0;
            }
        }
    }

    close(fd);

    return 1;
}

int send_file(client_session session, const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        return 0;
    }

    struct stat stats;
    int s = fstat(fd, &stats);
    if(s == -1) {
        close(fd);
        return 0;
    }

    int filesize = stats.st_size;

    int blocks_count = (filesize / BLOCK_SIZE), n;
    block_t block;
    int size = sizeof(block_t);

    int add_block = (filesize % BLOCK_SIZE) == 0;
    blocks_count += add_block ? 0 : 1;

    for (int i = 0; i < blocks_count; i++)
    {
        memset(&block, 0, size);
        n = read(fd, &block, size);

        if (n == -1)
        {
            perror("read");
            close(fd);
            return 0;
        }

        if (i == (blocks_count - 1))
        {
            if (add_block)
            {   
                encrypt_block(&block, session.session_key);
                // send the block
                n = send(session.sfd, &block, BLOCK_SIZE, 0x0);
                if(n == -1) {
                    perror("send");
                }

                // add a last block
                memset(&block, 0, size);
                block = 0x8;
                encrypt_block(&block, session.session_key);

                n = send(session.sfd, &block, BLOCK_SIZE, 0x0);
                if(n == -1) {
                    perror("send");
                }
            }
            else
            {
                int shift = BLOCK_SIZE - n;
                if(!is_little_endian()) {
                    block |= shift;
                } else {
                    block = block << (shift * 8);
                    block |= shift;
                }
                
                encrypt_block(&block, session.session_key);
                // send the block
                n = send(session.sfd, &block, BLOCK_SIZE, 0x0);
                if(n == -1) {
                    perror("send");
                }
            }
        } else {
            encrypt_block(&block, session.session_key);
            n = send(session.sfd, &block, n, 0x0);
            if(n == -1) {
                perror("send");
            }
        }
    }

    close(fd);

    return 1;
}