#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
void main()
{
    char opcode[10],label[10],operand[10],mnemonic[10],code[10];
    FILE *input,*output,*symtab,*optab,*symch;
    int locctr,start;
    bool err = false,found;
    input=fopen("sourcecode.txt","r");
    output=fopen("intermediate.txt","w");
    optab=fopen("optab.txt","r");
    symtab=fopen("symtab.txt","w");
    symch=fopen("symtab.txt","r");

    fscanf(input,"%s\t%s\t%s",label,opcode,operand);
    if(strcmp(opcode,"START")==0)
    {
        fseek(input,SEEK_SET,0);
        fscanf(input,"%s\t%s\t%X",label,opcode,&start);
        locctr=start;
        fprintf(output,"-\t%s\t%s\t%s\n",label,opcode,operand);
        fscanf(input,"%s\t%s\t%s",label,opcode,operand);
    }
    else{locctr=0;}
    while(strcmp(opcode,"END")!=0)
    {
        if(label[0]=='#'){continue;}
        fprintf(output,"%X\t",locctr);
        if(strcmp(label,"-")!=0)
        {
            char symbol[10],loc[10];
            fseek(symch,SEEK_SET,0);
            fscanf(symch,"%s\t%s",symbol,loc);
            while(!feof(symch))
            {
                if(strcmp(symbol,label)==0)
                {
                    err = true;
                    printf("Duplicate Symbol\n");
                    exit(0);
                }
                fscanf(symch,"%s\t%X",symbol,loc);
            }
            fprintf(symtab,"%s\t%d\n",label,locctr);
        }
        fseek(optab,SEEK_SET,0);
        fscanf(optab,"%s\t%s",code,mnemonic);
        found=false;
        while(strcmp(code,"END")!=0)
        {
            if(strcmp(code,opcode)==0)
            {
                locctr+=3;
                found=true;
                break;
            }
            fscanf(optab,"%s\t%s",code,mnemonic);
        }
        if(strcmp(opcode,"WORD")==0)
        {
            locctr+=3;
        }
        else if(strcmp(opcode,"BYTE")==0)
        {
            if(opcode[0]=='X'){
                locctr+=1;
                continue;
            }
            locctr+=strlen(operand)-3;
        }
        else if(strcmp(opcode,"RESW")==0)
        {
            locctr+=3*(atoi(operand));
        }
        else if(strcmp(opcode,"RESB")==0)
        {
            locctr+=atoi(operand);
        }
        else if(!found)
        {
            printf("Invalid Opcode %s\n",opcode);
            err=true;
            exit(0);
        }
        fprintf(output,"%s\t%s\t%s\n",label,opcode,operand);
        fscanf(input,"%s\t%s\t%s",label,opcode,operand);
    }
    fprintf(output,"-\t%s\t%s\t%s\n",label,opcode,operand);
    printf("Length of code = %d\n",locctr-start);
    FILE *length;
    length=fopen("length.txt","w");
    fprintf(length,"%d",locctr-start);
    fclose(input);
    fclose(output);
    fclose(symtab);
    fclose(optab);
    fclose(symch);
    fclose(length);
}
/*
SOURCE CODE:

-	START	2000
-	LDA FIVE
-	STA	ALPHA
-	LDCH 	CHARZ
- 	STCH 	C1
ALPHA 	RESW 	2
FIVE 	WORD 	5
CHARZ 	BYTE 	C'Z'
C1 	RESB 	1
- 	END	-

OBJECT TABLE:

LDA 03
STA	0F
LDCH	53
STCH	57
END -

SYMBOL TABLE:

ALPHA	200C
FIVE	2012
CHARZ	2015
C1	2016

INTERMEDIATE FILE:

-	-	START	2000
2000	-	LDA	FIVE
2003	-	STA	ALPHA
2006	-	LDCH	CHARZ
2009	-	STCH	C1
200C	ALPHA	RESW	2
2012	FIVE	WORD	5
2015	CHARZ	BYTE	C'Z'
2016	C1	RESB	1
-	-	END	-

LENGTH FILE:

23

*/
