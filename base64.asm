global b64encode

section .text
b64encode:
        push    ebx
        mov     eax, [esp+8]
        mov     ebx, eax
        mov     ecx, eax
        mov     edx, eax
        shr     eax, 18
        shr     ebx, 12
        shr     ecx, 6
        and     eax, 63
        and     ecx, 63
        and     ebx, 63
        and     edx, 63
        movzx   eax, byte [b64alphabet + eax]
        movzx   ebx, byte [b64alphabet + ebx]
        movzx   ecx, byte [b64alphabet + ecx]
        movzx   edx, byte [b64alphabet + edx]
        sal     eax, 8
        or      eax, ebx
        sal     eax, 8
        or      eax, ecx
        sal     eax, 8
        or      eax, edx
        pop     ebx
        bswap   eax
        ret

section .data
	b64alphabet	db "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
