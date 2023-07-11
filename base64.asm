global b64encode

section .text
b64encode:
        mov     ecx, edi
        mov     rdi, rbx
        mov     rbx, b64alphabet
	
        mov     al, cl
        and     al, 63	; Pega os ultimos 6bits
        xlatb		; Traduz da tabela
        shr     ecx, 6	; Desloca 6 bits para direita 
        sal     eax, 8	; Desloca 8 bits pro próximo char base64

        mov     al, cl
        and     al, 63
        xlatb
        shr     ecx, 6
        sal     eax, 8

        mov     al, cl
        and     al, 63
        xlatb
        shr     ecx, 6
        sal     eax, 8

        mov     al, cl
        and     al, 63
        xlatb

        mov     rbx, rdi
        ret

section .data
	b64alphabet	db "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
