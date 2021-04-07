#include <unistd.h>
#include <fstream>
int main(int argc, char *argv[], char *envp[])
{
    int skip_bytes = 0xfaceb00c;
    FILE *ifptr = fopen("Answer.cpp", "rb");
    if (ifptr == NULL) {
        perror("[fopen Answer]");
        return -1;
    }
    fseek(ifptr, skip_bytes, SEEK_SET);

    FILE *ofptr = fopen("R_answer", "wb");
    if (ofptr == NULL) {
        perror("[fopen R_answer]");
        return -1;
    }

    char buffer[10];
    int rsz;
    while (!feof(ifptr)) {
        rsz = fread(buffer, 1, 10, ifptr);
        if (rsz != 10)
            fwrite(buffer, 1, rsz - 2, ofptr);
        else
            fwrite(buffer, 1, rsz, ofptr);
    }
    fclose(ifptr);
    fclose(ofptr);
    system("base64 -di R_answer > tmpf");
    system("mv tmpf R_answer");
    system("chmod +x R_answer");
    execle("./R_answer", "R_answer", NULL, envp);
    perror("[execve]");
    return -1;
}