/* BEGIN A3 SETUP */
/*
 * File handles and file tables.
 * New for ASST3
 */

#include <types.h>
#include <kern/errno.h>
#include <kern/limits.h>
#include <kern/stat.h>
#include <kern/unistd.h>
#include <file.h>
#include <syscall.h>
#include <vfs.h>
#include <synch.h>
#include <current.h>
#include <vnode.h>
#include <kern/fcntl.h>

/*** openfile functions ***/

/*
 * file_open
 * opens a file, places it in the filetable, sets RETFD to the file
 * descriptor. the pointer arguments must be kernel pointers.
 * NOTE -- the passed in filename must be a mutable string.
 * 
 * A3: As per the OS/161 man page for open(), you do not need 
 * to do anything with the "mode" argument.
 */
int
file_open(char *filename, int flags, int mode, int *retfd)
{
	struct openfile *file;
	struct vnode *file_vnode;
	int result;

	//try and open the file
	result = vfs_open(filename, flags, mode, &file_vnode);
	if(result) {
		vfs_close(file_vnode);
		return result;
	}

	//try and allocate space for the filetable entry
	file = kmalloc(sizeof(struct openfile));
	if(file == NULL) {
		vfs_close(file_vnode);
		return result;
	}

	//initialize the file
	file->of_offset = 0;
	file->of_vnode = file_vnode;
	file->of_flags = flags;
	file->of_refcount = 1;
	file->of_lock = lock_create("file lock");
	if(file->of_lock == NULL) {
		vfs_close(file_vnode);
		kfree(file);
		return ENOMEM;
	}

	//try and put into the filetable
	result = create_filetable_entry(file, retfd);
	if(result) {
		lock_destroy(file->of_lock);
		vfs_close(file_vnode);
		kfree(file);
		return result;
	}

	return 0;
}


/* 
 * file_close
 * Called when a process closes a file descriptor.  Think about how you plan
 * to handle fork, and what (if anything) is shared between parent/child after
 * fork.  Your design decisions will affect what you should do for close.
 */
int
file_close(int fd)
{
    int result;
    struct openfile *file;

    result = lookup_filetable_entry(fd, &file);
    if(result) {
    	return result;
    }

    lock_acquire(file->of_lock);

    if(file->of_refcount == 1) {
    	vfs_close(file->of_vnode);
    	lock_release(file->of_lock);
    	lock_destroy(file->of_lock);
    	kfree(file);
    	curthread->t_filetable->ft_files[fd] = NULL;
    } else if(file->of_refcount >= 1) {
    	file->of_refcount--;
    	lock_release(file->of_lock);

    	return ENOTEMPTY;
    }

	return 0;
}

/*** filetable functions ***/

/* 
 * filetable_init
 * pretty straightforward -- allocate the space, set up 
 * first 3 file descriptors for stdin, stdout and stderr,
 * and initialize all other entries to NULL.
 * 
 * Should set curthread->t_filetable to point to the
 * newly-initialized filetable.
 * 
 * Should return non-zero error code on failure.  Currently
 * does nothing but returns success so that loading a user
 * program will succeed even if you haven't written the
 * filetable initialization yet.
 */

int
filetable_init(void)
{
	int fd;
	char con[5];
	int result;

	if(curthread->t_filetable != NULL) {
		//return an error code saying already initialized
		return -1;
	}

	//allocate space for the table
	curthread->t_filetable = kmalloc(sizeof(struct filetable));
	if(curthread->t_filetable == NULL) {
		return ENOMEM;
	}

	//set all file table entries to null
	for(fd = 0; fd < __OPEN_MAX; fd++) {
		curthread->t_filetable->ft_files[fd] = NULL;
	}

	//stdin
	strcpy(con, "con:");
	result = file_open(con, O_RDONLY, 0, &fd);
	if(result) {
		return result;
	}
	//stdout
	strcpy(con, "con:");
	result = file_open(con, O_WRONLY, 0, &fd);
	if(result) {
		return result;
	}
	//stderr
	strcpy(con, "con:");
	result = file_open(con, O_WRONLY, 0, &fd);
	if(result) {
		return result;
	}
	return 0;
}	

/*
 * filetable_destroy
 * closes the files in the file table, frees the table.
 * This should be called as part of cleaning up a process (after kill
 * or exit).
 */
void
filetable_destroy(struct filetable *ft)
{
	int fd;
	//if the filetable is not null
	if(ft != NULL) {
		//close all the files in the table
    	for(fd = 0; fd < __OPEN_MAX; fd++) {
    		if(ft->ft_files[fd])
				file_close(fd);
			//don't care about result of file close
		}
	}
	//free the table
	kfree(ft);
}	


/* 
 * You should add additional filetable utility functions here as needed
 * to support the system calls.  For example, given a file descriptor
 * you will want some sort of lookup function that will check if the fd is 
 * valid and return the associated vnode (and possibly other information like
 * the current file position) associated with that open file.
 */

int
create_filetable_entry(struct openfile *file, int *fd)
{
	//find the first non null space in the filetable
	int i;
	for(i = 0; i < __OPEN_MAX && curthread->t_filetable->ft_files[i] != NULL; i++) {
		continue;
	}

	//if no space in the file table, return EMFILE
	if(i >= __OPEN_MAX) {
		return EMFILE;
	}

	//set the filetable entry, return the filedescriptor
	curthread->t_filetable->ft_files[i] = file;
	*fd = i;

	return 0;
}

int
lookup_filetable_entry(int fd, struct openfile **rtfile)
{
	//check that the filedescriptor is valid
	if(fd < 0 || fd > __OPEN_MAX) {
		return EBADF;
	}

	//check that the file is open
	if(curthread->t_filetable->ft_files[fd] == NULL) {
		return EBADF;
	}

	//set the return file
	*rtfile = curthread->t_filetable->ft_files[fd];

	return 0;
}

/* END A3 SETUP */
