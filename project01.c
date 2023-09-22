#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "passwords.h"
#include "sha256.h"



#define DIG_BIN_LEN 32
#define DIG_STR_LEN ((DIG_BIN_LEN * 2) + 1)



void sha256(char *dest, char *src) {
    // zero out the sha256 context
    struct sha256_ctx ctx;
    memset(&ctx, 0, sizeof(ctx));

    // zero out the binary version of the hash digest
    unsigned char dig_bin[DIG_BIN_LEN];
    memset(dig_bin, 0, DIG_BIN_LEN);

    // zero out the string version of the hash digest
    memset(dest, 0, DIG_STR_LEN);

    // compute the binary hash digest
    __sha256_init_ctx(&ctx);
    __sha256_process_bytes(src, strlen(src), &ctx);
    __sha256_finish_ctx(&ctx, dig_bin);

    // convert it into a string of hexadecimal digits
    for (int i = 0; i < DIG_BIN_LEN; i++) {
        snprintf(dest, 3, "%02x", dig_bin[i]);
        dest += 2;
    }
}



char change(char curr)
{
    switch (curr)
    {
    case 'o':
        return '0';
    case 'e':
        return '3';
    case 'i':
        return '!';
    case 'a':
        return '@';
    case 'h':
        return '#';
    case 's':
        return '$';
    case 't':
        return '+';
    }
    return curr;
}

void change_password(char *password)
{
    for (int i = 0; i < strlen(password); i++)
    {
        password[i] = change(password[i]);
    }
}

void add1(char *password)
{
    strcat(password, "1");
}

int main(int argc, const char **argv)
{
    if (argc != 2)
    {
        perror("Usage: ./project01 <password>");
        return 1;
    }

 	char *given_hash = (char *)argv[1];
	char *hash = malloc(DIG_STR_LEN);
	char *password = malloc(256);

  // compare the given hash to the hash of each password
	for (int i = 0; i < 10000; i++)
	{
     	sha256(hash, passwords[i]);
        if (strcmp(hash, given_hash) == 0)
 		 {
	      	strcpy(password, passwords[i]);
            break;
         }
    }

   // if the password is not found, try changing the password
   // using l335t speak
    if (password[0] == '\0')
    {
    	for (int i = 0; i < 10000; i++)
        {
           	 strcpy(password, passwords[i]);
        	 change_password(password);
        	 sha256(hash, password);

             if (strcmp(hash, given_hash) == 0)
             {
				 break;
             }
	          password[0] = '\0';
         }
    }

  // if the password is not found, try adding a 1 to the end
    if (password[0] == '\0')
     {
      	for (int i = 0; i < 10000; i++)
        {
         	strcpy(password, passwords[i]);
         	add1(password);
            sha256(hash, password);

            if (strcmp(hash, given_hash) == 0)
            {
             	break;
             }

            password[0] = '\0';
         }
      }
            
    if (password[0] == '\0')
    {
     	printf("not found\n");
       return 1;
    }
    printf(password);

    return 0;
}
            

