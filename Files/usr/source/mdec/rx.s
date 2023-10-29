/ rx02 external diskette driver

/ core value must be the same as fsboot.s
core = 18.	/ first core loc (in KW) not used
.. = [core*2048.]

rxcs = 177170
/ read a block in
rblk:
	/ save some registers to stack
	mov	r2,-(sp)
	mov	r3,-(sp)
	
	/ get desired block and begin
	asl	r0
	mov	r0,-(sp)
	
	/ save block address and call
	mov	r1,-(sp)
	jsr	pc,rxrd
	
	/ restore and empty
	mov	(sp)+,r2
	jsr	pc,rxem
	
	/ get the next block
	mov	(sp)+,r0
	inc	r0
	mov	r2,-(sp)
	jsr	pc,rxrd
	
	/ restore again and empty
	mov	(sp)+,r2
	jsr	pc,rxem
	mov	(sp)+,r3
	mov	(sp)+,r2
	rts	pc
	
/ empty the buffer
rxem:
	/ empty command
	mov	$403,(r0)
	jsr	pc,rxtr
	
	/ word count
	mov	$200,(r1)
	jsr	pc,rxtr
	
	/ block address + add
	mov	r2,(r1)
	add	$256.,r2
	jmp	rxdo
	
/ read a block into the buffer
rxrd:
	/ begin to calculate interleave
	/ set up sect, track
	mov	$1.,r2
	mov	r2,r3
0:
	mov	$26.,r1
1:
	/ decrement block counter
	dec	r0
	bmi	7f
	
	/ increment sector twice
	inc	r2
	inc	r2
	/ increment sector if last
	cmp	r1,$1.
	bne	2f
	add	$5.,r2
2:
	/ increment sector if middle
	cmp	r1,$14.
	bne	3f
	inc	r2
3:
	/ check if wraps
	cmp	r2,$27.
	bmi	4f
	sub	$26.,r2
4:
	dec	r1
	beq	5f
	br	1b
5:
	inc	r3
	cmp	r3,$77.
	bne	6f
	clr	r3
6:
	br	0b

	/ read the block
7:
	mov	$rxcs,r0
	mov	r0,r1
	
	/ read command
	mov	$407,(r1)+
	jsr	pc,rxtr
	
	/ sector
	mov	r2,(r1)
	jsr	pc,rxtr
	
	/ track
	mov	r3,(r1)
	/ fall to done
	
/ rx02 done
rxdo:
	bit	$40,(r0)
	beq	rxdo
	rts	pc

/ rx02 transfer request
rxtr:
	bit	$200,(r0)
	beq	rxtr
	rts	pc
