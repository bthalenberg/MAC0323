            EXTERN      main
            
c           IS          $0					*guarda o tamanho da coluna
s           IS          $1
cur         IS          $2
aux         IS          $3
ret         IS          $4
temp        IS          $5
ini         IS          $6					*guarda a posicao inicial de rSP quando o programa comeca
cont        IS          $7
isparag     IS          $8
wordcont    IS          $9
begin       IS          $10
auxl        IS          $11
val         IS          $12
spaces      IS          $13
auxpl       IS          $14
sum         IS          $15
auxst       IS          $16
multiaux    IS          $17
								*restante sao variaveis usadas para propositos diversos no programa

readnum     SETW        ret,0				*funcao que le o numero dado na linha de comando, coloca em ret ($4) e volta para a funcao main
            SETW        rX,1
            SUBU 		begin,rSP,16
            LDOU 		begin,begin,0
readnumcont LDB 		temp,begin,0
            JZ          temp,end6
            ADDU		begin,begin,1
            MULU        ret,ret,10
            SUBU        aux,temp,48
            ADDU        ret,ret,aux
            JMP         readnumcont
end6        SETW        aux,0
            SETW        temp,0
            JMP         alread

print       SETW        rX,2				*funcao que escreve na saida padrao os caracteres a partir de M[begin+8] ate M[begin+8*M[begin]]
            ADDU        cur,begin,8
            LDO         aux,begin,0
printcont   LDO         rY,cur,0
            ADDU        cur,cur,8
            SUBU        aux,aux,1
            INT         #80
            JZ          aux,end3
            JMP         printcont
end3        RET         0

printnl     SETW        rX,2				*funcao que da quebra de linha na saida padrao
            SETW        rY,10
            INT         #80
            RET         0

printspaces SETW        rX,2				*funcao que escreve cont ($7) espacos na saida padrao
            SETW        rY,32
spacecont   SUBU        cont,cont,1
            INT         #80
            JNZ         cont,spacecont
            RET         0

printline   SUBU        auxpl,wordcont,1	*funcao que escreve toda uma linha justificada a partir da quantidade de palavras que ela deve mostrar
plcont      JZ          auxpl,end4
            CALL        print
            DIVU        cont,spaces,auxpl	*calculo da quantidade de espacos a serem mostrados
            SUBU        spaces,spaces,cont
            CALL        printspaces
            SUBU        auxpl,auxpl,1
            LDO         aux,begin,0
            MULU        multiaux,aux,8
            ADDU        begin,begin,multiaux
            ADDU        begin,begin,8
            JMP         plcont
end4        CALL        print
            LDO         aux,begin,0
            MULU        multiaux,aux,8
            ADDU        begin,begin,multiaux
            ADDU        begin,begin,8
            CALL        printnl
            RET         0

start       ADDU        val,ini,0		*funcao que le todas as palavras guardadas na memoria e quando acha quantidade suficiente para escrever uma linha
										*chama a funcao printline
			SETW		s,1
reset       ADDU        begin,val,0
            LDO         sum,val,0
            SETW        wordcont,1
            MULU        multiaux,sum,8
            ADDU        val,val,multiaux
            ADDU        val,val,8
            SUB			temp,s,10
            JZ 			temp,end
            ADDU		s,s,1
            JZ          sum,newl
            JN          sum,end5		*terminar o processo se lermos -1 na memoria
startcont   LDO         auxst,val,0
			SETW		s,0
            JNP         auxst,shouldpldc
            ADDU        auxst,auxst,sum
            SUBU        auxst,auxst,c
            ADDU        auxst,auxst,wordcont
            JNP         auxst,contword	*se a quantidade de palavras lidas ate agora ainda nao permite criar uma linha inteira, ler mais uma palavra
            JMP         shouldpl
contword    LDO         spaces,val,0
            ADDU        sum,sum,spaces
            MULU        multiaux,spaces,8
            ADDU        val,val,multiaux
            ADDU        val,val,8
            ADDU        wordcont,wordcont,1
            JMP         startcont
shouldpldc  ADDU        temp,c,0
            SUBU        spaces,wordcont,1
            ADDU        c,sum,spaces
            CALL        printline
            ADDU        c,temp,0
            JMP         reset
shouldpl    SUB         spaces,c,sum
            CALL        printline
            JMP         reset
newl        CALL        printnl
            JMP         reset
end5        JMP         end

readspace   SETW        ret,1			*essa funcao coloca 1 em ret se rA for 9,13 ou 32; 2 se for 10 (novalinha); 0 se for qualquer outro caractere
            SUBU        aux,rA,9
            JZ          aux,end7
            SUBU        aux,rA,13
            JZ          aux,end7
            SUBU        aux,rA,32
            JZ          aux,end7
            SETW        ret,2
            SUBU        aux,rA,10
            JZ          aux,end7
            SETW        ret,0
end7        RET         0

limbo       SETW        cont,0			*essa funcao eh chamada quando se le um espaco, e para quando um caractere que nao eh um espaco eh lido
										*ela tambem verifica se essa sequencia de espacos contem duas ou mais quebras de linha, para serem tradados
										*como o fim de um paragrafo
            ADDU        temp,ret,0
getc        ADDU        temp,ret,0
            SUB 		temp,temp,2
            JNZ 		temp,limbcont
            ADDU 		cont,cont,1
limbcont    INT         #80
            CALL        readspace
            JZ          ret,parag
            JMP         getc
parag		JZ          cont,end1
            SUB         rX,cont,2
            JN          rX,end1
            SETW        isparag,1		*paragrafo identificado
end1        SETW        rX,1
            RET         0

read        SETW        rX,1			*essa funcao le todos os caracteres e os coloca na memoria na forma {tamanho da palavra, {sequencia de letras}}...
										*essa funcao tambem adiciona um zero na memoria quando naquela posicao eh necessario um novo paragrafo
										*e adiciona -1 quando a entrada acaba
word        ADDU        begin,rSP,0
            SETW        cont,0
            PUSH        cont
            ADD         temp,rA,1
            JZ          temp,stopping
            PUSH        rA
character   INT         #80
            ADD         temp,rA,1
            JZ          temp,endfile
            CALL        readspace
            JNZ         ret,shouldlimbo
            ADDU        cont,cont,1
            PUSH        rA
            JMP         character
shouldlimbo ADDU        cont,cont,1
            STO         cont,begin,0
            CALL        limbo
            JZ          isparag,word
            SETW        temp,0
            PUSH        temp
            SETW        isparag,0
            JMP         word
endfile     SUB         temp,temp,1
            PUSH        temp
            ADDU        cont,cont,1
            STO         cont,begin,0
            JMP			conti
stopping	STO 		rA,begin,0
			JMP			conti

main        ADDU        ini,rSP,0		*a funcao main chama primeiro a funcao para ler o valor c da linha de comando
										*depois chama limbo para o caso em que a entrada comeca com espacos em branco
										*depois chama read para que as palavras sejam armazenadas na memoria
										*e por fim, chama start para que executar o algoritmo nas palavras que entao na memoria
            JMP	        readnum
alread      ADDU        c,ret,0
			CALL		limbo
            JMP         read
conti       ADDU		begin,ini,0
            JMP       	start
end 		INT         0
