#include <cstdio>
#include <cstring>
#include "SRecord.h"

using std::fopen;
using std::strcmp;

//static char *sTestArgs[]={"mkdata", "rcx.nqh", "rcx_data.h", "rcxData" };
static char *sTestArgs[] = {"mkdata", "-s", "fastdl.srec", "RCX_nub.h", "rcxNub" };


int main(int argc, char **argv)
{
	FILE *src;
	FILE *dst;
	int n;
	bool srecMode = false;
	SRecord srec;

	if (argc == 0)
	{
		// special case for debugging under the Metrowerks console
		argv = sTestArgs;
		argc = sizeof(sTestArgs) / sizeof(char *);
	}

	if (argc == 5 && (strcmp(argv[1],"-s")==0))
	{
		for(int i=1; i<4; i++)
			argv[i]=argv[i+1];
		argc--;
		srecMode = true;
	}

	if (argc != 4)
	{
		fprintf(stderr, "Usage: mkdata [-s] sourceFile destFile arrayName\n");
		return -1;
	}

	src = fopen(argv[1], "r");
	if (!src)
	{
		fprintf(stderr, "Error: could not open %s\n", argv[1]);
		return -1;
	}
	if (srecMode && !srec.Read(src, 65536))
	{
		fprintf(stderr, "Error: %s is not a valid S-Record file\n", argv[1]);
		return -1;
	}

	dst = fopen(argv[2], "w");
	if (!dst)
	{
		fprintf(stderr, "Error: could not create %s\n", argv[2]);
		fclose(src);
		return -1;
	}

	fprintf(dst,"static const %s char %s[]={\n", srecMode ? "unsigned" : "", argv[3]);

	n = 0;

	if (srecMode)
	{
		const UByte *ptr = srec.GetData();
		int length = srec.GetLength();

		while(length--)
		{
			fprintf(dst,"0x%02x,", *ptr++);
			if (++n == 16)
			{
				n = 0;
				fputs("\n", dst);
			}
		}
	}
	else
	{
		int c;
		while((c=fgetc(src)) != EOF)
		{
			if (c=='\n')
				fputs("\'\\n\',", dst);
			else
				fprintf(dst,"%d,", c);

			if (++n == 16)
			{
				n = 0;
				fputs("\n", dst);
			}
		}
	}

	fprintf(dst,"};\n");

	fclose(src);
	fclose(dst);

	return 0;
}
