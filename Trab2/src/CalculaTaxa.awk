{
        # Define os momentos de envio
        if ( $1 == "+" )
        {
                # verifica se é o primeiro envio do pacote
                if ( envios[ $12 ] == "" ) {
                        envios[ $12 ] = 1
                }
        }
        # Define se o pacote foi recebido
        else if ( $1 == "d" ) 
        {
                        envios[ $12 ] = -1
        }
}

END{
        arrSize = length( envios )
        amostra = length( envios )
        cont = 0
        pacotesPerdidosNaAmostra = 0
        for ( i = 0; i < arrSize; i++ )
        {
                cont++
                if ( envios[i] == -1 ) {
                        pacotesPerdidosNaAmostra++
                }
        }
        print (mensagens" "( ( ( amostra-pacotesPerdidosNaAmostra)/amostra )*100 )) >> out
        print ((amostra-pacotesPerdidosNaAmostra)/amostra )*100 > twentypercent
}

