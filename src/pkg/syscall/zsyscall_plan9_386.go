// mksyscall.pl -l32 -plan9 syscall_plan9.go syscall_plan9_386.go
// MACHINE GENERATED BY THE COMMAND ABOVE; DO NOT EDIT

package syscall

import "unsafe"

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func exits(msg *byte) {
	Syscall(SYS_EXITS, uintptr(unsafe.Pointer(msg)), 0, 0)
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func fd2path(fd int, buf []byte) (err Error) {
	var _p0 unsafe.Pointer
	if len(buf) > 0 {
		_p0 = unsafe.Pointer(&buf[0])
	} else {
		_p0 = unsafe.Pointer(&_zero)
	}
	r0, _, e1 := Syscall(SYS_FD2PATH, uintptr(fd), uintptr(_p0), uintptr(len(buf)))
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func pipe(p *[2]_C_int) (err Error) {
	r0, _, e1 := Syscall(SYS_PIPE, uintptr(unsafe.Pointer(p)), 0, 0)
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func sleep(millisecs int32) (err Error) {
	r0, _, e1 := Syscall(SYS_SLEEP, uintptr(millisecs), 0, 0)
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func await(s []byte) (n int, err Error) {
	var _p0 unsafe.Pointer
	if len(s) > 0 {
		_p0 = unsafe.Pointer(&s[0])
	} else {
		_p0 = unsafe.Pointer(&_zero)
	}
	r0, _, e1 := Syscall(SYS_AWAIT, uintptr(_p0), uintptr(len(s)), 0)
	n = int(r0)
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func Dup(oldfd int, newfd int) (fd int, err Error) {
	r0, _, e1 := Syscall(SYS_DUP, uintptr(oldfd), uintptr(newfd), 0)
	fd = int(r0)
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func Open(path string, mode int) (fd int, err Error) {
	r0, _, e1 := Syscall(SYS_OPEN, uintptr(unsafe.Pointer(StringBytePtr(path))), uintptr(mode), 0)
	fd = int(r0)
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func Create(path string, mode int, perm uint32) (fd int, err Error) {
	r0, _, e1 := Syscall(SYS_CREATE, uintptr(unsafe.Pointer(StringBytePtr(path))), uintptr(mode), uintptr(perm))
	fd = int(r0)
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func Remove(path string) (err Error) {
	r0, _, e1 := Syscall(SYS_REMOVE, uintptr(unsafe.Pointer(StringBytePtr(path))), 0, 0)
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func Pread(fd int, p []byte, offset int64) (n int, err Error) {
	var _p0 unsafe.Pointer
	if len(p) > 0 {
		_p0 = unsafe.Pointer(&p[0])
	} else {
		_p0 = unsafe.Pointer(&_zero)
	}
	r0, _, e1 := Syscall6(SYS_PREAD, uintptr(fd), uintptr(_p0), uintptr(len(p)), uintptr(offset), uintptr(offset>>32), 0)
	n = int(r0)
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func Pwrite(fd int, p []byte, offset int64) (n int, err Error) {
	var _p0 unsafe.Pointer
	if len(p) > 0 {
		_p0 = unsafe.Pointer(&p[0])
	} else {
		_p0 = unsafe.Pointer(&_zero)
	}
	r0, _, e1 := Syscall6(SYS_PWRITE, uintptr(fd), uintptr(_p0), uintptr(len(p)), uintptr(offset), uintptr(offset>>32), 0)
	n = int(r0)
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func Close(fd int) (err Error) {
	r0, _, e1 := Syscall(SYS_CLOSE, uintptr(fd), 0, 0)
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func Chdir(path string) (err Error) {
	r0, _, e1 := Syscall(SYS_CHDIR, uintptr(unsafe.Pointer(StringBytePtr(path))), 0, 0)
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func Bind(name string, old string, flag int) (err Error) {
	r0, _, e1 := Syscall(SYS_BIND, uintptr(unsafe.Pointer(StringBytePtr(name))), uintptr(unsafe.Pointer(StringBytePtr(old))), uintptr(flag))
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func Mount(fd int, afd int, old string, flag int, aname string) (err Error) {
	r0, _, e1 := Syscall6(SYS_MOUNT, uintptr(fd), uintptr(afd), uintptr(unsafe.Pointer(StringBytePtr(old))), uintptr(flag), uintptr(unsafe.Pointer(StringBytePtr(aname))), 0)
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func Stat(path string, edir []byte) (n int, err Error) {
	var _p0 unsafe.Pointer
	if len(edir) > 0 {
		_p0 = unsafe.Pointer(&edir[0])
	} else {
		_p0 = unsafe.Pointer(&_zero)
	}
	r0, _, e1 := Syscall(SYS_STAT, uintptr(unsafe.Pointer(StringBytePtr(path))), uintptr(_p0), uintptr(len(edir)))
	n = int(r0)
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func Fstat(fd int, edir []byte) (n int, err Error) {
	var _p0 unsafe.Pointer
	if len(edir) > 0 {
		_p0 = unsafe.Pointer(&edir[0])
	} else {
		_p0 = unsafe.Pointer(&_zero)
	}
	r0, _, e1 := Syscall(SYS_FSTAT, uintptr(fd), uintptr(_p0), uintptr(len(edir)))
	n = int(r0)
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func Wstat(path string, edir []byte) (err Error) {
	var _p0 unsafe.Pointer
	if len(edir) > 0 {
		_p0 = unsafe.Pointer(&edir[0])
	} else {
		_p0 = unsafe.Pointer(&_zero)
	}
	r0, _, e1 := Syscall(SYS_WSTAT, uintptr(unsafe.Pointer(StringBytePtr(path))), uintptr(_p0), uintptr(len(edir)))
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}

// THIS FILE IS GENERATED BY THE COMMAND AT THE TOP; DO NOT EDIT

func Fwstat(fd int, edir []byte) (err Error) {
	var _p0 unsafe.Pointer
	if len(edir) > 0 {
		_p0 = unsafe.Pointer(&edir[0])
	} else {
		_p0 = unsafe.Pointer(&_zero)
	}
	r0, _, e1 := Syscall(SYS_FWSTAT, uintptr(fd), uintptr(_p0), uintptr(len(edir)))
	err = nil
	if int(r0) == -1 {
		err = NewError(e1)
	}
	return
}
