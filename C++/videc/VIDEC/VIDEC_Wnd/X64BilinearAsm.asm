


; void BilinearMMX_Asm
;(unsigned char*pSrcLine0,unsigned char*pSrcLine2,unsigned char*pDstLine1,unsigned int nLineSize)
;�����ҵ�ǰ4���������δ����rcx,rdx,r8,r9��ʣ��Ĳ������ҵ���˳����ջ

.data

.code
PUBLIC BilinearMMX_Asm

BilinearMMX_Asm PROC

			pxor mm7, mm7

			mov  rsi, rcx
			mov  rax, rdx
			mov  rdi, r8 
			mov edx, r9d

START_LOOP:
		movq mm1, [rsi];
		movq mm3, [rax];

		punpcklbw mm1, mm7;
		punpcklbw mm3, mm7;
		paddw mm1, mm3;

		psrlw   mm1, 1;
		packuswb mm1,mm7;

		movq  [rdi], mm1;

		add  rsi, 4
			add  rax, 4
			add  rdi, 4

			dec         edx
			jnz         START_LOOP			


RET
BilinearMMX_Asm ENDP
End


