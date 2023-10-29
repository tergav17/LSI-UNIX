/ rx02 external driver adapter

rxcs = 177170

/ driver init
/ have the rx02 install the external driver
dinit:
	mov	$rxcs,r1
	mov	r1,r2
	
	/ read sec 12 tra 0 from rx
	mov	$407,(r2)+
	jsr	pc,rxtr
	mov	$12.,(r2)
	jsr	pc,rxtr
	clr	(r2)
	jsr	pc,rxdo
	mov	$403,(r1)
	jsr	pc,rxtr
	mov	$200,(r2)
	jsr	pc,rxtr
	mov	$[core*2048.],(r2)
/ fall through to rx done

/ rx done
rxdo:
	bit	$40,(r1)
	beq	rxdo
	rts	pc

/ rx02 transfer request
rxtr:
	bit	$200,(r1)
	beq	rxtr
	rts	pc

/ calls the external driver
rblk:
	mov	dska,r0
	mov	ba,r1
	jmp	[core*2048.]
