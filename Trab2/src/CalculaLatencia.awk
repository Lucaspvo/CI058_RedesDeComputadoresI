{
        # Define os momentos de envio
        if ( $1 == "+" )
        {
                # verifica se é o primeiro envio do pacote
                if ( temposEnvio[ $12 ] == "" ) {
                        temposEnvio[ $12 ] = $2
                        temposRecebe[ $12 ] = -1
                }
        }
        # Define os momentos de recebimento
        else if ( $1 == "r")
        {
        		
                if ( temposRecebe[ $12 ] < $2 ) {
                        temposRecebe[ $12 ] = $2
                }
        }
}


END{
		arrSize = length( temposEnvio )
        amostra = length( temposEnvio )
        cont = 0
        perdas = 0
        somaAmostra = 0
        for ( i = 0; i < arrSize; i++ )
        {
                if ( temposRecebe[i] == -1 ) {
                        perdas++
                }else{
                        somaAmostra +=  temposRecebe[i]-temposEnvio[i]
                        
                }
        }
        print (intervalo" "somaAmostra/amostra) >> out
}

