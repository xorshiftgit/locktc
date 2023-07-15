#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdint.h>

#define BUF_SIZE (65536*2)

uint8_t buf[BUF_SIZE];

int main(int argc, char *argv[])
{
    int i;
    FILE *fp = NULL;
    FILE *fout = NULL;
    char *outname1 = NULL;
    char *outname2 = NULL;
    off64_t size;

    if (sizeof(size) != 8)
    {
        printf("Wrong size\n");
        return -1;
    }

    if (argc < 2)
    {
        printf("Unknown number of arguments\n");
        return -2;
    }

    if (argc > 2)
    {
        outname1 = argv[2];
    }

    if (argc > 3)
    {
        outname2 = argv[3];
    }

    fp = fopen(argv[1], "rb+");
    if (!fp)
    {
        printf("Failed to open %s\n", argv[1]);
        return -3;
    }

    if (1 != fread(buf, BUF_SIZE, 1, fp))
    {
        fclose(fp);
        return -4;
    }

    fseek(fp, 0, SEEK_END);
    size = ftello64(fp);
    if (size <= 2 * BUF_SIZE)
    {
        fclose(fp);
        return -5;
    }
    printf("size %zd\n", size);

    if (outname1 != NULL)
    {
        for(i=0;i<BUF_SIZE;i++)
            buf[i] ^= 0xff;

        fout = fopen(outname1, "wb");
        if (!fout)
        {
            fclose(fp);
            return -6;
        }

        if (1 != fwrite(buf, BUF_SIZE, 1, fout))
        {
            fclose(fp);
            fclose(fout);
            return -7;
        }

        fclose(fout);
    }

    if (outname2 != NULL)
    {
        fseeko64(fp, -BUF_SIZE, SEEK_CUR);

        printf("pos %zd\n", ftello64(fp));
        if (1 != fread(buf, BUF_SIZE, 1, fp))
        {
            fclose(fp);
            return -8;
        }

        for(i=0;i<BUF_SIZE;i++)
            buf[i] ^= 0xff;

        fout = fopen(outname2, "wb");
        if (!fout)
        {
            fclose(fp);
            return -9;
        }

        if (1 != fwrite(buf, BUF_SIZE, 1, fout))
        {
            fclose(fp);
            fclose(fout);
            return -10;
        }

        fclose(fout);
    }

    printf("pos %zd\n", ftello64(fp));
    fseeko64(fp, -BUF_SIZE, SEEK_CUR);

    if (1 != fwrite(buf, BUF_SIZE, 1, fp))
    {
        fclose(fp);
        printf("Failed to write at the end");
        return -11;
    }

    fseeko64(fp, 0, SEEK_SET);

    if (1 != fread(buf, BUF_SIZE, 1, fp))
    {
        fclose(fp);
        return -12;
    }

    for(i=0;i<BUF_SIZE;i++)
        buf[i] ^= 0xff;

    fseeko64(fp, 0, SEEK_SET);

    if (1 != fwrite(buf, BUF_SIZE, 1, fp))
    {
        fclose(fp);
        printf("Failed to write at the beginning");
        return -13;
    }

    fclose(fp);
}
