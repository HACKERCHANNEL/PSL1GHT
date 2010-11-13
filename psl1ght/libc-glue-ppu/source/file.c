#include <psl1ght/lv2/filesystem.h>
#include <psl1ght/lv2/tty.h>
#include <psl1ght/lv2/errno.h>

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/socket.h>

#define DEFAULT_FILE_MODE (S_IRWXU | S_IRWXG | S_IRWXO)
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

int fstat(int fd, struct stat* st)
{
	errno = ENOSYS;
	return -1;
}

int stat(const char* path, struct stat* buf)
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

