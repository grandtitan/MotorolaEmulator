      NAM VAJA
NPOD  EQU  10      
*
*
*
      ORG  $100     
      LDX  #ZNPOD   
      LDAB #NPOD    
      CLRA          
ZANKA ADDA 0,X      
      INX           
      DECB          
      BNE ZANKA     
      STAA 0,X      
TU    JMP TU        
*
*
*
ZNPOD RMB NPOD-1    
      END
