#include <lv2/process.h>
#include <psl1ght/lv2/filesystem.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <zlib.h>
#include <zip.h>
#include <zipint.h>

//#include <dirent.h>
#define DT_DIR 1

#define PS3LOAD_VERSION "v0.2"
#define PORT 4299
#define MAX_ARG_COUNT 0x100

#define ERROR(a, msg) { \
	if (a < 0) { \
		fprintf(stderr, "PS3Load: " msg "\n"); \
		return 1; \
	} \
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ZIP_PATH "/dev_hdd0/tmp/ps3load"
#define SELF_PATH ZIP_PATH ".self"

void DeleteDirectory(const char* path)
{
	int dfd;
	u64 read;
	Lv2FsDirent dir;
	if (lv2FsOpenDir(path, &dfd))
		return;
	while (!lv2FsReadDir(dfd, &dir, &read)) {
		char newpath[0x440];
		if (!read)
			break;
		if (!strcmp(dir.d_name, ".") || !strcmp(dir.d_name, ".."))
			continue;

		strcpy(newpath, path);
		strcat(newpath, "/");
		strcat(newpath, dir.d_name);

		if (dir.d_type == DT_DIR) {
			DeleteDirectory(newpath);
			rmdir(newpath);
		} else {
			remove(newpath);
		}
	}
	lv2FsCloseDir(dfd);
}

int main(int argc, const char* argv[], const char* envp[])
{
	printf("PS3Load " PS3LOAD_VERSION "\n");
	
	mkdir(ZIP_PATH, 0777);
	DeleteDirectory(ZIP_PATH);

	int s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	ERROR(s, "Error creating socket()");

	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);

	ERROR(bind(s, (struct sockaddr*)&server, sizeof(server)), "Error bind()ing socket");

	ERROR(listen(s, 10), "Error calling listen()");

#define continueloop() { close(c); goto reloop; }
reloop:
	while (1) {
		printf("Waiting for connection...\n");
		int c = accept(s, NULL, NULL);
		ERROR(c, "Error calling accept()");

		u32 magic = 0;
		if (read(c, &magic, sizeof(magic)) < 0)
			continueloop();
		if (strncmp((char*)&magic, "HAXX", 4)) {
			fprintf(stderr, "Wrong HAXX magic.\n");
			continueloop();
		}
		if (read(c, &magic, sizeof(magic)) < 0)
			continueloop();
		u16 argslen = magic & 0xFFFF;
		
		u32 filesize = 0;
		if (read(c, &filesize, sizeof(filesize)) < 0)
			continueloop();

		u32 uncompressed = 0;
		if (read(c, &uncompressed, sizeof(uncompressed)) < 0)
			continueloop();

		printf("Receiving data... (0x%08x/0x%08x)\n", filesize, uncompressed);

		u8* data = (u8*)malloc(filesize);
		u32 pos = 0;
		u32 count;
		while (pos < filesize) {
			u32 count = MIN(0x1000, filesize - pos);
			int ret = read(c, data + pos, count);
			if (ret < 0)
				continueloop();
			pos += ret;
		}

		printf("Receiving arguments... 0x%08x\n", argslen);

		u8* args = NULL;
		if (argslen) {
			args = (u8*)malloc(argslen);
			if (read(c, args, argslen) < 0)
				continueloop();
		}

		close(c);

		printf("Decompressing...\n");

		if (uncompressed) {
			u8* compressed = data;
			uLongf final = uncompressed;
			data = (u8*)malloc(final);
			int ret = uncompress(data, &final, compressed, filesize);
			if (ret != Z_OK)
				continue;
			free(compressed);
			if (uncompressed != final)
				continue;
			uncompressed = final;
		} else
			uncompressed = filesize;

		printf("Launching...\n");

		int fd = open(SELF_PATH, O_CREAT | O_TRUNC | O_WRONLY);
		ERROR(fd, "Error opening temporary file.");

		pos = 0;
		while (pos < uncompressed) {
			count = MIN(0x1000, uncompressed - pos);
			write(fd, data + pos, count);
			pos += count;
		}

		close(fd);

		free(data);

		char bootpath[MAXPATHLEN];
		strcpy(bootpath, SELF_PATH);

		struct zip* archive = zip_open(SELF_PATH, ZIP_CHECKCONS, NULL);
		int files = zip_get_num_files(archive);
		if (files > 0) {
			strcpy(bootpath, "");

			for (int i = 0; i < files; i++) {
				char path[MAXPATHLEN];
				strcpy(path, ZIP_PATH);
				const char* filename = zip_get_name(archive, i, 0);
				if (!filename)
					continue;
				if (filename[0] != '/')
					strcat(path, "/");
				strcat(path, filename);

				#define ENDS_WITH(needle) \
					(strlen(filename) >= strlen(needle) && !strcasecmp(filename + strlen(filename) - strlen(needle), needle))

				if (ENDS_WITH("EBOOT.BIN") || ENDS_WITH(".self"))
					strcpy(bootpath, path);

				if (filename[strlen(filename) - 1] != '/') {
					struct zip_stat st;
					if (zip_stat_index(archive, i, 0, &st)) {
						printf("Unable to access file %s in zip.\n", filename);
						continue;
					}
					struct zip_file* zfd = zip_fopen_index(archive, i, 0);
					if (!zfd) {
						printf("Unable to open file %s in zip.\n", filename);
						continue;
					}

					int tfd = open(path, O_CREAT | O_TRUNC | O_WRONLY);
					ERROR(tfd, "Error opening temporary file.");

					pos = 0;
					u8* buffer = malloc(0x1000);
					while (pos < st.size) {
						count = MIN(0x1000, st.size - pos);
						if (zip_fread(zfd, buffer, count) != count)
							ERROR(1, "Error reading from zip.");
						write(tfd, buffer, count);
						pos += count;
					}
					free(buffer);

					zip_fclose(zfd);
					close(tfd);
				} else
					mkdir(path, 0777);
			}
		}
		if (archive)
			zip_close(archive);

		if (!strlen(bootpath))
			continue;

		char* launchenvp[2];
		char* launchargv[MAX_ARG_COUNT];
		memset(launchenvp, 0, sizeof(launchenvp));
		memset(launchargv, 0, sizeof(launchargv));

		launchenvp[0] = (char*)malloc(0x440);
		snprintf(launchenvp[0], 0x440, "ps3load=%s", argv[0]);

		pos = 0;
		int i = 0;
		while (pos < argslen) {
			int len = strlen((char*)args + pos);
			if (!len)
				break;
			launchargv[i] = (char*)malloc(len + 1);
			strcpy(launchargv[i], (char*)args + pos);
			pos += len + 1;
			i++;
		}

		sysProcessExitSpawn2(bootpath, (const char**)launchargv, (const char**)launchenvp, NULL, 0, 1001, SYS_PROCESS_SPAWN_STACK_SIZE_1M);
	}

	return 0;
}
