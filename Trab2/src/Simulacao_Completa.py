#!/usr/bin/python
#Trabalho 2 - Redes I
#Jessiel Heitor Hacke - GRR20112890
#Lucas Pazelo Vargas de Oliveira - GRR------
#############################################
import os
#variavel de iteracoes
iter= 1.0
taxa= 1.0
TesteTaxa = 100.0

while TesteTaxa > 20 :
	os.system('echo set interval '+ str(iter) + ' > source.tcl')
			
	os.system('echo set saidaNS out-ns'+ str(iter)+ '.tr >> source.tcl')
	os.system('echo Gerando simulacao de '+ str(taxa) +' mensagens por segundo!' )
	
	os.system('ns Trabalho2_simulacao_rede.tcl')
	os.system('rm source.tcl')
	
	os.system('awk -v out=Latencia.out -v intervalo='+str(taxa)+' -f CalculaLatencia.awk out-ns'+str(iter)+'.tr')
	
	os.system('awk -v twentypercent=TestTaxa.out -v out=Taxa.out -v mensagens='+str(taxa)+' -f CalculaTaxa.awk out-ns'+str(iter)+'.tr')
	
	file_teste= open("TestTaxa.out", 'r')
	a = file_teste.read() 
	file_teste.close()
	TesteTaxa=float(a)
	
	os.system('rm out-ns'+str(iter)+'.tr')
	
	iter = (iter/1.4)
	taxa = (taxa*1.4)

os.system('gnuplot geraGraficoTaxa.plt')
#os.system('rm Taxa.out')
	
os.system('gnuplot geraGraficoLatencia.plt')
#os.system('rm Latencia.out')
 
os.system('rm TestTaxa.out')

