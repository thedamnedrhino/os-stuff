/*-
 * Copyright (c) 1999 Assar Westerlund
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: release/9.3.0/share/examples/kld/syscall/module/syscall.c 193374 2009-06-03 09:28:58Z pjd $
 */

#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysproto.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/priority.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/sched.h>


static int
sys_mfq_add(struct thread *td, void *arg)
{

	thread_lock(td);
	if(td->td_pri_class != PRI_TIMESHARE){
		//todo put a message in the argument, telling the user to use rtprio
		return 1;
	}
	sched_mfq_add(td);
	thread_unlock(td);
	return (0);
}

/*
 * The `sysent' for the new syscall
 */
static struct sysent  add_sysent= {
	0,			/* sy_narg */
	sys_mfq_add			/* sy_call */
};

static int
sys_mfq_remove(struct thread *td, void *arg){
	
	thread_lock(td);
	sched_mfq_remove(td);
	thread_unlock(td);
	return 0;
}

static struct sysent remove_sysent = {
	0,
	sys_mfq_remove
};


struct 
status_args{
	short *in_mfq;
	int *q;
};


static int 
sys_get_mfq_status(struct thread *td, void *arg){
	
	struct status_args *args = arg;
	*(args->in_mfq) = sched_is_in_mfq(td);
	*(args->q) = sched_mfq_q(td);

	return 0;
}
static struct sysent status_sysent = {
	2,
	sys_get_mfq_status
};

/*
 * The offset in sysent where the syscall is allocated.
 */
static int offset1 = NO_SYSCALL;
static int offset2 = NO_SYSCALL;
static int offset3 = NO_SYSCALL;

/*
 * The function called at load/unload.
 */
static int
load(struct module *module, int cmd, void *arg)
{
	int error = 0;

	switch (cmd) {
	case MOD_LOAD :
		printf("syscall loaded at %d\n", offset1);
		break;
	case MOD_UNLOAD :
		printf("syscall unloaded from %d\n", offset1);
		break;
	default :
		error = EOPNOTSUPP;
		break;
	}
	return (error);
}

static int
loadr(struct module *module, int cmd, void *arg)
{
	int error = 0;

	switch (cmd) {
	case MOD_LOAD :
		printf("syscall loaded at %d\n", offset2);
		break;
	case MOD_UNLOAD :
		printf("syscall unloaded from %d\n", offset2);
		break;
	default :
		error = EOPNOTSUPP;
		break;
	}
	return (error);
}

SYSCALL_MODULE(mfq_add, &offset1, &add_sysent, load, NULL);

SYSCALL_MODULE(mfq_remove, &offset2, &remove_sysent, loadr, NULL);

SYSCALL_MODULE(mfq_status, &offset3, &status_sysent, load, NULL);
