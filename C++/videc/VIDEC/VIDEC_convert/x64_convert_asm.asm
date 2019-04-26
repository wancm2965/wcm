
;从左到右的前4个参数依次存放在rcx,rdx,r8,r9，剩余的参数从右到左按顺序入栈

;void mmx_process_frame2field_asm(const BYTE* srcp, int src_pitch, int row_size, BYTE* dstp, int dst_pitch, int height) {
;height--;
;static const __int64 add_2=0x0002000200020002;
;if ((row_size&3)==0) {  // row width divideable with 4 (one dword per loop)
;__asm {


.data

add_2 DQ  0002000200020002h
srcp    DQ 0
src_pitch DD 0
row_size DD 0
dstp DQ 0
dst_pitch DD 0
height DD 0

.code
PUBLIC mmx_process_frame2field_asm

mmx_process_frame2field_asm PROC

;parameter:
mov         qword ptr [srcp],rcx
mov         dword ptr [src_pitch],edx
mov         dword ptr [row_size],r8d
mov         qword ptr [dstp],r9

;mov         dword ptr [dst_pitch],DWORD PTR [RSP+28H]
mov         edx,DWORD PTR [RSP+28H]
mov         dword ptr [dst_pitch],edx
;mov         dword ptr [height],DWORD PTR [RSP+30H]
mov          edx,DWORD PTR [RSP+30H]
mov          dword ptr [height],edx

;#define R_SRC edx
;#define R_DST edi
;#define R_XOFFSET eax
;#define R_YLEFT ebx
;#define R_SRC_PITCH ecx
;#define R_DST_PITCH esi

;push ebx  ;// avoid compiler bug
movq mm7,[add_2];
add [srcp],-4
mov eax,0
mov rdx,srcp
mov rdi,dstp
mov ecx,[src_pitch]
mov esi,[dst_pitch]
mov ebx,[height]
loopback:
pxor mm1,mm1
punpckhbw mm0,[rdx]  ;// line0
punpckhbw mm1,[rdx+rcx]  ;// line1
punpckhbw mm2,[rdx+rcx*2]  ;// line2
psrlw mm0,8
psrlw mm1,7
paddw mm0,mm7
psrlw mm2,8
paddw mm0,mm1
paddw mm0,mm2
psrlw mm0,2
packuswb mm0,mm1
movq [rdi+rax],mm0
add rdx,4
add eax,4
cmp  eax,[row_size]
jl loopback						; Jump back
add srcp, rcx
mov eax,0
add srcp, rcx
add rdi,rsi
mov rdx,srcp
dec ebx
jnz loopback

;// last line
loopback_last:
pxor mm1,mm1
punpckhbw mm0,[rdx]  ;// line0
punpckhbw mm1,[rdx+rcx]  ;// line1
psrlw mm0,8
movq mm2,mm1 ; // dupe line 1
psrlw mm1,7
paddw mm0,mm7
psrlw mm2,8
paddw mm0,mm1
paddw mm0,mm2
psrlw mm0,2
packuswb mm0,mm1
movq [rdi+rax],mm0
add eax,4
add edx,4
cmp  eax,[row_size]
jl loopback_last						; Jump back


;  emms
;  pop ebx
;  }
;}
;}

RET
mmx_process_frame2field_asm ENDP
End









;//sample
;CombineAsmAdd(int a, int b, int c, int d, int e, int f)

.data
a DD 0
b DD 0
c DD 0
d DD 0
e DD 0
f DD 0

.code
PUBLIC CombineAsmAdd

CombineAsmAdd PROC

parameter:

mov         dword ptr [a],ecx  
mov         dword ptr [b],edx  
mov         dword ptr [c],r8d  
mov         dword ptr [d],r9d

;mov         dword ptr [e],DWORD PTR [RSP+28H]
mov         edx,DWORD PTR [RSP+28H]
mov         dword ptr [e],edx
;mov         dword ptr [f],DWORD PTR [RSP+30H]
mov         edx,DWORD PTR [RSP+30H]
mov         dword ptr [f],edx


mov  ECX,0
ADD ECX, dword ptr [a]
ADD ECX, dword ptr [b]
ADD ECX, dword ptr [c]
ADD ECX, dword ptr [d]
ADD ECX, dword ptr [e]
ADD ECX, dword ptr [f]
mov EAX,ECX

RET
CombineAsmAdd ENDP
End

