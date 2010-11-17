#include <psl1ght/lv2/filesystem.h>
#include <psl1ght/lv2/tty.h>
#include <psl1ght/lv2/errno.h>

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <utime.h>

#include <sys/socket.h>

#define UMASK(mode) ((mode) & ~glue_umask)
static mode_t glue_umask = 0;
mode_t umask(mode_t cmask)
{
	mode_t mode = glue_umask;
	glue_umask = cmask;
	return mode;
}

#define DEFAULT_FILE_MODE UMASK(S_IRWXU | S_IRWXG | S_IRWXO)
int open(const char* path, int oflag, ...)
{
	Lv2FsFile fd;

	int lv2flag = oflag & (O_ACCMODE | LV2_O_MSELF);
	if (oflag & O_CREAT)
		lv2flag |= LV2_O_CREAT;
	if (oflag & O_TRUNC)
		lv2flag |= LV2_O_TRUNC;
	if (oflag & O_EXCL)
		lv2flag |= LV2_O_EXCL;
	if (oflag & O_APPEND)
		lv2flag |= LV2_O_APPEND;

	int mode = 0;
	if (oflag & O_WRONLY)
		mode = DEFAULT_FILE_MODE;

	int ret = lv2FsOpen(path, lv2flag, &fd, mode, NULL, 0);
	if (ret)
		return lv2Errno(ret);

	if (oflag & O_CREAT)
		lv2FsChmod(path, DEFAULT_FILE_MODE);

	return fd;
}

int fsync(int fd)
{
	return lv2Errno(lv2FsFsync(fd));
}

int truncate(const char* path, off_t length)
{
	return lv2Errno(lv2FsTruncate(path, length));
}

int ftruncate(int fd, off_t length)
{
	return lv2Errno(lv2FsFtruncate(fd, length));
}

int net_close(int fd);
int close(int fd)
{
	if (fd & SOCKET_FD_MASK)
		return net_close(fd);

	return lv2Errno(lv2FsClose(fd));
}

int unlink(const char* path)
{
	return lv2Errno(lv2FsUnlink(path));
}

ssize_t write(int fd, const void* buffer, size_t size)
{
	if (fd & SOCKET_FD_MASK)
		return send(fd, buffer, size, 0);

	u64 written;
	int ret;
	if (fd == stdout->_file || fd == stderr->_file) {
		ret = lv2TtyWrite(fd, buffer, size, (u32*)(void*)&written);
		written >>= 32;
	} else
		ret = lv2FsWrite(fd, buffer, size, &written);

	if (ret)
		return lv2Errno(ret);
	return written;
}

ssize_t read(int fd, void* buffer, size_t size)
{
	if (fd & SOCKET_FD_MASK)
		return recv(fd, buffer, size, 0);

	u64 bytes;
	int ret;
	
	if (fd == stdin->_file) {
		ret = lv2TtyRead(fd, buffer, size, (u32*)(void*)&bytes);
		bytes >>= 32;
	} else
		ret = lv2FsRead(fd, buffer, size, &bytes);
	
	if (ret)
		return lv2Errno(ret);
	return bytes;
}

static void convertLv2Stat(struct stat* st, Lv2FsStat* stat)
{
	memset(st, 0, sizeof(struct stat));
	st->st_mode = stat->st_mode;
	st->st_uid = stat->st_uid;
	st->st_gid = stat->st_gid;
	st->st_atime = stat->st_atime;
	st->st_mtime = stat->st_mtime;
	st->st_ctime = stat->st_ctime;
	st->st_size = stat->st_size;
	st->st_blksize = stat->st_blksize;
}

int fstat(int fd, struct stat* buf)
{
	Lv2FsStat stat;
	int ret = lv2FsFstat(fd, &stat);
	if (!ret && buf)
		convertLv2Stat(buf, &stat);
	return lv2Errno(ret);
}

int stat(const char* path, struct stat* buf)
{
	Lv2FsStat stat;
	int ret = lv2FsStat(path, &stat);
	if (!ret && buf)
		convertLv2Stat(buf, &stat);
	return lv2Errno(ret);
}

int mkdir(const char* path, mode_t mode)
{
	return lv2Errno(lv2FsMkdir(path, UMASK(mode)));
}

int rmdir(const char* path)
{
	return lv2Errno(lv2FsRmdir(path));
}

/* Newlib already implements this? How?
int rename(const char* old, const char* new)
{
	return lv2Errno(lv2FsRename(old, new));
}
*/

int link(const char* old, const char* new)
{
	errno = ENOSYS;
	return -1;
}

off_t lseek(int fd, off_t offset, int whence)
{
	u64 position;
	int ret = lv2FsLSeek64(fd, offset, whence, &position);
	if (ret)
		return (off_t)lv2Errno(ret);
	return position;
}

int utime(const char* path, const struct utimbuf* times)
{
	return lv2Errno(lv2FsUtime(path, (const Lv2FsUtimbuf*)times));
}

int isatty(int fd)
{
	if (fd == stdout->_file || fd == stdin->_file || fd == stderr->_file)
		return 1;
	errno = ENOTTY;
	return 0;
}

int chmod(const char* path, mode_t mode)
{
	return lv2Errno(lv2FsChmod(path, mode));
}
